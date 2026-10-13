
/*
 * interrupt.c
 *
 *  Created on: Sep 27, 2012
 *      Author: VCA
 */
#define	CCEXTERN	extern
#define	INIT0
#define	INITARRAY0
#include "f28x_project.h"
#include "../ccVariables.h"
#include "inc/hw_memmap.h"
#include "sci.h"
#include "../include/CNTL_PI_F.h"

extern pllBlock  pllBypass;
volatile float current_zero = 0;
volatile float current_d = 0;
volatile float current_q = 0;
volatile float prevSin = 0;
long  adcStart_Timer = 0;

volatile int Dac1val = 0, Dac2val = 0;
volatile N16 invfCount = 0;
volatile N16 invokcounter = 0;
volatile N16 syncTimer = 0;
long val1 = 0;
long val2 = 0;
long *nptr = 0;
long *nptr1 = 0;
int  canTx = 0;

N16 volatile canIntrval = 0;
N16 Id = 0;


#ifdef _FLASH
#pragma CODE_SECTION(cpu_timer0_isr, ".TI.ramfunc");
#pragma CODE_SECTION(cpu_timer1_isr, ".TI.ramfunc");
#pragma CODE_SECTION(cpu_timer2_isr, ".TI.ramfunc");
#pragma CODE_SECTION(SCIRxISR, ".TI.ramfunc");
#pragma CODE_SECTION(AdcISR, ".TI.ramfunc");
#pragma CODE_SECTION(CurrentModeSelection, ".TI.ramfunc");
#pragma CODE_SECTION(pll_and_sincos, ".TI.ramfunc");
#pragma CODE_SECTION(SendCANPackets, ".TI.ramfunc");
#pragma CODE_SECTION(rmsCal, ".TI.ramfunc");
#pragma CODE_SECTION(getReferenceVoltage, ".TI.ramfunc");
#pragma CODE_SECTION(getReferenceCurrent, ".TI.ramfunc");
#endif

inline void getReferenceCurrent(void);

// Preload fundamental resonator with the current-mode output (positive sequence
// oscillator state: X_alpha = Y_beta, X_beta = -Y_alpha) for bumpless transfer
#define PR_PRELOAD_AB(a,b)    \
    prF[0].Y = (a); prF[0].X = (b);  \
    prF[1].Y = (b); prF[1].X = -(a)
#define PR_PRELOAD()    PR_PRELOAD_AB(Cur_Alpha, Cur_Beta)

const float32_t prOrder[PR_NHARM] = {5.0, 7.0, 11.0, 13.0};
Uint16 prActive = 0;        // controller in use last sample, for bumpless PI <-> PR switching
Uint16 olActive = 0;        // open-loop test (VoltCtrlMode = 2) in use last sample
float32_t olMod = 0.0;      // open-loop modulation index, ramped to OL_MOD_MAX
float32_t fbTrimY = 1.0;    // VinvYSF / VinvRSF, updated every 1 ms
float32_t fbTrimB = 1.0;    // VinvBSF / VinvRSF, updated every 1 ms
float32_t fbTrimYinv = 1.0, fbTrimBinv = 1.0;                // 1/fbTrimY, 1/fbTrimB
float32_t sfOutInvR = 1.0, sfOutInvY = 1.0, sfOutInvB = 1.0; // 1/VoutxSF

inline void SendCANPackets(void);
inline void CurrentModeSelection(void);
inline void pll_and_sincos(void);

interrupt void AdcISR(void)
{
    adcStart_Timer = CpuTimer0Regs.TIM.all;
    AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;
   // GPIO_WritePin(24, 1);

    if(AdcaRegs.ADCINTOVF.bit.ADCINT1)
    {
        AdcaRegs.ADCINTOVFCLR.bit.ADCINT1 = 1; //clear INT1 overflow flag
        AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1; //clear INT1 flag
    }

    temp_AdcIsrCount++;

    // Common Controller v2 SOC map (BoardInit.c setupADC(), board_pin_map.h):
    //        SOC0        SOC1        SOC2        SOC3          SOC4
    //  A   Ir_ind A0   Ir_out A5   Ib_out A8   Vb_byp A14    Temp1 A9
    //  B   Vb_inv B3   Iy_out B8   Vy_byp B9   Vref1.5 B0    Temp3 B1
    //  C   Iy_ind C5   VdcLink C0  Vr_byp C9   Vbat C2       Temp2 C8
    //  D   Vr_inv D14  Vb_out D15  Vy_out D12  Ibat D11      -
    //  E   Ib_ind E11  Vy_inv E12  Vr_out E16  -             -
    // A14 reads Vb_byp (bypass jumper on the board, not Vload).
    AnalogInput1 = AdcdResultRegs.ADCRESULT0;   // VinvR   D14
    AnalogInput2 = AdceResultRegs.ADCRESULT1;   // VinvY   E12
    AnalogInput3 = AdcbResultRegs.ADCRESULT0;   // VinvB   B3
    AnalogInput4 = AdcaResultRegs.ADCRESULT1;   // IoutR   A5
    AnalogInput5 = AdcbResultRegs.ADCRESULT1;   // IoutY   B8
    AnalogInput6 = AdcaResultRegs.ADCRESULT2;   // IoutB   A8
    AnalogInput7 = AdcaResultRegs.ADCRESULT4;   // Temp1   A9

    AnalogInput8 = AdcbResultRegs.ADCRESULT3;   // Vref1.5 B0
    AnalogInput9 = AdccResultRegs.ADCRESULT2;   // Vr Bypass C9
    AnalogInput10 = AdcbResultRegs.ADCRESULT2;  // Vy Bypass B9
    AnalogInput11 = AdcaResultRegs.ADCRESULT3;  // Vb Bypass A14
    AnalogInput12 = AdcaResultRegs.ADCRESULT0;  // Ir Ind  A0
    AnalogInput13 = AdccResultRegs.ADCRESULT0;  // Iy Ind  C5
    AnalogInput14 = AdceResultRegs.ADCRESULT0;  // Ib Ind  E11

    AnalogInput15 = AdccResultRegs.ADCRESULT1;  // DC Link C0
    AnalogInput16 = AdccResultRegs.ADCRESULT4;  // Temp2   C8
    AnalogInput17 = AdceResultRegs.ADCRESULT2;  // Vout R  E16
    AnalogInput18 = AdcdResultRegs.ADCRESULT2;  // Vout Y  D12
    AnalogInput19 = AdcdResultRegs.ADCRESULT1;  // Vout B  D15
    AnalogInput20 = AdccResultRegs.ADCRESULT3;  // Vbat    C2
    AnalogInput21 = AdcdResultRegs.ADCRESULT3;  // Ibat    D11

    lpf_bias.u = (float) (AnalogInput8)  * FLOAT_CONVERTER_AC;
    LOWPASSFILTER(lpf_bias);

    lpf_vInvR.u = (float) (AnalogInput1 - an1Offset) * FLOAT_CONVERTER_AC - lpf_bias.u;
    lpf_vInvY.u = (float) (AnalogInput2 - an2Offset) * FLOAT_CONVERTER_AC - lpf_bias.u;
    lpf_vInvB.u = (float) (AnalogInput3 - an3Offset) * FLOAT_CONVERTER_AC - lpf_bias.u;
    LOWPASSFILTER(lpf_vInvR);
    LOWPASSFILTER(lpf_vInvY);
    LOWPASSFILTER(lpf_vInvB);

if(Master==0)
{
    if(lpf_vInvR.u >vacPeakLimit) { if(vachighTripCount<20) vachighTripCount++;}
    else {if(vachighTripCount) vachighTripCount--; }
    if(vachighTripCount==20) vacHighFault = TRUE;
    else if(vachighTripCount==0) vacHighFault = FALSE;

 }
else {vachighTripCount = 0; vacHighFault = FALSE;}

    lpf_iInvR.u = (float) (AnalogInput12 - an12Offset) * FLOAT_CONVERTER_AC - lpf_bias.u;
    lpf_iInvY.u = (float) (AnalogInput13 - an13Offset) * FLOAT_CONVERTER_AC - lpf_bias.u;
    lpf_iInvB.u = (float) (AnalogInput14 - an14Offset) * FLOAT_CONVERTER_AC - lpf_bias.u;
    LOWPASSFILTER(lpf_iInvR);
    LOWPASSFILTER(lpf_iInvY);
    LOWPASSFILTER(lpf_iInvB);

    lpf_voutR.u = (float) (AnalogInput17 - an17Offset) * FLOAT_CONVERTER_AC - lpf_bias.u;
    lpf_voutY.u = (float) (AnalogInput18 - an18Offset) * FLOAT_CONVERTER_AC - lpf_bias.u;
    lpf_voutB.u = (float) (AnalogInput19 - an19Offset) * FLOAT_CONVERTER_AC - lpf_bias.u;
    LOWPASSFILTER(lpf_voutR);
    LOWPASSFILTER(lpf_voutY);
    LOWPASSFILTER(lpf_voutB);

    lpf_ioutR.u = (float) (AnalogInput4 - an4Offset) * FLOAT_CONVERTER_AC - lpf_bias.u;
    lpf_ioutY.u = (float) (AnalogInput5 - an5Offset) * FLOAT_CONVERTER_AC - lpf_bias.u;
    lpf_ioutB.u = (float) (AnalogInput6 - an6Offset) * FLOAT_CONVERTER_AC - lpf_bias.u;
    LOWPASSFILTER(lpf_ioutR);
    LOWPASSFILTER(lpf_ioutY);
    LOWPASSFILTER(lpf_ioutB);
//
//    lpf_ioutR.y = theta_fb.sin_val * 0.2;
//    lpf_ioutY.y = theta_fb_120.sin_val * 0.1;
//    lpf_ioutB.y = theta_fb_240.sin_val * 0.15;

    lpf_vbypR.u = (float) (AnalogInput9 - an9Offset) * FLOAT_CONVERTER_AC - lpf_bias.u;
    lpf_vbypY.u = (float) (AnalogInput10 - an10Offset) * FLOAT_CONVERTER_AC - lpf_bias.u;
    lpf_vbypB.u = (float) (AnalogInput11 - an11Offset) * FLOAT_CONVERTER_AC - lpf_bias.u;
    LOWPASSFILTER(lpf_vbypR);
    LOWPASSFILTER(lpf_vbypY);
    LOWPASSFILTER(lpf_vbypB);


    lpf_vbat.u = (float)AnalogInput20 * FLOAT_CONVERTER_DC;
    LOWPASSFILTER(lpf_vbat);
    lpf_ibat.u = (float)(AnalogInput21 - IbatOffset) * FLOAT_CONVERTER_DC;
    LOWPASSFILTER(lpf_ibat);

    lpf_dclink.u = (float)(AnalogInput15) * FLOAT_CONVERTER_DC;
    LOWPASSFILTER(lpf_dclink);

     checkLimitHN(lpf_dclink.u, dcOV_Inst_Level, DCOV_Inst_Delay,DCOVTimer);

    if(DCOV_Inst_Delay == DCOVTimer) dcOV = TRUE;
    else if(DCOVTimer == 0) dcOV = FALSE;

    lpf_temp1.u = (float)AnalogInput7 * FLOAT_CONVERTER_DC;
    lpf_temp2.u = (float)AnalogInput16 * FLOAT_CONVERTER_DC;
    LOWPASSFILTER(lpf_temp1);
    LOWPASSFILTER(lpf_temp2);


//#define DUMMYSIG
#ifdef DUMMYSIG
#ifndef SHIFTED_FBACK
    #define th120   theta_inv_120
    #define th240   theta_inv_240
    #else
    #define th120   theta_fb_120
    #define th240   theta_fb_240
#endif

if(pwmEnabled)    {
    lpf_vInvR.y = theta_inv.sin_val*.271;
    lpf_vInvY.y = th120.sin_val*.271;
    lpf_vInvB.y = th240.sin_val*.271;
}
else
{
    lpf_vInvR.y = 0;
    lpf_vInvY.y = 0;
    lpf_vInvB.y = 0;
}

    lpf_vbypR.y = theta_inv.sin_val*.261;
    lpf_vbypY.y = th120.sin_val*.261;
    lpf_vbypB.y = th240.sin_val*.261;

    if(SystemFlag.bit.LOADONINV)
    {
        lpf_voutR.y = lpf_vInvR.y;
        lpf_voutY.y = lpf_vInvY.y;
        lpf_voutB.y = lpf_vInvB.y;
    }

    if(SystemFlag.bit.LOADONBYP)
    {
        lpf_voutR.y = lpf_vbypR.y;
        lpf_voutY.y = lpf_vbypY.y;
        lpf_voutB.y = lpf_vbypB.y;
    }

    lpf_iInvR.y = theta_inv.sin_val* 0.20;
    lpf_iInvY.y = th120.sin_val*.20;
    lpf_iInvB.y = th240.sin_val*.20;

    lpf_ioutR.y = theta_inv.sin_val* 0.20;
    lpf_ioutY.y = th120.sin_val*.20;
    lpf_ioutB.y = th240.sin_val*.20;
    lpf_dclink.y = 0.4;

#endif


   /*** Reference current calculation ***/
        // Per-phase sensor calibration applied to the feedback, relative to R
        // (the voltage reference is already scaled by VinvRSF). Without it a
        // sensor gain mismatch looks like an imbalance -> 100 Hz ripple in d-q.
        vcontrol.vol.a = lpf_vInvR.y;
        vcontrol.vol.b = lpf_vInvY.y * fbTrimY;
        vcontrol.vol.c = lpf_vInvB.y * fbTrimB;

        // Output voltages for measurement and plots with the SVPWM common-mode
        // (zero-sequence) part removed: the sensors are not referenced to the load
        // neutral. Weighted by each channel's scale factor so a gain mismatch does
        // not leak into the common-mode estimate. lpf_vout*.y (filter state) is untouched.
        {
            float32_t zo = (lpf_voutR.y*VoutRSF + lpf_voutY.y*VoutYSF + lpf_voutB.y*VoutBSF) * (1.0/3.0);
            vOutR = lpf_voutR.y - zo*sfOutInvR;
            vOutY = lpf_voutY.y - zo*sfOutInvY;
            vOutB = lpf_voutB.y - zo*sfOutInvB;
        }
       vcontrol.theta_o = theta_inv;


   #ifdef SHIFTED_FBACK
       vcontrol.theta = theta_fb;
   #else
       vcontrol.theta = theta_inv;
   #endif

       getReferenceVoltage();
       getReferenceCurrent();

//***********************************************************************************

       if(Master==0)
           {
               CurrentModeSelection();
               if(MasterPresentTimer>5) CommStatus.bit.CAN_MASTER =  TRUE;
               else  CommStatus.bit.CAN_MASTER =  FALSE;
           }

//-------------------------------------------------------------------------------------
       SendCANPackets();
       pll_and_sincos();
//-----------------------------------------------------------------------------------------------------------------

#ifndef SHIFTED_FBACK
            theta_inv_120.sin_val = sinf(angleActive + ANGLE120);
            theta_inv_240.sin_val = sinf(angleActive + ANGLE240);
#else
             theta_fb.sin_val = sinf(angleActive + Angle30);
             theta_fb.cos_val = cosf(angleActive + Angle30);
             theta_fb_120.sin_val = sinf(angleActive + ANGLE120 + Angle30);
             theta_fb_240.sin_val = sinf(angleActive + ANGLE240 + Angle30);
#endif

//---------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
             if(Synced && (syncTimer==0) && (bypassFailed==0) && (Master || !CommStatus.bit.CAN_MASTER))
             {
                 bypSin = pllBypass.theta.sin_val;
                 bypCos = pllBypass.theta.cos_val;
                 omegaActive = 2;
             }
             else
             {
                 bypSin = theta_inv.sin_val;
                 bypCos = theta_inv.cos_val;
                 omegaActive = 1;
                // omegaShift = 0.00001;
             }
                  if((bypSin >= 0.0) && (prevSin < 0.0) && (bypCos > 0.8))
                      {
                          Angle = 0;

                          if(LimitFlag.bit.BYPASSOK && (frequencyMatch))
                          {
                              if( (pllBypass.theta.cos_val > 0.8) && (fabsf(pllBypass.theta.sin_val) < LockingLevel) && (Synced==FALSE))
                                  {
                                         Synced = TRUE;
                                         SystemFlag.bit.NOTSYNCED = FALSE;
                                         syncTimer = 1000;
                                  }
                          }
                          rsync = 1;

                          samplesPerCycle = zcdptr;
                          if(samplesPerCycle < MINPOINTS) samplesPerCycle = FULLCYCLE;
                          nPoints = samplesPerCycle >> 2;

                          zcdptr = 0;
                          if (snap_end == 0) snap_start = 1;

                          if(pwmEnabled && (pulseON==0))
                          {
                              ENABLE_PWM();

                          }

                      } //rsync condition

                  prevSin = bypSin;
                  zcdptr++;
//--------------------------------------------------------------------------------------------------
        // (debug DAC output of bypSin removed: no free DAC pin on the Common Controller v2)

        rmsCal();

        adcStart_Timer -= (long) CpuTimer0Regs.TIM.all;      // timer counts down, 1 count = 1 SYSCLK
        if(adcStart_Timer < 0) adcStart_Timer += (long) CpuTimer0Regs.PRD.all + 1;
        adcIsrTime = (int) adcStart_Timer;
        if(adrIsrDuration < adcIsrTime) adrIsrDuration = adcIsrTime;

//-------------------------------------------------------------------------------------------------------
     //   GPIO_WritePin(24, 0);
       PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;// Acknowledge interrupt to PIE
 }


inline void getReferenceCurrent(void)
{
float   tmpAlpha = 0;
float   tmpBeta = 0;
float   tmpZero = 0;

    RYB2AB_F(tmpAlpha, tmpBeta, tmpZero, lpf_ioutR.y, lpf_ioutY.y, lpf_ioutB.y);
    AB2DQ_F(cur.lpf_de.u,cur.lpf_qe.u,tmpAlpha,tmpBeta,vcontrol.theta);

    LOWPASSFILTER(cur.lpf_de);
    LOWPASSFILTER(cur.lpf_qe);

    cur.pi_qe.Ref = 0;
    cur.pi_qe.Fbk = cur.lpf_qe.y;
    PIREGULATOR(cur.pi_qe);

    cur.pi_de.Ref = newCurReference;
    cur.pi_de.Fbk = cur.lpf_de.y;
    PIREGULATOR(cur.pi_de);
//--------------------------------------------------------------------------------
    AB2DQN_F(cur_neg.lpf_de.u,cur_neg.lpf_qe.u,tmpAlpha,tmpBeta,vcontrol.theta);
    LOWPASSFILTER(cur_neg.lpf_de);
    LOWPASSFILTER(cur_neg.lpf_qe);

    cur_neg.pi_qe.Ref = 0;
    cur_neg.pi_qe.Fbk = cur_neg.lpf_qe.y;
    PIREGULATOR(cur_neg.pi_qe);

    cur_neg.pi_de.Ref = 0;
    cur_neg.pi_de.Fbk = cur_neg.lpf_de.y;
    PIREGULATOR(cur_neg.pi_de);

    DQ2AB_F(Cur_neg_Alpha, Cur_neg_Beta, cur_neg.pi_de.Out, cur_neg.pi_qe.Out, vcontrol.theta_o);

       ffvd = (0.98) - cur.pi_de.Umax;
       cross = (0.05);
       ccvd = cur.lpf_qe.y*cross;
       ccvq = cur.lpf_de.y*cross;

       Cur_De = cur.pi_de.Out + ffvd + ccvd;
       Cur_Qe = cur.pi_qe.Out  - ccvq + pllBypass.lpf1.y;
       DQ2AB_F(Cur_Alpha, Cur_Beta, Cur_De, Cur_Qe, vcontrol.theta_o);
       if(UnbalanceCorrection)
       {
           Cur_Alpha += Cur_neg_Alpha;
           Cur_Beta += Cur_neg_Beta;
       }

}

inline void getReferenceVoltage(void)
{
        RYB2AB_F(vcontrol.vol.d,vcontrol.vol.q,vcontrol.vol.z,vcontrol.vol.a,vcontrol.vol.b,vcontrol.vol.c);
    //ALPHA_BETA to DQ
        AB2DQ_F(vcontrol.vol.lpf_de.u,vcontrol.vol.lpf_qe.u,vcontrol.vol.d,vcontrol.vol.q,vcontrol.theta);

//ALPHA_BETA to DQ
    AB2DQ_F(vcontrol.vol.lpf_de.u,vcontrol.vol.lpf_qe.u,vcontrol.vol.d,vcontrol.vol.q,vcontrol.theta);

    LOWPASSFILTER(vcontrol.vol.lpf_de);
    vcontrol.vol.de = vcontrol.vol.lpf_de.y;

    if(vcontrol.vol.lpf_de.y < inverterFailLevel)
    {
            if(invfCount<10) invfCount++;
            if(invfCount==10)
                {
                    inverterFailed = TRUE;
                    SystemFlag.bit.INVFAILURE = TRUE;
                    invokcounter = 0;
                }
    }

    else {
            if(invfCount) invfCount--;
            if(invokcounter<50) invokcounter++;
            if(invokcounter>=50) { inverterFailed = FALSE; invfCount = 0; SystemFlag.bit.INVFAILURE = FALSE;}
        }

    LOWPASSFILTER(vcontrol.vol.lpf_qe);
    vcontrol.vol.qe = vcontrol.vol.lpf_qe.y;

if(VoltCtrlMode > 1.5)
{
    //------------------- open-loop test: fixed balanced modulation -------------------
    // No voltage feedback, no Y/B factors: shows whether an imbalance comes from the
    // power stage / transformer rather than from the controller.
    if(!olActive)
    {
        // start from the present output level so entering the test is bumpless
        olMod = sqrtf(vcontrol.alpha*vcontrol.alpha + vcontrol.beta*vcontrol.beta);
        olActive = 1;
    }
    if(pwmEnabled)
    {
        olMod += OL_MOD_STEP;
        if(olMod > OL_MOD_MAX) olMod = OL_MOD_MAX;
    }
    else olMod = 0.0;

    vcontrol.alpha =  olMod*vcontrol.theta_o.sin_val;
    vcontrol.beta  = -olMod*vcontrol.theta_o.cos_val;
    AB2DQ_F(vcontrol.deo, vcontrol.qeo, vcontrol.alpha, vcontrol.beta, vcontrol.theta_o);
    AB2RYB_F(vcontrol.refa, vcontrol.refb, vcontrol.refc, vcontrol.alpha, vcontrol.beta);

    // keep PI preloaded for a bumpless return; PR is preloaded by the switch below
    vcontrol.vol.pid_de.data.i1 = vcontrol.deo;
    vcontrol.vol.pid_qe.data.i1 = vcontrol.qeo;
    prActive = 0;
}
else
{
olActive = 0;

if((VoltCtrlMode > 0.5) != prActive)
{
    // bumpless switch: preload the incoming controller with the present output
    if(prActive == 0)
    {
        PR_PRELOAD_AB(vcontrol.alpha, vcontrol.beta);
        for(prActive = 0; prActive < PR_NHARM; prActive++)
        {
            prH[prActive][0].Y = 0; prH[prActive][0].X = 0;
            prH[prActive][1].Y = 0; prH[prActive][1].X = 0;
        }
        prActive = 1;
    }
    else
    {
        vcontrol.vol.pid_de.data.i1 = vcontrol.deo;
        vcontrol.vol.pid_qe.data.i1 = vcontrol.qeo;
        prActive = 0;
    }
}

if(prActive)
{
    //------------------- PR control in alpha-beta ------------------------------------
    // feedback rotated by -Angle30 (plane rotation: -30 for +ve seq, +30 for -ve seq)
    float32_t fa, fb, ea, eb, ua, ub;
    Uint16 h;

    fa = vcontrol.vol.d*prRotC + vcontrol.vol.q*prRotS;
    fb = vcontrol.vol.q*prRotC - vcontrol.vol.d*prRotS;

    ea =  soft_VdRef*vcontrol.theta_o.sin_val - fa;
    eb = -soft_VdRef*vcontrol.theta_o.cos_val - fb;

    PR_RES_MACRO(prF[0], prKF, ea, prCoefF, prWd, PR_FUND_LIMIT);
    PR_RES_MACRO(prF[1], prKF, eb, prCoefF, prWd, PR_FUND_LIMIT);

    ua = inv_kp*ea + prF[0].out;
    ub = inv_kp*eb + prF[1].out;

    for(h = 0; h < PR_NHARM; h++)
    {
        if((prHMask & (1 << h)) && !SystemFlag.bit.SOFTSTART && !currentMode)
        {
            PR_RES_MACRO(prH[h][0], prKH, ea, prCoefH[h], prWd, pr_hLimit);
            PR_RES_MACRO(prH[h][1], prKH, eb, prCoefH[h], prWd, pr_hLimit);
            ua += prH[h][0].out;
            ub += prH[h][1].out;
        }
        else
        {
            prH[h][0].Y = 0; prH[h][0].X = 0;
            prH[h][1].Y = 0; prH[h][1].X = 0;
        }
    }

   if( (xfr2InvTimer==0) && (addOn>0))
        {
           ua += addOn*vcontrol.theta_o.sin_val;
           ub -= addOn*vcontrol.theta_o.cos_val;
           addOn -= 0.00002;
        }

   vcontrol.alpha = ua;
   vcontrol.beta = ub;
   AB2DQ_F(vcontrol.deo, vcontrol.qeo, ua, ub, vcontrol.theta_o);
   AB2RYB_F(vcontrol.refa, vcontrol.refb, vcontrol.refc, ua, ub);
}
else
{
    vcontrol.vol.pid_qe.term.Ref = (0.0);
    vcontrol.vol.pid_qe.term.Fbk = vcontrol.vol.qe;
    PID_GRANDO_F_MACRO(vcontrol.vol.pid_qe);

    vcontrol.vol.pid_de.term.Ref = soft_VdRef;
    vcontrol.vol.pid_de.term.Fbk = vcontrol.vol.de;
    PID_GRANDO_F_MACRO(vcontrol.vol.pid_de);

    vcontrol.deo = vcontrol.vol.pid_de.term.Out;
    vcontrol.qeo = vcontrol.vol.pid_qe.term.Out;

   if( (xfr2InvTimer==0) && (addOn>0))
        {
           vcontrol.deo += addOn;
           addOn -= 0.00002;
        }

   DQ2AB_F(vcontrol.alpha, vcontrol.beta, vcontrol.deo, vcontrol.qeo, vcontrol.theta_o);
   AB2RYB_F(vcontrol.refa, vcontrol.refb, vcontrol.refc, vcontrol.alpha, vcontrol.beta);

   //------------------------------------------------------------------------------------------
    vcontrol.refb = vcontrol.refb*Yfactor;
    vcontrol.refc = vcontrol.refc*Bfactor;
}
}   // closed loop (PI / PR)

    if(vcontrol.refa > SAT_LIMIT)  vcontrol.refa = SAT_LIMIT;
    if(vcontrol.refb > SAT_LIMIT)  vcontrol.refb = SAT_LIMIT;
    if(vcontrol.refc > SAT_LIMIT)  vcontrol.refc = SAT_LIMIT;
    if(vcontrol.refa < SAT_LIMITN) vcontrol.refa = SAT_LIMITN;
    if(vcontrol.refb < SAT_LIMITN) vcontrol.refb = SAT_LIMITN;
    if(vcontrol.refc < SAT_LIMITN) vcontrol.refc = SAT_LIMITN;


}


__interrupt void cpu_timer0_isr(void) {
//	float32_t temp_omega = 0;
 	CpuTimer0.InterruptCount++;
	mainLoopCount++;
	temp_T0IsrCount++;

    saveCounter++;
    saveCounter &= 1023;

        if (snap_start == 1) {
            if ((saveCounter % nCycles) == 0)
            {
                waveBuffer[snap_ptr] = *waveptr;
                snap_ptr++;

                if (snap_ptr >= SNAP_SIZE) {
                    snap_start = 0;
                    snap_end = 1;
                    snap_ptr = 0;   //count starts from 0 to SnapSize
                }
            }
        }

	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}


N16 volatile LedShiftFlag = 0;

/*
 Routine tasks like IO control, delays etc runs at 1ms
 */
N16 volatile fTimesCount = 0;
__interrupt void cpu_timer1_isr(void)
{
    N16 volatile iii = 0;

	CpuTimer1.InterruptCount++;
	mainLoopCount_prev = mainLoopCount;
	mainLoopCount = 0;

	if(syncTimer) syncTimer--;

	PR_UpdateCoeffs();

	fbTrimY = VinvYSF / VinvRSF;    // scale factors are range limited to 10..5000
	fbTrimB = VinvBSF / VinvRSF;
	fbTrimYinv = VinvRSF / VinvYSF;
	fbTrimBinv = VinvRSF / VinvBSF;
	sfOutInvR = 1.0 / VoutRSF;
	sfOutInvY = 1.0 / VoutYSF;
	sfOutInvB = 1.0 / VoutBSF;

	// CANOKtimer: reloaded by each CAN-B paralleling frame (mcanInterface.c).
	// No re-init on silence: a single unit has no CAN-B traffic; bus-off is
	// handled in the main loop.
	if(CANOKtimer) CANOKtimer--;

	if(currentMode) LimitFlag.bit.CURMODE = TRUE;
	else LimitFlag.bit.CURMODE = FALSE;

	lpf_curRef.u = (remoteIref + cur.lpf_de.y) * 0.5;
	LOWPASSFILTER(lpf_curRef);
	if(lpf_curRef.y>0.0) newCurReference = lpf_curRef.y;
	else newCurReference =0.0;

    // (GPIO21 pulse-on flag removed: GPIO21 is DI_3 on the Common Controller v2)

    if(waitTimer) waitTimer--;

    #define OMEGA_INCDEC 0.00000015708
    slaveOnInverter = PairStatusFlag.bit.LOADONINV;
    slaveOnBypass =  PairStatusFlag.bit.LOADONBYP;
    slaveDisabled = PairStatusFlag.bit.INVRTRDISABL;
    slaveInvOK = PairStatusFlag.bit.INVOK;

    if(Master==0) StaticSwitchControl_Slave ();
        else
            {
                StaticSwitchControl_Master();
                            //OMEGA TRACKING
                            if(LimitFlag.bit.BYPASSOK && !bypassFailed)
                             {
                                 if((pll_omega > PLL_LOCK_MIN) && (pll_omega < PLL_LOCK_MAX))
                                 {
                                     if( (omega_inv - omegaShift) > pll_omega)
                                     {
                                         omega_inv -= 0.000001;
                                     }
                                     else if((omega_inv - omegaShift) < pll_omega)
                                     {
                                         omega_inv += 0.000001;
                                     }

                                     if(fabsf(omega_inv - pll_omega) < omegaShift)
                                     {
                                         if(fTimesCount<10) fTimesCount++;
                                         if(fTimesCount==10) frequencyMatch = TRUE;
                                     }
                                     else if(fTimesCount) fTimesCount--;

                                     if(Synced) omega_inv = pll_omega;

                                 }
                                 else    frequencyMatch = FALSE;
                             } // Omega Within Range


                             else
                             {
                                 frequencyMatch = FALSE;
                                 if(omega_inv > defaultOmega) omega_inv -= OMEGA_INCDEC;
                                 else if(omega_inv < defaultOmega) omega_inv += OMEGA_INCDEC;
                                 SystemFlag.bit.NOTSYNCED = TRUE;
                                 Synced = FALSE;
                             }
                        }//10ms once
     //____________________________OMEGA TRACKING END__________________________

	 if (interchar_silent_timer < INTERCHARDELAY)    interchar_silent_timer++;
	 if (interchar_silent_timer1 < INTERCHARDELAY)   interchar_silent_timer1++;

	    if (spi_timeout < SPI_TIME)
	        spi_timeout++;

	    if(msDelayCount)    msDelayCount--;

if(Master == 0)
{
	    if(MasterPresentTimer )    MasterPresentTimer--;
	    else
	    {
                PairStatusFlag.all  = 0;
                CommStatus.bit.CAN_MASTER =  FALSE;
	    }
	    //else Master = 1;
}

        CheckHiLo(&chkVr);
        CheckHiLo(&chkVy);
        CheckHiLo(&chkVb);

        CheckHiLo(&chkIr1);
        CheckHiLo(&chkIy1);
        CheckHiLo(&chkIb1);

        CheckHiLo(&chkIr2);
        CheckHiLo(&chkIy2);
        CheckHiLo(&chkIb2);
        CheckHiLo(&chkIr3);
        CheckHiLo(&chkIy3);
        CheckHiLo(&chkIb3);

        CheckHiLo(&chkVbypr);
        CheckHiLo(&chkVbypy);
        CheckHiLo(&chkVbypb);

        CheckHiLo(&chkVbat);
        CheckHiLo(&chkVbatpre);
        CheckHiLo(&chkIbat);
        CheckHiLo(&chkdcLink);

        if(IoutR < (InvCurHL_Lvl1a - 2.0))  chkIr1.maxTimer = 0;
        if(IoutR < (InvCurHL_Lvl2a - 2.0))   chkIr2.maxTimer = 0;
        if(IoutR < (InvCurHL_Lvl3a - 2.0))   chkIr3.maxTimer = 0;

        IdRef = (cur.lpf_de.y + slave_Id)* 0.5;

	    milliseconds++;

	        if (milliseconds> 999)
	        {
	            milliseconds = 0;
	            seconds++;
	            txMsgCount = txMsgCount_tmp;
	            txMsgCount_tmp = 0;

	            canRxCounter = tmp_canRxCounter;
	            tmp_canRxCounter = 0;

	            FSW_Inv = temp_fsw1;
	            temp_fsw1 = 0;
	            AdcIsrCount = temp_AdcIsrCount;
	            temp_AdcIsrCount = 0;
	            T0IsrCount = temp_T0IsrCount;
	            temp_T0IsrCount = 0;

	            RTCSecond++;

	            if(seconds > 59)
	                {
	                    seconds = 0;
	                    minutes++;
	                    if(minutes>59)
	                         {
	                            minutes = 0;
//	                            hours++ ;
//	                            if (hours>23) { hours = 0; days++; }
	                         }
	                    RTCSecond = 0;
	                    RTCMinute++;
	                    if(RTCMinute>59)
	                        {
	                        RTCMinute = 0;
	                        RTCHour++;
	                            if(RTCHour>23)
	                            {
	                                RTCDate++;
	                            }
	                        }
	                }

	            if(systemRunningTimer < FaultResetDelay) systemRunningTimer++;
	            if((systemRunningTimer == FaultResetDelay) && (SystemFlag.bit.LATCHFAULT == FALSE) ) TripCount = 0;

	                if(restartTimer)
	                {
	                    restartTimer--;

	                    if( (TripCount < MaxRestartCount) && (restartTimer==0))
	                    {
	                        AUTO_RESET_FAULTS();

	                    }
	                }

	                if(xfr2InvTimer>xfrToInverterDelay) xfr2InvTimer = xfrToInverterDelay;
	                if(xfr2InvTimer) xfr2InvTimer--;
	                if(xfr2BypTimer>xfrToBypassDelay) xfr2BypTimer = xfrToBypassDelay;
	                if(xfr2BypTimer) xfr2BypTimer--;

	                if(AutoSaveTimer) AutoSaveTimer--;
	                if(AutoSaveTimer==1) autoSaveFlag = TRUE;

	                if(mainsFailTimer) mainsFailTimer--;
	        } //seconds condition

	        if(SystemFlag.bit.POWERINGUP)
	        {
	            if (poweronTimer < PowerOnDelay) poweronTimer++;
	        else    SystemFlag.bit.POWERINGUP = FALSE;

	        }

	            if (spi_timeout < SPI_TIME)
	                spi_timeout++;

	            if(msDelayCount)    msDelayCount--;

	        // SYSLED (GPIO62, LED5): 5 Hz when running, 1 Hz when stopped
	        if(pwmEnabled)
	        {
	            { if(milliseconds%100 == 0) GPIO_WritePin(62,2); }
	        }

	        else
	            { if(milliseconds%500 == 0) GPIO_WritePin(62,2); }

	        if (SystemFlag.bit.SOFTSTART || ReferenceChangedFlag )// && (startDelayTimer==0))
	        {
	                if (soft_VdRef < volRef_Id) soft_VdRef += ref_inc;

	                else SystemFlag.bit.SOFTSTART = 0;
	                if (soft_VqRef < volRef_Iq) soft_VqRef += ref_inc;


	                if(soft_VdRef > volRef_Id) soft_VdRef = volRef_Id;
	                if(soft_VqRef > volRef_Iq) soft_VqRef = volRef_Iq;

	                if(soft_VdRef == volRef_Id) ReferenceChangedFlag = FALSE;

	        }

            if(DI_NO_Flag & 0x0001) { checkDI_NO(DINP1,DI1timer,DI_DELAY); }
            else { checkDI_NC(DINP1,DI1timer,DI_DELAY); }
            if(DI_NO_Flag & 0x0002) { checkDI_NO(DINP2,DI2timer,DI_DELAY); }
            else { checkDI_NC(DINP2,DI2timer,DI_DELAY); }
            if(DI_NO_Flag & 0x0004) { checkDI_NO(DINP3,DI3timer,DI_DELAY); }
            else { checkDI_NC(DINP3,DI3timer,DI_DELAY); }
            if(DI_NO_Flag & 0x0008) { checkDI_NO(DINP4,DI4timer,DI_DELAY); }
            else { checkDI_NC(DINP4,DI4timer,DI_DELAY); }
            if(DI_NO_Flag & 0x0010) { checkDI_NO(DINP5,DI5timer,DI_DELAY); }
            else { checkDI_NC(DINP5,DI5timer,DI_DELAY); }
            if(DI_NO_Flag & 0x0020) { checkDI_NO(DINP6,DI6timer,DI_DELAY); }
            else { checkDI_NC(DINP6,DI6timer,DI_DELAY); }
            if(DI_NO_Flag & 0x0040) { checkDI_NO(DINP7,DI7timer,DI_DELAY); }
            else { checkDI_NC(DINP7,DI7timer,DI_DELAY); }
            if(DI_NO_Flag & 0x0080) { checkDI_NO(DINP8,DI8timer,DI_DELAY); }
            else { checkDI_NC(DINP8,DI8timer,DI_DELAY); }
            if(DI_NO_Flag & 0x0100) { checkDI_NO(DINP9,DI9timer,DI_DELAY); }
            else { checkDI_NC(DINP9,DI9timer,DI_DELAY); }
            if(DI_NO_Flag & 0x0200) { checkDI_NO(DINP10,DI10timer,DI_DELAY); }
            else { checkDI_NC(DINP10,DI10timer,DI_DELAY); }
//-----------------------------------------------------------------------

#define START_SAMPLE    1000
#define BIAS_CHANNEL    AnalogInput8

if(OffsetNotDone && (poweronTimer>=START_SAMPLE) && (poweronTimer<(1000+START_SAMPLE)))
  {
      offsets[1] += AnalogInput1 - BIAS_CHANNEL;
      offsets[2] += AnalogInput2 - BIAS_CHANNEL;
      offsets[3] += AnalogInput3 - BIAS_CHANNEL;

      offsets[4] += AnalogInput4 - BIAS_CHANNEL;
      offsets[5] += AnalogInput5 - BIAS_CHANNEL;
      offsets[6] += AnalogInput6 - BIAS_CHANNEL;

      offsets[9] += AnalogInput9 - BIAS_CHANNEL;
      offsets[10] += AnalogInput10 - BIAS_CHANNEL;
      offsets[11] += AnalogInput11 - BIAS_CHANNEL;
      offsets[12] += AnalogInput12 - BIAS_CHANNEL;
      offsets[13] += AnalogInput13 - BIAS_CHANNEL;
      offsets[14] += AnalogInput14 - BIAS_CHANNEL;
      offsets[15] += AnalogInput15 - BIAS_CHANNEL;

      offsets[17] += AnalogInput17 - BIAS_CHANNEL;
      offsets[18] += AnalogInput18 - BIAS_CHANNEL;
      offsets[19] += AnalogInput19 - BIAS_CHANNEL;

int g=0;
      if(poweronTimer==(START_SAMPLE + 999))
      {
          for(g=1;g<20;g++)
              ArrayParamsI[159+g] = offsets[g]/1000;
          OffsetNotDone = FALSE;
      }
  }

}

__interrupt void cpu_timer2_isr(void) {

	CpuTimer2.InterruptCount++;

}

interrupt void SCIRxISR(void) {

	N16 temp_char = 0;

	errorRx = SciaRegs.SCIRXST.bit.RXERROR;
	if (!errorRx)
	{

	//temp_char = SCI_readCharNonBlocking(SCIA_BASE);//SciaRegs.SCIRXBUF.all;

	temp_char = SciaRegs.SCIRXBUF.all;


	interchar_silent_timer = 0;
	rxbuffer = &rx_buffer[0];

	if ((live_frame == FALSE) && (temp_char == address)) {
		char_waiting = 0;
		live_frame = TRUE;
		frame_complete = FALSE;
	}
	//rxata_block[char_waiting] = (char) temp_char;
	*(rxbuffer + char_waiting) = temp_char;
	++char_waiting;
	char_waiting &= 0x1f; //to prevent over flow
	}

	PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;
}

// SCIB (RS485 #1, GUI port) receive. Framing / break errors on an idle or
// unconnected RS485 line are counted and the receiver is software-reset here;
// they must not touch errorRx, which makes main() re-initialise SCIA.
interrupt void SCIBRx_ISR(void)
{
    N16 temp_char1 = 0;

    if (ScibRegs.SCIRXST.bit.RXERROR)
    {
        temp_char1 = ScibRegs.SCIRXBUF.all;     // discard
        errorRx1++;
        ScibRegs.SCICTL1.bit.SWRESET = 0;       // clears RXERROR / BRKDT / FE / OE
        ScibRegs.SCICTL1.bit.SWRESET = 1;
        live_frame1 = FALSE;
    }
    else
    {
        temp_char1 = ScibRegs.SCIRXBUF.all;
        rxbuffer1 = &rx_buffer1[0];

        if ((live_frame1 == FALSE) && (temp_char1 == address1) && (interchar_silent_timer1 == INTERCHARDELAY))
        {
            char_waiting1 = 0;
            live_frame1 = TRUE;
            frame_complete1 = FALSE;
        }

        *(rxbuffer1 + char_waiting1) = temp_char1;
        ++char_waiting1;
        char_waiting1 &= 0x3f;      // to prevent overflow
        interchar_silent_timer1 = 0;
    }

    PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;
}


//-----------------------------------------------------------------


void PR_Reset(void)
{
    Uint16 h;

    prF[0].Y = 0; prF[0].X = 0; prF[0].out = 0;
    prF[1].Y = 0; prF[1].X = 0; prF[1].out = 0;
    for(h = 0; h < PR_NHARM; h++)
    {
        prH[h][0].Y = 0; prH[h][0].X = 0; prH[h][0].out = 0;
        prH[h][1].Y = 0; prH[h][1].X = 0; prH[h][1].out = 0;
    }
}

/*
 Resonator coefficients, frequency adaptive. Called every 1ms.
 w is the per-sample angle increment used by pll_and_sincos()
 */
void PR_UpdateCoeffs(void)
{
    float32_t w, wh, phi;
    Uint16 h;

    if((Master==1) || !CommStatus.bit.CAN_MASTER) w = omega_inv;
    else w = remoteOmega;
    if((w < PLL_LOCK_MIN*0.9) || (w > PLL_LOCK_MAX*1.1)) w = OMEGADEFAULT;

    prCoefF.a = 2.0*sinf(0.5*w);
    phi = w*pr_delay;
    prCoefF.c = cosf(phi);
    prCoefF.s = sinf(phi);

    for(h = 0; h < PR_NHARM; h++)
    {
        float32_t phExtra;      // per-harmonic plant phase correction, degrees

        if(h == 0)      phExtra = pr_ph5;
        else if(h == 1) phExtra = pr_ph7;
        else            phExtra = pr_ph11;      // 11th and 13th

        wh = prOrder[h]*w;
        prCoefH[h].a = 2.0*sinf(0.5*wh);
        phi = wh*pr_delay + phExtra*(PI/180.0);
        prCoefH[h].c = cosf(phi);
        prCoefH[h].s = sinf(phi);
    }

    prKF = 2.0*inv_ki;
    prKH = pr_kh;
    prWd = 2.0*pr_wc*SAMPLING_TIME;
    prHMask = (Uint16) pr_hMask;

#ifdef SHIFTED_FBACK
    prRotC = cosf(Angle30);
    prRotS = sinf(Angle30);
#else
    prRotC = 1.0;
    prRotS = 0.0;
#endif
}

inline void rmsCal(void)
{

    /* Accumulate squared value of grid voltages for mean square */
    // inverter voltages without the common-mode part (vol.z, from the trimmed
    // feedback), converted back to each channel's own scale for its scale factor
    UPDATE_RMS(volt_meter_R, (vcontrol.vol.a - vcontrol.vol.z));
    UPDATE_RMS(volt_meter_Y, (vcontrol.vol.b - vcontrol.vol.z) * fbTrimYinv);
    UPDATE_RMS(volt_meter_B, (vcontrol.vol.c - vcontrol.vol.z) * fbTrimBinv);
    UPDATE_RMS(cur_meter_R, lpf_iInvR.y);
    UPDATE_RMS(cur_meter_Y, lpf_iInvY.y);
    UPDATE_RMS(cur_meter_B, lpf_iInvB.y);


    UPDATE_RMS(iout_meter_R, lpf_ioutR.y);
    UPDATE_RMS(iout_meter_Y, lpf_ioutY.y);
    UPDATE_RMS(iout_meter_B, lpf_ioutB.y);

    UPDATE_RMS(vout_meter_R, vOutR);
    UPDATE_RMS(vout_meter_Y, vOutY);
    UPDATE_RMS(vout_meter_B, vOutB);

    UPDATE_RMS(vll_meter_R, (vOutR - vOutY));
    UPDATE_RMS(vll_meter_Y, (vOutY - vOutB));
    UPDATE_RMS(vll_meter_B, (vOutB - vOutR));

    UPDATE_RMS(vbyp_meter_R, lpf_vbypR.y);
    UPDATE_RMS(vbyp_meter_Y, lpf_vbypY.y);
    UPDATE_RMS(vbyp_meter_B, lpf_vbypB.y);

    vry = (lpf_vbypR.y - lpf_vbypY.y);
    vyb = (lpf_vbypY.y - lpf_vbypB.y);
    vbr = (lpf_vbypB.y - lpf_vbypR.y);

    UPDATE_RMS(vllb_meter_R, vry);
    UPDATE_RMS(vllb_meter_Y, vyb);
    UPDATE_RMS(vllb_meter_B, vbr);

    UPDATE_KW(kw_meter_R, vOutR, lpf_ioutR.y, VoutRSF, IoutRSF);
    UPDATE_KW(kw_meter_Y, vOutY, lpf_ioutY.y, VoutYSF, IoutYSF);
    UPDATE_KW(kw_meter_B, vOutB, lpf_ioutB.y, VoutBSF, IoutBSF);

    if (rsync) {

        rms_samplecount = volt_meter_R.sample_count;

        SAVE_METER(volt_meter_R);
        SAVE_METER(volt_meter_Y);
        SAVE_METER(volt_meter_B);

        SAVE_METER(cur_meter_R);
        SAVE_METER(cur_meter_Y);
        SAVE_METER(cur_meter_B);
        SAVE_METER(vout_meter_R);
        SAVE_METER(vout_meter_Y);
        SAVE_METER(vout_meter_B);

        SAVE_METER(vll_meter_R);
        SAVE_METER(vll_meter_Y);
        SAVE_METER(vll_meter_B);

        SAVE_METER(vllb_meter_R);
        SAVE_METER(vllb_meter_Y);
        SAVE_METER(vllb_meter_B);

        SAVE_METER(iout_meter_R);
        SAVE_METER(iout_meter_Y);
        SAVE_METER(iout_meter_B);
        SAVE_METER(vbyp_meter_R);
        SAVE_METER(vbyp_meter_Y);
        SAVE_METER(vbyp_meter_B);

        SAVE_METER(kw_meter_R);
        SAVE_METER(kw_meter_Y);
        SAVE_METER(kw_meter_B);

        rms_update_flag = 1;
        rsync = 0;
    }

}
inline void pll_and_sincos(void)
{
        pllBypass.va =  lpf_vbypR.y;
        pllBypass.vb =  lpf_vbypY.y;
        pllBypass.vc =  lpf_vbypB.y;
        pll_omega = phaseLockedLoopB(&pllBypass);

if((Master==1) || !CommStatus.bit.CAN_MASTER)
     {
         Angle += omega_inv;
         if(Angle>= TWOPI) Angle -= TWOPI;
         angleActive =  Angle;
     }
else if(Master==0)
     {
         remoteAngle += remoteOmega;
         if(remoteAngle >= TWOPI) remoteAngle -= TWOPI;
         angleActive =  remoteAngle;// + PhaseShift;
         Synced = TRUE;
         SystemFlag.bit.NOTSYNCED = FALSE;
         Angle = angleActive;
     }
    theta_inv.sin_val = sinf(angleActive);
    theta_inv.cos_val = cosf(angleActive);
}
inline void CurrentModeSelection(void)
{
    if(!SystemFlag.bit.LOADONINV && !SystemFlag.bit.LOADONBYP)
               {
                   if(VoutR > 180)MasterVoltagePresent = TRUE;
                   else if(VoutR < 150) MasterVoltagePresent = FALSE;
               }
           if(!PairStatusFlag.bit.INVRTRDISABL && PairStatusFlag.bit.LOADONINV &&
                   !PairStatusFlag.bit.OUTMCBOFF && PairStatusFlag.bit.BITSVALID) MasterOnline = TRUE;
           else { MasterOnline = FALSE; MasterVoltagePresent = FALSE; }
           if(MasterPresentTimer>10) CommStatus.bit.CAN_MASTER =  TRUE;
           else CommStatus.bit.CAN_MASTER =  FALSE;
    if( (Master==0) && MasterOnline && (vacHighFault==0) && !UPSFlag.bit.OUTMCBOFF)
    {
        if(MasterOnline && SystemFlag.bit.LOADONINV)
               {
                   if(currentMode==FALSE)
                       {
                           if(VoltCtrlMode > 0.5) cur.pi_de.ui = vcontrol.deo * 0.4;
                           else cur.pi_de.ui = vcontrol.vol.pid_de.data.ui  * 0.4;
                       }
                   currentMode = TRUE;
                   capCount = 0;
                   CaptureNow = TRUE;
               }
           else  {
                   if(currentMode) //leaving
                   {
                       vcontrol.vol.pid_de.data.i1 = Cur_De * 2.0;
                       vcontrol.vol.pid_qe.data.i1 = Cur_Qe * 2.0;
                       PR_PRELOAD();
                   }
                       currentMode = FALSE;
                       capCount = 0;
                       CaptureNow = TRUE;
                 }
    } //voltage mode
    else {
                if(currentMode) //first entry to voltage mode
                {
                    vcontrol.vol.pid_de.data.i1 = Cur_De * 2.0;
                    vcontrol.vol.pid_qe.data.i1 = Cur_Qe * 2.0;
                    PR_PRELOAD();
                }
                 currentMode = FALSE;
        }
}
inline void SendCANPackets(void)
{
    //-------------------------------------------------------------------------------------
              if(Master)
                  {
                                  if(canIntrval==0)
                                      {
                                          nptr = (long *) &Angle;
                                          val1 = *nptr;
                                          nptr = (long *) &omega_inv;
                                          val2 = *nptr;
                                          Id = 2;
                                      }
                                      if(canIntrval==8)
                                      {
                                          val1 = 0;
                                          val1 = AlarmFlag2;
                                          Nop();

                                          val1 <<=16;
                                          Nop();
                                          val1 |= (N16) UPSFlag.all;
                                          Nop();
                                          nptr = (long *) &cur.lpf_de.y;
                                          val2 = *nptr;
                                          Id = 3;
                                      }
                                  if(Id)
                                          {
                                               // CAN-B (MCANB), TX buffer 0: ID 2 = angle + omega,
                                               // ID 3 = flags + Id current (mcanInterface.c)
                                               if(ParTransmit(Id, val1, val2)) txMsgCount_tmp++;
                                               Id = 0;
                                          }
                                  canIntrval++; canIntrval &= 0x00f;
                      }

                       else //slave
                           {
                                       if(canIntrval==6)
                                       {
                                           val1 = AlarmFlag2;
                                           val1 <<=16;
                                           val1 |= (N16) UPSFlag.all;
                                           nptr = (long *) &cur.lpf_de.y;
                                           val2 = *nptr;
                                           Id = 15;    // slave status: flags + Id current
                                                if(ParTransmit(Id, val1, val2)) txMsgCount_tmp++;
                                                Id = 0;
                                         }
                                   canIntrval++; canIntrval &= 0x001f;
                           }
    //-----------------------------------------------------------------------------------------------------------------

          }
//===========================================================================
// 							End of file
//===========================================================================

