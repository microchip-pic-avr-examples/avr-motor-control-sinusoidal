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

#include <string.h>             /* for memset() */
#include "conversion_macros.h"
#include "mcc_mapping.h"
#include "analog.h"
#include "config.h"

typedef struct
{
    analog_fault_handler_t      handler;
    uint16_t                    threshold;
    uint8_t                     event;
} analog_emergency_t;

/** private data */
static uint8_t                  index,  sequencer_idx;
static phase_currents_t         last_current;
static analog_emergency_t       emergency[ID_MAX];
static split_u24_t              analog_results[ID_MAX];
static bool                     analog_types[ID_MAX];
static uint8_t                  pins_list[ID_MAX];
static split_i16_t              phase_current;
static analog_bus_crt_t         bus_current;
static const analog_id_t        analog_sequencer[] = 
{
    ID_CRT_A, ID_REF, ID_CRT_B, ID_CRT_A, ID_CRT_B, ID_VBUS,
    ID_CRT_A, ID_POT, ID_CRT_B, ID_CRT_A, ID_CRT_B, ID_TEMP,
};
static const phase_currents_t   current_sequencer[] = 
{
    CRT_PHASE_A, NO_CURRENT, CRT_PHASE_B, NO_CURRENT, CRT_PHASE_C, NO_CURRENT,
    CRT_PHASE_A, NO_CURRENT, CRT_PHASE_B, NO_CURRENT, CRT_PHASE_C, NO_CURRENT,
};
#define ANALOG_SEQUENCER_MAX  (sizeof(analog_sequencer)/sizeof(analog_id_t))

static inline void      AdataClear(analog_id_t id)                           {analog_results[id].W24 = 0;}
static inline void      AdataInit(analog_id_t id, adc_result_t value)        {analog_results[id].H16 = value; analog_results[id].L8 = 0;}
static inline uint16_t  AdataRead (analog_id_t id)                           {return analog_results[id].H16;}
static inline void      AdataFilterWrite(analog_id_t id, adc_result_t data)  {uint24_t x = analog_results[id].W24; uint24_t y = x >> 8; analog_results[id].W24 = x - y + (uint24_t)data;}
static inline void      AdataSimpleWrite(analog_id_t id, adc_result_t data)  {analog_results[id].H16 = data;}

static inline void      Analog_ComputeCurrents(phase_currents_t phase, bool new_ibus_sample)
{
    static split_u32_t crt_acc;
    static uint16_t crt_samples = 0;
    if(phase != NO_CURRENT)
    {
        switch(phase)
        {
            case CRT_PHASE_A: phase_current.W16 = (int16_t)AdataRead(ID_REF)   - (int16_t)AdataRead(ID_CRT_A);                                  last_current = CRT_PHASE_A; break;
            case CRT_PHASE_B: phase_current.W16 = (int16_t)AdataRead(ID_REF)   - (int16_t)AdataRead(ID_CRT_B);                                  last_current = CRT_PHASE_B; break;
            case CRT_PHASE_C: phase_current.W16 = (int16_t)AdataRead(ID_CRT_A) + (int16_t)AdataRead(ID_CRT_B) - 2*(int16_t)AdataRead(ID_REF);   last_current = CRT_PHASE_C; break;
            default:                                                                                                                                                        break;
        }
        if(phase_current.H8 & 0x80) crt_acc.W32 -= phase_current.W16;
        else                        crt_acc.W32 += phase_current.W16;
        crt_samples ++;
        if(new_ibus_sample)
        {
            bus_current.result = crt_acc.W32;
            bus_current.samples = crt_samples;
            crt_acc.W32 = 0;
            crt_samples = 0; 
        }
    }
}

/********************************/
/******* public functions *******/
/********************************/

/* 50us calling interval on interrupt context */
void Analog_Handler(bool new_ibus_sample)
{
    uint16_t adc = ADC_RESULT_GET();
    if(analog_types[index] == true)
        AdataFilterWrite(index, adc);
    else
        AdataSimpleWrite(index, adc);
    if(emergency[index].handler != NULL)
    {
        if(adc > emergency[index].threshold)
            emergency[index].handler(emergency[index].event);
    }
    Analog_ComputeCurrents(current_sequencer[sequencer_idx], new_ibus_sample);
    sequencer_idx++; if(sequencer_idx == ANALOG_SEQUENCER_MAX) sequencer_idx = 0;
    index = analog_sequencer[sequencer_idx];
    ADC_MUX_SET(pins_list[index]);
}

void Analog_Initialize(void)
{
    memset(analog_results, 0, sizeof(analog_results));
    memset(emergency, 0, sizeof(emergency));
    analog_types[ID_CRT_A] = false;
    analog_types[ID_CRT_B] = false;
    analog_types[ID_REF] = false;
    analog_types[ID_VBUS] = true;
    analog_types[ID_POT] = true;
    analog_types[ID_TEMP] = true;
    pins_list[ID_CRT_A] = CRT_A_ADC_PIN;
    pins_list[ID_CRT_B] = CRT_B_ADC_PIN;
    pins_list[ID_REF]   = CRT_N_ADC_PIN;
    pins_list[ID_VBUS]  = VBUS_ADC_PIN;
    pins_list[ID_POT]   = POT_ADC_PIN;
    pins_list[ID_TEMP]  = TEMP_ADC_PIN;
    bus_current.result = 0;
    bus_current.samples = 0;
    index = analog_sequencer[0];
    sequencer_idx = 0;
    ADC_MUX_SET(pins_list[index]);
}

uint16_t Analog_Max(void)
{
    return ADC_MAX;
}

uint16_t Analog_Get(analog_id_t id)
{
	uint16_t retval;
    ATOMIC_COPY(retval, AdataRead(id));
    return retval;
}

void Analog_EmergencyStop_Register(analog_id_t id, analog_fault_handler_t handler, uint16_t threshold, uint8_t event)
{
    ATOMIC_COPY(emergency[id].event,     event);
    ATOMIC_COPY(emergency[id].threshold, threshold);
    ATOMIC_COPY(emergency[id].handler,   handler);
}

void Analog_LastMeasuredCurrent(int16_t* value, phase_currents_t* current)
{
    *current = last_current;
    *value = phase_current.W16;
}

analog_bus_crt_t Analog_BusCurrentGet(bool state)
{
    analog_bus_crt_t current;
    if(!state)
    {
        current.result = 0;
        current.samples = 0;
    }
    else
    {
        ATOMIC_COPY(current.result,  bus_current.result);
        ATOMIC_COPY(current.samples, bus_current.samples);
    }
    return current;
}
