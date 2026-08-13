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

#ifndef CONFIG_H
#define	CONFIG_H

/* PWM-In settings */
#define PWM_IN_PERIOD                   (20.0)       /* ms */
#define PWM_IN_MAX_DCY                  ( 2.0)       /* ms */
#define PWM_IN_MIN_DCY                  ( 1.0)       /* ms */

/* Board specific settings - MPPB */
#define CURRENT_SHUNT_RESISTANCE        (0.01)       /* Resistance in ohm of the current shunt */
#define CURRENT_AMPLIFIER_GAIN          (15.0)
#define VBUS_DIVIDER                    (16.0)
#define ANALOG_REFERENCE                (3300.0)     /* mV */
#define ADC_VBUS_TRIP                   (51000.0)    /* mV - VBUS trip level */
#define ADC_CURRENT_TRIP                (15000.0)    /* mA - ADC current trip level */
#define COMPARATOR_CURRENT_TRIP         (22000.0)    /* mA - Comparator trip level - not used on external-comparator configurations */
#define FAULT_COMPARATOR_EN             true
#define BOARD_MOSFET_RDSON              (0.004)      /* ohm - MOSFETs Rds-on resistance */

/* Motor-specific settings */
/* ACT57BLF02:  supply 24V, 3A limit */
#define MOTOR_PHASE_ADVANCE             (12.0)       /* Phase advance 0.0 ... 30.0 electrical degrees */
#define MOTOR_STARTUP_CURRENT           (13000.0)    /* mA - Initial alignment current pulse may be higher than overcurrent protection */
#define MOTOR_RPP                       (0.45)       /* ohm - Motor resistance measured phase-to-phase */
#define MOTOR_LPP                       (0.3)        /* mH - Motor inductance measured phase-to-phase */
#define MOTOR_ALIGNMENT_DURATION        (100)        /* ms */
#define MOTOR_AMPRAMP_STEP_INTERVAL     (3)          /* Amplitude ramp step interval: expressed in milliseconds, representing the time passed between two amplitude steps - 1 for aggresive ramp, 10 for soft ramp */  
#define MOTOR_STARTUP_TIME              (100)        /* ms - Delay until commands (POT or PWM-in) are accepted; a value of '-1' causes commands to be ignored */
#define MOTOR_RAMP_START_SPEED          (500.0)      /* e-RPM - Specifies the initial speed - minimum value is 100 e-RPM*/
#define MOTOR_RAMP_END_SPEED            (3600.0)     /* e-RPM */
#define MOTOR_RAMP_STEP_SPEED           (3)          /* e-RPM */
#define HALL_ENABLED                    false        /* Setting to 'true' enables the Hall synchronization, while setting to 'false' enables the BEMF synchronization */
#define HALL_MISALIGNMENT_CW            (8.0)        /* Hall misalignment 0.0 ... 90.0 electrical degrees */
#define HALL_MISALIGNMENT_CCW           (10.0)       /* Hall misalignment 0.0 ... 90.0 electrical degrees */

/* Speed regulation specific settings */
#define REGULATOR_SPEED_EN              true         /* Setting to 'true' enables speed control in Closed Loop, while setting to 'false' enables amplitude control in Open Loop */
#define REGULATOR_MAX_SPEED             (10000.0)    /* e-RPM target speed for 100% command */
#define REGULATOR_MIN_SPEED             (4000.0)     /* e-RPM target speed for   0% command */
#define REGULATOR_PI_KI                 (2.0)        /* PI speed control loop integral coefficient, minimum 1.0 */
#define REGULATOR_PI_KP                 (4.0)        /* PI speed control loop proportional coefficient, minimum 1.0 */
#define REGULATOR_PI_DT                 (10)         /* PI time step size [ms] - minimum 10 ms */

/* Drive Algorithm settings */
#define DRIVE_FORCED                    false        /* Setting to 'true' runs the motor without BEMF sensing, only blind drive and no rotor synchronization, which could overheat the motor */
#define STALL_DETECTION_THRESHOLD       (50)         /* Stall detection tolerance: a higher number leads to more tolerance to perturbances, but slower detection */
#define STALL_DETECTION_ENABLED         true         /* Setting to 'false' disables the stall detection mechanism */
#define STALL_MAXIMUM_ERPM              (30000.0)    /* e-RPM - Threshold when the algorithm might lose synchronization */


#endif	/* CONFIG_H */
