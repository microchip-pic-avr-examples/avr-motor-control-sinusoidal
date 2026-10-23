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

#ifndef CONVERSION_MACROS_H
#define	CONVERSION_MACROS_H

#include <stdint.h>
#include <util/atomic.h>
#include <math.h>
#include "analog.h"
#include "mcc_mapping.h"
#include "config.h"

#define ADC_MAX_SCALE                     (float)ADC_MAX
#define DAC_MAX_SCALE                     AC_REF_DAC_MAX
#define CURRENT_AMPLIFIER_OFFSET          (ANALOG_REFERENCE / 2)
#define PWM_FREQUENCY                     (F_CPU / (2.0 * (float)PWM_PER)) // Hz - 50us
#define SAMPLING_FREQUENCY                (PWM_FREQUENCY / 2.0)            // Hz - PWM_FREQUENCY divide by 2

/* potentiometer conversion formulas to/from percents */
#define ADC_TO_PERCENT(X)                 (((float)(X) + 0.5) * 100.0 / (ADC_MAX_SCALE + 1))
#define PERCENT_TO_ADC(X)                 (uint16_t)((float)(X) * (ADC_MAX_SCALE + 1) / 100.0 + 0.5)

/* voltage conversion formulas to/from millivolts */
#define ADC_TO_VOLTAGE(X)                 (((float)(X) + 0.5) * VBUS_DIVIDER * ANALOG_REFERENCE / (ADC_MAX_SCALE + 1))
#define VOLTAGE_TO_ADC(X)                 (uint16_t)((float)(X) * (ADC_MAX_SCALE + 1) / (VBUS_DIVIDER * ANALOG_REFERENCE) + 0.5)

/* temperature to Celsius degrees conversion formulas */
#define ADC_TO_CELSIUS(X)                 ((float)((X) * (ANALOG_REFERENCE / (ADC_MAX_SCALE + 1)) - TEMP_SENSOR_K1) / TEMP_SENSOR_K2)
#define CELSIUS_TO_ADC(X)                 (uint16_t)(((float)(X) * TEMP_SENSOR_K2 + TEMP_SENSOR_K1) * (ADC_MAX_SCALE + 1) / ANALOG_REFERENCE + 0.5)

/* current conversion formulas to/from milliamperes */
#define ADC_TO_CURRENT(ADC, N)            ((((float)((ADC) / (N)) + 0.5) * ANALOG_REFERENCE / (ADC_MAX_SCALE + 1)) / (CURRENT_AMPLIFIER_GAIN * CURRENT_SHUNT_RESISTANCE))
#define CURRENT_TO_ADC(MA)                (uint16_t)( ((float)(MA) * CURRENT_AMPLIFIER_GAIN * CURRENT_SHUNT_RESISTANCE) * (ADC_MAX_SCALE + 1) / ANALOG_REFERENCE + 0.5)
#define CURRENT_CONSTANT                  (M_PI / (2.0 * M_SQRT2))

/* current conversion formulas to/from milliamperes - reference DAC */
#define ACREF_TO_CURRENT(X)               ( ((float)(X) * ANALOG_REFERENCE / DAC_MAX_SCALE - CURRENT_AMPLIFIER_OFFSET) / (CURRENT_AMPLIFIER_GAIN * CURRENT_SHUNT_RESISTANCE) )
#define CURRENT_TO_ACREF(X)               (uint8_t)( ((float)(X) * CURRENT_AMPLIFIER_GAIN * CURRENT_SHUNT_RESISTANCE + CURRENT_AMPLIFIER_OFFSET) * DAC_MAX_SCALE / ANALOG_REFERENCE + 0.5)

/* conversion macros from/into microseconds into/from clocks count */
#define CONVERT_US_TO_CLKS(US)            (uint16_t)((US) * (F_CPU) / 1000000.0 + 0.5)
#define CONVERT_CLKS_TO_US(CLKS)          ((float)(CLKS) * 1000000.0 / (F_CPU))

/* Mapping from 0 - 359.99 electrical degrees -> 0 - 65535 */
#define CONVERT_DEGREES_TO_U16(DEG)       (uint16_t)((float)(DEG) * 65536.0 / 360.0 + 0.5)
#define CONVERT_U16_TO_DEGREES(U16)       ((float)(U16) * 360.0 / 65536.0)

/* these macros must not be called with variable arguments within interrupt context */
#define ERPM_TO_INC(RPM)                  (uint16_t)(((float)(RPM) * 65536.0) / (SAMPLING_FREQUENCY * 60.0) + 0.5)
#define INC_TO_ERPM(INC)                  (float)((SAMPLING_FREQUENCY) * (float)(INC) * 60.0 / 65536.0)

/* these macros must not be called with variable arguments within interrupt context */
#define HZ_TO_INC(HZ)                     (uint16_t)(((float)(HZ) * 65536.0) / SAMPLING_FREQUENCY + 0.5)
#define INC_TO_HZ(INC)                    (float)(SAMPLING_FREQUENCY) * (float)(INC) / 65536.0

typedef __int24  int24_t;
typedef __uint24 uint24_t;

typedef union
{
    struct
    {
        uint8_t L8;
        uint8_t H8;
    };
    uint16_t W16;
} split_u16_t;

typedef union
{
    struct
    {
        uint8_t L8;
        int8_t  H8;
    };
    int16_t W16;
} split_i16_t;

typedef union
{
    struct
    {
        uint8_t L8;
        uint8_t M8;
        uint8_t H8;
    };
    struct
    {
        uint8_t dummy0;
        uint16_t H16;
    };
    struct
    {
        uint16_t L16;
        uint8_t dummy1;
    };
    uint24_t W24;
} split_u24_t;


typedef union
{
    struct
    {
        uint8_t L8;
        int8_t M8;
        int8_t H8;
    };
    struct
    {
        uint8_t dummy0;
        int16_t H16;
    };
    struct
    {
        uint16_t L16;
        uint8_t dummy1;
    };
    int24_t W24;
} split_i24_t;

typedef union
{
	struct
	{
		uint16_t L16;
		uint16_t H16;
	};
    struct
	{
		uint8_t  L8;
		uint24_t H24;
	};
    struct
	{
		uint24_t L24;
		uint8_t  H8;
	};
	uint32_t W32;
} split_u32_t;

typedef union
{
	struct
	{
		uint16_t L16;
		 int16_t H16;
	};
    struct
	{
		uint8_t  L8;
		int24_t  H24;
	};
    struct
	{
		uint24_t L24;
		 int8_t  H8;
	};
	int32_t W32;
} split_i32_t;

typedef struct
{
    split_u16_t a;
    split_u16_t b;
    split_u16_t c;
} triple16_t;

typedef uint16_t fixp16_t;  /* U1Q15 0...32768 / 0.0...1.0000 */

#define FLOAT_TO_FIXP16(FLOAT)             (fixp16_t)((FLOAT)*32768.0 + 0.5)
#define FIXP16_TO_FLOAT(FIXP)              ((float)(FIXP)/32768.0)

#define MAXIMUM(A,B)                       (((A)>(B))?(A):(B))
#define MINIMUM(A,B)                       (((A)<(B))?(A):(B))
#define SATURATE(X, MIN, MAX)              (MINIMUM(MAX,MAXIMUM(MIN,X)))

/* 
 * {return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;}
 */
#define SCALE_ZERO(VAL,         IN_MAX,          OUT_MAX)     ((uint32_t)(VAL) * (uint32_t)(OUT_MAX) / (uint32_t)(IN_MAX))
#define SCALE_FULL(VAL, IN_MIN, IN_MAX, OUT_MIN, OUT_MAX)    (((uint32_t)(VAL) - (uint32_t)(IN_MIN)) * ((uint32_t)(OUT_MAX) - (uint32_t)(OUT_MIN)) / ((uint32_t)(IN_MAX) - (uint32_t)(IN_MIN)) + (uint32_t)OUT_MIN)


#define ATOMIC_COPY(DST_VAR, SRC_VAR)      do{ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {(DST_VAR) = (SRC_VAR);}}while(0)


#endif	/* CONVERSION_MACROS_H */


