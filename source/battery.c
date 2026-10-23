/*
 * battery.c
 *
 * Battery management, 1 ms (BatteryControl, from cpu_timer1_isr) and main
 * loop (BatteryStatus):
 *
 *  BuckBoostEnable = 0: battery directly on the DC link. The DC-link reference
 *    is the float voltage VbatFloat, lowered (DcLinkMinNoBuck .. DcLinkMaxNoBuck,
 *    320 .. 400 V) by a PI on the charging current so it stays at IbatRef.
 *    On mains failure the battery feeds the DC link directly.
 *
 *  BuckBoostEnable = 1: DC link at DcLinkReference (680 V), battery ~400 V on
 *    the buck-boost (EPWM6):
 *      PFC running (DC link ramped)  -> buck, CC/CV charging (VbatFloat, IbatRef)
 *      PFC off (mains failure)       -> boost, DC link held at DcLinkBoostRef
 *      battery at end of discharge   -> off
 *    The loops run in the control sample (controlCore.h ctl_buckboost).
 *
 *  Created on: 23-Oct-2026
 *      Author: VCA
 */

#define CCEXTERN    extern
#define INIT0
#define INITARRAY0
#include "f28x_project.h"
#include "../ccVariables.h"

#define VBAT_REF_STEP   0.01f       // buck voltage reference ramp, V per ms (10 V/s)

void BatteryControl(void)
{
    float32_t ibat, dv, ref, span, sgn;
    N16 mode;

    sgn = IbatPolarity ? -1.0f : 1.0f;
    ctl.ibatSign = IbatSF * sgn;
    ctl.vbatSF = VbatSF;
    ibat = lpf_ibat.y * ctl.ibatSign;           // charging positive
    IbatCharge = ibat;
    BattFlag.bit.BUCKFITTED = BuckBoostEnable;

    if(!BuckBoostEnable)
    {
        //---------------- battery on the DC link ----------------
        if(bbPulseON) { DISABLE_BBPWM(); }
        bbMode = 0;
        ctl.bbSel = 0;

        span = DcLinkMaxNoBuck - DcLinkMinNoBuck;
        if(span < 0.0f) span = 0.0f;
        if(pwmEnabled)
        {
            // excess charging current lowers the DC-link reference
            float32_t e = ibat - IbatRef;
            nobuck_ui += nobuck_ki * e;
            if(nobuck_ui < 0.0f) nobuck_ui = 0.0f;
            if(nobuck_ui > span) nobuck_ui = span;
            dv = nobuck_kp * e + nobuck_ui;
            if(dv < 0.0f) dv = 0.0f;
            if(dv > span) dv = span;
        }
        else { nobuck_ui = 0.0f; dv = 0.0f; }

        ref = VbatFloat - dv;
        if(ref < DcLinkMinNoBuck) ref = DcLinkMinNoBuck;
        if(ref > DcLinkMaxNoBuck) ref = DcLinkMaxNoBuck;
        DcLinkRefAuto = ref;
        dcLinkReference = ref / DcLinkVoltageSF;

        BattFlag.bit.CURRENTLIMIT = (dv > 1.0f);
        BattFlag.bit.CHARGING = pwmEnabled;
        BattFlag.bit.DISCHARGING = !pwmEnabled && invPwmEnabled;
        return;
    }

    //-------------------- buck-boost --------------------
    dcLinkReference = DcLinkReference / DcLinkVoltageSF;
    DcLinkRefAuto = DcLinkReference;

    pid_vbat.Kp = vbat_kp;   pid_vbat.Ki = vbat_ki;   pid_vbat.Umax = 0.95f;  pid_vbat.Umin = 0.0f;
    pid_ibat.Kp = ibat_kp;   pid_ibat.Ki = ibat_ki;   pid_ibat.Umax = 0.95f;  pid_ibat.Umin = 0.0f;
    pid_boost.Kp = boost_kp; pid_boost.Ki = boost_ki; pid_boost.Umin = 0.0f;
    ctl.ibatRef = IbatRef;
    ctl.boostRef = DcLinkBoostRef;
    ctl.boostDmax = BoostDmax;
    ctl.bbOcLevel = IbatOC_Level;
    ctl.bbOcDelay = BBOC_Delay;

    if(pwmEnabled) BattFlag.bit.BATTEOD = FALSE;    // mains back: end of discharge cleared

    if(SystemFlag.bit.POWERINGUP || EmergencyStop || BattFlag.bit.BBOCTRIP || BattFlag.bit.BBVCESAT)
        mode = 0;
    else if(pwmEnabled && !pfcSoftStart && !BattFlag.bit.VBATHIGH)
        mode = 1;
    else if(!pwmEnabled && (UnitOnCommand || unitOnCommand) && !BattFlag.bit.BATTEOD && (Vbat > VbatLL))
        mode = 2;
    else mode = 0;

    if(mode != bbMode)
    {
        if(mode == 0)
        {
            DISABLE_BBPWM();
            ctl.bbSel = 0;
        }
        else
        {
            if(mode == 1) ctl.vbatRef = Vbat;   // CV reference ramps from the present voltage
            ctl.bbSel = mode;                  // the control resets the PI states on a change
            ENABLE_BBPWM();
        }
        bbMode = mode;
    }

    if(ctl.vbatRef < VbatFloat) { ctl.vbatRef += VBAT_REF_STEP; if(ctl.vbatRef > VbatFloat) ctl.vbatRef = VbatFloat; }
    else if(ctl.vbatRef > VbatFloat) { ctl.vbatRef -= VBAT_REF_STEP; if(ctl.vbatRef < VbatFloat) ctl.vbatRef = VbatFloat; }

    BattFlag.bit.CHARGING = (bbMode == 1);
    BattFlag.bit.DISCHARGING = (bbMode == 2);
    BattFlag.bit.CURRENTLIMIT = (bbMode == 1) && (pid_ibat.Out < pid_vbat.Out);
}

/*
 Main loop: battery limits and AlarmFlag7. Battery overcurrent (TRIP_BB_OC) is
 latched into BattFlag by updateStatus(); it and VCESAT2 clear with the
 inverter fault reset (INV_RESET_FAULTS / INV_AUTO_RESET_FAULTS).
 */
void BatteryStatus(void)
{
    BattFlag.bit.VBATLOW = chkVbat.stat.bit.LOW;
    BattFlag.bit.VBATHIGH = chkVbat.stat.bit.HIGH;
    if(BuckBoostEnable && (bbMode == 2) && chkVbat.stat.bit.LOW) BattFlag.bit.BATTEOD = TRUE;

    bbDuty_mon = ctl.bbDuty;
    BattFlag.bit.BITSVALID = TRUE;
    AlarmFlag7 = BattFlag.all;
}

//===========================================================================
//                          End of file
//===========================================================================
