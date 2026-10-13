/*
 * claShared.h
 *
 * Variables shared by the C28 CPU and the CLA control tasks (cla_tasks.cla).
 * Included by ccVariables.h (CPU) and by cla_tasks.cla, so both compilers see
 * one declaration of each. Types are float32_t / int16_t / uint16_t only:
 * int and pointers are 32 bits on the CLA and would break the shared layout.
 *
 * Defined in main.c (CLA_SHARED_DEFINE), placed in "Cla1DataRam" (RAMLS67,
 * configured as CLA data memory in claInit.c). The CPU keeps full read/write
 * access, so the rest of the firmware uses these names unchanged.
 *
 * Ownership: the CLA control task writes the filter / controller / meter
 * state; the CPU writes the angles (theta_*), set points and the preloads in
 * CurrentModeSelection() - only from ClaTask1EndISR / the 1 ms timer, i.e.
 * while CLA task 1 is idle.
 */

#ifndef CLASHARED_H_
#define CLASHARED_H_

#include "controlTypes.h"

#define NPARAMS_F   (200 + 50)      // NPARAMS + 50, see inv3phDefines.h
#define NPARAMS_I   (200 + 200)     // NPARAMS + NPARAMS

#ifdef CLA_SHARED_DEFINE
#pragma DATA_SECTION(ArrayParamsF, "Cla1DataRam")
#pragma DATA_SECTION(ArrayParamsI, "Cla1DataRam")
#pragma DATA_SECTION(cur, "Cla1DataRam")
#pragma DATA_SECTION(cur_neg, "Cla1DataRam")
#pragma DATA_SECTION(vcontrol, "Cla1DataRam")
#pragma DATA_SECTION(svgen_dq1, "Cla1DataRam")
#pragma DATA_SECTION(lpf_vInvR, "Cla1DataRam")
#pragma DATA_SECTION(lpf_vInvY, "Cla1DataRam")
#pragma DATA_SECTION(lpf_vInvB, "Cla1DataRam")
#pragma DATA_SECTION(lpf_iInvR, "Cla1DataRam")
#pragma DATA_SECTION(lpf_iInvY, "Cla1DataRam")
#pragma DATA_SECTION(lpf_iInvB, "Cla1DataRam")
#pragma DATA_SECTION(lpf_ioutR, "Cla1DataRam")
#pragma DATA_SECTION(lpf_ioutY, "Cla1DataRam")
#pragma DATA_SECTION(lpf_ioutB, "Cla1DataRam")
#pragma DATA_SECTION(lpf_voutR, "Cla1DataRam")
#pragma DATA_SECTION(lpf_voutY, "Cla1DataRam")
#pragma DATA_SECTION(lpf_voutB, "Cla1DataRam")
#pragma DATA_SECTION(lpf_vbypR, "Cla1DataRam")
#pragma DATA_SECTION(lpf_vbypY, "Cla1DataRam")
#pragma DATA_SECTION(lpf_vbypB, "Cla1DataRam")
#pragma DATA_SECTION(lpf_dclink, "Cla1DataRam")
#pragma DATA_SECTION(lpf_vbat, "Cla1DataRam")
#pragma DATA_SECTION(lpf_ibat, "Cla1DataRam")
#pragma DATA_SECTION(lpf_temp1, "Cla1DataRam")
#pragma DATA_SECTION(lpf_temp2, "Cla1DataRam")
#pragma DATA_SECTION(lpf_bias, "Cla1DataRam")
#pragma DATA_SECTION(volt_meter_R, "Cla1DataRam")
#pragma DATA_SECTION(volt_meter_Y, "Cla1DataRam")
#pragma DATA_SECTION(volt_meter_B, "Cla1DataRam")
#pragma DATA_SECTION(cur_meter_R, "Cla1DataRam")
#pragma DATA_SECTION(cur_meter_Y, "Cla1DataRam")
#pragma DATA_SECTION(cur_meter_B, "Cla1DataRam")
#pragma DATA_SECTION(iout_meter_R, "Cla1DataRam")
#pragma DATA_SECTION(iout_meter_Y, "Cla1DataRam")
#pragma DATA_SECTION(iout_meter_B, "Cla1DataRam")
#pragma DATA_SECTION(vout_meter_R, "Cla1DataRam")
#pragma DATA_SECTION(vout_meter_Y, "Cla1DataRam")
#pragma DATA_SECTION(vout_meter_B, "Cla1DataRam")
#pragma DATA_SECTION(vllb_meter_R, "Cla1DataRam")
#pragma DATA_SECTION(vllb_meter_Y, "Cla1DataRam")
#pragma DATA_SECTION(vllb_meter_B, "Cla1DataRam")
#pragma DATA_SECTION(vll_meter_R, "Cla1DataRam")
#pragma DATA_SECTION(vll_meter_Y, "Cla1DataRam")
#pragma DATA_SECTION(vll_meter_B, "Cla1DataRam")
#pragma DATA_SECTION(vbyp_meter_R, "Cla1DataRam")
#pragma DATA_SECTION(vbyp_meter_Y, "Cla1DataRam")
#pragma DATA_SECTION(vbyp_meter_B, "Cla1DataRam")
#pragma DATA_SECTION(kw_meter_R, "Cla1DataRam")
#pragma DATA_SECTION(kw_meter_Y, "Cla1DataRam")
#pragma DATA_SECTION(kw_meter_B, "Cla1DataRam")
#pragma DATA_SECTION(prF, "Cla1DataRam")
#pragma DATA_SECTION(prH, "Cla1DataRam")
#pragma DATA_SECTION(prCoefF, "Cla1DataRam")
#pragma DATA_SECTION(prCoefH, "Cla1DataRam")
#pragma DATA_SECTION(theta_inv, "Cla1DataRam")
#pragma DATA_SECTION(theta_fb, "Cla1DataRam")
#pragma DATA_SECTION(theta_fb_120, "Cla1DataRam")
#pragma DATA_SECTION(theta_fb_240, "Cla1DataRam")
#pragma DATA_SECTION(theta_inv_120, "Cla1DataRam")
#pragma DATA_SECTION(theta_inv_240, "Cla1DataRam")
#pragma DATA_SECTION(ctl, "Cla1DataRam")
#endif

// Parameter arrays (inv3phParams.h): the CLA reads gains, offsets and limits
// through the same ArrayParamsF/I macros as the CPU
CCEXTERN    float32_t ArrayParamsF[NPARAMS_F] INITARRAY0;
CCEXTERN    int16_t   ArrayParamsI[NPARAMS_I] INITARRAY0;

// Controllers
CCEXTERN    harmonic cur;               // current-share PI, positive sequence
CCEXTERN    harmonic cur_neg;           // negative sequence
CCEXTERN    VolControl vcontrol;        // voltage loop (PI dq / PR alpha-beta)
CCEXTERN    SVGEN_CLA svgen_dq1;        // space vector modulator

// Measurement filters
CCEXTERN    LPF lpf_vInvR, lpf_vInvY, lpf_vInvB;
CCEXTERN    LPF lpf_iInvR, lpf_iInvY, lpf_iInvB;
CCEXTERN    LPF lpf_ioutR, lpf_ioutY, lpf_ioutB;
CCEXTERN    LPF lpf_voutR, lpf_voutY, lpf_voutB;
CCEXTERN    LPF lpf_vbypR, lpf_vbypY, lpf_vbypB;
CCEXTERN    LPF lpf_dclink, lpf_vbat, lpf_ibat;
CCEXTERN    LPF lpf_temp1, lpf_temp2;
CCEXTERN    LPF lpf_bias;

// RMS / kW meters (accumulated per sample, saved at each inverter zero cross)
CCEXTERN    rms_meter volt_meter_R, volt_meter_Y, volt_meter_B;
CCEXTERN    rms_meter cur_meter_R, cur_meter_Y, cur_meter_B;
CCEXTERN    rms_meter iout_meter_R, iout_meter_Y, iout_meter_B;
CCEXTERN    rms_meter vout_meter_R, vout_meter_Y, vout_meter_B;
CCEXTERN    rms_meter vllb_meter_R, vllb_meter_Y, vllb_meter_B;
CCEXTERN    rms_meter vll_meter_R, vll_meter_Y, vll_meter_B;
CCEXTERN    rms_meter vbyp_meter_R, vbyp_meter_Y, vbyp_meter_B;
CCEXTERN    rms_meter kw_meter_R, kw_meter_Y, kw_meter_B;

// PR voltage controller (coefficients from PR_UpdateCoeffs(), CPU, 1 ms)
CCEXTERN    PR_STATE prF[2];                 // fundamental resonator, alpha/beta
CCEXTERN    PR_STATE prH[PR_NHARM][2];       // harmonic resonators, alpha/beta
CCEXTERN    PR_COEF  prCoefF;
CCEXTERN    PR_COEF  prCoefH[PR_NHARM];

// Angles, written by the CPU (pll_and_sincos), used by the next control sample
CCEXTERN    sincos theta_inv;
CCEXTERN    sincos theta_fb;
CCEXTERN    sincos theta_fb_120;
CCEXTERN    sincos theta_fb_240;
CCEXTERN    sincos theta_inv_120;
CCEXTERN    sincos theta_inv_240;

// Scalars. Kept in one struct so they can be placed with one pragma; the
// old names stay usable through the #defines below.
typedef struct
{
    // CPU -> control
    float32_t prKF;             // fundamental resonant gain (2*inv_ki)
    float32_t prKH;             // harmonic resonant gain
    float32_t prWd;             // resonator damping 2*wc*Ts
    float32_t prRotC, prRotS;   // cos/sin of feedback rotation (Angle30)
    float32_t fbTrimY, fbTrimB;         // VinvYSF / VinvRSF, VinvBSF / VinvRSF
    float32_t fbTrimYinv, fbTrimBinv;   // 1/fbTrimY, 1/fbTrimB
    float32_t sfOutInvR, sfOutInvY, sfOutInvB;  // 1/VoutxSF
    float32_t soft_VdRef;       // soft-started voltage reference
    float32_t addOn;            // transfer-to-inverter voltage boost, decays
    float32_t inverterFailLevel;
    float32_t pllQeComp;        // pllBypass.lpf1.y, q-axis feed-forward
    float32_t dtGain;           // dead-time compensation DtComp*DBRED/TBPRD
    float32_t dtBandInv;        // 1/DtBand
    float32_t olModMax;         // open-loop test modulation limit (OL_MOD_MAX at start up)
    uint16_t  prHMask;
    uint16_t  ctlSoftStart;     // SystemFlag.bit.SOFTSTART
    uint16_t  Inv_HalfPeriod;   // inverter TBPRD/2
    uint16_t  rsync;            // CPU: zero cross seen -> control saves meters

    // control -> CPU
    float32_t Cur_Alpha, Cur_Beta;          // current-mode modulation
    float32_t Cur_neg_Alpha, Cur_neg_Beta;
    float32_t ffvd, ccvq, ccvd, cross;
    float32_t vry, vyb, vbr;                // bypass line voltages
    float32_t vOutR, vOutY, vOutB;          // output voltages, common mode removed
    float32_t olMod;            // open-loop modulation index
    uint16_t  prActive;         // PR in use last sample (bumpless PI <-> PR)
    uint16_t  olActive;         // open-loop test in use last sample
    uint16_t  invfCount, invokcounter;
    uint16_t  invFailEvt;       // 1 = set INVFAILURE, 2 = clear it (CPU applies)
    uint16_t  vacHighFault, vachighTripCount;
    uint16_t  DCOVTimer;
    uint16_t  rms_samplecount;
    uint16_t  rms_update_flag;
    uint16_t  temp_fsw1;        // PWM updates in the last second (task 2 count)
    uint16_t  claT1Count;       // CLA task 1 runs (free running)
    uint16_t  claT1Ticks;       // task 1 end, SYSCLK counts after the ADC trigger
    uint16_t  claLayout;        // sizeof(CTL_SHARED) seen by the CLA (task 8)
} CTL_SHARED;

CCEXTERN    volatile CTL_SHARED ctl;

#define prKF                ctl.prKF
#define prKH                ctl.prKH
#define prWd                ctl.prWd
#define prRotC              ctl.prRotC
#define prRotS              ctl.prRotS
#define prHMask             ctl.prHMask
#define fbTrimY             ctl.fbTrimY
#define fbTrimB             ctl.fbTrimB
#define fbTrimYinv          ctl.fbTrimYinv
#define fbTrimBinv          ctl.fbTrimBinv
#define sfOutInvR           ctl.sfOutInvR
#define sfOutInvY           ctl.sfOutInvY
#define sfOutInvB           ctl.sfOutInvB
#define soft_VdRef          ctl.soft_VdRef
#define addOn               ctl.addOn
#define inverterFailLevel   ctl.inverterFailLevel
#define Inv_HalfPeriod      ctl.Inv_HalfPeriod
#define rsync               ctl.rsync
#define Cur_Alpha           ctl.Cur_Alpha
#define Cur_Beta            ctl.Cur_Beta
#define Cur_neg_Alpha       ctl.Cur_neg_Alpha
#define Cur_neg_Beta        ctl.Cur_neg_Beta
#define ffvd                ctl.ffvd
#define ccvq                ctl.ccvq
#define ccvd                ctl.ccvd
#define cross               ctl.cross
#define vry                 ctl.vry
#define vyb                 ctl.vyb
#define vbr                 ctl.vbr
#define vOutR               ctl.vOutR
#define vOutY               ctl.vOutY
#define vOutB               ctl.vOutB
#define olMod               ctl.olMod
#define prActive            ctl.prActive
#define olActive            ctl.olActive
#define invfCount           ctl.invfCount
#define invokcounter        ctl.invokcounter
#define vacHighFault        ctl.vacHighFault
#define vachighTripCount    ctl.vachighTripCount
#define DCOVTimer           ctl.DCOVTimer
#define rms_samplecount     ctl.rms_samplecount
#define rms_update_flag     ctl.rms_update_flag
#define temp_fsw1           ctl.temp_fsw1

#endif /* CLASHARED_H_ */
