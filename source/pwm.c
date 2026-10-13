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

#ifdef _FLASH
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
//#define INV_PWM_PERIOD  SWFREQ8C

//#define INV_HALFPERIOD  (INV_PWM_PERIOD >>1)

SVGEN_CLA svgen_dq1;

/*--------------------------------------------------------------------------
 Function Name: EPwmISR
 Description:   Handles the generation of PWM pulses as per details in
 the design document W20491_01_011_DesignDocumentv0.31.doc
 FreqAcc = (Generated Freq x Resolution) / Sampling Freq.
 Takes      :   None
 Returns        :   None
 Funcs Called:  None

 Author     :   Vijayan C A
 History Log:
 ---------------------------------------------------------------------------
 Date       | Modified By |         Comments
 ---------------------------------------------------------------------------
 10/02/2012   VCA               Created
 ---------------------------------------------------------------------------*/

// sign of the current, linear between -1 and +1 (input already divided by the band)
static inline float dtSign(float x)
{
    if(x > 1.0) return 1.0;
    if(x < -1.0) return -1.0;
    return x;
}

interrupt void  EPWM_INV_ISR (void) {

    float z = 0;

    temp_fsw1++;

#ifdef PHASE3PH
    if(currentMode)
    {
        svgen_dq1.Ualpha = Cur_Alpha;
        svgen_dq1.Ubeta = Cur_Beta;
    }
    else
    {
        RYB2AB_F(svgen_dq1.Ualpha,svgen_dq1.Ubeta,z,vcontrol.refa, vcontrol.refb, vcontrol.refc);
    }
    if(z) z = 0.0;
    SVGEN_CLA_MACRO(svgen_dq1);
    svgen_dq1.Ta = -svgen_dq1.Ta;
    svgen_dq1.Tb = -svgen_dq1.Tb;
    svgen_dq1.Tc = -svgen_dq1.Tc;

    // Dead-time compensation: each leg loses Td/Tsw of duty against its current.
    // Full correction in normalised units is DBRED/TBPRD (= 2*Td/Tsw); DtComp scales
    // it and sets the sign (current sensor polarity). Linear within +/-DtBand amps
    // so the correction does not chatter around current zero crossings.
    if(DtComp != 0.0)
    {
        float dtGain = DtComp * (float)EPwm1Regs.DBRED.all / (float)EPwm1Regs.TBPRD;

        svgen_dq1.Ta += dtGain * dtSign(lpf_iInvR.y * IinvRSF / DtBand);
        svgen_dq1.Tb += dtGain * dtSign(lpf_iInvY.y * IinvYSF / DtBand);
        svgen_dq1.Tc += dtGain * dtSign(lpf_iInvB.y * IinvBSF / DtBand);
    }

    if(svgen_dq1.Ta > SAT_LIMIT)  svgen_dq1.Ta = SAT_LIMIT;
    if(svgen_dq1.Tb > SAT_LIMIT)  svgen_dq1.Tb= SAT_LIMIT;
    if(svgen_dq1.Tc > SAT_LIMIT)  svgen_dq1.Tc = SAT_LIMIT;
    if(svgen_dq1.Ta < SAT_LIMITN) svgen_dq1.Ta = SAT_LIMITN;
    if(svgen_dq1.Tb < SAT_LIMITN) svgen_dq1.Tb = SAT_LIMITN;
    if(svgen_dq1.Tc < SAT_LIMITN) svgen_dq1.Tc = SAT_LIMITN;

    EPwm1Regs.CMPA.bit.CMPA = Inv_HalfPeriod*svgen_dq1.Ta + Inv_HalfPeriod;
    EPwm2Regs.CMPA.bit.CMPA = Inv_HalfPeriod*svgen_dq1.Tb + Inv_HalfPeriod;
    EPwm3Regs.CMPA.bit.CMPA = Inv_HalfPeriod*svgen_dq1.Tc + Inv_HalfPeriod;

#else
        if(currentMode)
        {
            vcontrol.refa = reference_current;
        }


        EPwm1Regs.CMPA.bit.CMPA = INV_HALFPERIOD * -vcontrol.refa + INV_HALFPERIOD;
        EPwm2Regs.CMPA.bit.CMPA = INV_HALFPERIOD * vcontrol.refa + INV_HALFPERIOD;
#endif

    //---------------Reinitialize for next PWM sequence--------------
        EPwm1Regs.ETCLR.bit.INT = 1;
        PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
}

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


