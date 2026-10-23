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
 * state; the CPU writes the grid angle (theta), set points and soft-start
 * limits - from ClaTask1EndISR / the 1 ms timer / the main loop. Controller
 * state is reset by the CLA itself while ctl.pwmOn is 0.
 */

#ifndef CLASHARED_H_
#define CLASHARED_H_

#include "controlTypes.h"

#define NPARAMS_F   (200 + 50)      // NPARAMS + 50, see pfc3phDefines.h
#define NPARAMS_I   (200 + 200)     // NPARAMS + NPARAMS

#ifdef CLA_SHARED_DEFINE
#pragma DATA_SECTION(ArrayParamsF, "Cla1DataRam")
#pragma DATA_SECTION(ArrayParamsI, "Cla1DataRam")
#pragma DATA_SECTION(pfc, "Cla1DataRam")
#pragma DATA_SECTION(pid_dclink, "Cla1DataRam")
#pragma DATA_SECTION(prA, "Cla1DataRam")
#pragma DATA_SECTION(prB, "Cla1DataRam")
#pragma DATA_SECTION(prA5, "Cla1DataRam")
#pragma DATA_SECTION(prB5, "Cla1DataRam")
#pragma DATA_SECTION(prA7, "Cla1DataRam")
#pragma DATA_SECTION(prB7, "Cla1DataRam")
#pragma DATA_SECTION(svgen_dq1, "Cla1DataRam")
#pragma DATA_SECTION(lpf_vR, "Cla1DataRam")
#pragma DATA_SECTION(lpf_vY, "Cla1DataRam")
#pragma DATA_SECTION(lpf_vB, "Cla1DataRam")
#pragma DATA_SECTION(lpf_iR, "Cla1DataRam")
#pragma DATA_SECTION(lpf_iY, "Cla1DataRam")
#pragma DATA_SECTION(lpf_iB, "Cla1DataRam")
#pragma DATA_SECTION(lpf_dclink, "Cla1DataRam")
#pragma DATA_SECTION(lpf_vbat, "Cla1DataRam")
#pragma DATA_SECTION(lpf_ibat, "Cla1DataRam")
#pragma DATA_SECTION(lpf_temp1, "Cla1DataRam")
#pragma DATA_SECTION(lpf_temp2, "Cla1DataRam")
#pragma DATA_SECTION(lpf_bias, "Cla1DataRam")
#pragma DATA_SECTION(volt_meter_R, "Cla1DataRam")
#pragma DATA_SECTION(volt_meter_Y, "Cla1DataRam")
#pragma DATA_SECTION(volt_meter_B, "Cla1DataRam")
#pragma DATA_SECTION(volt_meter_RY, "Cla1DataRam")
#pragma DATA_SECTION(volt_meter_YB, "Cla1DataRam")
#pragma DATA_SECTION(volt_meter_BR, "Cla1DataRam")
#pragma DATA_SECTION(cur_meter_R, "Cla1DataRam")
#pragma DATA_SECTION(cur_meter_Y, "Cla1DataRam")
#pragma DATA_SECTION(cur_meter_B, "Cla1DataRam")
#pragma DATA_SECTION(kw_meter_R, "Cla1DataRam")
#pragma DATA_SECTION(kw_meter_Y, "Cla1DataRam")
#pragma DATA_SECTION(kw_meter_B, "Cla1DataRam")
#pragma DATA_SECTION(theta, "Cla1DataRam")
#pragma DATA_SECTION(ctl, "Cla1DataRam")
#pragma DATA_SECTION(lpf_vinv, "Cla1DataRam")
#pragma DATA_SECTION(lpf_vout, "Cla1DataRam")
#pragma DATA_SECTION(lpf_vbyp, "Cla1DataRam")
#pragma DATA_SECTION(lpf_iinv, "Cla1DataRam")
#pragma DATA_SECTION(lpf_iout, "Cla1DataRam")
#pragma DATA_SECTION(vinv_meter, "Cla1DataRam")
#pragma DATA_SECTION(vout_meter, "Cla1DataRam")
#pragma DATA_SECTION(vbyp_meter, "Cla1DataRam")
#pragma DATA_SECTION(iinv_meter, "Cla1DataRam")
#pragma DATA_SECTION(iout_meter, "Cla1DataRam")
#pragma DATA_SECTION(kwo_meter, "Cla1DataRam")
#pragma DATA_SECTION(prV1, "Cla1DataRam")
#pragma DATA_SECTION(prV3, "Cla1DataRam")
#pragma DATA_SECTION(prV5, "Cla1DataRam")
#pragma DATA_SECTION(prV7, "Cla1DataRam")
#pragma DATA_SECTION(pid_vbat, "Cla1DataRam")
#pragma DATA_SECTION(pid_ibat, "Cla1DataRam")
#pragma DATA_SECTION(pid_boost, "Cla1DataRam")
#pragma DATA_SECTION(theta_inv, "Cla1DataRam")
#endif

// Parameter arrays (pfc3phParams.h): the CLA reads gains, offsets and limits
// through the same ArrayParamsF/I macros as the CPU
CCEXTERN    float32_t ArrayParamsF[NPARAMS_F] INITARRAY0;
CCEXTERN    int16_t   ArrayParamsI[NPARAMS_I] INITARRAY0;

// Controllers
CCEXTERN    PFC_CTRL pfc;               // dq current filters and PI regulators
CCEXTERN    PFC_PI pid_dclink;          // DC-link voltage PI -> Id reference
CCEXTERN    PR_RES prA, prB;            // PR current control, fundamental, alpha / beta
CCEXTERN    PR_RES prA5, prB5, prA7, prB7;  // 5th / 7th harmonic resonators
CCEXTERN    SVGEN_CLA svgen_dq1;        // space vector modulator

// Measurement filters
CCEXTERN    LPF lpf_vR, lpf_vY, lpf_vB;     // grid phase voltages (Vbyp sensors)
CCEXTERN    LPF lpf_iR, lpf_iY, lpf_iB;     // PFC inductor currents (Ir/y/b_ind)
CCEXTERN    LPF lpf_dclink, lpf_vbat, lpf_ibat;
CCEXTERN    LPF lpf_temp1, lpf_temp2;
CCEXTERN    LPF lpf_bias;

// RMS / kW meters (accumulated per sample, saved at each grid zero cross)
CCEXTERN    rms_meter volt_meter_R, volt_meter_Y, volt_meter_B;
CCEXTERN    rms_meter volt_meter_RY, volt_meter_YB, volt_meter_BR;
CCEXTERN    rms_meter cur_meter_R, cur_meter_Y, cur_meter_B;
CCEXTERN    rms_meter kw_meter_R, kw_meter_Y, kw_meter_B;

// Grid angle for the next control sample, written by the CPU (pll_and_sincos)
CCEXTERN    sincos theta;

// 1-phase inverter, output, bypass
CCEXTERN    LPF lpf_vinv, lpf_vout, lpf_vbyp;   // AI1 Vinv (before static), AI17 Vout, AI18 Vbypass
CCEXTERN    LPF lpf_iinv, lpf_iout;             // AI4 Iinv, AI5 Iout
CCEXTERN    rms_meter vinv_meter, vout_meter, vbyp_meter;
CCEXTERN    rms_meter iinv_meter, iout_meter, kwo_meter;   // saved at the inverter zero cross
CCEXTERN    PR_RES prV1, prV3, prV5, prV7;      // inverter voltage resonators (volts)
CCEXTERN    sincos theta_inv;                   // inverter angle, written by the CPU

// battery buck-boost (BuckBoostEnable = 1)
CCEXTERN    PFC_PI pid_vbat, pid_ibat;          // buck: battery voltage / current (min select)
CCEXTERN    PFC_PI pid_boost;                   // boost: DC-link voltage from the battery

// Instantaneous trip events from the control sample (ctl.tripEvt)
#define TRIP_OC_R       0x0001
#define TRIP_OC_Y       0x0002
#define TRIP_OC_B       0x0004
#define TRIP_DCOV       0x0008
#define TRIP_INV_OC     0x0010      // inverter current (Iinv), EPWM4/5 tripped
#define TRIP_BB_OC      0x0020      // battery current, EPWM6 tripped

// Scalars. Kept in one struct so they can be placed with one pragma; the
// old names stay usable through the #defines below.
typedef struct
{
    // CPU -> control
    float32_t dcLinkSoftRef;    // soft-started DC-link reference, p.u.
    float32_t DMinVal;          // soft-start lower limit of the d output
    float32_t qRef;             // reactive current reference (QRef, PFSign)
    float32_t crossXL;          // dq cross coupling (CrossXL)
    float32_t vol_qe_ff;        // grid q voltage (pllGrid.lpf1.y), q feed-forward
    float32_t omega;            // grid angular frequency, rad/s (PR resonators)
    float32_t ocLevel;          // instantaneous overcurrent, p.u. peak
    float32_t dcOVLevel;        // instantaneous DC-link overvoltage, p.u.
    float32_t prKp, prKr, prKh; // PR current controller gains
    float32_t dtGain;           // dead-time compensation DtComp*DBRED/TBPRD
    float32_t dtBandInv;        // 1/DtBand
    uint16_t  pwmOn;            // pwmEnabled: 0 holds the controller states at zero
    uint16_t  dmaxRamping;      // Dmax_Ramping
    uint16_t  ctrlMode;         // PfcCtrlMode: 0 = dq PI, 1 = PR
    uint16_t  revPolarity;      // RevPolarity: current sensor sign
    uint16_t  forceOn;          // test: fixed modulation (0.96, 0), no current control
    uint16_t  ocDelay;          // OC_Delay, samples
    uint16_t  dcovDelay;        // DCOV_Inst_Delay, samples
    uint16_t  PFC_HalfPeriod;   // PFC TBPRD/2
    uint16_t  rsync;            // CPU: zero cross seen -> control saves meters

    // CPU -> control, 1-phase inverter (volts, amps)
    float32_t invVref;          // soft-started output voltage amplitude, V peak
    float32_t omegaInv;         // inverter angular frequency, rad/s (resonators)
    float32_t invKp, invKr, invKh, invKci;
    float32_t invOcLevel;       // A peak
    float32_t vinvSF, iinvSF, vdcSF;    // scale factors for the volts / amps control
    uint16_t  invOn;            // invPwmEnabled: 0 holds the voltage controller at zero
    uint16_t  invHMask;         // resonators: b0 3rd, b1 5th, b2 7th
    uint16_t  invRevPol;        // InvRevPolarity
    uint16_t  invOcDelay;       // samples
    uint16_t  invPeriod;        // inverter TBPRD (EPWM4/5)
    uint16_t  rsyncInv;         // CPU: inverter zero cross -> control saves output meters

    // CPU -> control, buck-boost (volts, amps)
    float32_t vbatRef;          // buck: battery voltage reference (soft started)
    float32_t ibatRef;          // buck: charging current limit
    float32_t boostRef;         // boost: DC-link reference
    float32_t boostDmax;
    float32_t vbatSF, ibatSign; // ibatSign = +/-IbatSF: charging positive
    float32_t bbOcLevel;        // A peak
    uint16_t  bbSel;            // bbMode: 0 off, 1 buck, 2 boost
    uint16_t  bbOcDelay;        // samples
    uint16_t  bbPeriod;         // buck-boost TBPRD (EPWM6)

    // control -> CPU
    float32_t vout_alpha, vout_beta;        // modulation reference
    float32_t vdref, vqref;                 // dq modulation (before dq -> alpha beta)
    float32_t ffvd, ccvd, ccvq;
    float32_t idUnf, iqUnf;                 // unfiltered dq currents
    float32_t invVrefInst;      // inverter reference voltage, V
    float32_t invMod;           // inverter modulation index (-1..1)
    float32_t invU;             // voltage controller output, V
    float32_t bbDuty;           // buck or boost duty
    uint16_t  tripEvt;          // TRIP_xx bits, set by the control, cleared by the CPU
    uint16_t  ocTimerR, ocTimerY, ocTimerB;
    uint16_t  DCOVTimer;
    uint16_t  invOcTimer, bbOcTimer;
    uint16_t  bbModeOld;        // control: mode of the last sample (PI reset on change)
    uint16_t  rms_samplecount2; // output meters, samples in the last inverter cycle
    uint16_t  rms_update_flag2;
    uint16_t  rms_samplecount;
    uint16_t  rms_update_flag;
    uint16_t  temp_fsw1;        // PWM updates in the last second (task 2 count)
    uint16_t  claT1Count;       // CLA task 1 runs (free running)
    uint16_t  claT1Ticks;       // task 1 end, SYSCLK counts after the ADC trigger
    uint16_t  claLayout;        // sizeof(CTL_SHARED) seen by the CLA (task 8)
} CTL_SHARED;

CCEXTERN    volatile CTL_SHARED ctl;

#define dcLinkSoftRef       ctl.dcLinkSoftRef
#define DMinVal             ctl.DMinVal
#define PFC_HalfPeriod      ctl.PFC_HalfPeriod
#define rsync               ctl.rsync
#define vout_alpha          ctl.vout_alpha
#define vout_beta           ctl.vout_beta
#define ffvd                ctl.ffvd
#define ccvd                ctl.ccvd
#define ccvq                ctl.ccvq
#define DCOVTimer           ctl.DCOVTimer
#define rms_samplecount     ctl.rms_samplecount
#define rms_update_flag     ctl.rms_update_flag
#define temp_fsw1           ctl.temp_fsw1

#endif /* CLASHARED_H_ */
