/*
 * controlCore.h
 *
 * Inverter control math, one copy for both cores:
 *   CONTROL_ON_CLA = 1 : cla_tasks.cla, Cla1Task1 (ADCA INT1, 20 kHz) and
 *                        Cla1Task2 (EPWM1 INT, inverter PWM rate)
 *   CONTROL_ON_CLA = 0 : AdcISR (interrupt.c) and EPWM_INV_ISR (pwm.c) on the CPU
 *
 * CLA-safe C: float32_t / 16-bit types, no double arithmetic (constants are
 * cast or 'f' suffixed), no SystemFlag bitfields, no library calls. Flags for
 * the CPU go through ctl (claShared.h); the CPU applies them in
 * ctl_cpuApplyFlags() after each sample.
 *
 * Include after f28x_project.h (register structs), claShared.h,
 * inv3phParams.h and controlMacros.h.
 */

#ifndef CONTROLCORE_H_
#define CONTROLCORE_H_

#ifdef __TMS320C28XX_CLA__
#define CORE_FMAX(a,b)      __mmaxf32((a),(b))
#define CORE_FMIN(a,b)      __mminf32((a),(b))
#else
#define CORE_FMAX(a,b)      __fmax((a),(b))
#define CORE_FMIN(a,b)      __fmin((a),(b))
#endif

// always inlined: the CPU build runs them inside the RAM ISRs (.TI.ramfunc)
#define CORE_INLINE     static inline __attribute__((always_inline))

#define CONV_AC         ((float32_t)FLOAT_CONVERTER_AC)
#define CONV_DC         ((float32_t)FLOAT_CONVERTER_DC)
#define SAT_HI          ((float32_t)SAT_LIMIT)
#define SAT_LO          ((float32_t)SAT_LIMITN)

#define INVFAIL_SET     1
#define INVFAIL_CLEAR   2

// Preload fundamental resonator with the current-mode output (positive sequence
// oscillator state: X_alpha = Y_beta, X_beta = -Y_alpha) for bumpless transfer
#define PR_PRELOAD_AB(a,b)    \
    prF[0].Y = (a); prF[0].X = (b);  \
    prF[1].Y = (b); prF[1].X = -(a)
#define PR_PRELOAD()    PR_PRELOAD_AB(Cur_Alpha, Cur_Beta)

// PID_GRANDO_F_MACRO (PID_GRANDO_F.h) with portable min/max and float constants
#define PID_GRANDO_CORE(v)                                                              \
    v.data.up = (v.param.Kr * v.term.Ref) - v.term.Fbk;                                 \
    v.data.ui = (v.param.Ki * (v.data.w1 * (v.term.Ref - v.term.Fbk))) + v.data.i1;     \
    v.data.i1 = v.data.ui;                                                              \
    v.data.d2 = (v.param.Kd * (v.term.c1 * ((v.term.Ref * v.param.Km) - v.term.Fbk))) - v.data.d2; \
    v.data.ud = v.data.d2 + v.data.d1;                                                  \
    v.data.d1 = (v.data.ud * v.term.c2);                                                \
    v.data.v1 = (v.param.Kp * (v.data.up + v.data.ui + v.data.ud));                     \
    v.term.Out = CORE_FMAX(CORE_FMIN(v.param.Umax, v.data.v1), v.param.Umin);           \
    v.data.w1 = (v.term.Out == v.data.v1) ? 1.0f : 0.0f

CORE_INLINE float32_t core_sqrtf(float32_t x)
{
#ifdef __TMS320C28XX_CLA__
    float32_t r;

    if(x <= 0.0f) return 0.0f;
    r = __meisqrtf32(x);                    // 1/sqrt(x) estimate
    r = r * (1.5f - 0.5f * x * r * r);      // two Newton-Raphson steps
    r = r * (1.5f - 0.5f * x * r * r);
    return x * r;
#else
    return sqrtf(x);
#endif
}

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
 A14 reads Vb_byp (bypass jumper on the board, not Vload).
 */
CORE_INLINE void ctl_acquire(void)
{
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

    lpf_bias.u = (float32_t)(AnalogInput8) * CONV_AC;
    LOWPASSFILTER(lpf_bias);

    lpf_vInvR.u = (float32_t)(AnalogInput1 - an1Offset) * CONV_AC - lpf_bias.u;
    lpf_vInvY.u = (float32_t)(AnalogInput2 - an2Offset) * CONV_AC - lpf_bias.u;
    lpf_vInvB.u = (float32_t)(AnalogInput3 - an3Offset) * CONV_AC - lpf_bias.u;
    LOWPASSFILTER(lpf_vInvR);
    LOWPASSFILTER(lpf_vInvY);
    LOWPASSFILTER(lpf_vInvB);

    if(Master == 0)
    {
        if(lpf_vInvR.u > vacPeakLimit) { if(vachighTripCount < 20) vachighTripCount++; }
        else { if(vachighTripCount) vachighTripCount--; }
        if(vachighTripCount == 20) vacHighFault = 1;
        else if(vachighTripCount == 0) vacHighFault = 0;
    }
    else { vachighTripCount = 0; vacHighFault = 0; }

    lpf_iInvR.u = (float32_t)(AnalogInput12 - an12Offset) * CONV_AC - lpf_bias.u;
    lpf_iInvY.u = (float32_t)(AnalogInput13 - an13Offset) * CONV_AC - lpf_bias.u;
    lpf_iInvB.u = (float32_t)(AnalogInput14 - an14Offset) * CONV_AC - lpf_bias.u;
    LOWPASSFILTER(lpf_iInvR);
    LOWPASSFILTER(lpf_iInvY);
    LOWPASSFILTER(lpf_iInvB);

    lpf_voutR.u = (float32_t)(AnalogInput17 - an17Offset) * CONV_AC - lpf_bias.u;
    lpf_voutY.u = (float32_t)(AnalogInput18 - an18Offset) * CONV_AC - lpf_bias.u;
    lpf_voutB.u = (float32_t)(AnalogInput19 - an19Offset) * CONV_AC - lpf_bias.u;
    LOWPASSFILTER(lpf_voutR);
    LOWPASSFILTER(lpf_voutY);
    LOWPASSFILTER(lpf_voutB);

    lpf_ioutR.u = (float32_t)(AnalogInput4 - an4Offset) * CONV_AC - lpf_bias.u;
    lpf_ioutY.u = (float32_t)(AnalogInput5 - an5Offset) * CONV_AC - lpf_bias.u;
    lpf_ioutB.u = (float32_t)(AnalogInput6 - an6Offset) * CONV_AC - lpf_bias.u;
    LOWPASSFILTER(lpf_ioutR);
    LOWPASSFILTER(lpf_ioutY);
    LOWPASSFILTER(lpf_ioutB);

    lpf_vbypR.u = (float32_t)(AnalogInput9 - an9Offset) * CONV_AC - lpf_bias.u;
    lpf_vbypY.u = (float32_t)(AnalogInput10 - an10Offset) * CONV_AC - lpf_bias.u;
    lpf_vbypB.u = (float32_t)(AnalogInput11 - an11Offset) * CONV_AC - lpf_bias.u;
    LOWPASSFILTER(lpf_vbypR);
    LOWPASSFILTER(lpf_vbypY);
    LOWPASSFILTER(lpf_vbypB);

    lpf_vbat.u = (float32_t)AnalogInput20 * CONV_DC;
    LOWPASSFILTER(lpf_vbat);
    lpf_ibat.u = (float32_t)(AnalogInput21 - IbatOffset) * CONV_DC;
    LOWPASSFILTER(lpf_ibat);

    lpf_dclink.u = (float32_t)(AnalogInput15) * CONV_DC;
    LOWPASSFILTER(lpf_dclink);

    // instantaneous DC overvoltage, DCOV_Inst_Delay samples
    if(lpf_dclink.u > dcOV_Inst_Level) { if(DCOVTimer < DCOV_Inst_Delay) DCOVTimer++; }
    else if(DCOVTimer) DCOVTimer--;
    if(DCOVTimer > DCOV_Inst_Delay) DCOVTimer = DCOV_Inst_Delay;
    if(DCOV_Inst_Delay == DCOVTimer) dcOV = 1;
    else if(DCOVTimer == 0) dcOV = 0;

    lpf_temp1.u = (float32_t)AnalogInput7 * CONV_DC;
    lpf_temp2.u = (float32_t)AnalogInput16 * CONV_DC;
    LOWPASSFILTER(lpf_temp1);
    LOWPASSFILTER(lpf_temp2);
}

CORE_INLINE void ctl_feedback(void)
{
    float32_t zo;

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
    zo = (lpf_voutR.y*VoutRSF + lpf_voutY.y*VoutYSF + lpf_voutB.y*VoutBSF) * (1.0f/3.0f);
    vOutR = lpf_voutR.y - zo*sfOutInvR;
    vOutY = lpf_voutY.y - zo*sfOutInvY;
    vOutB = lpf_voutB.y - zo*sfOutInvB;

    vcontrol.theta_o = theta_inv;
#ifdef SHIFTED_FBACK
    vcontrol.theta = theta_fb;
#else
    vcontrol.theta = theta_inv;
#endif
}

CORE_INLINE void ctl_voltage(void)
{
    uint16_t h;

    RYB2AB_F(vcontrol.vol.d,vcontrol.vol.q,vcontrol.vol.z,vcontrol.vol.a,vcontrol.vol.b,vcontrol.vol.c);
    AB2DQ_F(vcontrol.vol.lpf_de.u,vcontrol.vol.lpf_qe.u,vcontrol.vol.d,vcontrol.vol.q,vcontrol.theta);

    LOWPASSFILTER(vcontrol.vol.lpf_de);
    vcontrol.vol.de = vcontrol.vol.lpf_de.y;

    if(vcontrol.vol.lpf_de.y < inverterFailLevel)
    {
        if(invfCount < 10) invfCount++;
        if(invfCount == 10)
        {
            inverterFailed = 1;
            ctl.invFailEvt = INVFAIL_SET;
            invokcounter = 0;
        }
    }
    else
    {
        if(invfCount) invfCount--;
        if(invokcounter < 50) invokcounter++;
        if(invokcounter >= 50) { inverterFailed = 0; invfCount = 0; ctl.invFailEvt = INVFAIL_CLEAR; }
    }

    LOWPASSFILTER(vcontrol.vol.lpf_qe);
    vcontrol.vol.qe = vcontrol.vol.lpf_qe.y;

    if(VoltCtrlMode > 1.5f)
    {
        //------------------- open-loop test: fixed balanced modulation -------------------
        // No voltage feedback, no Y/B factors: shows whether an imbalance comes from the
        // power stage / transformer rather than from the controller.
        if(!olActive)
        {
            // start from the present output level so entering the test is bumpless
            olMod = core_sqrtf(vcontrol.alpha*vcontrol.alpha + vcontrol.beta*vcontrol.beta);
            olActive = 1;
        }
        if(pwmEnabled)
        {
            olMod += (float32_t)OL_MOD_STEP;
            if(olMod > (float32_t)OL_MOD_MAX) olMod = (float32_t)OL_MOD_MAX;
        }
        else olMod = 0.0f;

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

        if((VoltCtrlMode > 0.5f) != prActive)
        {
            // bumpless switch: preload the incoming controller with the present output
            if(prActive == 0)
            {
                PR_PRELOAD_AB(vcontrol.alpha, vcontrol.beta);
                for(h = 0; h < PR_NHARM; h++)
                {
                    prH[h][0].Y = 0.0f; prH[h][0].X = 0.0f;
                    prH[h][1].Y = 0.0f; prH[h][1].X = 0.0f;
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

            fa = vcontrol.vol.d*prRotC + vcontrol.vol.q*prRotS;
            fb = vcontrol.vol.q*prRotC - vcontrol.vol.d*prRotS;

            ea =  soft_VdRef*vcontrol.theta_o.sin_val - fa;
            eb = -soft_VdRef*vcontrol.theta_o.cos_val - fb;

            PR_RES_MACRO(prF[0], prKF, ea, prCoefF, prWd, (float32_t)PR_FUND_LIMIT);
            PR_RES_MACRO(prF[1], prKF, eb, prCoefF, prWd, (float32_t)PR_FUND_LIMIT);

            ua = inv_kp*ea + prF[0].out;
            ub = inv_kp*eb + prF[1].out;

            for(h = 0; h < PR_NHARM; h++)
            {
                if((prHMask & (1U << h)) && !ctl.ctlSoftStart && !currentMode)
                {
                    PR_RES_MACRO(prH[h][0], prKH, ea, prCoefH[h], prWd, pr_hLimit);
                    PR_RES_MACRO(prH[h][1], prKH, eb, prCoefH[h], prWd, pr_hLimit);
                    ua += prH[h][0].out;
                    ub += prH[h][1].out;
                }
                else
                {
                    prH[h][0].Y = 0.0f; prH[h][0].X = 0.0f;
                    prH[h][1].Y = 0.0f; prH[h][1].X = 0.0f;
                }
            }

            if((xfr2InvTimer == 0) && (addOn > 0.0f))
            {
                ua += addOn*vcontrol.theta_o.sin_val;
                ub -= addOn*vcontrol.theta_o.cos_val;
                addOn -= 0.00002f;
            }

            vcontrol.alpha = ua;
            vcontrol.beta = ub;
            AB2DQ_F(vcontrol.deo, vcontrol.qeo, ua, ub, vcontrol.theta_o);
            AB2RYB_F(vcontrol.refa, vcontrol.refb, vcontrol.refc, ua, ub);
        }
        else
        {
            vcontrol.vol.pid_qe.term.Ref = 0.0f;
            vcontrol.vol.pid_qe.term.Fbk = vcontrol.vol.qe;
            PID_GRANDO_CORE(vcontrol.vol.pid_qe);

            vcontrol.vol.pid_de.term.Ref = soft_VdRef;
            vcontrol.vol.pid_de.term.Fbk = vcontrol.vol.de;
            PID_GRANDO_CORE(vcontrol.vol.pid_de);

            vcontrol.deo = vcontrol.vol.pid_de.term.Out;
            vcontrol.qeo = vcontrol.vol.pid_qe.term.Out;

            if((xfr2InvTimer == 0) && (addOn > 0.0f))
            {
                vcontrol.deo += addOn;
                addOn -= 0.00002f;
            }

            DQ2AB_F(vcontrol.alpha, vcontrol.beta, vcontrol.deo, vcontrol.qeo, vcontrol.theta_o);
            AB2RYB_F(vcontrol.refa, vcontrol.refb, vcontrol.refc, vcontrol.alpha, vcontrol.beta);

            vcontrol.refb = vcontrol.refb*Yfactor;
            vcontrol.refc = vcontrol.refc*Bfactor;
        }
    }   // closed loop (PI / PR)

    if(vcontrol.refa > SAT_HI) vcontrol.refa = SAT_HI;
    if(vcontrol.refb > SAT_HI) vcontrol.refb = SAT_HI;
    if(vcontrol.refc > SAT_HI) vcontrol.refc = SAT_HI;
    if(vcontrol.refa < SAT_LO) vcontrol.refa = SAT_LO;
    if(vcontrol.refb < SAT_LO) vcontrol.refb = SAT_LO;
    if(vcontrol.refc < SAT_LO) vcontrol.refc = SAT_LO;
}

CORE_INLINE void ctl_current(void)
{
    float32_t tmpAlpha, tmpBeta, tmpZero;

    RYB2AB_F(tmpAlpha, tmpBeta, tmpZero, lpf_ioutR.y, lpf_ioutY.y, lpf_ioutB.y);
    (void)tmpZero;
    AB2DQ_F(cur.lpf_de.u,cur.lpf_qe.u,tmpAlpha,tmpBeta,vcontrol.theta);

    LOWPASSFILTER(cur.lpf_de);
    LOWPASSFILTER(cur.lpf_qe);

    cur.pi_qe.Ref = 0.0f;
    cur.pi_qe.Fbk = cur.lpf_qe.y;
    PIREGULATOR(cur.pi_qe);

    cur.pi_de.Ref = newCurReference;
    cur.pi_de.Fbk = cur.lpf_de.y;
    PIREGULATOR(cur.pi_de);
    //--------------------------------------------------------------------------------
    AB2DQN_F(cur_neg.lpf_de.u,cur_neg.lpf_qe.u,tmpAlpha,tmpBeta,vcontrol.theta);
    LOWPASSFILTER(cur_neg.lpf_de);
    LOWPASSFILTER(cur_neg.lpf_qe);

    cur_neg.pi_qe.Ref = 0.0f;
    cur_neg.pi_qe.Fbk = cur_neg.lpf_qe.y;
    PIREGULATOR(cur_neg.pi_qe);

    cur_neg.pi_de.Ref = 0.0f;
    cur_neg.pi_de.Fbk = cur_neg.lpf_de.y;
    PIREGULATOR(cur_neg.pi_de);

    DQ2AB_F(Cur_neg_Alpha, Cur_neg_Beta, cur_neg.pi_de.Out, cur_neg.pi_qe.Out, vcontrol.theta_o);

    ffvd = 0.98f - cur.pi_de.Umax;
    cross = 0.05f;
    ccvd = cur.lpf_qe.y*cross;
    ccvq = cur.lpf_de.y*cross;

    Cur_De = cur.pi_de.Out + ffvd + ccvd;
    Cur_Qe = cur.pi_qe.Out - ccvq + ctl.pllQeComp;
    DQ2AB_F(Cur_Alpha, Cur_Beta, Cur_De, Cur_Qe, vcontrol.theta_o);
    if(UnbalanceCorrection)
    {
        Cur_Alpha += Cur_neg_Alpha;
        Cur_Beta += Cur_neg_Beta;
    }
}

CORE_INLINE void ctl_rms(void)
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

    // rsync is set by the CPU at the inverter zero cross of the previous sample
    if(rsync)
    {
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
    ctl_feedback();
    ctl_voltage();
    ctl_current();
    ctl_rms();
}

// Modulator, inverter PWM rate (CLA task 2 / CPU EPWM_INV_ISR)
CORE_INLINE void ctl_pwm(void)
{
    float32_t z;

    temp_fsw1++;

    if(currentMode)
    {
        svgen_dq1.Ualpha = Cur_Alpha;
        svgen_dq1.Ubeta = Cur_Beta;
    }
    else
    {
        RYB2AB_F(svgen_dq1.Ualpha,svgen_dq1.Ubeta,z,vcontrol.refa, vcontrol.refb, vcontrol.refc);
        (void)z;
    }
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
        svgen_dq1.Ta += ctl.dtGain * core_dtSign(lpf_iInvR.y * IinvRSF * ctl.dtBandInv);
        svgen_dq1.Tb += ctl.dtGain * core_dtSign(lpf_iInvY.y * IinvYSF * ctl.dtBandInv);
        svgen_dq1.Tc += ctl.dtGain * core_dtSign(lpf_iInvB.y * IinvBSF * ctl.dtBandInv);
    }

    if(svgen_dq1.Ta > SAT_HI) svgen_dq1.Ta = SAT_HI;
    if(svgen_dq1.Tb > SAT_HI) svgen_dq1.Tb = SAT_HI;
    if(svgen_dq1.Tc > SAT_HI) svgen_dq1.Tc = SAT_HI;
    if(svgen_dq1.Ta < SAT_LO) svgen_dq1.Ta = SAT_LO;
    if(svgen_dq1.Tb < SAT_LO) svgen_dq1.Tb = SAT_LO;
    if(svgen_dq1.Tc < SAT_LO) svgen_dq1.Tc = SAT_LO;

    {
        float32_t half = (float32_t)Inv_HalfPeriod;

        EPwm1Regs.CMPA.bit.CMPA = (uint16_t)(half*svgen_dq1.Ta + half);
        EPwm2Regs.CMPA.bit.CMPA = (uint16_t)(half*svgen_dq1.Tb + half);
        EPwm3Regs.CMPA.bit.CMPA = (uint16_t)(half*svgen_dq1.Tc + half);
    }

    EPwm1Regs.ETCLR.bit.INT = 1;
}

// One-time state set up, before the ADC / PWM triggers run (CLA task 8 / CPU init)
CORE_INLINE void ctl_init(void)
{
    fbTrimY = 1.0f;
    fbTrimB = 1.0f;
    fbTrimYinv = 1.0f;
    fbTrimBinv = 1.0f;
    sfOutInvR = 1.0f;
    sfOutInvY = 1.0f;
    sfOutInvB = 1.0f;
    prActive = 0;
    olActive = 0;
    olMod = 0.0f;
    ctl.invFailEvt = 0;
    ctl.claLayout = (uint16_t)sizeof(CTL_SHARED);
}

#endif /* CONTROLCORE_H_ */
