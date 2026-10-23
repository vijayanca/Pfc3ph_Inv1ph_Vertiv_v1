
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
#include "../controlCore.h"

volatile float prevSin = 0;
volatile float prevR = 0;


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
#pragma CODE_SECTION(pll_and_sincos, ".TI.ramfunc");
#endif

inline void pll_and_sincos(void);

/*
 CPU part of each 20 kHz control sample, after the control math (controlCore.h
 ctl_task1(), on the CLA or in AdcISR): instantaneous trips from the sample,
 grid PLL and angle for the next sample, zero cross.
 */
static inline void cpuPostControl(void)
{
    N16 evt = ctl.tripEvt;

    if(evt)
    {
        // the pulses are already off (ctl_protect forced the one-shot trip);
        // the main loop turns the latched bits into flags and trips
        ctl.tripEvt = 0;
        DISABLE_PFCPWM();
        tripLatch |= evt;
    }

    pll_and_sincos();

    if((pllGrid.theta.sin_val >= 0.0) && (prevSin < 0.0))
    {
        rsync = 1;      // the next control sample saves the RMS meters
        if (snap_end == 0) snap_start = 1;
    }
    prevSin = pllGrid.theta.sin_val;
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


/*
 Routine tasks like IO control, delays etc runs at 1ms
 */
__interrupt void cpu_timer1_isr(void)
{
	CpuTimer1.InterruptCount++;
	mainLoopCount_prev = mainLoopCount;
	mainLoopCount = 0;

    // phase sequence: at the R zero cross (rising) Y must be negative
    if((lpf_vR.y>0) && prevR<0)
    {
        if(lpf_vY.y>0) { if(wrongSeqCount<10) wrongSeqCount++; }
        else { if(wrongSeqCount) wrongSeqCount--; }
    }
    prevR = lpf_vR.y;

    if(Syncfailtimer) Syncfailtimer--;

    //--------- set points and gains for the control sample (CLA) ----------
	// dead-time compensation for the modulator (ctl_pwm): full correction DBRED/TBPRD
	ctl.dtGain = DtComp * (float32_t)EPwm1Regs.DBRED.all / (float32_t)EPwm1Regs.TBPRD;
	ctl.dtBandInv = (DtBand > 0.001f) ? (1.0f / DtBand) : 1000.0f;

    ctl.qRef = PFSign ? -QRef : QRef;
    ctl.crossXL = CrossXL;
    ctl.prKp = PR_Kp;
    ctl.prKr = PR_Kr;
    ctl.prKh = PR_Kh;
    ctl.ctrlMode = PfcCtrlMode;
    ctl.revPolarity = RevPolarity;
    ctl.ocDelay = OC_Delay;
    ctl.dcovDelay = DCOV_Inst_Delay;
    ctl.ocLevel = OC_Level / IinpRSF;                   // scale factors are range limited to 10..5000
    ctl.dcOVLevel = DCOV_Inst_Level / DcLinkVoltageSF;

	 if (interchar_silent_timer < INTERCHARDELAY)    interchar_silent_timer++;
	 if (interchar_silent_timer1 < INTERCHARDELAY)   interchar_silent_timer1++;

	    if (spi_timeout < SPI_TIME)
	        spi_timeout++;

	    if(msDelayCount)    msDelayCount--;

        CheckHiLo(&chkVr);
        CheckHiLo(&chkVy);
        CheckHiLo(&chkVb);

        CheckHiLo(&chkIr);
        CheckHiLo(&chkIy);
        CheckHiLo(&chkIb);

        CheckHiLo(&chkdcLink);
        CheckHiLo(&chkFrequency);

	    milliseconds++;

	        if (milliseconds> 999)
	        {
	            milliseconds = 0;
	            seconds++;
	            txMsgCount = txMsgCount_tmp;
	            txMsgCount_tmp = 0;

	            canRxCounter = tmp_canRxCounter;
	            tmp_canRxCounter = 0;

	            FSW_pfc = temp_fsw1;
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

	                if(AutoSaveTimer) AutoSaveTimer--;
	                if(AutoSaveTimer==1) autoSaveFlag = TRUE;

	                // precharge: DC link above PreChargeLevel for PreCharge_delay seconds
	                checkLimitL(PreChargeLevel, DcLinkVoltage, PreCharge_delay, PrechargeTimer);
	        } //seconds condition

	        if(SystemFlag.bit.POWERINGUP)
	        {
	            if (poweronTimer < PowerOnDelay) poweronTimer++;
	        else    SystemFlag.bit.POWERINGUP = FALSE;

	        }

	        // SYSLED (GPIO62, LED5): 5 Hz when running, 1 Hz when stopped
	        if(pwmEnabled)
	        {
	            { if(milliseconds%100 == 0) GPIO_WritePin(62,2); }
	        }

	        else
	            { if(milliseconds%500 == 0) GPIO_WritePin(62,2); }

            // main contactor without aux feedback: closed 0.5 s after the command
            if(SystemFlag.bit.MAINCON) { if(mainConTimer < 500) mainConTimer++; }
            else mainConTimer = 0;

            //----------------------- PFC soft start (RBC) -----------------------
            // d output lower limit released from 0 to Umin at 0.1 p.u./s
            if(Dmax_Ramping)
            {
                if(DMinVal > pfc.pid_de.Umin)  DMinVal -= 0.0001;
                if(DMinVal <= pfc.pid_de.Umin) Dmax_Ramping = 0;
            }
            ctl.dmaxRamping = Dmax_Ramping;

            if(pfcSoftStart)
            {
                // Hold the DC-link ramp while the d output is pinned at the Dmax_Ramping
                // limit: the link reference must not run ahead of the boost authority.
                if(Dmax_Ramping && (pfc.pid_de.Out <= (DMinVal + 0.005f))) Nop();
                else if(dcLinkSoftRef < dcLinkReference) dcLinkSoftRef += ref_inc;
                if(dcLinkSoftRef > dcLinkReference) dcLinkSoftRef = dcLinkReference;
                if(dcLinkSoftRef == dcLinkReference) pfcSoftStart = FALSE;
            }
            else
            {
                // reference lowered while running: follow it down at the same rate
                if(dcLinkSoftRef > dcLinkReference) dcLinkSoftRef -= ref_inc;
                if(dcLinkSoftRef < dcLinkReference) dcLinkSoftRef = dcLinkReference;
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

// ADC offsets against the 1.5 V reference, averaged over 1 s (50 grid cycles)
// at power up, before the contactors close
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

	temp_char = SciaRegs.SCIRXBUF.all;


	interchar_silent_timer = 0;
	rxbuffer = &rx_buffer[0];

	if ((live_frame == FALSE) && (temp_char == address)) {
		char_waiting = 0;
		live_frame = TRUE;
		frame_complete = FALSE;
	}
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
    PR_RES_CLEAR(prA);  PR_RES_CLEAR(prB);
    PR_RES_CLEAR(prA5); PR_RES_CLEAR(prB5);
    PR_RES_CLEAR(prA7); PR_RES_CLEAR(prB7);
}

/*
 Grid PLL on the grid phase voltages (Vbyp sensors) and the angle for the
 next control sample: the PLL angle of this sample advanced by one sample,
 so the dq transform of the next sample's currents is not one sample late.
 */
inline void pll_and_sincos(void)
{
    float32_t s, c, w, c0;

        pllGrid.va =  lpf_vR.y;
        pllGrid.vb =  lpf_vY.y;
        pllGrid.vc =  lpf_vB.y;
        pll_omega = phaseLockedLoopB(&pllGrid);
        lpf_omega.u = pll_omega;
        LOWPASSFILTER(lpf_omega);

    w = pll_omega;
    c0 = 1.0f - 0.5f*w*w;
    s = pllGrid.theta.sin_val;
    c = pllGrid.theta.cos_val;
    theta.sin_val = s*c0 + c*w;
    theta.cos_val = c*c0 - s*w;

    ctl.vol_qe_ff = pllGrid.lpf1.y;
    ctl.omega = lpf_omega.y * (float32_t)PFC_SAMPLING_FREQ;     // rad/sample -> rad/s
}

//===========================================================================
// 							End of file
//===========================================================================

