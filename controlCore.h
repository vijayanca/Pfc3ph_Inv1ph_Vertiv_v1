/*
 * controlCore.h
 *
 * 3-phase PFC control math, one copy for both cores:
 *   CONTROL_ON_CLA = 1 : cla_tasks.cla, Cla1Task1 (ADCA INT1, 20 kHz) and
 *                        Cla1Task2 (EPWM1 INT, PFC PWM rate)
 *   CONTROL_ON_CLA = 0 : AdcISR (interrupt.c) and EPWM_PFC_ISR (pwm.c) on the CPU
 *
 * Control as in the RBC (RBC2_KDSENG_v4, interrupt.c getReferenceCurrent):
 *   DC-link PI -> Id reference, Iq reference = QRef
 *   current control: dq PI (PfcCtrlMode 0) or alpha-beta PR (PfcCtrlMode 1)
 *   + feed-forward (0.9999 - Umax), dq cross coupling (CrossXL), grid q voltage
 *   soft start: d output held >= DMinVal while Dmax_Ramping
 *
 * CLA-safe C: float32_t / 16-bit types, no double arithmetic (constants are
 * cast or 'f' suffixed), no SystemFlag bitfields, no library calls. Flags for
 * the CPU go through ctl (claShared.h); the CPU applies them in
 * cpuPostControl() after each sample.
 *
 * Include after f28x_project.h (register structs), claShared.h,
 * pfc3phParams.h and controlMacros.h.
 */

#ifndef CONTROLCORE_H_
#define CONTROLCORE_H_

#ifdef __TMS320C28XX_CLA__
#define CORE_FABS(a)        __mmaxf32((a), -(a))    // no abs intrinsic on the CLA
#define CORE_EALLOW()       __meallow()
#define CORE_EDIS()         __medis()
#else
#define CORE_FABS(a)        fabsf(a)
#define CORE_EALLOW()       __asm(" EALLOW")
#define CORE_EDIS()         __asm(" EDIS")
#endif

// always inlined: the CPU build runs them inside the RAM ISRs (.TI.ramfunc)
#define CORE_INLINE     static inline __attribute__((always_inline))

#define CONV_AC         ((float32_t)FLOAT_CONVERTER_AC)
#define CONV_DC         ((float32_t)FLOAT_CONVERTER_DC)
#define SAT_HI          ((float32_t)SAT_LIMIT)
#define SAT_LO          ((float32_t)SAT_LIMITN)

#define PR_DAMP2        2.0f        // 2*wc, wc = 1 rad/s: gain Kr/2 at fundamental, robust to frequency error
#define PR_DAMP2_H      10.0f       // 2*wc for harmonic resonators

// sign of the current, linear between -1 and +1 (input already divided by the band)
CORE_INLINE float32_t core_dtSign(float32_t x)
{
    if(x > 1.0f) return 1.0f;
    if(x < -1.0f) return -1.0f;
    return x;
}

/*
 Common Controller v2 SOC map (BoardInit.c setupADC(), board_pin_map.h):
        SOC0        SOC1        SOC2        SOC3          SOC4
  A   Ir_ind A0   Ir_out A5   Ib_out A8   Vb_byp A14    Temp1 A9
  B   Vb_inv B3   Iy_out B8   Vy_byp B9   Vref1.5 B0    Temp3 B1
  C   Iy_ind C5   VdcLink C0  Vr_byp C9   Vbat C2       Temp2 C8
  D   Vr_inv D14  Vb_out D15  Vy_out D12  Ibat D11      -
  E   Ib_ind E11  Vy_inv E12  Vr_out E16  -             -
 PFC use: grid voltage = Vr/y/b_byp (line to neutral), PFC current =
 Ir/y/b_ind, DC link C0. The other channels are read for display only.
 */
CORE_INLINE void ctl_acquire(void)
{
    float32_t x;

    AnalogInput1 = AdcdResultRegs.ADCRESULT0;   // Vr_inv  D14
    AnalogInput2 = AdceResultRegs.ADCRESULT1;   // Vy_inv  E12
    AnalogInput3 = AdcbResultRegs.ADCRESULT0;   // Vb_inv  B3
    AnalogInput4 = AdcaResultRegs.ADCRESULT1;   // Ir_out  A5
    AnalogInput5 = AdcbResultRegs.ADCRESULT1;   // Iy_out  B8
    AnalogInput6 = AdcaResultRegs.ADCRESULT2;   // Ib_out  A8
    AnalogInput7 = AdcaResultRegs.ADCRESULT4;   // Temp1   A9

    AnalogInput8 = AdcbResultRegs.ADCRESULT3;   // Vref1.5 B0
    AnalogInput9 = AdccResultRegs.ADCRESULT2;   // Vgrid R (Vr_byp) C9
    AnalogInput10 = AdcbResultRegs.ADCRESULT2;  // Vgrid Y (Vy_byp) B9
    AnalogInput11 = AdcaResultRegs.ADCRESULT3;  // Vgrid B (Vb_byp) A14
    AnalogInput12 = AdcaResultRegs.ADCRESULT0;  // Ipfc R (Ir_ind) A0
    AnalogInput13 = AdccResultRegs.ADCRESULT0;  // Ipfc Y (Iy_ind) C5
    AnalogInput14 = AdceResultRegs.ADCRESULT0;  // Ipfc B (Ib_ind) E11

    AnalogInput15 = AdccResultRegs.ADCRESULT1;  // DC Link C0
    AnalogInput16 = AdccResultRegs.ADCRESULT4;  // Temp2   C8
    AnalogInput17 = AdceResultRegs.ADCRESULT2;  // Vr_out  E16
    AnalogInput18 = AdcdResultRegs.ADCRESULT2;  // Vy_out  D12
    AnalogInput19 = AdcdResultRegs.ADCRESULT1;  // Vb_out  D15
    AnalogInput20 = AdccResultRegs.ADCRESULT3;  // Vbat    C2
    AnalogInput21 = AdcdResultRegs.ADCRESULT3;  // Ibat    D11

    lpf_bias.u = (float32_t)(AnalogInput8) * CONV_AC;
    LOWPASSFILTER(lpf_bias);

    lpf_vR.u = (float32_t)(AnalogInput9 - an9Offset) * CONV_AC - lpf_bias.u;
    lpf_vY.u = (float32_t)(AnalogInput10 - an10Offset) * CONV_AC - lpf_bias.u;
    lpf_vB.u = (float32_t)(AnalogInput11 - an11Offset) * CONV_AC - lpf_bias.u;
    LOWPASSFILTER(lpf_vR);
    LOWPASSFILTER(lpf_vY);
    LOWPASSFILTER(lpf_vB);

    x = ctl.revPolarity ? -1.0f : 1.0f;         // current sensor direction
    lpf_iR.u = ((float32_t)(AnalogInput12 - an12Offset) * CONV_AC - lpf_bias.u) * x;
    lpf_iY.u = ((float32_t)(AnalogInput13 - an13Offset) * CONV_AC - lpf_bias.u) * x;
    lpf_iB.u = ((float32_t)(AnalogInput14 - an14Offset) * CONV_AC - lpf_bias.u) * x;
    LOWPASSFILTER(lpf_iR);
    LOWPASSFILTER(lpf_iY);
    LOWPASSFILTER(lpf_iB);

    lpf_dclink.u = (float32_t)(AnalogInput15) * CONV_DC;
    LOWPASSFILTER(lpf_dclink);

    lpf_vbat.u = (float32_t)AnalogInput20 * CONV_DC;
    LOWPASSFILTER(lpf_vbat);
    lpf_ibat.u = (float32_t)(AnalogInput21 - IbatOffset) * CONV_DC;
    LOWPASSFILTER(lpf_ibat);

    lpf_temp1.u = (float32_t)AnalogInput7 * CONV_DC;
    lpf_temp2.u = (float32_t)AnalogInput16 * CONV_DC;
    LOWPASSFILTER(lpf_temp1);
    LOWPASSFILTER(lpf_temp2);
}

// Instantaneous trips (unfiltered values), counted in samples. A trip stops the
// PFC pulses here, in the same sample: one-shot trip of EPWM1-3. The CPU sees
// ctl.tripEvt after the sample, sets the flags and runs the trip sequence.
#define CORE_TRIPCOUNT(cond, tmr, dly)                  \
    if(cond) { if(tmr < (dly)) tmr++; }                 \
    else if(tmr) tmr--;                                 \
    if(tmr > (dly)) tmr = (dly)

CORE_INLINE void ctl_protect(void)
{
    uint16_t evt = 0;

    if(ctl.pwmOn)
    {
        CORE_TRIPCOUNT(CORE_FABS(lpf_iR.u) > ctl.ocLevel, ctl.ocTimerR, ctl.ocDelay);
        CORE_TRIPCOUNT(CORE_FABS(lpf_iY.u) > ctl.ocLevel, ctl.ocTimerY, ctl.ocDelay);
        CORE_TRIPCOUNT(CORE_FABS(lpf_iB.u) > ctl.ocLevel, ctl.ocTimerB, ctl.ocDelay);
        CORE_TRIPCOUNT(lpf_dclink.u > ctl.dcOVLevel, DCOVTimer, ctl.dcovDelay);

        if(ctl.ocTimerR == ctl.ocDelay) evt |= TRIP_OC_R;
        if(ctl.ocTimerY == ctl.ocDelay) evt |= TRIP_OC_Y;
        if(ctl.ocTimerB == ctl.ocDelay) evt |= TRIP_OC_B;
        if(DCOVTimer == ctl.dcovDelay) evt |= TRIP_DCOV;

        if(evt)
        {
            CORE_EALLOW();
            EPwm1Regs.TZFRC.bit.OST = 1;
            EPwm2Regs.TZFRC.bit.OST = 1;
            EPwm3Regs.TZFRC.bit.OST = 1;
            CORE_EDIS();
            ctl.tripEvt |= evt;
        }
    }
    else
    {
        ctl.ocTimerR = 0; ctl.ocTimerY = 0; ctl.ocTimerB = 0;
        DCOVTimer = 0;
    }
}

CORE_INLINE void ctl_current(void)
{
    float32_t tmpAlpha, tmpBeta, tmpZero;

    RYB2AB_F(tmpAlpha, tmpBeta, tmpZero, lpf_iR.y, lpf_iY.y, lpf_iB.y);
    (void)tmpZero;
    AB2DQ_F(pfc.lpf_de.u, pfc.lpf_qe.u, tmpAlpha, tmpBeta, theta);
    ctl.idUnf = pfc.lpf_de.u;
    ctl.iqUnf = pfc.lpf_qe.u;
    LOWPASSFILTER(pfc.lpf_de);
    LOWPASSFILTER(pfc.lpf_qe);

    // DC-link voltage loop -> d current reference
    pid_dclink.Fbk = lpf_dclink.y;
    pid_dclink.Ref = dcLinkSoftRef;
    if(ctl.pwmOn) { PFC_PI_MACRO(pid_dclink); }
    else { pid_dclink.ui = 0.0f; pid_dclink.Out = 0.0f; }     // no wind-up while PFC is off

    pfc.pid_qe.Ref = ctl.qRef;
    pfc.pid_qe.Fbk = pfc.lpf_qe.y;
    pfc.pid_de.Ref = -pid_dclink.Out;
    pfc.pid_de.Fbk = pfc.lpf_de.y;

    if(!ctl.pwmOn)
    {
        // PWM off: no current can flow, so the integrators would wind up to their
        // limits. Hold everything at zero here so the first switching period
        // starts clean and nothing on the CPU races with the controller state.
        pfc.pid_de.ui = 0.0f;  pfc.pid_qe.ui = 0.0f;
        pfc.pid_de.Out = 0.0f; pfc.pid_qe.Out = 0.0f;
        pfc.pid_de.Err = pfc.pid_de.Ref - pfc.pid_de.Fbk;
        pfc.pid_qe.Err = pfc.pid_qe.Ref - pfc.pid_qe.Fbk;
        PR_RES_CLEAR(prA);  PR_RES_CLEAR(prB);
        PR_RES_CLEAR(prA5); PR_RES_CLEAR(prB5);
        PR_RES_CLEAR(prA7); PR_RES_CLEAR(prB7);
    }
    else if(ctl.ctrlMode == 1)
    {
        //------------------- PR current control, alpha-beta frame -------------------
        // Works on the unfiltered currents, so the dq feedback filter is not inside
        // the current loop. Resonant frequency follows the PLL. Output is projected
        // to dq so the limits, soft-start clamp, feed-forward and decoupling are
        // the same as for the PI.
        float32_t irefA, irefB, eA, eB, uA, uB, ud, uq, w;
        float32_t dmin, udRaw, uqRaw, ed, eq;
        float32_t hA = 0.0f, hB = 0.0f;     // harmonic resonator outputs
        uint16_t freeze;

        DQ2AB_F(irefA, irefB, pfc.pid_de.Ref, pfc.pid_qe.Ref, theta);
        eA = irefA - tmpAlpha;
        eB = irefB - tmpBeta;

        w = ctl.omega;

        // output from the present resonator states; the states are advanced below
        // only if that does not wind them further into a limit
        if(ctl.prKh >= 1.0f)
        {
            hA = prA5.v1 + prA7.v1;
            hB = prB5.v1 + prB7.v1;
        }
        else
        {
            PR_RES_CLEAR(prA5); PR_RES_CLEAR(prB5);
            PR_RES_CLEAR(prA7); PR_RES_CLEAR(prB7);
        }
        uA = ctl.prKp*eA + prA.v1 + hA;
        uB = ctl.prKp*eB + prB.v1 + hB;

        // same limits as the PI outputs
        AB2DQ_F(ud, uq, uA, uB, theta);
        udRaw = ud;
        uqRaw = uq;
        dmin = pfc.pid_de.Umin;
        if(ctl.dmaxRamping && (DMinVal > dmin)) dmin = DMinVal;
        if(ud > pfc.pid_de.Umax) ud = pfc.pid_de.Umax;
        if(ud < dmin) ud = dmin;
        if(uq > pfc.pid_qe.Umax) uq = pfc.pid_qe.Umax;
        if(uq < pfc.pid_qe.Umin) uq = pfc.pid_qe.Umin;
        pfc.pid_de.Out = ud;
        pfc.pid_qe.Out = uq;

        // Clamped anti-windup: hold the resonators while an output is limited and
        // its error pushes further into that limit. "Hold" = keep oscillating with
        // zero input: the output stays constant in dq (like a held PI integrator).
        AB2DQ_F(ed, eq, eA, eB, theta);
        freeze = ((udRaw > ud) && (ed > 0.0f)) || ((udRaw < ud) && (ed < 0.0f))
              || ((uqRaw > uq) && (eq > 0.0f)) || ((uqRaw < uq) && (eq < 0.0f));
        if(freeze) { eA = 0.0f; eB = 0.0f; }
        PR_RES_MACRO(prA, eA, ctl.prKr, w, PR_DAMP2);
        PR_RES_MACRO(prB, eB, ctl.prKr, w, PR_DAMP2);
        if(ctl.prKh >= 1.0f)
        {
            PR_RES_MACRO(prA5, eA, ctl.prKh, 5.0f*w, PR_DAMP2_H);
            PR_RES_MACRO(prA7, eA, ctl.prKh, 7.0f*w, PR_DAMP2_H);
            PR_RES_MACRO(prB5, eB, ctl.prKh, 5.0f*w, PR_DAMP2_H);
            PR_RES_MACRO(prB7, eB, ctl.prKh, 7.0f*w, PR_DAMP2_H);
        }

        // PI integrators follow, so switching back to PI is bumpless
        pfc.pid_de.Err = pfc.pid_de.Ref - pfc.pid_de.Fbk;
        pfc.pid_qe.Err = pfc.pid_qe.Ref - pfc.pid_qe.Fbk;
        pfc.pid_de.ui = ud - pfc.pid_de.Kp*pfc.pid_de.Err;
        pfc.pid_qe.ui = uq - pfc.pid_qe.Kp*pfc.pid_qe.Err;
        if(pfc.pid_de.ui > pfc.pid_de.Umax) pfc.pid_de.ui = pfc.pid_de.Umax;
        if(pfc.pid_de.ui < pfc.pid_de.Umin) pfc.pid_de.ui = pfc.pid_de.Umin;
        if(pfc.pid_qe.ui > pfc.pid_qe.Umax) pfc.pid_qe.ui = pfc.pid_qe.Umax;
        if(pfc.pid_qe.ui < pfc.pid_qe.Umin) pfc.pid_qe.ui = pfc.pid_qe.Umin;
    }
    else
    {
        //------------------- dq PI current control ----------------------------------
        float32_t uiA, uiB;

        PFC_PI_MACRO(pfc.pid_qe);
        PFC_PI_MACRO(pfc.pid_de);

        // PR states follow the PI integrators so switching to PR is bumpless:
        // dq constant (ud,uq) -> alpha = ud*s + uq*c, its quadrature = beta
        DQ2AB_F(uiA, uiB, pfc.pid_de.ui, pfc.pid_qe.ui, theta);
        prA.v1 = uiA;  prA.v2 = uiB;
        prB.v1 = uiB;  prB.v2 = -uiA;
        PR_RES_CLEAR(prA5); PR_RES_CLEAR(prB5);
        PR_RES_CLEAR(prA7); PR_RES_CLEAR(prB7);
    }

    // soft start: the d output (boost authority) is released slowly from 0
    if(ctl.dmaxRamping)
    {
        if(pfc.pid_de.Out < DMinVal) pfc.pid_de.Out = DMinVal;
    }

    ffvd = 0.9999f - pfc.pid_de.Umax;
    ccvd = pfc.lpf_qe.y * ctl.crossXL;     // add
    ccvq = pfc.lpf_de.y * ctl.crossXL;     // sub

    ctl.vdref = pfc.pid_de.Out + ffvd + ccvd;
    ctl.vqref = pfc.pid_qe.Out - ccvq + ctl.vol_qe_ff;

    if(ctl.forceOn) { ctl.vdref = 0.96f; ctl.vqref = 0.0f; }

    DQ2AB_F(vout_alpha, vout_beta, ctl.vdref, ctl.vqref, theta);
}

CORE_INLINE void ctl_rms(void)
{
    float32_t vR = lpf_vR.y, vY = lpf_vY.y, vB = lpf_vB.y;

    /* Accumulate squared value of grid voltages for mean square */
    UPDATE_RMS(volt_meter_R, vR);
    UPDATE_RMS(volt_meter_Y, vY);
    UPDATE_RMS(volt_meter_B, vB);

    UPDATE_RMS(volt_meter_RY, (vR - vY));
    UPDATE_RMS(volt_meter_YB, (vY - vB));
    UPDATE_RMS(volt_meter_BR, (vB - vR));

    UPDATE_RMS(cur_meter_R, lpf_iR.y);
    UPDATE_RMS(cur_meter_Y, lpf_iY.y);
    UPDATE_RMS(cur_meter_B, lpf_iB.y);

    // input power: current drawn from the grid is negative (RBC sign convention)
    UPDATE_KW(kw_meter_R, vR, -lpf_iR.y, VinpRSF, IinpRSF);
    UPDATE_KW(kw_meter_Y, vY, -lpf_iY.y, VinpYSF, IinpYSF);
    UPDATE_KW(kw_meter_B, vB, -lpf_iB.y, VinpBSF, IinpBSF);

    // rsync is set by the CPU at the grid zero cross of the previous sample
    if(rsync)
    {
        rms_samplecount = volt_meter_R.sample_count;

        SAVE_METER(volt_meter_R);
        SAVE_METER(volt_meter_Y);
        SAVE_METER(volt_meter_B);

        SAVE_METER(volt_meter_RY);
        SAVE_METER(volt_meter_YB);
        SAVE_METER(volt_meter_BR);

        SAVE_METER(cur_meter_R);
        SAVE_METER(cur_meter_Y);
        SAVE_METER(cur_meter_B);

        SAVE_METER(kw_meter_R);
        SAVE_METER(kw_meter_Y);
        SAVE_METER(kw_meter_B);

        rms_update_flag = 1;
        rsync = 0;
    }
}

// SYSCLK counts since the ADC trigger (EPWM8 SOCA at period, up-down, TBPRD 3750)
CORE_INLINE uint16_t ctl_ticksSinceSoc(void)
{
    uint16_t c = EPwm8Regs.TBCTR;

    if(EPwm8Regs.TBSTS.bit.CTRDIR) return EPwm8Regs.TBPRD + c;    // up: past zero
    return EPwm8Regs.TBPRD - c;                                     // down: from period
}

// Control sample, 20 kHz, after the ADC conversions (CLA task 1 / CPU AdcISR)
CORE_INLINE void ctl_task1(void)
{
    ctl_acquire();
    ctl_protect();
    ctl_current();
    ctl_rms();
}

// Modulator, PFC PWM rate (CLA task 2 / CPU EPWM_PFC_ISR)
CORE_INLINE void ctl_pwm(void)
{
    temp_fsw1++;

    svgen_dq1.Ualpha = vout_alpha;
    svgen_dq1.Ubeta = vout_beta;
    SVGEN_CLA_MACRO(svgen_dq1);
    svgen_dq1.Ta = -svgen_dq1.Ta;
    svgen_dq1.Tb = -svgen_dq1.Tb;
    svgen_dq1.Tc = -svgen_dq1.Tc;

    // Dead-time compensation: each leg loses Td/Tsw of duty against its current.
    // Full correction in normalised units is DBRED/TBPRD (= 2*Td/Tsw); DtComp scales
    // it and sets the sign (current sensor polarity). Linear within +/-DtBand amps
    // so the correction does not chatter around current zero crossings.
    // ctl.dtGain / ctl.dtBandInv are updated by the CPU every 1 ms.
    if(ctl.dtGain != 0.0f)
    {
        svgen_dq1.Ta += ctl.dtGain * core_dtSign(lpf_iR.y * IinpRSF * ctl.dtBandInv);
        svgen_dq1.Tb += ctl.dtGain * core_dtSign(lpf_iY.y * IinpYSF * ctl.dtBandInv);
        svgen_dq1.Tc += ctl.dtGain * core_dtSign(lpf_iB.y * IinpBSF * ctl.dtBandInv);
    }

    if(svgen_dq1.Ta > SAT_HI) svgen_dq1.Ta = SAT_HI;
    if(svgen_dq1.Tb > SAT_HI) svgen_dq1.Tb = SAT_HI;
    if(svgen_dq1.Tc > SAT_HI) svgen_dq1.Tc = SAT_HI;
    if(svgen_dq1.Ta < SAT_LO) svgen_dq1.Ta = SAT_LO;
    if(svgen_dq1.Tb < SAT_LO) svgen_dq1.Tb = SAT_LO;
    if(svgen_dq1.Tc < SAT_LO) svgen_dq1.Tc = SAT_LO;

    {
        float32_t half = (float32_t)PFC_HalfPeriod;

        EPwm1Regs.CMPA.bit.CMPA = (uint16_t)(half*svgen_dq1.Ta + half);
        EPwm2Regs.CMPA.bit.CMPA = (uint16_t)(half*svgen_dq1.Tb + half);
        EPwm3Regs.CMPA.bit.CMPA = (uint16_t)(half*svgen_dq1.Tc + half);
    }

    EPwm1Regs.ETCLR.bit.INT = 1;
}

// One-time state set up, before the ADC / PWM triggers run (CLA task 8 / CPU init)
CORE_INLINE void ctl_init(void)
{
    ctl.pwmOn = 0;
    ctl.dmaxRamping = 0;
    ctl.forceOn = 0;
    ctl.tripEvt = 0;
    ctl.claLayout = (uint16_t)sizeof(CTL_SHARED);
}

#endif /* CONTROLCORE_H_ */
