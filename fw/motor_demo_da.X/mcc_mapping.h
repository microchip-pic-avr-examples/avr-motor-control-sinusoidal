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
#include "mcc_generated_files/reset/rstctrl.h"

#define __CPU_STRING__                      "AVR128DA48"
#define DEBUG_BUFF_SIZE                     12000        /* This has to be an even number */

/* external/internal comparator - fault trip */
#define FAULT_CONTROL                       AC0.INTCTRL
#define FAULT_DISABLE                       0
#define CURRENT_TRIP_SetInterruptHandler    AC0_CallbackRegister                    
#define COMP_EDGE_RISING                    (AC_INTMODE_NORMAL_POSEDGE_gc | AC_CMP_bm)
#define COMP_EDGE_FALLING                   (AC_INTMODE_NORMAL_NEGEDGE_gc | AC_CMP_bm)
#define FAULT_COMPARATOR_EDGE               true /* true - on rising edge  | false - on falling edge */
#define CURRENT_TRIP_LEVEL_SET              AC0_DACRefValueSet
#define AC_REF_DAC_MAX                      255
#define ADC_MAX                             4095

#define PWM_PER                             600 // 50us
#define PWM_DT                              3   // 250ns

/* timer used to generate PWM and sample comparator */
extern void TCA_sync_start(void);
#define PWM_TIMER_SYNC()                    do{TCA0_Stop(); TCA1_Stop(); TCA0_CounterSet(6); TCA1_CounterSet(4*PWM_DT); TCA_sync_start(); sei();}while(0)
#define PWM_TIMER_DCY_SET(X, Y)             do{\
                                            fmul_16x16k_n3((uint16_t*)&(X), (Y));\
                                            TCA0.SINGLE.CMP0BUF = (X.a.W16);           TCA0.SINGLE.CMP1BUF = (X.b.W16);           TCA0.SINGLE.CMP2BUF = (X.c.W16);\
                                            TCA1.SINGLE.CMP0BUF = (X.a.W16)+ 2*PWM_DT; TCA1.SINGLE.CMP1BUF = (X.b.W16)+ 2*PWM_DT; TCA1.SINGLE.CMP2BUF = (X.c.W16)+ 2*PWM_DT;\
                                            }while(0)
#define PWM_OFF()                           do{\
                                            PORTB.PIN0CTRL = 0x0; PORTB.PIN1CTRL = 0x0; PORTB.PIN2CTRL = 0x0;\
                                            TCA1.SINGLE.CTRLB &= ~(TCA_SINGLE_CMP0EN_bm | TCA_SINGLE_CMP1EN_bm | TCA_SINGLE_CMP2EN_bm);\
                                            TCA0.SINGLE.CTRLB &= ~(TCA_SINGLE_CMP0EN_bm | TCA_SINGLE_CMP1EN_bm | TCA_SINGLE_CMP2EN_bm);\
                                            }while(0)
#define PWM_ON()                            do{\
                                            PORTB.PIN0CTRL = 0x80; PORTB.PIN1CTRL = 0x80; PORTB.PIN2CTRL = 0x80;\
                                            TCA1.SINGLE.CTRLB |= (TCA_SINGLE_CMP0EN_bm | TCA_SINGLE_CMP1EN_bm | TCA_SINGLE_CMP2EN_bm);\
                                            TCA0.SINGLE.CTRLB |= (TCA_SINGLE_CMP0EN_bm | TCA_SINGLE_CMP1EN_bm | TCA_SINGLE_CMP2EN_bm);\
                                            }while(0)
#define PWM_TIMER_DIR                       (TCA0.SINGLE.CTRLESET & TCA_SINGLE_DIR_bm)
#define PWM_TIMER_CB_REGISTER               TCA0_OverflowCallbackRegister

/* periodic 1ms software timer */
#define SW_TIMER_CB_REGISTER                RTC_SetPITIsrCallback
#define SW_TIMER_PERIOD                     (970) /* us */

/* ADC pins definitions */
#define CRT_A_ADC_PIN                       ADC_MUXPOS_AIN10_gc // PE2
#define CRT_B_ADC_PIN                       ADC_MUXPOS_AIN9_gc  // PE1
#define CRT_N_ADC_PIN                       ADC_MUXPOS_AIN8_gc  // PE0
#define VBUS_ADC_PIN                        ADC_MUXPOS_AIN0_gc  // PD0
#define POT_ADC_PIN                         ADC_MUXPOS_AIN1_gc  // PD1
#define TEMP_ADC_PIN               			ADC_MUXPOS_AIN5_gc  // PD5

/* ADC APIs */
#define ADC_MUX_SET                         ADC0_ChannelSelect
#define ADC_RESULT_GET                      ADC0_ConversionResultGet
#define ADC_MUX_GET                         ADC0.MUXPOS

#define System_Reset_Command                RSTCTRL_SoftwareReset

/* Button polarity */
#define BUTTON_ACTIVE                       0

/* PWM Input capture */
#define PWM_IN_PIN_REGISTER                 PWM_IN_SetInterruptHandler
#define PWM_IN_PIN_READ                     PWM_IN_GetValue
#define PWM_IN_TMR_REGISTER                 TCB2_OverflowCallbackRegister
#define PWM_IN_TMR_CLEAR()                  TCB2_CounterSet(0)
#define PWM_IN_TMR_READ                     TCB2_CounterGet

/* HALL Sensored Functions */

#define HALL_PORT                           PORTF
#define HALL_A_PIN                          PIN2CTRL
#define HALL_B_PIN                          PIN3CTRL
#define HALL_C_PIN                          PIN4CTRL
#define HALL_READ                           ((PORTF.IN & (PIN2_bm | PIN3_bm | PIN4_bm)) >> 2)                          
#define HALL_INVERT()                       do{\
                                            HALL_PORT.HALL_A_PIN = PORT_INVEN_bm;\
                                            HALL_PORT.HALL_B_PIN = PORT_INVEN_bm;\
                                            HALL_PORT.HALL_C_PIN = PORT_INVEN_bm;\
                                            }while(0)


#endif /* MCC_MAPPING_H */
