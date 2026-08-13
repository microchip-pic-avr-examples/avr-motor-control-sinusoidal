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

#include <string.h>              /* for memset() */
#include "conversion_macros.h"
#include "mcc_mapping.h"
#include "analog.h"
#include "config.h"

typedef struct
{
    analog_fault_handler_t handler;
    uint16_t               threshold;
    uint8_t                event;
} analog_emergency_t;


/** private data */
static analog_emergency_t emergency[ID_MAX];
static split_u24_t        analog_results[ID_MAX];
static uint8_t            pins_list[ID_MAX];
static analog_bus_crt_t   bus_current;
static split_i16_t        ia_adc, ib_adc, ic_adc;
static int16_t            iref_adc;
static bool               new_crt;

#define ANALOG_SEQUENCER_MAX  (sizeof(analog_sequencer)/sizeof(analog_id_t))

static inline void             AdataClear(analog_id_t id)                       {analog_results[id].W24 = 0;}
static inline void             AdataInit(analog_id_t id, adc_result_t value)    {analog_results[id].H16 = value; analog_results[id].L8 = 0;}
static inline uint16_t         AdataRead (analog_id_t id)                       {return analog_results[id].H16;}
static inline void             AdataFilterWrite(analog_id_t id, adc_result_t data) {uint24_t x = analog_results[id].W24; uint24_t y = x >> 8; analog_results[id].W24 = x - y + (uint24_t)data;}
static inline void             AdataSimpleWrite(analog_id_t id, adc_result_t data) {analog_results[id].H16 = data;}


/********************************/
/******* public functions *******/
/********************************/

void Analog_HandlerCrt(void)
{
    static analog_id_t index = ID_REF;
    int16_t adc = ADC_RESULT_GET();
    if(ADC_MUX_GET == CRT_A_ADC_PIN) 
    {
        ia_adc.W16 = iref_adc - adc; 
        AdataSimpleWrite(ID_CRT_A, ia_adc.W16);
        if(emergency[ID_CRT_A].handler != NULL)
        {
            if(adc > emergency[ID_CRT_A].threshold)
                emergency[ID_CRT_A].handler(emergency[ID_CRT_A].event);
        } 
        new_crt = true;
    }
    else                             
    {
        ib_adc.W16 = iref_adc - adc; 
        AdataSimpleWrite(ID_CRT_B, ib_adc.W16);
        ic_adc.W16 = - (ia_adc.W16 + ib_adc.W16);
        AdataSimpleWrite(ID_CRT_C, ic_adc.W16);
        new_crt = false;
    }
    if(index == ID_REF)       index = ID_VBUS;
    else if(index == ID_VBUS) index = ID_POT;
    else                      index = ID_REF;
    ADC_MUX_SET(pins_list[index]);
}

bool Analog_NewCrtSample(void)
{
   return new_crt; 
}

void Analog_HandlerOthers(bool new_bus_crt_sample)
{
    static split_u32_t crt_acc;
    static uint16_t crt_samples = 0;
    static analog_id_t index = ID_CRT_A;
    uint16_t adc = ADC_RESULT_GET();
    if(ADC_MUX_GET == CRT_N_ADC_PIN)    
    {
        AdataSimpleWrite(ID_REF, adc); iref_adc = adc;
    }
    else if(ADC_MUX_GET == VBUS_ADC_PIN) 
    {
        AdataFilterWrite(ID_VBUS, adc);
        if(emergency[ID_VBUS].handler != NULL)
        {
            if(adc > emergency[ID_VBUS].threshold)
                emergency[ID_VBUS].handler(emergency[ID_VBUS].event);
        } 
    }
    else 
        AdataFilterWrite(ID_POT, adc);
    if(index == ID_CRT_A)   index = ID_CRT_B;
    else                    index = ID_CRT_A;
    ADC_MUX_SET(pins_list[index]);
    if(new_bus_crt_sample)
    {
        bus_current.result = crt_acc.W32;
        bus_current.samples = crt_samples;
        crt_acc.W32 = 0;
        crt_samples = 0;
    }
    else
    {
        if(ia_adc.H8 & 0x80) crt_acc.W32 -= ia_adc.W16;
        else                 crt_acc.W32 += ia_adc.W16;
        crt_samples ++;
    }
}

void Analog_HandlerIdle(void)
{
   static analog_id_t index = ID_CRT_A;
   uint16_t adc = ADC_RESULT_GET();
   switch(index)
   {
       case ID_CRT_A: AdataSimpleWrite(index, adc); index = ID_REF;   break;
       case ID_REF:   AdataSimpleWrite(index, adc); index = ID_VBUS;  break;
       case ID_VBUS:  AdataFilterWrite(index, adc); index = ID_POT;   break;
       case ID_POT:   AdataFilterWrite(index, adc); index = ID_CRT_A; break;
       default:                                                       break;
   }
   ADC_MUX_SET(pins_list[index]);
   bus_current.result = 0;
   bus_current.samples = 0;
}

void Analog_Initialize(void)
{
    memset(analog_results, 0, sizeof(analog_results));
    memset(emergency, 0, sizeof(emergency));
    pins_list[ID_CRT_A] = CRT_A_ADC_PIN;
    pins_list[ID_CRT_B] = CRT_B_ADC_PIN;
    pins_list[ID_REF]   = CRT_N_ADC_PIN;
    pins_list[ID_VBUS]  = VBUS_ADC_PIN;
    pins_list[ID_POT]   = POT_ADC_PIN;
    ADC_MUX_SET(CRT_A_ADC_PIN);
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

void     Analog_EmergencyStop_Register(analog_id_t id, analog_fault_handler_t handler, uint16_t threshold, uint8_t event)
{
    ATOMIC_COPY(emergency[id].event,     event);
    ATOMIC_COPY(emergency[id].threshold, threshold);
    ATOMIC_COPY(emergency[id].handler,   handler);
}

analog_bus_crt_t Analog_BusCurrentGet(void)
{
    return bus_current;   
}

