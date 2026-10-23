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
#pragma CODE_SECTION(EPWM_PFC_ISR, ".TI.ramfunc");
#endif

volatile struct EPWM_REGS *ePWM[] = {
        &EPwm1Regs,         //intentional: (ePWM[0] not used)
        &EPwm1Regs, &EPwm2Regs, &EPwm3Regs, &EPwm4Regs, &EPwm5Regs, &EPwm6Regs, &EPwm7Regs, &EPwm8Regs};


#define DEADBAND_PFC 375        // 2.5 us at TBCLK = 150 MHz

#if !CONTROL_ON_CLA
/*
 PFC modulator on the CPU (CONTROL_ON_CLA = 0), EPWM1 INT at counter zero:
 SVPWM, dead-time compensation and CMPA update, controlCore.h ctl_pwm().
 With CONTROL_ON_CLA = 1 the same code runs as CLA task 2 on the EPWM1 INT.
 */
interrupt void  EPWM_PFC_ISR (void)
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
 SystemFlag.VCESAT from the latched TZFLG.DCAEVT1; ENABLE_PFCPWM() clears it.
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


/*
 PFC bridge (bridge 1, EPWM1-3 -> U30 -> CON15): up-down carrier, complementary
 outputs with dead band, CMPA shadow loaded at zero. Period from UPDATE_PFC_FSW().
 */
void ePWMInit_PFC(N16 ch1, N16 ch2, N16 ch3 )
{
    N16 ch, k;

    if(PFCPwmPeriod<2000) { PFCPwmPeriod = 7500; PFC_HalfPeriod = 3750; }

    for(k = 0; k < 3; k++)
    {
        ch = (k == 0) ? ch1 : ((k == 1) ? ch2 : ch3);

        (*ePWM[ch]).TBCTL.bit.PRDLD = TB_SHADOW;
        (*ePWM[ch]).TBPRD = PFCPwmPeriod;
        (*ePWM[ch]).TBPHS.bit.TBPHS = 0;
        (*ePWM[ch]).TBCTR = 0;
        (*ePWM[ch]).TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN;
        (*ePWM[ch]).TBCTL.bit.HSPCLKDIV = TB_DIV1;
        (*ePWM[ch]).TBCTL.bit.CLKDIV = TB_DIV1;

        // Counter Compare Submodule Registers
        (*ePWM[ch]).CMPA.bit.CMPA = 0; // set duty 0% initially
        (*ePWM[ch]).CMPCTL.bit.SHDWAMODE = CC_SHADOW;
        (*ePWM[ch]).CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;

        // Action Qualifier SubModule Registers
        (*ePWM[ch]).AQCTLA.bit.CAU = AQ_SET; // CTR = CMPA@UP , set to 1
        (*ePWM[ch]).AQCTLA.bit.CAD = AQ_CLEAR; // CTR = CMPA@Down , clear

        // Active high complementary PWMs - Set up the deadband
        (*ePWM[ch]).DBCTL.bit.IN_MODE = DBA_ALL;
        (*ePWM[ch]).DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;
        (*ePWM[ch]).DBCTL.bit.POLSEL = DB_ACTV_HIC;
        (*ePWM[ch]).DBRED.all =  DEADBAND_PFC;
        (*ePWM[ch]).DBFED.all =  DEADBAND_PFC;
        (*ePWM[ch]).TBCTL.bit.PHSEN = TB_DISABLE;
        (*ePWM[ch]).EPWMSYNCOUTEN.bit.ZEROEN = SYNC_OUT_SRC_ENABLE;

        EALLOW;
        (*ePWM[ch]).TZCTL.bit.TZA = TZ_FORCE_LO;
        (*ePWM[ch]).TZCTL.bit.TZB = TZ_FORCE_LO;
        EDIS;
    }
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


