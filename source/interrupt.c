
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
#include "../controlCore.h"

extern pllBlock  pllBypass;
volatile float current_zero = 0;
volatile float current_d = 0;
volatile float current_q = 0;
volatile float prevSin = 0;

volatile int Dac1val = 0, Dac2val = 0;
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
#if CONTROL_ON_CLA
#pragma CODE_SECTION(ClaTask1EndISR, ".TI.ramfunc");
#else
#pragma CODE_SECTION(AdcISR, ".TI.ramfunc");
#endif
#pragma CODE_SECTION(CurrentModeSelection, ".TI.ramfunc");
#pragma CODE_SECTION(pll_and_sincos, ".TI.ramfunc");
#pragma CODE_SECTION(SendCANPackets, ".TI.ramfunc");
#endif

const float32_t prOrder[PR_NHARM] = {5.0, 7.0, 11.0, 13.0};

inline void SendCANPackets(void);
inline void CurrentModeSelection(void);
inline void pll_and_sincos(void);

/*
 CPU part of each 20 kHz control sample, after the control math (controlCore.h
 ctl_task1(), on the CLA or in AdcISR): flags from the sample, master/slave
 current mode, CAN-B paralleling TX, bypass PLL and inverter angle for the next
 sample, zero cross / sync / pulse enable.
 */
static inline void cpuPostControl(void)
{
    if(ctl.invFailEvt == INVFAIL_SET) SystemFlag.bit.INVFAILURE = TRUE;
    else if(ctl.invFailEvt == INVFAIL_CLEAR) SystemFlag.bit.INVFAILURE = FALSE;
    ctl.invFailEvt = 0;

       if(Master==0)
           {
               CurrentModeSelection();
               if(MasterPresentTimer>5) CommStatus.bit.CAN_MASTER =  TRUE;
               else  CommStatus.bit.CAN_MASTER =  FALSE;
           }

//-------------------------------------------------------------------------------------
       SendCANPackets();
       pll_and_sincos();
       ctl.pllQeComp = pllBypass.lpf1.y;
       ctl.ctlSoftStart = SystemFlag.bit.SOFTSTART;
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
                          rsync = 1;      // the next control sample saves the RMS meters

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
}

#if CONTROL_ON_CLA
/*
 CLA task 1 (control sample, triggered by ADCA INT1) has finished: CPU part of
 the sample. adcIsrTime = SYSCLK counts from the ADC trigger to the end of this
 ISR (conversions + CLA task + CPU part); claT1Ticks = same, to the end of the
 CLA task.
 */
interrupt void ClaTask1EndISR(void)
{
    temp_AdcIsrCount++;

    cpuPostControl();

    adcIsrTime = (int) ctl_ticksSinceSoc();
    if(adrIsrDuration < adcIsrTime) adrIsrDuration = adcIsrTime;

    PieCtrlRegs.PIEACK.all = PIEACK_GROUP11;
}
#else
/*
 Control sample on the CPU (CONTROL_ON_CLA = 0). adcIsrTime = SYSCLK counts
 from the ADC trigger to the end of this ISR, as for the CLA build.
 */
interrupt void AdcISR(void)
{
    AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;

    if(AdcaRegs.ADCINTOVF.bit.ADCINT1)
    {
        AdcaRegs.ADCINTOVFCLR.bit.ADCINT1 = 1; //clear INT1 overflow flag
        AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1; //clear INT1 flag
    }

    temp_AdcIsrCount++;

    ctl_task1();
    ctl.claT1Ticks = ctl_ticksSinceSoc();

    cpuPostControl();

    adcIsrTime = (int) ctl_ticksSinceSoc();
    if(adrIsrDuration < adcIsrTime) adrIsrDuration = adcIsrTime;

    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;// Acknowledge interrupt to PIE
}
#endif


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

	// dead-time compensation for the modulator (ctl_pwm): full correction DBRED/TBPRD
	ctl.dtGain = DtComp * (float32_t)EPwm1Regs.DBRED.all / (float32_t)EPwm1Regs.TBPRD;
	ctl.dtBandInv = (DtBand > 0.001f) ? (1.0f / DtBand) : 1000.0f;

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

