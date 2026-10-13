/*
 * pwm.c
 *
 *  Created on: 09-Feb-2023
 *      Author: vijay
 */


#define CCEXTERN    extern
#define INIT0
#define INITARRAY0
#include "f28x_project.h"
#include "../ccVariables.h"
#include "inc/hw_memmap.h"
#include "sci.h"
#include "SVGEN_CLA.h"
#include "../controlCore.h"

#if defined(_FLASH) && !CONTROL_ON_CLA
#pragma CODE_SECTION(EPWM_INV_ISR, ".TI.ramfunc");
#endif

volatile struct EPWM_REGS *ePWM[] = {
        &EPwm1Regs,         //intentional: (ePWM[0] not used)
        &EPwm1Regs, &EPwm2Regs, &EPwm3Regs, &EPwm4Regs, &EPwm5Regs, &EPwm6Regs, &EPwm7Regs, &EPwm8Regs};



#define SWFREQ6C    10000
#define SWFREQ8C    7500
#define SWFREQ10C   6000
#define SWFREQ12C   5000
#define SWFREQ128C  4688

#define SWFREQ10     10000
#define SWFREQ12     4000
#define DEADBAND_INV 375        // 2.5 us at TBCLK = 150 MHz

#if !CONTROL_ON_CLA
/*
 Inverter modulator on the CPU (CONTROL_ON_CLA = 0), EPWM1 INT at counter zero:
 SVPWM, dead-time compensation and CMPA update, controlCore.h ctl_pwm().
 With CONTROL_ON_CLA = 1 the same code runs as CLA task 2 on the EPWM1 INT.
 */
interrupt void  EPWM_INV_ISR (void)
{
    ctl_pwm();
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
}
#endif

/*
 Hardware desaturation trip, Bridge 1 (Common Controller v2):
 VCESAT1 (GPIO54, CON15 driver fault, active high) -> INPUTXBAR1 (InitFaultXbar)
 -> TRIPIN1 -> DCAH high -> DCAEVT1 (async, unfiltered) -> one-shot trip of EPWM1-3.
 Outputs go to TZCTL TZA/TZB (forced low) without the CPU. updateStatus() sets
 SystemFlag.VCESAT from the latched TZFLG.DCAEVT1; ENABLE_PWM() clears it.
 */
void DesatTripInit(void)
{
    N16 ch;

    EALLOW;
    for(ch = 1; ch <= 3; ch++)
    {
        (*ePWM[ch]).DCTRIPSEL.bit.DCAHCOMPSEL = DC_TRIPIN1;
        (*ePWM[ch]).TZDCSEL.bit.DCAEVT1 = TZ_DCAH_HI;       // fault = VCESAT1 high
        (*ePWM[ch]).DCACTL.bit.EVT1SRCSEL = DC_SRC_DCAEVT1; // unfiltered
        (*ePWM[ch]).DCACTL.bit.EVT1FRCSYNCSEL = DC_EVT_ASYNC;
        (*ePWM[ch]).TZSEL.bit.DCAEVT1 = 1;                  // one-shot trip source
        (*ePWM[ch]).TZCLR.bit.DCAEVT1 = 1;
    }
    EDIS;
}


void ePWMInit_INV(N16 ch1, N16 ch2, N16 ch3 )
{

if(InvPwmPeriod<2000) { InvPwmPeriod = 5000; Inv_HalfPeriod = 2500; }
(*ePWM[ch1]).TBCTL.bit.PRDLD = TB_SHADOW;
(*ePWM[ch1]).TBPRD = InvPwmPeriod;
(*ePWM[ch1]).TBPHS.bit.TBPHS = 0;
(*ePWM[ch1]).TBCTR = 0;
(*ePWM[ch1]).TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN;
(*ePWM[ch1]).TBCTL.bit.HSPCLKDIV = TB_DIV1;
(*ePWM[ch1]).TBCTL.bit.CLKDIV = TB_DIV1;

// Counter Compare Submodule Registers
(*ePWM[ch1]).CMPA.bit.CMPA = 0; // set duty 0% initially
(*ePWM[ch1]).CMPCTL.bit.SHDWAMODE = CC_SHADOW;
(*ePWM[ch1]).CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;

// Action Qualifier SubModule Registers
(*ePWM[ch1]).AQCTLA.bit.CAU = AQ_SET; // CTR = CMPA@UP , set to 1
(*ePWM[ch1]).AQCTLA.bit.CAD = AQ_CLEAR; // CTR = CMPA@Down , toggle

// Active high complementary PWMs - Set up the deadband
(*ePWM[ch1]).DBCTL.bit.IN_MODE = DBA_ALL;
(*ePWM[ch1]).DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;
(*ePWM[ch1]).DBCTL.bit.POLSEL = DB_ACTV_HIC;
(*ePWM[ch1]).DBRED.all =  DEADBAND_INV;
(*ePWM[ch1]).DBFED.all =  DEADBAND_INV;
(*ePWM[ch1]).TBCTL.bit.PHSEN = TB_DISABLE;
(*ePWM[ch1]).EPWMSYNCOUTEN.bit.ZEROEN = SYNC_OUT_SRC_ENABLE;
//CHANNEL 2

(*ePWM[ch2]).TBCTL.bit.PRDLD = TB_SHADOW;
(*ePWM[ch2]).TBPRD = InvPwmPeriod; // PWM frequency = 1 / period
(*ePWM[ch2]).TBPHS.bit.TBPHS = 0;
(*ePWM[ch2]).TBCTR = 0;
(*ePWM[ch2]).TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN;
(*ePWM[ch2]).TBCTL.bit.HSPCLKDIV = TB_DIV1;
(*ePWM[ch2]).TBCTL.bit.CLKDIV = TB_DIV1;

// Counter Compare Submodule Registers
(*ePWM[ch2]).CMPA.bit.CMPA = 0; // set duty 0% initially
(*ePWM[ch2]).CMPCTL.bit.SHDWAMODE = CC_SHADOW;
(*ePWM[ch2]).CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;

(*ePWM[ch2]).AQCTLA.bit.CAU = AQ_SET; // CTR = CMPA@UP , set to 1
(*ePWM[ch2]).AQCTLA.bit.CAD = AQ_CLEAR; // CTR = CMPA@Down , toggle

// Active high complementary PWMs - Set up the deadband
(*ePWM[ch2]).DBCTL.bit.IN_MODE = DBA_ALL;
(*ePWM[ch2]).DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;
(*ePWM[ch2]).DBCTL.bit.POLSEL = DB_ACTV_HIC;
(*ePWM[ch2]).DBRED.all =  DEADBAND_INV;
(*ePWM[ch2]).DBFED.all =  DEADBAND_INV;
(*ePWM[ch2]).TBCTL.bit.PHSEN = TB_DISABLE;

(*ePWM[ch2]).EPWMSYNCOUTEN.bit.ZEROEN = SYNC_OUT_SRC_ENABLE;
//CHANNEL 3

(*ePWM[ch3]).TBCTL.bit.PRDLD = TB_SHADOW;
(*ePWM[ch3]).TBPRD = InvPwmPeriod; // PWM frequency = 1 / period
(*ePWM[ch3]).TBPHS.bit.TBPHS = 0;
(*ePWM[ch3]).TBCTR = 0;
(*ePWM[ch3]).TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN;
(*ePWM[ch3]).TBCTL.bit.HSPCLKDIV = TB_DIV1;
(*ePWM[ch3]).TBCTL.bit.CLKDIV = TB_DIV1;

// Counter Compare Submodule Registers
(*ePWM[ch3]).CMPA.bit.CMPA = 0; // set duty 0% initially
(*ePWM[ch3]).CMPCTL.bit.SHDWAMODE = CC_SHADOW;
(*ePWM[ch3]).CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
(*ePWM[ch3]).AQCTLA.bit.CAU = AQ_SET; // CTR = CMPA@UP , set to 1
(*ePWM[ch3]).AQCTLA.bit.CAD = AQ_CLEAR; // CTR = CMPA@Down , toggle

// Active high complementary PWMs - Set up the deadband
(*ePWM[ch3]).DBCTL.bit.IN_MODE = DBA_ALL;
(*ePWM[ch3]).DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;
(*ePWM[ch3]).DBCTL.bit.POLSEL = DB_ACTV_HIC;
(*ePWM[ch3]).DBRED.all =  DEADBAND_INV;
(*ePWM[ch3]).DBFED.all =  DEADBAND_INV;

(*ePWM[ch3]).TBCTL.bit.PHSEN = TB_DISABLE;
//(*ePWM[ch3]).TBCTL.bit.SYNCOSEL = 0;//TB_SYNC_DISABLE;//TB_CTR_ZERO; // sync "down-stream"
(*ePWM[ch3]).EPWMSYNCOUTEN.bit.ZEROEN = SYNC_OUT_SRC_ENABLE;


//-----------------------------------------------------------------------------

EALLOW;
(*ePWM[ch1]).TZCTL.bit.TZA = TZ_FORCE_LO;
(*ePWM[ch1]).TZCTL.bit.TZB = TZ_FORCE_LO;
(*ePWM[ch2]).TZCTL.bit.TZA = TZ_FORCE_LO;
(*ePWM[ch2]).TZCTL.bit.TZB = TZ_FORCE_LO;
(*ePWM[ch3]).TZCTL.bit.TZA = TZ_FORCE_LO;
(*ePWM[ch3]).TZCTL.bit.TZB = TZ_FORCE_LO;

EDIS;
}

void InitDrives(N16 ch1)
{
    (*ePWM[ch1]).TBPRD = 3999;
    (*ePWM[ch1]).CMPA.bit.CMPA = 1333;
    (*ePWM[ch1]).CMPB.bit.CMPB = 1333;
    (*ePWM[ch1]).TBPHS.all = 0;
    (*ePWM[ch1]).TBCTR = 0;
    (*ePWM[ch1]).TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN;
    (*ePWM[ch1]).TBCTL.bit.PHSEN = TB_DISABLE;
    (*ePWM[ch1]).TBCTL.bit.PRDLD = TB_SHADOW;
    (*ePWM[ch1]).EPWMSYNCOUTEN.bit.ZEROEN = SYNC_OUT_SRC_DISABLE;
    (*ePWM[ch1]).TBCTL.bit.HSPCLKDIV = TB_DIV1;
    (*ePWM[ch1]).TBCTL.bit.CLKDIV = TB_DIV1;
    (*ePWM[ch1]).CMPCTL.bit.SHDWAMODE = CC_SHADOW;
    (*ePWM[ch1]).CMPCTL.bit.SHDWBMODE = CC_SHADOW;
    (*ePWM[ch1]).CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
    (*ePWM[ch1]).CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;
    (*ePWM[ch1]).AQCTLA.bit.CAD = AQ_SET;
    (*ePWM[ch1]).AQCTLA.bit.CAU = AQ_CLEAR;
    (*ePWM[ch1]).AQCTLB.bit.CBD = AQ_SET   ;
    (*ePWM[ch1]).AQCTLB.bit.CBU = AQ_CLEAR;

    EALLOW;
    (*ePWM[ch1]).TZCTL.bit.TZA = TZ_FORCE_LO;
    (*ePWM[ch1]).TZCTL.bit.TZB = TZ_FORCE_LO;
    EDIS;
}


/*
 Static switch gate pulse trains, Common Controller v2:
   EPWM7A -> STAT_INV (GPIO12) -> U33 -> CON24
   EPWM7B -> STAT_BYP (GPIO13) -> U33 -> CON24
 15 kHz, 33 % (same as the reference board's per-SCR trains, rescaled for
 150 MHz): up-down, TBPRD 4999, output high while CTR < CMPx. A source is
 switched on by AQ_SET on the down-count compare (EnableInvSS / EnableBypSS,
 controlMacros.h) and off by AQ_CLEAR. Not tripped by the inverter trip zone:
 the load must stay on the bypass when the inverter trips.
 */
#define SS_TBPRD    4999        // 150 MHz / (2 x 5000) = 15 kHz
#define SS_CMP      1666        // 33 %

void StaticSwitchDrivesInit(void)
{
    EPwm7Regs.TBCTL.bit.PRDLD = TB_SHADOW;
    EPwm7Regs.TBPRD = SS_TBPRD;
    EPwm7Regs.CMPA.bit.CMPA = SS_CMP;
    EPwm7Regs.CMPB.bit.CMPB = SS_CMP;
    EPwm7Regs.TBPHS.all = 0;
    EPwm7Regs.TBCTR = 0;
    EPwm7Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN;
    EPwm7Regs.TBCTL.bit.PHSEN = TB_DISABLE;
    EPwm7Regs.EPWMSYNCOUTEN.bit.ZEROEN = SYNC_OUT_SRC_DISABLE;
    EPwm7Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;
    EPwm7Regs.TBCTL.bit.CLKDIV = TB_DIV1;
    EPwm7Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
    EPwm7Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
    EPwm7Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
    EPwm7Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;
    EPwm7Regs.AQCTLA.bit.CAD = AQ_CLEAR;     // off until EnableInvSS()
    EPwm7Regs.AQCTLA.bit.CAU = AQ_CLEAR;
    EPwm7Regs.AQCTLB.bit.CBD = AQ_CLEAR;     // off until EnableBypSS()
    EPwm7Regs.AQCTLB.bit.CBU = AQ_CLEAR;
}

/*
 ADC trigger: EPWM8 (internal only, no pin), SOCA at period, 20 kHz:
 up-down, TBPRD = 150 MHz / (2 x 20 kHz) = 3750 -> SAMPLING_TIME 50 us.
 */
void AdcTriggerInit(void)
{
    EPwm8Regs.TBCTL.bit.PRDLD = TB_IMMEDIATE;
    EPwm8Regs.TBPRD = 3750;
    EPwm8Regs.TBPHS.all = 0;
    EPwm8Regs.TBCTR = 0;
    EPwm8Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN;
    EPwm8Regs.TBCTL.bit.PHSEN = TB_DISABLE;
    EPwm8Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;
    EPwm8Regs.TBCTL.bit.CLKDIV = TB_DIV1;

    EPwm8Regs.ETSEL.bit.SOCASEL = ET_CTR_PRD;
    EPwm8Regs.ETPS.bit.SOCAPRD  = ET_1ST;
    EPwm8Regs.ETSEL.bit.SOCAEN  = 1;
}
//===========================================================================
//                          End of file
//===========================================================================


