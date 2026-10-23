/*
© [2026] Microchip Technology Inc. and its subsidiaries.
 
    Subject to your compliance with these terms, you may use Microchip 
    software and any derivatives exclusively with Microchip products. 
    You are responsible for complying with 3rd party license terms  
    applicable to your use of 3rd party software (including open source  
    software) that may accompany Microchip software. SOFTWARE IS ?AS IS.? 
    NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS 
    SOFTWARE, INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT,  
    MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT 
    WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY 
    KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF 
    MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE 
    FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP?S 
    TOTAL LIABILITY ON ALL CLAIMS RELATED TO THE SOFTWARE WILL NOT 
    EXCEED AMOUNT OF FEES, IF ANY, YOU PAID DIRECTLY TO MICROCHIP FOR 
    THIS SOFTWARE.
*/

#include <stdbool.h>
#include <stdint.h>
#include "conversion_macros.h"
#include "precise_delay.h"
#include "motor.h"
#include "mcc_mapping.h"
#include "tables.h"
#include "config.h"

/****************************/
/*** internal definitions ***/
/****************************/

#define MAX_AMP_GET                 PWM_PER
#define MIN_AMP_GET                 (0)
#define MAX_INC                     CONVERT_DEGREES_TO_U16(90.0)   
#define MIN_INC                     ERPM_TO_INC(100.0)
#define SENSOR_TO_POS_TABLE         ((direction == DIR_CW) ? pos_table_cw : pos_table_ccw)
#define HALL_OFFSET                 ((direction == DIR_CW) ? CONVERT_DEGREES_TO_U16(HALL_MISALIGNMENT_CW + 60.0) : CONVERT_DEGREES_TO_U16(HALL_MISALIGNMENT_CCW + 180.0))
#define BEMF_OFFSET                 CONVERT_DEGREES_TO_U16(30.0)
#define SENSOR_OFFSET               ((HALL_ENABLED) ? HALL_OFFSET : BEMF_OFFSET)
#define RS                          (MOTOR_RPP/2.0)             // Ohm
#define LS                          (MOTOR_LPP/2000.0)          // Henry
#define DT                          (3.0 / SAMPLING_FREQUENCY)  // Second
#define NSCALE                      (16777216)
#define EMF_K1                      (int32_t)(((ANALOG_REFERENCE / 1000.0) * VBUS_DIVIDER * NSCALE) / (ADC_MAX * 3.0 * MAX_AMP_GET))
#define EMF_K2                      (int32_t)(((ANALOG_REFERENCE / 1000.0) * RS           * NSCALE) / (ADC_MAX * CURRENT_SHUNT_RESISTANCE * CURRENT_AMPLIFIER_GAIN))
#define EMF_K3                      (int32_t)(((ANALOG_REFERENCE / 1000.0) * LS           * NSCALE) / (ADC_MAX * CURRENT_SHUNT_RESISTANCE * CURRENT_AMPLIFIER_GAIN * DT))

typedef enum
{
    STALL_NONE,
    STALL_TOO_LATE,
    STALL_TOO_FAST,
} stall_ev_t;

/****************************/
/****** internal data *******/
/****************************/

/* status flags */
static          bool                status_running, command_enabled, drive_forced;
static volatile motor_status_t      status_events;
static volatile fixp16_t            actual_amplitude, target_amplitude;
static motor_dir_t                  direction;
static volatile uint32_t            integrator;
static volatile triple16_t          stator_angles;
static volatile triple16_t          drive_data;
static volatile uint8_t             sensor_cnt;

/****************************/
/***** private functions ****/
/****************************/

static void __OpenLoop_Control(void)
{
    static uint16_t cnt = 0;
    cnt++;
    if(cnt == MOTOR_AMPRAMP_STEP_INTERVAL)
    {
        cnt = 0;
        if(actual_amplitude > target_amplitude)
        {
            actual_amplitude --;
        }
        else if(actual_amplitude < target_amplitude)
        {
            actual_amplitude ++;
        }
    }
}

int24_t  Multiply_U16xI16(uint16_t a,  int16_t b)
{
    int24_t result;
    result = (uint24_t)a * (int24_t)b;
    return   result;
}

/* data shared across __Speed_* functions only */
static volatile uint16_t stator_speed, target_speed;

/* parameter: 'false' - normal run , 'true' - first run & initialization */
static void __Speed_Control(bool state)
{
    static uint32_t speed_integrator = 0;    
    if(state == true)
    {
        speed_integrator = ((uint32_t)actual_amplitude << 16);
        target_speed = stator_speed;
    }
    if((REGULATOR_SPEED_EN == true) && (command_enabled))
    {
            int32_t Ki_err, Kp_err, tmp;
            int16_t err;
            tmp = (int32_t)target_speed - (int32_t)stator_speed;
            err = (int16_t)SATURATE(tmp, -32000, 32000);

            Ki_err = Multiply_U16xI16((uint16_t)(REGULATOR_PI_KI * REGULATOR_PI_DT), err);
            Kp_err = Multiply_U16xI16((uint16_t)(REGULATOR_PI_KP), err);

            tmp = Ki_err + (int32_t)speed_integrator;
            speed_integrator = (uint32_t)SATURATE(tmp, MIN_AMP_GET * 65536L, MAX_AMP_GET * 65536L);

            tmp = Kp_err + (int32_t)speed_integrator;
            split_u32_t amp_acc;
            amp_acc.W32 = (uint32_t)SATURATE(tmp, MIN_AMP_GET * 65536L, MAX_AMP_GET  * 65536L);
            actual_amplitude = amp_acc.H16;
    }
}

static void __ShutOff(void)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        PWM_OFF();
        command_enabled = false;
        status_running = false;
        drive_forced = true;
        actual_amplitude = 0;
        stator_speed = 0;
        sensor_cnt = 0;
    }
}

static inline void  __Motor_StallHandler(stall_ev_t ev)
{
    static uint8_t stall_counter = 0;
    if(status_running == false)
    {
        stall_counter = 0;
        /* motor already off, nothing to do */
    }
    else
    {
        switch(ev)
        {
            case STALL_NONE:     if(stall_counter) stall_counter--;                                                                                 break;
            case STALL_TOO_LATE: stall_counter++; if(stall_counter > STALL_DETECTION_THRESHOLD) {__ShutOff(); status_events |= MOTOR_EVENT_STALL; } break;
            case STALL_TOO_FAST:                                                                 __ShutOff(); status_events |= MOTOR_EVENT_STALL;   break;
            default:                                                                                                                                break;
        }
    }
}

static inline uint8_t __Emf_Rebuild(void)
{
    static int16_t          pia_adc = 0, pib_adc = 0, pic_adc = 0;
    static split_i32_t      a32, b32, c32;
    int16_t                 i = 0;
    int16_t                 phase = 0;
    phase_currents_t        index = 0;
    uint8_t                 retval = 0;
    uint16_t                vbus_adc = Analog_Get(ID_VBUS);
    Analog_LastMeasuredCurrent(&i, &index); 
    switch(index)
    {
        case CRT_PHASE_A:   phase = (int16_t)((drive_data.a.W16 << 1) - drive_data.b.W16 - drive_data.c.W16);
                            a32.W32 = (int32_t)phase * (int32_t)vbus_adc * EMF_K1 - (int32_t)i * (EMF_K2 + EMF_K3) + (int32_t)pia_adc * EMF_K3;
                            pia_adc = i;
                            break;
        case CRT_PHASE_B:   phase = (int16_t)((drive_data.b.W16 << 1) - drive_data.a.W16 - drive_data.c.W16);
                            b32.W32 = (int32_t)phase * (int32_t)vbus_adc * EMF_K1 - (int32_t)i * (EMF_K2 + EMF_K3) + (int32_t)pib_adc * EMF_K3;
                            pib_adc = i;
                            break;
        case CRT_PHASE_C:   phase = (int16_t)((drive_data.c.W16 << 1) - drive_data.a.W16 - drive_data.b.W16);
                            c32.W32 = (int32_t)phase * (int32_t)vbus_adc * EMF_K1 - (int32_t)i * (EMF_K2 + EMF_K3) + (int32_t)pic_adc * EMF_K3;
                            pic_adc = i;
                            break;
        default:            break;
    }    
    if(!(a32.H8 & 0x80))  retval |=  0x1;
    if(!(b32.H8 & 0x80))  retval |=  0x2;
    if(!(c32.H8 & 0x80))  retval |=  0x4;
    return retval;
}

static inline uint16_t __Rotor_Get(void)
{
    static uint16_t rotor_angle = 0;
    static uint8_t prev_pos = 0;
    static split_i16_t phase_error;
    uint8_t actual_pos;
    if(HALL_ENABLED) actual_pos = HALL_READ;
    else             actual_pos = __Emf_Rebuild();   
    if(actual_pos != prev_pos)
    {
        uint16_t feedback_angle = pos_to_angle_table[SENSOR_TO_POS_TABLE[actual_pos]] + SENSOR_OFFSET;
        phase_error.W16 = (int16_t)(feedback_angle - rotor_angle);
        prev_pos = actual_pos;
        sensor_cnt ++;
    }
    int16_t correction_angle = phase_error.H8;
    rotor_angle += stator_speed + (int16_t)correction_angle;
    phase_error.W16 -= correction_angle;
    return rotor_angle;
}

static inline void __Drive_Synchronization(void)
{
    uint16_t rotor_angle;
    rotor_angle = __Rotor_Get();
    if(drive_forced) (void)rotor_angle;
    else
    {
        int16_t deltaValue = (int16_t)(rotor_angle + CONVERT_DEGREES_TO_U16(MOTOR_PHASE_ADVANCE) - stator_angles.a.W16);
        split_u32_t tmp;
        tmp.W32 = integrator + (int32_t)(deltaValue << 2);
        tmp.W32 = (uint32_t)SATURATE(tmp.W32, MIN_INC * 65536L, MAX_INC * 65536L);
        integrator = tmp.W32;
        tmp.W32 = integrator + (int32_t)(deltaValue << 4);
        tmp.W32 = (uint32_t)SATURATE(tmp.W32, MIN_INC * 65536L, MAX_INC * 65536L);
        stator_speed = tmp.H16;
    }
}

static inline bool __Rollover_Event_Crt(void)
{
    static uint8_t prev_angle = 0;
    bool state = false;
    if(stator_angles.a.H8 < prev_angle) state = true;
    else                                state = false;
    prev_angle = stator_angles.a.H8;
    return state;
}

static inline bool __Rollover_Event_Stall(void)
{
    static uint8_t prev_angle = 0;
    bool state = false;
    if(stator_angles.a.H8 < prev_angle) state = true;
    else                                state = false;
    prev_angle = stator_angles.a.H8;
    return state;
}

static inline void __Stall_Detection(void)
{
    static uint8_t stall_cnt = 0;
    if((STALL_DETECTION_ENABLED) && (command_enabled))
    {
            if(__Rollover_Event_Stall())
            {
                if(sensor_cnt < 4)     stall_cnt ++;
                else if(stall_cnt > 0) stall_cnt --;
                sensor_cnt = 0;
            }
            if(stator_speed > ERPM_TO_INC(STALL_MAXIMUM_ERPM))
                __Motor_StallHandler(STALL_TOO_FAST);
            if((stall_cnt > STALL_DETECTION_THRESHOLD) || (stator_speed == MIN_INC) || (actual_amplitude == 0))
            {
                __Motor_StallHandler(STALL_TOO_LATE);
                stall_cnt = 0;
            }
    }   
}

static void  __Drive_Updater(void)
{
    static bool alternate = false;
    Analog_Handler(__Rollover_Event_Crt());
    if(status_running)
    {
        if(alternate)
        {
            add_u16k_n3((uint16_t *)&stator_angles, stator_speed);
            drive_data.a.W16  = saddle_lookup_table[stator_angles.a.H8];
            drive_data.b.W16  = saddle_lookup_table[stator_angles.b.H8];
            drive_data.c.W16  = saddle_lookup_table[stator_angles.c.H8];
            PWM_TIMER_DCY_SET(drive_data, actual_amplitude);
            __Stall_Detection();
            alternate = false;
        }
        else
        {
            __Drive_Synchronization();
            alternate = true;
        }
    }
}

static inline void __Align(void)
{
    status_running = true;
    stator_speed = 0;
    PRECISE_DELAY_MS(MOTOR_ALIGNMENT_DURATION);
    status_running = false;
}

static inline void __Current_Trip_EdgeSense(bool edge)
{
    if(edge == true)
        FAULT_CONTROL = COMP_EDGE_RISING;
    else
        FAULT_CONTROL = COMP_EDGE_FALLING;  
}

static inline void __Current_Trip_Disable(void)
{
    FAULT_CONTROL = FAULT_DISABLE;
}

static void __Current_Trip_Handler(void)
{
    if(status_running == false)
        return;  /* motor already off, nothing to do */
    __ShutOff();
    status_events |= MOTOR_EVENT_FAULT;
}

/****************************/
/***** public functions *****/
/****************************/
void Motor_Initialize(void)
{
    __ShutOff();
    __Current_Trip_Disable();
    __Current_Trip_EdgeSense(FAULT_COMPARATOR_EDGE);
    /* this setting has no effect in case of external-comparator setup: */
    CURRENT_TRIP_LEVEL_SET(CURRENT_TO_ACREF(COMPARATOR_CURRENT_TRIP));
    status_events = 0;
    PWM_TIMER_CB_REGISTER(__Drive_Updater);
}


motor_status_t Motor_StatusGet(void)
{
    motor_status_t  retval = status_events;
    status_events = 0;
    if(status_running == true)
        retval |= MOTOR_RUNNING;
    else
        retval |= MOTOR_IDLE;
    return retval;
}


uint32_t Motor_ErpmGet(void)
{
    uint32_t retval;
    ATOMIC_COPY(retval, stator_speed);
    return retval;
}


void Motor_CommandSet(uint16_t ref)
{
    if(command_enabled)
    {
        if(REGULATOR_SPEED_EN == false) ATOMIC_COPY(target_amplitude, ref);
        else                            ATOMIC_COPY(target_speed, ref);
    }
}


uint16_t  Motor_AmplitudeGet(void)
{
    uint16_t retval;
    ATOMIC_COPY(retval, actual_amplitude);
    return retval;
}

void Motor_Start(motor_dir_t dir)
{
    if(status_running == true)
        return;  /* already running */
    __ShutOff();
    drive_forced = true;
    uint16_t vbus_adc = Analog_Get(ID_VBUS);
    if(vbus_adc < VOLTAGE_TO_ADC(MOTOR_STARTUP_CURRENT * (MOTOR_RPP + 2*BOARD_MOSFET_RDSON + CURRENT_SHUNT_RESISTANCE)))
        status_events |= MOTOR_EVENT_UV;
    else
    { 
        uint16_t start_amp = (uint16_t)(MOTOR_STARTUP_CURRENT * (float)PWM_PER * (MOTOR_RPP + 2*BOARD_MOSFET_RDSON + CURRENT_SHUNT_RESISTANCE) / ADC_TO_VOLTAGE(vbus_adc));
        ATOMIC_COPY(target_amplitude, start_amp);
        sensor_cnt = 0;
        direction = dir;
        if(direction == DIR_CW)
        {
            /* CW */
            stator_angles.a.W16 = CONVERT_DEGREES_TO_U16(000.0);
            stator_angles.b.W16 = CONVERT_DEGREES_TO_U16(240.0);
            stator_angles.c.W16 = CONVERT_DEGREES_TO_U16(120.0);
        }
        else
        {
            /* CCW */
            stator_angles.a.W16 = CONVERT_DEGREES_TO_U16(000.0);
            stator_angles.b.W16 = CONVERT_DEGREES_TO_U16(120.0);
            stator_angles.c.W16 = CONVERT_DEGREES_TO_U16(240.0);
        }
        PWM_ON();
        __Motor_StallHandler(STALL_NONE); /* clear any previous stall flag */
        if(HALL_ENABLED) {if(HALL_INVERTED) HALL_INVERT();}
        __Align();
        status_running = true;
        uint16_t speed = (uint16_t)MOTOR_RAMP_START_SPEED;
        uint16_t amp = 0;
        while(speed < (uint16_t)MOTOR_RAMP_END_SPEED)
        {
            
            PRECISE_DELAY_MS(1);
            uint16_t sp = ERPM_TO_INC(speed);
            ATOMIC_COPY(stator_speed, sp);
            speed += MOTOR_RAMP_STEP_SPEED;
            if(amp < start_amp)
                amp ++;
            ATOMIC_COPY(actual_amplitude, amp);
        }
        ATOMIC_COPY(actual_amplitude, start_amp);
        ATOMIC_COPY(stator_speed, ERPM_TO_INC(MOTOR_RAMP_END_SPEED));
        ATOMIC_COPY(integrator, stator_speed * 65536L);
        drive_forced = DRIVE_FORCED;
        if(FAULT_COMPARATOR_EN == true)
        {
            CURRENT_TRIP_SetInterruptHandler(__Current_Trip_Handler);
        }
        if(MOTOR_STARTUP_TIME != -1)
        {
            if(REGULATOR_SPEED_EN == true)
            {
                __Speed_Control(true); /* initialize speed regulator */
            }
            PRECISE_DELAY_MS(MOTOR_STARTUP_TIME);
            command_enabled = true;
        }
    }
}


void  Motor_Fault(motor_fault_t event)
{
    if(status_running == false)
        return;  /* motor already off, nothing to do */

    __ShutOff();
    if(FAULT_COMPARATOR_EN == true)
    {
        CURRENT_TRIP_SetInterruptHandler(NULL);
    }
    if(event == MOTOR_FAULT_OVERCURRENT)
        status_events |= MOTOR_EVENT_OC;
    if(event == MOTOR_FAULT_OVERVOLTAGE)
        status_events |= MOTOR_EVENT_OV;
}


void Motor_Stop(void)
{
    __ShutOff();
    if(FAULT_COMPARATOR_EN == true)
    {
        CURRENT_TRIP_SetInterruptHandler(NULL);
    }
}


uint16_t Motor_MaxAmpGet(void)
{
    return MAX_AMP_GET;
}


/* this function has to be called once every 1 ms, on interrupt context */
void  Motor_TimeTick(void)
{
    if(status_running == true)
    {
        if(command_enabled)
        {
            if(REGULATOR_SPEED_EN)
                __Speed_Control(false);
            else
                __OpenLoop_Control();
        }
    }
}
