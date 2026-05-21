/**
 * TCA1 Generated Driver API Header File
 * 
 * @file tca1.h
 * 
 * @ingroup tca1_normal
 * 
 * @brief This file contains the deprecated macros or APIs for the TCA1 driver.
 *
 * @version TCA1 Driver Version 3.0.0
 *
 * @version Package Version 7.1.0
*/
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

#ifndef TCA1_DEPRECATED_H
#define TCA1_DEPRECATED_H

#warning "The tca1_deprecated.h file contains the deprecated macros or functions. Replace the deprecated macro or functions with the recommended alternative."

/**
 * @misradeviation{@advisory,2.5}
 * MPLABÂ® Code Configurator (MCC) Melody drivers provide macros that can be added to an application.
 * It depends on the application whether a macro is used or not. 
 */
 
/**
 * @ingroup tca1_normal
 * @brief Defines the Custom Name for the \ref TCA1_CounterGet API. 
 *        The TCA1_Read will be deprecated in the future release. Use TCA1_CounterGet instead.
 */
/* cppcheck-suppress misra-c2012-2.5 */  
#define TCA1_Read TCA1_CounterGet
/**
 * @ingroup tca1_normal
 * @brief Defines the Custom Name for the \ref TCA1_PeriodSet API. 
 *        The TCA1_Write will be deprecated in the future release. Use TCA1_PeriodSet instead.
 */
/* cppcheck-suppress misra-c2012-2.5 */  
#define TCA1_Write TCA1_PeriodSet
/**
 * @ingroup tca1_normal
 * @brief Defines the Custom Name for the \ref TCA1_OverflowStatusClear API. 
 *        The TCA1_ClearOverflowInterruptFlag will be deprecated in the future release. Use TCA1_OverflowStatusClear instead.
 */
/* cppcheck-suppress misra-c2012-2.5 */  
#define TCA1_ClearOverflowInterruptFlag TCA1_OverflowStatusClear
/**
 * @ingroup tca1_normal
 * @brief Defines the Custom Name for the \ref TCA1_OverflowStatusGet API. 
 *        The TCA1_IsOverflowInterruptFlagSet will be deprecated in the future release. Use TCA1_OverflowStatusGet instead.
 */
/* cppcheck-suppress misra-c2012-2.5 */  
#define TCA1_IsOverflowInterruptFlagSet TCA1_OverflowStatusGet
/**
 * @ingroup tca1_normal
 * @brief Defines the Custom Name for the \ref TCA1_CMP0MatchStatusClear API. 
 *        The TCA1_ClearCMP0InterruptFlag will be deprecated in the future release. Use TCA1_CMP0MatchStatusClear instead.
 */
/* cppcheck-suppress misra-c2012-2.5 */  
#define TCA1_ClearCMP0InterruptFlag TCA1_CMP0MatchStatusClear
/**
 * @ingroup tca1_normal
 * @brief Defines the Custom Name for the \ref TCA1_CMP0MatchStatusGet API. 
 *        The TCA1_IsCMP0InterruptFlagSet will be deprecated in the future release. Use TCA1_CMP0MatchStatusGet instead.
 */
/* cppcheck-suppress misra-c2012-2.5 */  
#define TCA1_IsCMP0InterruptFlagSet TCA1_CMP0MatchStatusGet
/**
 * @ingroup tca1_normal
 * @brief Defines the Custom Name for the \ref TCA1_CMP1MatchStatusClear API. 
 *        The TCA1_ClearCMP1InterruptFlag will be deprecated in the future release. Use TCA1_CMP1MatchStatusClear instead.
 */
/* cppcheck-suppress misra-c2012-2.5 */  
#define TCA1_ClearCMP1InterruptFlag TCA1_CMP1MatchStatusClear
/**
 * @ingroup tca1_normal
 * @brief Defines the Custom Name for the \ref TCA1_CMP1MatchStatusGet API. 
 *        The TCA1_IsCMP1InterruptFlagSet will be deprecated in the future release. Use TCA1_CMP1MatchStatusGet instead.
 */
/* cppcheck-suppress misra-c2012-2.5 */  
#define TCA1_IsCMP1InterruptFlagSet TCA1_CMP1MatchStatusGet
/**
 * @ingroup tca1_normal
 * @brief Defines the Custom Name for the \ref TCA1_CMP2MatchStatusClear API. 
 *        The TCA1_ClearCMP2InterruptFlag will be deprecated in the future release. Use TCA1_CMP2MatchStatusClear instead.
 */
/* cppcheck-suppress misra-c2012-2.5 */  
#define TCA1_ClearCMP2InterruptFlag TCA1_CMP2MatchStatusClear
/**
 * @ingroup tca1_normal
 * @brief Defines the Custom Name for the \ref TCA1_CMP2MatchStatusGet API. 
 *        The TCA1_IsCMP2InterruptFlagSet will be deprecated in the future release. Use TCA1_CMP2MatchStatusGet instead.
 */
/* cppcheck-suppress misra-c2012-2.5 */ 
#define TCA1_IsCMP2InterruptFlagSet TCA1_CMP2MatchStatusGet
/**
 * @ingroup tca1_normal
 * @brief Defines the Custom Name for the \ref TCA1_FrequencySet API. 
 *        The TCA1_WaveformFreqRegCountSet will be deprecated in the future release. Use TCA1_FrequencySet instead.
 */
/* cppcheck-suppress misra-c2012-2.5 */  
#define TCA1_WaveformFreqRegCountSet TCA1_FrequencySet
/**
 * @ingroup tca1_normal
 * @brief Defines the Custom Name for the \ref TCA1_FrequencyGet API. 
 *        The TCA1_WaveformFreqRegCountGet will be deprecated in the future release. Use TCA1_FrequencyGet instead.
 */
/* cppcheck-suppress misra-c2012-2.5 */  
#define TCA1_WaveformFreqRegCountGet TCA1_FrequencyGet
/**
 * @ingroup tca1_normal
 * @brief Defines the Custom Name for the \ref TCA1_WO1OffsetSet API. 
 *        The TCA1_WO1OffsetRegCountSet will be deprecated in the future release. Use TCA1_WO1OffsetSet instead.
 */
/* cppcheck-suppress misra-c2012-2.5 */  
#define TCA1_WO1OffsetRegCountSet TCA1_WO1OffsetSet
/**
 * @ingroup tca1_normal
 * @brief Defines the Custom Name for the \ref TCA1_WO1OffsetGet API. 
 *        The TCA1_WO1OffsetRegCountGet will be deprecated in the future release. Use TCA1_WO1OffsetGet instead.
 */
/* cppcheck-suppress misra-c2012-2.5 */  
#define TCA1_WO1OffsetRegCountGet TCA1_WO1OffsetGet
/**
 * @ingroup tca1_normal
 * @brief Defines the Custom Name for the \ref TCA1_WO2OffsetSet API. 
 *        The TCA1_WO2OffsetRegCountSet will be deprecated in the future release. Use TCA1_WO2OffsetSet instead.
 */
/* cppcheck-suppress misra-c2012-2.5 */  
#define TCA1_WO2OffsetRegCountSet TCA1_WO2OffsetSet
/**
 * @ingroup tca1_normal
 * @brief Defines the Custom Name for the \ref TCA1_WO2OffsetGet API. 
 *        The TCA1_WO2OffsetRegCountGet will be deprecated in the future release. Use TCA1_WO2OffsetGet instead.
 */
/* cppcheck-suppress misra-c2012-2.5 */  
#define TCA1_WO2OffsetRegCountGet TCA1_WO2OffsetGet
/**
 * @ingroup tca1_normal
 * @brief Defines the Custom Name for the \ref TCA1_CounterGet API. 
 *        The TCA1CounterGet will be deprecated in the future release. Use TCA1_CounterGet instead.
 */
/* cppcheck-suppress misra-c2012-2.5 */  
#define TCA1CounterGet TCA1_CounterGet
/**
 * @ingroup tca1_normal
 * @brief Defines the Custom Name for the \ref TCA1_CounterSet API. 
 *        The TCA1CounterSet will be deprecated in the future release. Use TCA1_CounterSet instead.
 */
/* cppcheck-suppress misra-c2012-2.5 */  
#define TCA1CounterSet TCA1_CounterSet

#endif //TCA1_DEPRECATED_H