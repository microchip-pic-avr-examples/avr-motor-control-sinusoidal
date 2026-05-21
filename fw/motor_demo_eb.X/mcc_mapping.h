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

#ifndef MCC_MAPPING_H
#define MCC_MAPPING_H

#include <stdint.h>
#include <stdbool.h>
#include "frac-lib.h"
#include "mcc_generated_files/system/system.h"

#define __CPU_STRING__                  "AVR16EB32"
#define DEBUG_BUFF_SIZE                 100         /* This has to be an even number */

/* external/internal comparator - fault trip */
#define FAULT_CONTROL                       WEX0.FAULTCTRL
#define FAULT_DISABLE                       (WEX_FDDBD_bm | WEX_FDACT_NONE_gc)
#define CURRENT_TRIP_SetInterruptHandler(X) do{WEX0_SoftwareCommand(WEX_CMD_FAULTCLR_gc); WEX0_FaultDetectionAction(WEX_FDACT_LOW_gc); WEX0_FAULTIsrCallbackRegister(X);}while(0)
#define COMP_EDGE_RISING                    (WEX_FDDBD_bm | WEX_FDACT_NONE_gc)
#define COMP_EDGE_FALLING                   (WEX_FDDBD_bm | WEX_FDACT_NONE_gc)
#define FAULT_COMPARATOR_EDGE               true /* true - on rising edge  | false - on falling edge */
#define CURRENT_TRIP_LEVEL_SET              AC1_DACRefValueSet
#define AC_REF_DAC_MAX                      255

#define PWM_PER                             512  // 51.2us
#define PWM_DT                              5    // 250ns

/* timer used to generate PWM and sample comparator */
#define PWM_TIMER_SYNC()                    do{WEX0_DeadTimeBothSidesSet(PWM_DT);}while(0)
#define PWM_TIMER_DCY_SET(X, Y)             do{\
                                            TCE0_AmplitudeSet((Y)<<6);\
                                            TCE0_CompareChannels012BufferedSet(X.a.W16, X.b.W16, X.c.W16);\
                                            }while(0)
#define PWM_OFF()                           do{WEX0_PatternGenerationOverrideSet(0xFF);}while(0)                          
#define PWM_ON()                            do{WEX0_PatternGenerationOverrideSet(0x00);}while(0)
#define PWM_TIMER_CB_REGISTER               TCE0_OverflowCallbackRegister

/* periodic 1ms software timer */
#define SW_TIMER_CB_REGISTER       RTC_SetPITIsrCallback
#define SW_TIMER_PERIOD            (1260) /* us */

/* ADC pins definitions */
#define CRT_P_ADC_PIN           ADC_MUXPOS_AIN3_gc  // PD3
#define CRT_N_ADC_PIN           ADC_MUXPOS_AIN7_gc  // PD7
#define VBUS_ADC_PIN            ADC_MUXPOS_AIN20_gc // PF4
#define POT_ADC_PIN             ADC_MUXPOS_AIN1_gc  // PD1

/* ADC APIs */
#define ADC_MUX_SET             ADC0_ChannelSelect
#define ADC_CONV_START          ADC0_ConversionStart
#define ADC_RESULT_GET          ADC0_ConversionResultGet
#define ADC_RESOLUTION_GET      ADC0_ResolutionGet
#define ADC_NO_CONV_PROGRESS    ADC0_IsConversionDone


static void System_Reset_Command(void)  __attribute__ ((unused));
static void System_Reset_Command(void)  { _PROTECTED_WRITE(RSTCTRL.SWRR, RSTCTRL_SWRE_bm);  while(true);}

/* Button polarity */
#define BUTTON_ACTIVE           0

/* PWM Input capture */
#define PWM_IN_PIN_REGISTER     PWM_IN_SetInterruptHandler
#define PWM_IN_PIN_READ         PWM_IN_GetValue
#define PWM_IN_TMR_REGISTER     TCF0_OverflowCallbackRegister
#define PWM_IN_TMR_CLEAR()      TCF0_CounterSet(0)
#define PWM_IN_TMR_READ         (uint16_t)TCF0_CounterGet

/* HALL Sensored Functions */

#define HALL_PORT                           PORTF
#define HALL_A_PIN                          PIN1CTRL
#define HALL_B_PIN                          PIN2CTRL
#define HALL_C_PIN                          PIN3CTRL
#define HALL_READ                           ((PORTF.IN & (PIN1_bm | PIN2_bm | PIN3_bm)) >> 1)                          
#define HALL_INVERT()                       do{\
                                            HALL_PORT.HALL_A_PIN = PORT_INVEN_bm;\
                                            HALL_PORT.HALL_B_PIN = PORT_INVEN_bm;\
                                            HALL_PORT.HALL_C_PIN = PORT_INVEN_bm;\
                                            }while(0)


#endif /* MCC_MAPPING_H */

