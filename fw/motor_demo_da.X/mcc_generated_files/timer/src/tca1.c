/**
 * TCA1 Generated Driver File
 *
 * @file tca1.c
 *
 * @ingroup tca1_normal
 *
 * @brief This file contains the driver implementation for the TCA1 module in Normal (16-bit) mode.
 *
 * @version TCA1 Driver Version 3.1.0
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

#include "../tca1.h"

static void (*TCA1_CMP0Callback)(void) = NULL;
static void (*TCA1_CMP1Callback)(void) = NULL;
static void (*TCA1_CMP2Callback)(void) = NULL;
static void (*TCA1_OVFCallback)(void) = NULL;

void TCA1_Initialize(void) 
{
    TCA1.SINGLE.CMP0 = 0x0;  // CMP0 0x0

    TCA1.SINGLE.CMP1 = 0x0;  // CMP1 0x0

    TCA1.SINGLE.CMP2 = 0x0;  // CMP2 0x0

    TCA1.SINGLE.CNT = 0x0;  // CNT 0x0

    TCA1.SINGLE.CTRLB = (0 << TCA_SINGLE_ALUPD_bp)   // ALUPD disabled
        | (1 << TCA_SINGLE_CMP0EN_bp)   // CMP0EN enabled
        | (1 << TCA_SINGLE_CMP1EN_bp)   // CMP1EN enabled
        | (1 << TCA_SINGLE_CMP2EN_bp)   // CMP2EN enabled
        | (TCA_SINGLE_WGMODE_DSTOP_gc);  // WGMODE DSTOP

    TCA1.SINGLE.CTRLC = (0 << TCA_SINGLE_CMP0OV_bp)   // CMP0OV disabled
        | (0 << TCA_SINGLE_CMP1OV_bp)   // CMP1OV disabled
        | (0 << TCA_SINGLE_CMP2OV_bp);  // CMP2OV disabled

    TCA1.SINGLE.CTRLD = (0 << TCA_SINGLE_SPLITM_bp);  // SPLITM disabled

    TCA1.SINGLE.CTRLECLR = (TCA_SINGLE_CMD_NONE_gc)   // CMD NONE
        | (0 << TCA_SINGLE_DIR_bp)   // DIR disabled
        | (0 << TCA_SINGLE_LUPD_bp);  // LUPD disabled

    TCA1.SINGLE.CTRLESET = (TCA_SINGLE_CMD_NONE_gc)   // CMD NONE
        | (TCA_SINGLE_DIR_UP_gc)   // DIR UP
        | (0 << TCA_SINGLE_LUPD_bp);  // LUPD disabled

    TCA1.SINGLE.CTRLFCLR = (0 << TCA_SINGLE_CMP0BV_bp)   // CMP0BV disabled
        | (0 << TCA_SINGLE_CMP1BV_bp)   // CMP1BV disabled
        | (0 << TCA_SINGLE_CMP2BV_bp)   // CMP2BV disabled
        | (0 << TCA_SINGLE_PERBV_bp);  // PERBV disabled

    TCA1.SINGLE.CTRLFSET = (0 << TCA_SINGLE_CMP0BV_bp)   // CMP0BV disabled
        | (0 << TCA_SINGLE_CMP1BV_bp)   // CMP1BV disabled
        | (0 << TCA_SINGLE_CMP2BV_bp)   // CMP2BV disabled
        | (0 << TCA_SINGLE_PERBV_bp);  // PERBV disabled

    TCA1.SINGLE.DBGCTRL = (0 << TCA_SINGLE_DBGRUN_bp);  // DBGRUN disabled

    TCA1.SINGLE.EVCTRL = (0 << TCA_SINGLE_CNTAEI_bp)   // CNTAEI disabled
        | (0 << TCA_SINGLE_CNTBEI_bp)   // CNTBEI disabled
        | (TCA_SINGLE_EVACTA_CNT_POSEDGE_gc)   // EVACTA CNT_POSEDGE
        | (TCA_SINGLE_EVACTB_NONE_gc);  // EVACTB NONE

    TCA1.SINGLE.INTCTRL = (0 << TCA_SINGLE_CMP0_bp)   // CMP0 disabled
        | (0 << TCA_SINGLE_CMP1_bp)   // CMP1 disabled
        | (0 << TCA_SINGLE_CMP2_bp)   // CMP2 disabled
        | (0 << TCA_SINGLE_OVF_bp);  // OVF disabled

    TCA1.SINGLE.INTFLAGS = (0 << TCA_SINGLE_CMP0_bp)   // CMP0 disabled
        | (0 << TCA_SINGLE_CMP1_bp)   // CMP1 disabled
        | (0 << TCA_SINGLE_CMP2_bp)   // CMP2 disabled
        | (0 << TCA_SINGLE_OVF_bp);  // OVF disabled

    TCA1.SINGLE.PER = 0x258U;  // PER 0x258

    TCA1.SINGLE.TEMP = 0x0;  // TEMP 0x0

    TCA1.SINGLE.CTRLA = (TCA_SINGLE_CLKSEL_DIV1_gc)   // CLKSEL DIV1
        | (1 << TCA_SINGLE_ENABLE_bp)   // ENABLE enabled
        | (0 << TCA_SINGLE_RUNSTDBY_bp);  // RUNSTDBY disabled
}

void TCA1_Deinitialize(void)
{
    TCA1.SINGLE.CTRLA &= ~TCA_SINGLE_ENABLE_bm;

    TCA1.SINGLE.CTRLA = 0x0;
    TCA1.SINGLE.CTRLB = 0x0;
    TCA1.SINGLE.CTRLC = 0x0;
    TCA1.SINGLE.CTRLD = 0x0;
    
    TCA1.SINGLE.CNT = 0x0;
    TCA1.SINGLE.PER = 0xFFFFU;
    TCA1.SINGLE.CMP0 = 0x0;
    TCA1.SINGLE.CMP1 = 0x0;
    TCA1.SINGLE.CMP2 = 0x0;
    TCA1.SINGLE.EVCTRL = 0x0;
    TCA1.SINGLE.CTRLESET = 0x0;
    TCA1.SINGLE.CTRLECLR = 0x0;
    TCA1.SINGLE.CTRLFSET = 0x0;
    TCA1.SINGLE.CTRLFCLR = 0x0;

    TCA1.SINGLE.INTCTRL = 0x0;
    TCA1.SINGLE.INTFLAGS = ~0x0;
}

void TCA1_Start(void)
{
    TCA1.SINGLE.CTRLA |= TCA_SINGLE_ENABLE_bm;
}

void TCA1_Stop(void)
{
    TCA1.SINGLE.CTRLA &= ~TCA_SINGLE_ENABLE_bm;
}

void TCA1_CounterSet(uint16_t count)
{
    TCA1.SINGLE.CNT = count;
}

uint16_t TCA1_CounterGet(void)
{
    return TCA1.SINGLE.CNT;
}

void TCA1_PeriodSet(uint16_t periodVal)
{
    TCA1.SINGLE.PER = periodVal;
}

void TCA1_PeriodBufferSet(uint16_t periodVal)
{
    TCA1.SINGLE.PERBUF = periodVal;
}

uint16_t TCA1_PeriodGet(void)
{
    return TCA1.SINGLE.PER;
}

void TCA1_Compare0Set(uint16_t value)
{
    TCA1.SINGLE.CMP0 = value;
}

void TCA1_Compare0BufferSet(uint16_t value)
{
    TCA1.SINGLE.CMP0BUF = value;
}

void TCA1_Compare1Set(uint16_t value)
{
    TCA1.SINGLE.CMP1 = value;
}

void TCA1_Compare1BufferSet(uint16_t value)
{
    TCA1.SINGLE.CMP1BUF = value;
}

void TCA1_Compare2Set(uint16_t value)
{
    TCA1.SINGLE.CMP2 = value;
}

void TCA1_Compare2BufferSet(uint16_t value)
{
    TCA1.SINGLE.CMP2BUF = value;
}

uint16_t TCA1_MaxCountGet(void)
{
    return TCA1_MAX_COUNT;
}

void TCA1_ModeSet(TCA_SINGLE_WGMODE_t mode)
{
  if((mode == TCA_SINGLE_WGMODE_NORMAL_gc) ||
     (mode == TCA_SINGLE_WGMODE_FRQ_gc) ||
     (mode == TCA_SINGLE_WGMODE_SINGLESLOPE_gc) ||
     (mode == TCA_SINGLE_WGMODE_DSTOP_gc) ||
     (mode == TCA_SINGLE_WGMODE_DSBOTH_gc) ||
     (mode == TCA_SINGLE_WGMODE_DSBOTTOM_gc))
  {
      uint8_t temp = (TCA1.SINGLE.CTRLB & ~TCA_SINGLE_WGMODE_gm) |
             (mode & TCA_SINGLE_WGMODE_gm);
      TCA1.SINGLE.CTRLB = temp;
  }
}

void TCA1_OverflowStatusClear(void)
{
    TCA1.SINGLE.INTFLAGS = TCA_SINGLE_OVF_bm; 
}

bool TCA1_OverflowStatusGet(void)
{
    return ((TCA1.SINGLE.INTFLAGS & TCA_SINGLE_OVF_bm) > 0U);
}

void TCA1_CMP0MatchStatusClear(void)
{
    TCA1.SINGLE.INTFLAGS = TCA_SINGLE_CMP0_bm; /* Clear Compare Channel-0 Interrupt Flag */
}

bool TCA1_CMP0MatchStatusGet(void)
{
    return ((TCA1.SINGLE.INTFLAGS & TCA_SINGLE_CMP0_bm) > 0U);
}

void TCA1_CMP1MatchStatusClear(void)
{
    TCA1.SINGLE.INTFLAGS = TCA_SINGLE_CMP1_bm; /* Clear Compare Channel-1 Interrupt Flag */
}

bool TCA1_CMP1MatchStatusGet(void)
{
    return ((TCA1.SINGLE.INTFLAGS & TCA_SINGLE_CMP1_bm) > 0U);
}

void TCA1_CMP2MatchStatusClear(void)
{
    TCA1.SINGLE.INTFLAGS = TCA_SINGLE_CMP2_bm; 
}

bool TCA1_CMP2MatchStatusGet(void)
{
    return ((TCA1.SINGLE.INTFLAGS & TCA_SINGLE_CMP2_bm) > 0U);
}

void TCA1_Tasks(void)
{
    if(0U != (TCA1.SINGLE.INTFLAGS & TCA_SINGLE_OVF_bm))
    {
        if(NULL != TCA1_OVFCallback)
        {
          (*TCA1_OVFCallback)();
        }
        TCA1.SINGLE.INTFLAGS = TCA_SINGLE_OVF_bm;
    }

    if(0U != (TCA1.SINGLE.INTFLAGS & TCA_SINGLE_CMP0_bm))
    {
        if(NULL != TCA1_CMP0Callback)
        {
            (*TCA1_CMP0Callback)();
        }
        TCA1.SINGLE.INTFLAGS = TCA_SINGLE_CMP0_bm;
    }

    if(0U != (TCA1.SINGLE.INTFLAGS & TCA_SINGLE_CMP1_bm))
    {
        if(NULL != TCA1_CMP1Callback)
        {
            (*TCA1_CMP1Callback)();
        }
        TCA1.SINGLE.INTFLAGS = TCA_SINGLE_CMP1_bm;
    }

    if(0U != (TCA1.SINGLE.INTFLAGS & TCA_SINGLE_CMP2_bm))
    {
        if(NULL != TCA1_CMP2Callback)
        {
            (*TCA1_CMP2Callback)();
        }
        TCA1.SINGLE.INTFLAGS = TCA_SINGLE_CMP2_bm;
    }
}

void TCA1_OverflowCallbackRegister(TCA1_cb_t CallbackHandler)
{
    TCA1_OVFCallback = CallbackHandler;
}

void TCA1_Compare0CallbackRegister(TCA1_cb_t CallbackHandler)
{
    TCA1_CMP0Callback = CallbackHandler;
}

void TCA1_Compare1CallbackRegister(TCA1_cb_t CallbackHandler)
{
     TCA1_CMP1Callback = CallbackHandler;
}

void TCA1_Compare2CallbackRegister(TCA1_cb_t CallbackHandler)
{
     TCA1_CMP2Callback = CallbackHandler;
}
