
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
extern pllBlock  pllGrid;
#include "ecap.h"

#ifdef _FLASH
    #pragma CODE_SECTION(cpu_timer2_isr, ".TI.ramfunc");
    #pragma CODE_SECTION(ecap1ISR, ".TI.ramfunc");
    #pragma CODE_SECTION(SCIARx_ISR, ".TI.ramfunc");
    #pragma CODE_SECTION(SCIBRx_ISR, ".TI.ramfunc");
#endif

#include "../CosTable.h"
volatile int MasterRunning_old = 0;
float _vdcsum = 0, _idc1sum = 0,_idc2sum = 0, _ibatsum = 0;

int volatile diff1 = 0;
int volatile diff2 = 0;

volatile N16 counterValueR = 0;
volatile N16 counterValueY = 0;
volatile N16 counterValueB = 0;
float volatile prev_valueR = 0;
float volatile prev_valueY = 0;

volatile N16 simBalance = 0;        // 1 = use simulated Idc1/Idc2 counts
volatile N16 simIdc1Count = 1200;   // raw ADC counts (0..4095)
volatile N16 simIdc2Count = 1300;


#pragma CODE_SECTION(AdcISR, ".TI.ramfunc");

#define FLOAT_CONVERTER_DC    0.000244140625
#define FLOAT_CONVERTER_AC    0.00048828125

float prevSin = 0;
float prevSin1 = 0;
N16 npoints = 0;
N16 qrtrCycle = 0;
uint32_t cap_now = 0;
uint32_t cap_now_n = 0;
volatile uint32_t cap_prev = 0;
volatile uint32_t dt_ticks = 0;
volatile uint32_t sum_dt_ticks = 0;

int tmp_zcdCountN = 0;

void inline piCon(PI_CONTROLLER *v)
{
    v->err = v->Ref - v->Fbk;
    v->up = v->Kp * v->err;
    v->ui += v->Ki * v->err;
    if (v->ui > v->Umax) v->ui = v->Umax;
    if (v->ui < v->Umin) v->ui = v->Umin;
    v->v1 = v->up + v->ui;
    if (v->v1 > v->Umax) v->Out = v->Umax;
    else if (v->v1 < v->Umin) v->Out = v->Umin;
    else v->Out = v->v1;

}


/*--------------------------------------------------------------------------
 Function Name: AdcISR
 Description:   This interrupt captures the AD count values as well as acts
 as a ZC detector. The two half cycles are captured in two
 separate arrays. The very first half cycles is not captured
 Capture starts from second half cycle.

 Takes          :   None
 Returns        :   None
 Funcs Called   :   None
 Author         :   Vijayan C A

 History Log:
 ---------------------------------------------------------------------------
  Date      | Modified By |         Comments
 ---------------------------------------------------------------------------
 10/02/2012   VCA               Created
 ---------------------------------------------------------------------------*/
volatile int sptr = 0;
interrupt void AdcISR(void) {

    AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;
    if(AdcaRegs.ADCINTOVF.bit.ADCINT1)
    {
        AdcaRegs.ADCINTOVFCLR.bit.ADCINT1 = 1; //clear INT1 overflow flag
        AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1; //clear INT1 flag
    }

    // (ISR timing marker on GPIO60 removed: GPIO60 is RLY1 on the Common Controller v2)
    temp_AdcIsrCount++;
    if(piCounter<PI_Interval) piCounter++;
    else {piCounter = 0; doPID = TRUE; }

    // Common Controller v2, SOC map in BoardInit.c setupADC(); AnalogInputN
    // follow the labels in RectParams.h (ADC / SOC / channel)
    AnalogInput1  = AdcdResultRegs.ADCRESULT0; // D 0  D14  Vinp_R
    AnalogInput2  = AdceResultRegs.ADCRESULT1; // E 1  E12  Vinp_Y
    AnalogInput3  = AdcbResultRegs.ADCRESULT0; // B 0  B3   Vinp_B

    AnalogInput4  = AdcaResultRegs.ADCRESULT1; // A 1  A5   Iinp_R
    AnalogInput5  = AdcbResultRegs.ADCRESULT1; // B 1  B8   Iinp_Y
    AnalogInput6  = AdcaResultRegs.ADCRESULT2; // A 2  A8   Iinp_B

    AnalogInput7  = AdccResultRegs.ADCRESULT1; // C 1  C0   Vdc
    AnalogInput8  = AdcaResultRegs.ADCRESULT0; // A 0  A0   Idc1 (bridge 1)
    AnalogInput9  = AdccResultRegs.ADCRESULT0; // C 0  C5   Idc2 (bridge 2)
    AnalogInput10 = AdccResultRegs.ADCRESULT3; // C 3  C2   Vbat
    AnalogInput11 = AdcdResultRegs.ADCRESULT3; // D 3  D11  Ibat
    AnalogInput12 = AdcaResultRegs.ADCRESULT3; // A 3  A14  Vload
    AnalogInput13 = 0;                         //           Vref (external) - not on this board
    AnalogInput14 = 0;                         //           Iref (external) - not on this board
    AnalogInput15 = AdcbResultRegs.ADCRESULT3; // B 3  B0   Vref1.5
    AnalogInput16 = AdcaResultRegs.ADCRESULT4; // A 4  A9   Temp1
    AnalogInput17 = AdccResultRegs.ADCRESULT4; // C 4  C8   Temp2
    AnalogInput18 = AdcbResultRegs.ADCRESULT4; // B 4  B1   Temp3 (display only)
    adcSamples++;

    lpf_bias.u = (float) (AnalogInput15) * FLOAT_CONVERTER_AC;
     LOWPASSFILTER(lpf_bias);

     // Balancing test: set simBalance = 1 from the debugger to replace the
     // Idc1/Idc2 ADC counts with simIdc1Count/simIdc2Count. Leave 0 in service.
     if(simBalance)
     {
         AnalogInput8 = simIdc1Count;
         AnalogInput9 = simIdc2Count;
     }

     lpf_vInpR.u = (float) (AnalogInput1 - vrOffset) * FLOAT_CONVERTER_AC - lpf_bias.y;
     lpf_vInpY.u = (float) (AnalogInput2 - vyOffset) * FLOAT_CONVERTER_AC - lpf_bias.y;
     lpf_vInpB.u = (float) (AnalogInput3 - vbOffset) * FLOAT_CONVERTER_AC - lpf_bias.y;
     LOWPASSFILTER(lpf_vInpR);
     LOWPASSFILTER(lpf_vInpY);
     LOWPASSFILTER(lpf_vInpB);

     pllGrid.va =  lpf_vInpR.y;   //Vab
     pllGrid.vb =  lpf_vInpY.y;   //Vbc;
     pllGrid.vc =  lpf_vInpB.y;   //Vca;
     pll_omega = phaseLockedLoopB(&pllGrid);

//--------------------------------------------------------------------------
          //if( (lpf_vInpR.y>= 0.0) && (prev_valueR< 0.0) )
          if( (pllGrid.theta.sin_val>= 0.0) && (prev_valueR< 0.0) )
                 {
                    STOP_TIMER0;
                    LOAD_TIMER0(AlphaCount);
                    START_TIMER0;
                    alpha_R_start = ECap1Regs.TSCTR;
                    zcdCount = 0;
                    rp_count = 0;
                    rsync = TRUE;

                    if (snap_end == 0) snap_start = 1;

                    if((lpf_vInpY.y > 0) || (lpf_vInpB.y<0))
                    {
                        if (seqFailTimer<SEQFAILDELAY) seqFailTimer++;
                    }
                    else if(seqFailTimer) seqFailTimer--;

                    cap_now = ECap1Regs.TSCTR;
                    dt_ticks = (uint32_t)(cap_now - cap_prev);   // unsigned handles wrap safely
                    cap_prev = cap_now;
                    tmp_zcdCount++;

                    sum_dt_ticks += dt_ticks;
                    sptr++;
                    if(sptr>=32) {sptr = 0; periodCount = (sum_dt_ticks >>5); sum_dt_ticks = 0; }

                    if(pwmEnabled==1)
                        {
                            ncycle++;
                            if(ncycle==3)
                            {
                               ENABLE_PWM();
                               pwmEnabled = 2;
                               ncycle = 0;
                            }
                        }
                 }
            prev_valueR = pllGrid.theta.sin_val;
//            prev_valueR = lpf_vInpR.y;

            if( (pllGrid.theta.sin_val<= 0.0) && (prev_valueY> 0.0) )
                 {
                    STOP_TIMER1;
                    LOAD_TIMER1(AlphaCount);
                    START_TIMER1;
                    alpha_Y_start = ECap1Regs.TSCTR;
                    bp_count = 0;
                    tmp_zcdCountN++;

                    if(adcSamples<MinPoints) adcSamples = FullCycle;
                    SamplesPerCycle = adcSamples;
                    adcSamples = 0;
                    cycle3count = SamplesPerCycle/AverageCount;

                 }
            prev_valueY = pllGrid.theta.sin_val;
//----------------------------------------------------------------------------------------


     lpf_iInpR.y = (float) (AnalogInput4 - irOffset) * FLOAT_CONVERTER_AC - lpf_bias.y;
     lpf_iInpY.y = (float) (AnalogInput5 - iyOffset) * FLOAT_CONVERTER_AC - lpf_bias.y;
     lpf_iInpB.y = (float) (AnalogInput6 - ibOffset) * FLOAT_CONVERTER_AC - lpf_bias.y;

     lpf_vdc.u = (float)AnalogInput7 * FLOAT_CONVERTER_DC;
     LOWPASSFILTER(lpf_vdc);
     lpf_idc1.u = (float)AnalogInput8 * FLOAT_CONVERTER_DC;
     LOWPASSFILTER(lpf_idc1);
     lpf_idc2.u = (float)AnalogInput9 * FLOAT_CONVERTER_DC;
     LOWPASSFILTER(lpf_idc2);
     lpf_ibat.u = (float)(AnalogInput11 - IbatOffset) * FLOAT_CONVERTER_DC;
     LOWPASSFILTER(lpf_ibat);

     vdc_buf[vptr] = lpf_vdc.u;
     idc1_buf[vptr] = lpf_idc1.u;
     idc2_buf[vptr] = lpf_idc2.u;
     ibat_buf[vptr] = lpf_ibat.u;
     vptr++;
     if(vptr>=cycle3count) vptr = 0;
     if(vptr>=220) vptr = 0;


     samplingCount_tmp++;
     vdcCycleSum += lpf_vdc.u;

     lpf_vbat.u = (float)AnalogInput10 * FLOAT_CONVERTER_DC;
     LOWPASSFILTER(lpf_vbat);

     lpf_vload.u = (float)AnalogInput12 * FLOAT_CONVERTER_DC;
     LOWPASSFILTER(lpf_vload);

     lpf_temp1.u = (float)AnalogInput16 * FLOAT_CONVERTER_DC;
     lpf_temp2.u = (float)AnalogInput17 * FLOAT_CONVERTER_DC;
     lpf_temp3.u = (float)AnalogInput18 * FLOAT_CONVERTER_DC;
     LOWPASSFILTER(lpf_temp1);
     LOWPASSFILTER(lpf_temp2);
     LOWPASSFILTER(lpf_temp3);
//---------------------------------------------------------------------------

     // External V/I reference inputs are not on the Common Controller v2
     // (AnalogInput13/14 = 0): use ExtRefEnable = 0.
     lpf_vref.u = (float)AnalogInput13 * FLOAT_CONVERTER_DC;
     lpf_iref.u = (float)AnalogInput14 * FLOAT_CONVERTER_DC;
     LOWPASSFILTER(lpf_vref);
     LOWPASSFILTER(lpf_iref);

     if(lpf_vref.y<(0.66)) extVref = lpf_vref.y;
     else extVref = (0.66);
     if(lpf_iref.y<(0.66)) extIbatref = lpf_iref.y;
     else extIbatref = (0.66);

     vol_Rph = (lpf_vInpR.y - lpf_vInpB.y) * 0.33333333333f;
     vol_Yph = (lpf_vInpY.y - lpf_vInpR.y) * 0.33333333333f;
     vol_Bph = (lpf_vInpB.y - lpf_vInpY.y) * 0.33333333333f;
     rmsCal();

     //-----------------------------------------------------------------------------
          saveCounter++;
          saveCounter &= 1023;

          if (snap_start == 1) {
              if ((saveCounter % nCycles) == 0)
              {
                  waveBuffer[snap_ptr] = (*waveptr);
                  snap_ptr++;
                  if (snap_ptr >= SNAP_SIZE)
                  {
                      snap_start = 0;
                      snap_end = 1;
                      snap_ptr = 0;   //count starts from 0 to SnapSize
                  }
              }
          }
     //-----------------------------------------------------------------------------

     PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;// Acknowledge interrupt to PIE
 }

//---------------------------------------------------------------------------
//  rmsCal:
//---------------------------------------------------------------------------
//  This function calculates the RMS values of the voltages and currents
//  required for display through HMI

inline void rmsCal() {

    UPDATE_RMS(vll_meter_R, lpf_vInpR.y);
    UPDATE_RMS(vll_meter_Y, lpf_vInpY.y);
    UPDATE_RMS(vll_meter_B, lpf_vInpB.y);

    UPDATE_RMS(volt_meter_R, vol_Rph);
    UPDATE_RMS(volt_meter_Y, vol_Yph);
    UPDATE_RMS(volt_meter_B, vol_Bph);

    UPDATE_RMS(vdcRipple_meter, (lpf_vdc.u - vdcAvg));

    UPDATE_RMS(cur_meter_R, lpf_iInpR.y);
    UPDATE_RMS(cur_meter_Y, lpf_iInpY.y);
    UPDATE_RMS(cur_meter_B, lpf_iInpB.y);

    UPDATE_KW(kw_meter_R, vol_Rph, lpf_iInpR.y, VinpRSF, IinpRSF);
    UPDATE_KW(kw_meter_Y, vol_Yph, lpf_iInpY.y, VinpYSF, IinpYSF);
    UPDATE_KW(kw_meter_B, vol_Bph, lpf_iInpB.y, VinpBSF, IinpBSF);

    if (rsync) {
        //  Nop();
        rms_samplecount = volt_meter_R.sample_count;

        SAVE_METER(vll_meter_R);
        SAVE_METER(vll_meter_Y);
        SAVE_METER(vll_meter_B);

        SAVE_METER(volt_meter_R);
        SAVE_METER(volt_meter_Y);
        SAVE_METER(volt_meter_B);

        SAVE_METER(cur_meter_R);
        SAVE_METER(cur_meter_Y);
        SAVE_METER(cur_meter_B);

        SAVE_METER(kw_meter_R);
        SAVE_METER(kw_meter_Y);
        SAVE_METER(kw_meter_B);
        SAVE_METER(vdcRipple_meter);

        rms_update_flag = 1;
        rsync = 0;
    }

}


interrupt void SCIARx_ISR(void)
{

	N16 temp_char = 0;

	errorRx = SciaRegs.SCIRXST.bit.RXERROR;
	if (1)//!errorRx)
	{

	//temp_char = SCI_readCharNonBlocking(SCIA_BASE);//SciaRegs.SCIRXBUF.all;

	temp_char = SciaRegs.SCIRXBUF.all;

	rxbuffer = &rx_buffer[0];

	if ((live_frame == FALSE) && (temp_char == address) && (interchar_silent_timer==INTERCHARDELAY) ) {
		char_waiting = 0;
		live_frame = TRUE;
		frame_complete = FALSE;
	}
	//rxata_block[char_waiting] = (char) temp_char;
	*(rxbuffer + char_waiting) = temp_char;
	++char_waiting;
	char_waiting &= 0x3f; //to prevent over flow
	}
	else
	{
	    errorCount++;
	}
    interchar_silent_timer = 0;
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;
}

interrupt void SCIBRx_ISR(void) {

    N16 temp_char1 = 0;

    // SCIB errors (framing / break on an idle or unconnected RS485 #1) are handled
    // here: count, flush and software-reset the receiver. They must not touch
    // errorRx, which makes main() re-initialise SCIA.
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

    if ((live_frame1 == FALSE) && (temp_char1 == address1) && (interchar_silent_timer1==INTERCHARDELAY))
    {
        char_waiting1 = 0;
        live_frame1 = TRUE;
        frame_complete1 = FALSE;
    }

    *(rxbuffer1 + char_waiting1) = temp_char1;
    ++char_waiting1;
    char_waiting1 &= 0x3f; //to prevent over flow
    interchar_silent_timer1 = 0;
    }

    PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;
}

interrupt void ePWM1_ISR(void) {

  //  EPwm1Regs.CMPA.bit.CMPA = INV_HALFPERIOD;
//    EPwm2Regs.CMPA.bit.CMPA = INV_HALFPERIOD;
    //EPwm3Regs.CMPA.bit.CMPA = INV_HALFPERIOD;;


//---------------Reinitialize for next PWM sequence--------------
    EPwm1Regs.ETCLR.bit.INT = 1;
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;

}

#define BIASCHANNEL AnalogInput15       // Vref1.5 (B0) on the Common Controller v2
//volatile N16 zcdCount1 = 0;
N16 ch = 0;

void    ActiveBridgeBalancing(void)
{
        // Fast bridge currents (one-cycle moving average, idc1Fbk/idc2Fbk),
        // not the 32-cycle averaged Idc1/Idc2 display values, so the loop
        // can step every mains cycle (BalancingRate = 20 ms) without hunting
        float i1 = idc1Fbk * Idc1SF;
        float i2 = idc2Fbk * Idc2SF;

        if((i1 + i2) > MinLoadCurrent)  //min 20A
        {
            if(i1 > (i2 + MinUnbalanceCurrent))
                {
                    if(Bridge2LagCount_1) Bridge2LagCount_1--;
                    else if(Bridge2LeadCount_1 < MaxBalanceCount) Bridge2LeadCount_1++;
                }
            else if(i2 > (i1 + MinUnbalanceCurrent))
                {
                        if(Bridge2LeadCount_1) Bridge2LeadCount_1--;
                        else if(Bridge2LagCount_1 < MaxBalanceCount) Bridge2LagCount_1++;
                }
        }
        else {Bridge2LeadCount_1 = 0; Bridge2LagCount_1 = 0; }
}
int n = 0;
N16 MasterRunning_old1 = 0;
inline void ControlLoop(void)
{
    N16 tmpCount = 0;//, i = 0;
    int i = 0;
 //   float _maxVref = 0;
//------------------CLOSED LOOP CONTROL--------------------------

    if(cycle3count<100)
     {
         cycle3count =  133;
         if(GridFrequency==60) cycle3count = 55;
     }

    _vdcsum = 0; _idc1sum = 0;_ibatsum = 0;_idc2sum = 0;

   for(i=0;i<cycle3count;i++)
   {
       _vdcsum += vdc_buf[i];
       _idc1sum += idc1_buf[i];
       _idc2sum += idc2_buf[i];
       _ibatsum += ibat_buf[i];
   }
       vdcFbk  = _vdcsum/ cycle3count;
       idc1Fbk = _idc1sum/ cycle3count;
       idc2Fbk = _idc2sum/ cycle3count;
       ibatFbk = _ibatsum/ cycle3count;

    lpf_remVref.u = (vol_ref_remote + VrefAdjust);
    LOWPASSFILTER(lpf_remVref);
    if(lpf_ifback.y < (0.001)) lpf_ifback.y = (0.001);

         if(CurrentSensingMode==0) idc2Fbk = 0;
         IdcFback = idc1Fbk +  idc2Fbk * sfFactor;
//-----------------------------------
         lpf_ifback.u = cur_fback_remote;
         LOWPASSFILTER(lpf_ifback);
         lpf_ibatfback.u = ibat_fback_remote;
         LOWPASSFILTER(lpf_ibatfback);

         lpf_pid.u = IdcFback;
         LOWPASSFILTER(lpf_pid);

         tmp_pidcount++;
         SlaveIdcRef = (lpf_ifback.y + lpf_pid.y) * 0.5;// + VrefAdjust;
         if(SlaveIdcRef< 0.001) SlaveIdcRef = (0.001) ;

         SlaveIbatRef = (lpf_ifback.y + lpf_pid.y) * 0.5;// + VrefAdjust;
         if(SlaveIdcRef< 0.001) SlaveIdcRef = (0.001) ;


         SystemFlag_Remote = SystemFlag_Rem.all;
         MasterRunning = SystemFlag_Rem.bit.UNITON;
         if(address==1) MasterRunning = 0;

         if((MasterRunning_old == 0) && MasterRunning)
         {
             SystemFlag.bit.SOFTSTART = TRUE;
             Transition = TRUE;
         }
         MasterRunning_old = MasterRunning;
#define INC 0.00005
         if((address == 2) && SharingEnable && !LimitFlag.bit.CANFAIL && MasterRunning)
             {
                 if(Transition)
                 {
                     if(referenceI < SlaveIdcRef) referenceI += INC;
                     else if(referenceI > SlaveIdcRef) referenceI -= INC;
                     if(referenceV < (lpf_remVref.y + VrefAdjust)) referenceV += INC;
                     else if(referenceV > (lpf_remVref.y + VrefAdjust)) referenceV -= INC;
//                     soft_VdcRef = referenceV;
//                     soft_IdcRef = referenceI;
                 }
                 else
                 {
                     if(SlaveIdcRef <= soft_IdcRef) referenceI = SlaveIdcRef;
                     else referenceI = soft_IdcRef;
                     if(SlaveIbatRef <= soft_IbatRef) SlaveIbatRef = SlaveIbatRef;
                     else SlaveIbatRef = soft_IbatRef;

                      if((lpf_remVref.y + VrefAdjust) <= soft_VdcRef) referenceV = (lpf_remVref.y + VrefAdjust);
                     else referenceV = soft_VdcRef;

                 }

              }
         else
             {
               // if((MasterRunning==0) && MasterRunning_old1 ) SystemFlag.bit.SOFTSTART = TRUE;
                 referenceV = soft_VdcRef;
                 referenceI = soft_IdcRef;
             }
         MasterRunning_old1 = MasterRunning;
//-------------------------------------------
         pid_vdc.Ref = referenceV;
         pid_vdc.Fbk =  vdcFbk;//lpf_vdc.y;
         piCon(&pid_vdc);

         pid_idc.Ref = referenceI;
         pid_idc.Fbk =  IdcFback;
         piCon(&pid_idc);

         pid_ibat.Ref = Ibat_Ref;
         pid_ibat.Fbk =  ibatFbk; //lpf_ibat.y;
         piCon(&pid_ibat);

     if(CurrentSensingMode)
     {
         pid_idc1.Ref = Idc1_2_Ref;
         pid_idc1.Fbk =  idc1Fbk;
         piCon(&pid_idc1);
         pid_idc2.Ref = Idc1_2_Ref;
         pid_idc2.Fbk =  idc2Fbk;
         piCon(&pid_idc2);
     }
     else { pid_idc1.Out = 1.0; pid_idc2.Out = 1.0; }


     res_pid = pid_vdc.Out; LmtMode = 0; SystemFlag.bit.IBATMODE = FALSE;
     if(res_pid > pid_idc.Out)  { res_pid = pid_idc.Out; LmtMode = 1; }
     if(res_pid > pid_ibat.Out) { res_pid = pid_ibat.Out; LmtMode = 2; SystemFlag.bit.IBATMODE = TRUE; }
     if(res_pid > pid_idc1.Out) { res_pid = pid_idc1.Out; LmtMode = 3; }
     if(res_pid > pid_idc2.Out) { res_pid = pid_idc2.Out; LmtMode = 4; }

     if(address==2)
     {
         switch(LmtMode)
         {
             case 0:
                 pid_idc.ui = pid_vdc.ui;
                 pid_ibat.ui = pid_vdc.ui;
                 break;
             case 1:
                 pid_vdc.ui = pid_idc.ui;
                 pid_ibat.ui = pid_idc.ui;
                 break;
             case 2:
                 pid_vdc.ui = pid_idc.ui;
                 pid_idc.ui = pid_idc.ui;
                 break;
             default:
                 break;
         }
     }

//-----------------------------------------------------------------

         countPtr = res_pid * 4000;

         if(OpenLoop)
             {
                 tmpCount = COS_TABLE[(FiringAngle << 2)];
                 AlphaCount = tmpCount + SyncCorrection;
                 LimitFlag.bit.OPENLOOP = TRUE;
             }
         else
             {
                 tmpCount = COS_TABLE[countPtr];
                 AlphaCount = tmpCount + SyncCorrection;
                 LimitFlag.bit.OPENLOOP = FALSE;
             }
     }

//interrupt void msOne_delay_isr(void)
__interrupt void cpu_timer2_isr(void)
{
         //control();

    ControlLoop();


    if(canInitTimer) canInitTimer--;
    if(iom1_canFailTimer) iom1_canFailTimer--;
    if(iom2_canFailTimer) iom2_canFailTimer--;

    if(canFailureCount<CANFailDelay) canFailureCount++;

    if(TransTimer) TransTimer--;
    if(TransTimer==1) Transition = FALSE;

    if((milliseconds%BalancingRate)==0)
    {
        if(BalancingEnable && (CurrentSensingMode==1))
                {
                        ActiveBridgeBalancing();
                        Bridge2LagCount =   Bridge2LagCount_1;
                        Bridge2LeadCount =  Bridge2LeadCount_1;
                 }
                else
                {
                    Bridge2LagCount = 0;
                    Bridge2LeadCount = 0;
                    Bridge2LeadCount_1 = 0;
                    Bridge2LagCount_1 = 0;
                }
    }
//-----------------------------------------------------------------------

    milliseconds++;

    if(eventCounter<2) eventCounter++;
    if(msgCounter<PacketDelay) msgCounter++;


    if((milliseconds%100)==0) updateLED = TRUE;

    if(pwmEnabled)
    {
        if((milliseconds%100)==0) { GPIO_WritePin(62,2); }
    }
    else
    {
        if((milliseconds%500)==0) { GPIO_WritePin(62,2); }
    }

    if(OffsetNotDone && (poweronTimer>=2000) && (poweronTimer<3000))
    {
        offsets[0] += AnalogInput1 - BIASCHANNEL;
        offsets[1] += AnalogInput2 - BIASCHANNEL;
        offsets[2] += AnalogInput3 - BIASCHANNEL;
        offsets[3] += AnalogInput4 - BIASCHANNEL;
        offsets[4] += AnalogInput5 - BIASCHANNEL;
        offsets[5] += AnalogInput6 - BIASCHANNEL;

        if(poweronTimer==2999)
        {
            vrOffset = offsets[0]/1000;
            vyOffset = offsets[1]/1000;
            vbOffset = offsets[2]/1000;
            irOffset = offsets[3]/1000;
            iyOffset = offsets[4]/1000;
            ibOffset = offsets[5]/1000;

            OffsetNotDone = FALSE;
        }
    }

    if (spi_timeout < SPI_TIME)     spi_timeout++;
    if(msDelayCount)    msDelayCount--;

    zcdCount++;
    if(zcdCount>21)
    {
        zcdCount = 0;
    }

      if (milliseconds> 999) {
        milliseconds = 0;

        if(AutoSaveTimer) AutoSaveTimer--;
        if(AutoSaveTimer==1) autoSaveFlag = TRUE;

        AdcIsrCount = temp_AdcIsrCount;
        temp_AdcIsrCount = 0;
        T0IsrCount = temp_T0IsrCount;
        temp_T0IsrCount = 0;

        T1IsrCount = temp_T1IsrCount;
        temp_T1IsrCount = 0;

        PidCount = tmp_pidcount;
        tmp_pidcount = 0;

        zcdCount1 = tmp_zcdCount;
        tmp_zcdCount = 0;
        zcdCount2 = tmp_zcdnCount;
        tmp_zcdnCount = 0;

        canRxCount = temp_canRxCount;
        temp_canRxCount = 0;

     //   if((seconds%5)==0) dummyEvent = TRUE;
        LedFlag_old = 0;

        seconds++;
        if( seconds > 59)
            {
                seconds = 0;
                Minutes++;
                if(Minutes>59) Minutes = 0;

                if(BoostTimer) BoostTimer--;
                if(Batt_TestTimer) Batt_TestTimer--;
                if(EqualizeTimer) EqualizeTimer--;
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

             if(PrechargeTimer) PrechargeTimer--;


        } //seconds change condition

      checkDI_NO(DIpins[0],DI1timer,DIDELAY);
      checkDI_NO(DIpins[1],DI2timer,DIDELAY);
      checkDI(DIpins[2],DI3timer,DIDELAY);
      checkDI(DIpins[3],DI4timer,DIDELAY);
      checkDI(DIpins[4],DI5timer,DIDELAY);
      checkDI(DIpins[5],DI6timer,DIDELAY);
      checkDI(DIpins[6],DI7timer,DIDELAY);
      checkDI(DIpins[7],DI8timer,DIDELAY);
      checkDI(DIpins[8],DI9timer,DIDELAY);
      checkDI(DIpins[9],DI10timer,DIDELAY);
      // DI11 / DI12: not on the Common Controller v2 (DI01..DI10 only); timers stay 0


    if(SystemFlag.bit.POWERINGUP)
    {
        if (poweronTimer < POWERONDELAY) poweronTimer++;
    else SystemFlag.bit.POWERINGUP = FALSE;

    }

    if (interchar_silent_timer < INTERCHARDELAY)
        interchar_silent_timer++;

    if(WaitDelayTimer) WaitDelayTimer--;


      CheckHiLo(&chkVr);
      CheckHiLo(&chkVy);
      CheckHiLo(&chkVb);
      CheckHiLo(&chkVdc);
      CheckHiLo(&chkVbat);
      CheckHiLo(&chkVload);
      CheckHiLo(&chkFrequency);

          CheckHi(&chkIr);
          CheckHi(&chkIy);
          CheckHi(&chkIb);
          CheckHi(&chkIdc);
          CheckHi(&chkIbat);

    if (interchar_silent_timer < INTERCHARDELAY) interchar_silent_timer++;
    if (interchar_silent_timer1 < INTERCHARDELAY) interchar_silent_timer1++;

    if( (milliseconds%10)==0) runModeSelect = TRUE;

if((milliseconds%10)==0)
{
    if ( (SystemFlag.bit.SOFTSTART && (pwmEnabled==2)) || ReferenceChangedFlag)
        {
            softcounter++;
            if(softcounter >= SoftstartInterval)
            {
                softcounter = 0;


                if (soft_VdcRef < Vdc_Ref) soft_VdcRef += vdc_inc;
                if(soft_VdcRef > Vdc_Ref) soft_VdcRef = Vdc_Ref;
                if (soft_IdcRef < Idc_Ref) soft_IdcRef += idc_inc;
                if(soft_IdcRef > Idc_Ref) soft_IdcRef = Idc_Ref;

                soft_IbatRef = Ibat_Ref;
                soft_IdcRef = Idc_Ref ;

                if((soft_VdcRef == Vdc_Ref) && (soft_IdcRef == Idc_Ref) )
                {
                    SystemFlag.bit.SOFTSTART = 0;
                    WaitDelayTimer = 3000;
                    ReferenceChangedFlag = FALSE;
                }
            }
        } //soft start

    }//every 10ms

            if(vol_Rph >0) vR_rectified = vol_Rph * RP1;
            else vR_rectified = -vol_Rph * RN1;
            if(vol_Yph >0) vY_rectified = vol_Yph * YP1;
            else vY_rectified = -vol_Yph * YN1;
            if(vol_Bph >0) vB_rectified = vol_Bph * BP1;
            else vB_rectified = -vol_Bph * BN1;
            vOutput = MAX3(vR_rectified, vY_rectified, vB_rectified);

}
volatile uint32_t sum_zcd_ticks = 0;

__interrupt void ecap1ISR(void)
{
    if(ECap1Regs.ECFLG.bit.CEVT1)
    {
       /* cap_now = ECAP_getEventTimeStamp(ECAP1_BASE, ECAP_EVENT_1);
        dt_ticks = (uint32_t)(cap_now - cap_prev);   // unsigned handles wrap safely
        cap_prev = cap_now;
        tmp_zcdCount++;

        sum_dt_ticks += dt_ticks;
        sptr++;
        if(sptr>=32) {sptr = 0; periodCount = (sum_dt_ticks >>5); sum_dt_ticks = 0; }
        zcdCount1 = 0;
        rsync = 1;
        if (snap_end == 0) snap_start = 1;
*/
        ECap1Regs.ECCLR.bit.CEVT1 = 1;   // Clear rising flag
    }

        ECap1Regs.ECCLR.bit.INT = 1;
        PieCtrlRegs.PIEACK.all = PIEACK_GROUP4;
    // Clearing the interrupt event flags

}

//===========================================================================
// 							End of file
//===========================================================================

