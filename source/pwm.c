
#define	CCEXTERN	extern
#define	INIT0
#define	INITARRAY0

#include "f28x_project.h"
#include "../ccVariables.h"

// Variables used to indirectly address the peripheral
volatile struct EPWM_REGS *ePWM[] = {
        &EPwm1Regs,         //intentional: (ePWM[0] not used)
        &EPwm1Regs, &EPwm2Regs, &EPwm3Regs, &EPwm4Regs, &EPwm5Regs, &EPwm6Regs, &EPwm7Regs, &EPwm8Regs};

#define PWM_PERIOD  5999
#define PWM_PERIOD_33P  2400

void InitDrives(N16 ch1)
{
    (*ePWM[ch1]).TBPRD = PWM_PERIOD;
    (*ePWM[ch1]).CMPA.bit.CMPA = PWM_PERIOD_33P;
    (*ePWM[ch1]).CMPB.bit.CMPB = PWM_PERIOD_33P;
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
    (*ePWM[ch1]).TZCTL.bit.TZA = 0x2;
    (*ePWM[ch1]).TZCTL.bit.TZB = 0x2;
    EDIS;
}



void ePWM7_Init(void)
{
Uint16 n = 7;
    (*ePWM[n]).TBCTL.bit.PRDLD = TB_IMMEDIATE;
    (*ePWM[n]).TBPRD = 3750;
    (*ePWM[n]).CMPA.bit.CMPA = 1250;
    (*ePWM[n]).TBPHS.bit.TBPHS = 0;
    (*ePWM[n]).TBCTR = 0;
    (*ePWM[n]).TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN;
    (*ePWM[n]).TBCTL.bit.PHSEN = TB_DISABLE;
    (*ePWM[n]).TBCTL.bit.HSPCLKDIV = TB_DIV1;
    (*ePWM[n]).TBCTL.bit.CLKDIV = TB_DIV1;
    (*ePWM[n]).AQCTLA.bit.ZRO = AQ_SET;
    (*ePWM[n]).AQCTLA.bit.CAU = AQ_CLEAR;
    EALLOW;                       /* Enable EALLOW */
    (*ePWM[n]).TZCTL.bit.TZA = 0x2;
    (*ePWM[n]).TZCTL.bit.TZB = 0x2;
     EDIS;
 }

