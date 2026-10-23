/*
 * inverter.c
 *
 * 1-phase inverter (EPWM4/5) and static switch (EPWM7), CPU part:
 *   InverterSync()        20 kHz, after each control sample: bypass SOGI-PLL,
 *                         inverter angle (synchronised to the bypass)
 *   InverterTimer1ms()    gains / references to the control, soft start,
 *                         synchronised detection, bypass fail, restart timers,
 *                         static switch
 *   InverterApplication() main loop: inverter on / off
 *   InverterStatus()      main loop: flags, trips, AlarmFlag6
 * The voltage controller and the modulator run in the control sample
 * (controlCore.h ctl_inverter).
 *
 *  Created on: 23-Oct-2026
 *      Author: VCA
 */

#define CCEXTERN    extern
#define INIT0
#define INITARRAY0
#include "f28x_project.h"
#include "../ccVariables.h"

#ifdef _FLASH
#pragma CODE_SECTION(InverterSync, ".TI.ramfunc");
#endif

#define SQRT2           1.41421356f
#define SYNC_KP         0.000157f       // rad/sample per unit phase error: 0.5 Hz trim at 90 deg
#define OMEGA_SLEW      3.14e-8f        // free running frequency slew, rad/sample per ms (0.1 Hz/s)
#define SYNC_COUNT      50              // ms within LockingLevel -> synchronised

static float32_t prevInvSin = 0;
static float32_t syncErrInst = 0;
static float32_t omegaFree = OMEGADEFAULT;  // free running angle step when not tracking the bypass
static N16 bypfCount = 0, bypokCount = 0;
static N16 invOKold = 0;

/*
 20 kHz, from cpuPostControl(): bypass PLL and the inverter angle for the next
 control sample. With a usable bypass the inverter frequency is the bypass
 frequency trimmed by the phase error (sin of the angle difference), at most
 0.5 Hz; otherwise it runs free at omegaFree (slewed to 50 Hz in InverterTimer1ms).
 */
void InverterSync(void)
{
    float32_t err, w, sB, cB, sI, cI;

    phaseLockedLoop1ph(&pllByp, lpf_vbyp.y);
    lpf_omegaByp.u = pllByp.omega;
    LOWPASSFILTER(lpf_omegaByp);

    sI = theta_inv.sin_val;
    cI = theta_inv.cos_val;
    sB = pllByp.theta.sin_val;
    cB = pllByp.theta.cos_val;

    err = sB*cI - cB*sI;                        // sin(thetaB - thetaI)
    if((cB*cI + sB*sI) < 0.0f) err = (err >= 0.0f) ? 1.0f : -1.0f;     // beyond 90 deg: full trim
    syncErrInst = err;

    if(bypSyncOK)
    {
        w = lpf_omegaByp.y + SYNC_KP*err;
        omegaFree = w;
    }
    else w = omegaFree;

    if(w < PLL_LOCK_MIN*0.95) w = PLL_LOCK_MIN*0.95;
    if(w > PLL_LOCK_MAX*1.05) w = PLL_LOCK_MAX*1.05;
    omegaInv = w;

    angleInv += omegaInv;
    if(angleInv >= TWOPI) angleInv -= TWOPI;
    theta_inv.sin_val = sinf(angleInv);
    theta_inv.cos_val = cosf(angleInv);
    ctl.omegaInv = omegaInv * (float32_t)PFC_SAMPLING_FREQ;     // rad/s

    // inverter zero cross: the next control sample saves the output meters
    if((theta_inv.sin_val >= 0.0f) && (prevInvSin < 0.0f)) ctl.rsyncInv = 1;
    prevInvSin = theta_inv.sin_val;
}


void StaticSwitchControl(void)
{
    N16 invOK, bypOK;

    invOK = invPwmEnabled && !invSoftStart && chkVinv.stat.bit.OK && !InvFlag.bit.INVTRIPFAULT;
    bypOK = chkVbyp.stat.bit.OK && !bypassFailed;
    InvFlag.bit.INVOK = invOK;
    InvFlag.bit.BYPASSOK = bypOK;

    // inverter just became OK: wait xfrToInverterDelay before taking the load
    if(invOK && !invOKold) xfr2InvTimer = xfrToInverterDelay;
    invOKold = invOK;

    // to the inverter only when synchronised (or no bypass); an unsynchronised
    // inverter keeps the load where it is
    if(invOK && (Synced || !bypOK)) xfrCommand = 1;
    else if(!invOK && bypOK) xfrCommand = 2;
    else if(!invOK && !bypOK) xfrCommand = 0;
    else if(xfrCommand == 0) xfrCommand = 2;    // inverter OK, not synced, load off: bypass

    if(SystemFlag.bit.POWERINGUP || EmergencyStop || InvFlag.bit.MANUALBYP) xfrCommand = 0;

    if(xfrCommand == 0)
    {
        DisableInvSS();
        DisableBypSS();
        InvFlag.bit.LOADONINV = FALSE;
        InvFlag.bit.LOADONBYP = FALSE;
    }
    else if((xfrCommand == 1) && (xfr2InvTimer == 0))
    {
        DisableBypSS();
        EnableInvSS();
        InvFlag.bit.LOADONINV = TRUE;
        InvFlag.bit.LOADONBYP = FALSE;
    }
    else if(xfrCommand == 2)
    {
        DisableInvSS();
        EnableBypSS();
        InvFlag.bit.LOADONBYP = TRUE;
        InvFlag.bit.LOADONINV = FALSE;
    }
}


/*
 1 ms, from cpu_timer1_isr()
 */
void InverterTimer1ms(void)
{
    float32_t target;

    // gains, levels and scale factors for the control sample
    ctl.invKp = inv_kp;
    ctl.invKr = inv_kr;
    ctl.invKh = inv_kh;
    ctl.invKci = inv_kci;
    ctl.invHMask = (N16) pr_hMaskInv;
    ctl.invRevPol = InvRevPolarity;
    ctl.invOcLevel = InvOC_Level;
    ctl.invOcDelay = InvOC_Delay;
    ctl.vinvSF = VinvSF;
    ctl.iinvSF = IinvSF;
    ctl.vdcSF = DcLinkVoltageSF;
    bypassFailLevel = 0.8f * BypVoltageLL * SQRT2 / VbypSF;    // 80 % of the low limit, peak
    InvOverloadA = RatedCurrent * InvOverload * 0.01f;

    CheckHiLo(&chkVinv);
    CheckHiLo(&chkVbyp);
    CheckHiLo(&chkIout);

    // fast bypass failure: PLL amplitude below 80 % of the low limit for 5 ms
    if(pllByp.de < bypassFailLevel)
    {
        if(bypfCount < 5) bypfCount++;
        if(bypfCount == 5) { bypassFailed = TRUE; bypokCount = 0; }
    }
    else
    {
        if(bypfCount) bypfCount--;
        if(bypokCount < 50) bypokCount++;
        if(bypokCount == 50) { bypassFailed = FALSE; bypfCount = 0; }
    }

    bypSyncOK = chkVbyp.stat.bit.OK && !bypassFailed &&
                (lpf_omegaByp.y > PLL_LOCK_MIN) && (lpf_omegaByp.y < PLL_LOCK_MAX);

    // synchronised: phase error inside LockingLevel for SYNC_COUNT ms
    if(bypSyncOK && (fabsf(syncErrInst) < LockingLevel))
    {
        if(syncCount < SYNC_COUNT) syncCount++;
    }
    else if(syncCount) syncCount--;
    if(!bypSyncOK) syncCount = 0;
    if(syncCount == SYNC_COUNT) Synced = TRUE;
    else if(syncCount == 0) Synced = FALSE;
    syncError = syncErrInst;

    // free running: back to 50 Hz at 0.1 Hz/s
    if(!bypSyncOK)
    {
        if(omegaFree > OMEGADEFAULT + OMEGA_SLEW) omegaFree -= OMEGA_SLEW;
        else if(omegaFree < OMEGADEFAULT - OMEGA_SLEW) omegaFree += OMEGA_SLEW;
        else omegaFree = OMEGADEFAULT;
    }

    // output voltage soft start, 0 -> rated in 1 s
    target = RatedVoltage * SQRT2;
    if(invPwmEnabled)
    {
        if(invSoftStart)
        {
            ctl.invVref += target * 0.001f;
            if(ctl.invVref >= target) { ctl.invVref = target; invSoftStart = FALSE; }
        }
        else ctl.invVref = target;              // follows RatedVoltage changes
    }
    else ctl.invVref = 0.0f;

    // seconds: transfer delay, restart, trip count reset
    if(milliseconds == 0)
    {
        if(xfr2InvTimer) xfr2InvTimer--;

        if(invRunningTimer < FaultResetDelay) invRunningTimer++;
        if((invRunningTimer == FaultResetDelay) && !InvFlag.bit.INVLATCH) InvTripCount = 0;

        if(invRestartTimer)
        {
            invRestartTimer--;
            if((invRestartTimer == 0) && (InvTripCount < InvMaxRestart))
            {
                INV_AUTO_RESET_FAULTS();
            }
        }
    }

    StaticSwitchControl();
}


static void invStop(void)
{
    DISABLE_INVPWM();
    invPwmEnabled = FALSE;
    ctl.invOn = 0;
    invSoftStart = FALSE;
}

/*
 Main loop: inverter on with Unit ON (DI1 or remote), InvEnable and the DC link
 above InvDCLinkMin (10 V hysteresis for a start). The output ramps up in 1 s.
 */
void InverterApplication(void)
{
    if(SystemFlag.bit.POWERINGUP || EmergencyStop || InvFlag.bit.INVTRIPFAULT)
    {
        invStop();
        return;
    }

    if(DcLinkVoltage < InvDCLinkMin) InvFlag.bit.DCLOWINV = TRUE;
    else if(DcLinkVoltage > (InvDCLinkMin + 10.0f)) InvFlag.bit.DCLOWINV = FALSE;

    if(InvEnable && (UnitOnCommand || unitOnCommand) && !InvFlag.bit.DCLOWINV)
    {
        if(invPwmEnabled == FALSE)
        {
            UPDATE_INV_FSW();
            ctl.invVref = 0.0f;
            invSoftStart = TRUE;
            // the control holds the voltage controller at zero while ctl.invOn == 0:
            // set it first so the control runs before the first switching period
            invPwmEnabled = TRUE;
            ctl.invOn = 1;
            ENABLE_INVPWM();
            invRunningTimer = 0;
        }
    }
    else invStop();
}


/*
 Main loop, after updateStatus(): inverter flags and trips. Instantaneous
 inverter overcurrent (TRIP_INV_OC) is latched into InvFlag by updateStatus().
 */
void InverterStatus(void)
{
    InvFlag.bit.INVDISABLE = !invPwmEnabled;
    InvFlag.bit.INVSOFTSTART = invSoftStart;
    InvFlag.bit.MANUALBYP = ((DigitalInputFlag1 & DI_MANUALBYP) == 0);     // contact closed
    InvFlag.bit.NOTSYNCED = !Synced;
    InvFlag.bit.INVVLOW = invPwmEnabled && !invSoftStart && chkVinv.stat.bit.LOW;
    if(chkVinv.stat.bit.HIGH) InvFlag.bit.INVVHIGH = TRUE;
    if(chkIout.stat.bit.HIGH) InvFlag.bit.OVERLOAD = TRUE;

    // bridge 2 desaturation (VCESAT2 -> DCAEVT1 of EPWM4-6); each flag stays
    // latched until that bridge is enabled again, so only check enabled ones
    if((invPwmEnabled && EPwm4Regs.TZFLG.bit.DCAEVT1) || (bbPulseON && EPwm6Regs.TZFLG.bit.DCAEVT1))
        BattFlag.bit.BBVCESAT = TRUE;

    if(!SystemFlag.bit.POWERINGUP &&
       (InvFlag.bit.INVOCTRIP || InvFlag.bit.INVVHIGH || InvFlag.bit.OVERLOAD || BattFlag.bit.BBVCESAT))
    {
        invStop();
        if(!InvFlag.bit.INVTRIPFAULT)
        {
            if(InvTripCount < InvMaxRestart) InvTripCount++;
            if(BattFlag.bit.BBVCESAT) InvTripCount = InvMaxRestart;
            invRunningTimer = 0;
            invRestartTimer = RestartDelay;
            InvFlag.bit.INVTRIPFAULT = TRUE;
            if(InvTripCount == InvMaxRestart) InvFlag.bit.INVLATCH = TRUE;
        }
    }

    InvFlag.bit.BITSVALID = TRUE;
    AlarmFlag6 = InvFlag.all;
}

//===========================================================================
//                          End of file
//===========================================================================
