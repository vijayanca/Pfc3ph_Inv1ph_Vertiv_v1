/*
 * adc.c
 *
 *  Created on: 23-Mar-2023
 *      Author: vijay
 */
#define CCEXTERN    extern
#define INIT0
#define INITARRAY0

#include "f28x_project.h"
#include "../ccVariables.h"
#include "string.h"
#include "../include/device.h"
#include "math.h"

float tmpQx = 0;
volatile float temp_ref_d  = 0;
volatile float temp_ref_q  = 0;

#pragma CODE_SECTION(AdcISR, ".TI.ramfunc");

#define FLOAT_CONVERTER_DC    0.000244140625
#define FLOAT_CONVERTER_AC    0.00048828125

float prevSin = 0;
float prevSin1 = 0;
N16 npoints = 0;
N16 qrtrCycle = 0;
//#define SIM_SIGNAL  1

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
interrupt void AdcISR(void) {

    AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;
    if(AdcaRegs.ADCINTOVF.bit.ADCINT1)
    {
        AdcaRegs.ADCINTOVFCLR.bit.ADCINT1 = 1; //clear INT1 overflow flag
        AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1; //clear INT1 flag
    }
    temp_AdcIsrCount++;
    if(piCounter<PI_Interval) piCounter++;
    else {piCounter = 0; doPID = TRUE; }

     AnalogInput1 = AdcaResultRegs.ADCRESULT0;  //VRY
     AnalogInput2 = AdcbResultRegs.ADCRESULT0;  //VYB
     AnalogInput3 = AdcaResultRegs.ADCRESULT5;  //VBR
     AnalogInput4 = AdcbResultRegs.ADCRESULT5;   //IR
     AnalogInput5 = AdcbResultRegs.ADCRESULT1;  //IY
     AnalogInput6 = AdccResultRegs.ADCRESULT0;   //IB
     AnalogInput7 = AdcbResultRegs.ADCRESULT4;  //VDC
     AnalogInput8 = AdccResultRegs.ADCRESULT4;   //IDC
     AnalogInput9 = AdcaResultRegs.ADCRESULT4;  //IDC2
     AnalogInput10 = AdcbResultRegs.ADCRESULT2;  //VBAT
     AnalogInput11 = AdcaResultRegs.ADCRESULT1;  //IBAT
     AnalogInput12 = AdcbResultRegs.ADCRESULT3;  //VLOAD

     AnalogInput13 = AdcaResultRegs.ADCRESULT2;  //VREF
     AnalogInput14 = AdccResultRegs.ADCRESULT2;   //IREF
     AnalogInput15 = AdcbResultRegs.ADCRESULT6;   //Vref1.5

     AnalogInput16 = AdccResultRegs.ADCRESULT5;   //Temp1
     AnalogInput17 = AdcaResultRegs.ADCRESULT3;  //TEMP2

     AnalogInput18 = AdccResultRegs.ADCRESULT1;   //DI_1
     AnalogInput19 = AdcaResultRegs.ADCRESULT6;  //ADC6 DI2
     AnalogInput20 = AdccResultRegs.ADCRESULT3;   //DI_3
     AnalogInput21 = AdccResultRegs.ADCRESULT6;   //DI4

     lpf_bias.u = (float) (AnalogInput15) * FLOAT_CONVERTER_AC;
     LOWPASSFILTER(lpf_bias);

  //   AnalogInput8 = 1200;AnalogInput9 = 1300;

     lpf_vInpR.y = (float) (AnalogInput1 - vrOffset) * FLOAT_CONVERTER_AC - lpf_bias.y;
     lpf_vInpY.y = (float) (AnalogInput2 - vyOffset) * FLOAT_CONVERTER_AC - lpf_bias.y;
     lpf_vInpB.y = (float) (AnalogInput3 - vbOffset) * FLOAT_CONVERTER_AC - lpf_bias.y;

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

     samplingCount_tmp++;
     vdcCycleSum += lpf_vdc.u;

     lpf_vbat.u = (float)AnalogInput10 * FLOAT_CONVERTER_DC;
     LOWPASSFILTER(lpf_vbat);

     lpf_vload.u = (float)AnalogInput12 * FLOAT_CONVERTER_DC;
     LOWPASSFILTER(lpf_vload);
     lpf_temp1.u = (float)AnalogInput16 * FLOAT_CONVERTER_DC;
     lpf_temp2.u = (float)AnalogInput17 * FLOAT_CONVERTER_DC;
     LOWPASSFILTER(lpf_temp1);
     LOWPASSFILTER(lpf_temp2);
//---------------------------------------------------------------------------

     lpf_vref.u = (float)AnalogInput13 * FLOAT_CONVERTER_DC;
     lpf_iref.u = (float)AnalogInput14 * FLOAT_CONVERTER_DC;
     LOWPASSFILTER(lpf_vref);
     LOWPASSFILTER(lpf_iref);

     if(lpf_vref.y<(0.66)) extVref = lpf_vref.y;
     else extVref = (0.66);
     if(lpf_iref.y<(0.66)) extIbatref = lpf_iref.y;
     else extIbatref = (0.66);

     rmsCal();

     vol_Rph = (lpf_vInpR.y - lpf_vInpB.y) * 0.33333333333f;
     vol_Yph = (lpf_vInpY.y - lpf_vInpR.y) * 0.33333333333f;
     vol_Bph = (lpf_vInpB.y - lpf_vInpY.y) * 0.33333333333f;

     pllGrid.va =  lpf_vInpR.y;   //Vab
     pllGrid.vb =  lpf_vInpY.y;   //Vbc;
     pllGrid.vc =  lpf_vInpB.y;   //Vca;
     pll_omega = phaseLockedLoopB(&pllGrid);

     zcdCount++;
     if((pllGrid.theta.sin_val >= 0.0) && (prevSin<0.0))
     {
         rsync = TRUE;
         npoints = zcdCount;
         qrtrCycle = npoints >> 1;
         zcdCount = 0;

         if (snap_end == 0) snap_start = 1;

         if((lpf_vInpY.y > 0.0) || (lpf_vInpB.y<0))
         {
             if (seqFailTimer<SEQFAILDELAY) seqFailTimer++;
         }
         else if(seqFailTimer) seqFailTimer--;

     }//ZCD condition
     prevSin = pllGrid.theta.sin_val;

     saveCounter++;
     saveCounter &= 1023;

     if (snap_start == 1) {
         if ((saveCounter % nCycles) == 0)
         {
             waveBuffer[snap_ptr] = (*waveptr);
             snap_ptr++;

             if (snap_ptr >= SNAP_SIZE) {
                 snap_start = 0;
                 snap_end = 1;
                 snap_ptr = 0;   //count starts from 0 to SnapSize
            }
         }
     }

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


