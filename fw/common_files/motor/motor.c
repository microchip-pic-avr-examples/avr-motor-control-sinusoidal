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
#define MAX_AMP_GET                PWM_PER
#define MIN_AMP_GET                (0)
#define MAX_INC                    CONVERT_DEGREES_TO_U16(45.0)   
#define MIN_INC                    ERPM_TO_INC(100.0)  
#define HALL_TO_POS_TABLE          ((direction == DIR_CW) ? hall_to_pos_table_cw : hall_to_pos_table_ccw)
#define HALL_OFFSET                ((direction == DIR_CW) ? HALL_MISALIGNMENT_CW : HALL_MISALIGNMENT_CCW)

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
static          bool           status_running, command_enabled, hall_enabled;
static volatile motor_status_t status_events;
static volatile fixp16_t       actual_amplitude, target_amplitude;
static motor_dir_t             direction;
static volatile split_u24_t    phase_integrator;
static volatile triple16_t     stator_angles;
static volatile triple16_t     drive_data;

/****************************/
/***** private functions ****/
/****************************/

static void __OpenLoop_Control(void)
{
    if(actual_amplitude > target_amplitude)
    {
        actual_amplitude --;
    }
    else if(actual_amplitude < target_amplitude)
    {
        actual_amplitude ++;
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
    static uint24_t speed_integrator = 0;
    
    if(state == true)
    {
        speed_integrator = ((uint24_t)actual_amplitude << 8);
        target_speed = stator_speed;
    }
    if((REGULATOR_SPEED_EN == true) && (command_enabled))
    {
            int24_t Ki_err, Kp_err, tmp;
            int16_t err;
            tmp = (int24_t)target_speed - (int24_t)stator_speed;
            err = (int16_t)SATURATE(tmp, -32000, 32000);

            Ki_err = Multiply_U16xI16((uint16_t)(REGULATOR_PI_KI), err);
            Kp_err = Multiply_U16xI16((uint16_t)(REGULATOR_PI_KP), err);

            tmp = Ki_err + (int24_t)speed_integrator;
            speed_integrator = (uint24_t)SATURATE(tmp, MIN_AMP_GET * 256L, MAX_AMP_GET * 256L);

            tmp = Kp_err + (int24_t)speed_integrator;
            split_u24_t amp_acc;
            amp_acc.W24 = (uint24_t)SATURATE(tmp, MIN_AMP_GET * 256L, MAX_AMP_GET  * 256L);
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
        actual_amplitude = 0;
        stator_speed = 0;
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
            case STALL_NONE:     if(stall_counter) stall_counter--;  break;
            case STALL_TOO_LATE: stall_counter++; if(stall_counter > STALL_DETECTION_THRESHOLD) {__ShutOff(); status_events |= MOTOR_EVENT_STALL; } break;
            case STALL_TOO_FAST: __ShutOff(); status_events |= MOTOR_EVENT_STALL; break;
        }
    }
}

static inline bool __Rollover_Event(void)
{
    static uint8_t prev_angle = 0;
    bool state = false;
    if(stator_angles.a.H8 < prev_angle) state = true;
    else                                state = false;
    prev_angle = stator_angles.a.H8;
    return state;
}

static void  __Drive_Updater(void)
{
    static uint8_t prev_hall = 0;
    static split_u16_t rotor_angle;
    static uint8_t hall_cnt = 0;
    static uint8_t stall_cnt = 0;
    if(status_running == true)
    {
        add_u16k_n3((uint16_t *)&stator_angles, stator_speed);
        drive_data.a.W16  = saddle_lookup_table[stator_angles.a.H8];
        drive_data.b.W16  = saddle_lookup_table[stator_angles.b.H8];
        drive_data.c.W16  = saddle_lookup_table[stator_angles.c.H8];
        PWM_TIMER_DCY_SET(drive_data, actual_amplitude);
        if(DRIVE_FORCED == false)
        {
            uint8_t hall = HALL_READ;
            if(prev_hall != hall)
            {
                prev_hall = hall;
                rotor_angle.W16 = pos_to_angle_table[HALL_TO_POS_TABLE[hall]];
                hall_cnt ++;
                int16_t deltaValue = (int16_t)((int24_t)rotor_angle.W16 - (int24_t)CONVERT_DEGREES_TO_U16(HALL_OFFSET) - (int24_t)stator_angles.a.W16);
                split_u24_t tmp;
                tmp.W24 = phase_integrator.W24 + (int24_t)(deltaValue >> 2);
                tmp.W24 = (uint24_t)SATURATE(tmp.W24, MIN_INC * 256L, MAX_INC * 256L);
                phase_integrator.W24 = tmp.W24;
                tmp.W24 = phase_integrator.W24 + (int24_t)deltaValue;
                tmp.W24 = (uint24_t)SATURATE(tmp.W24, MIN_INC * 256L, MAX_INC * 256L);
                stator_speed = tmp.H16;
            }
            if(__Rollover_Event())
            {
                if(hall_cnt < 4) {stall_cnt ++; stator_speed >>= 1;}
                else             {if(stall_cnt > 0) stall_cnt --;}
                hall_cnt = 0;
            }
        }
        if((STALL_DETECTION_ENABLED) && (command_enabled))
        {
            if(stator_speed > ERPM_TO_INC(STALL_MAXIMUM_ERPM))
                __Motor_StallHandler(STALL_TOO_FAST);
            if(stall_cnt > STALL_DETECTION_THRESHOLD)
            {
                __Motor_StallHandler(STALL_TOO_LATE);
                stall_cnt = 0;
            }
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


uint16_t Motor_ErpmGet(void)
{
    uint16_t retval;
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


void Motor_Start(uint16_t vbus_adc, motor_dir_t dir)
{
    if(status_running == true)
        return;  /* already running */
//    if(vbus_adc < VOLTAGE_TO_ADC(MOTOR_STARTUP_CURRENT * (MOTOR_RPP + 2*BOARD_MOSFET_RDSON + CURRENT_SHUNT_RESISTANCE)))
//        status_events |= MOTOR_EVENT_UV;
//    else
//    {
        uint16_t start_amp = (uint16_t)(MOTOR_STARTUP_CURRENT * (float)PWM_PER * (MOTOR_RPP + 2*BOARD_MOSFET_RDSON + CURRENT_SHUNT_RESISTANCE) / ADC_TO_VOLTAGE(vbus_adc)); 
        ATOMIC_COPY(target_amplitude, start_amp);
        ATOMIC_COPY(actual_amplitude, start_amp);
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
        __Align();
        stator_speed = ERPM_TO_INC(MOTOR_STARTUP_SPEED);
        phase_integrator.W24 = ERPM_TO_INC(MOTOR_STARTUP_SPEED) * 256L; 
        status_running = true;
        if(HALL_ENABLED) {hall_enabled = true; if(HALL_INVERTED) HALL_INVERT();}
        else              hall_enabled = false;
        if(FAULT_COMPARATOR_EN == true)
        {
            CURRENT_TRIP_SetInterruptHandler(__Current_Trip_Handler);
        }
        if(MOTOR_STARTUP_TIME != -1)
        {
            PRECISE_DELAY_MS(MOTOR_STARTUP_TIME);
            if(REGULATOR_SPEED_EN == true)
            {
                __Speed_Control(true); /* initialize speed regulator */
            }
            command_enabled = true;
        }        
//    }
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
    Analog_Handler();
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
