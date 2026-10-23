/***********************************************************************************

-----------------------------------------------------------------------------------
File Name 	  : Initilization.C
Date Created  : 8th Feb 2010
Authors		  : Vijayan C A
History Log	  :
-----------------------------------------------------------------------------------
Date	| Modified |  		Description of Modification 		| Bug Number |
		|   By	   |											|			 |
23/10/26| VCA      | 3-phase PFC (RBC control)                  |			 |
************************************************************************************/

#define	CCEXTERN	extern
#define	INIT0
#define	INITARRAY0

#include "f28x_project.h"
#include "../ccVariables.h"
#include "string.h"
#include "../include/device.h"
#include "SVGEN_CLA.h"

#define	CARD_ID			2002        // PFC + 1-phase inverter (RBC / PFC family)
#define	SWVERSION_SET	100

void    InitHiLoParams(void)
 {
     InitHiLo(&chkFrequency, &GridFrequency, &FrequencyHighLimit, &FrequencyLowLimit, &FrequencyHighDelay, &FrequencyLowDelay);

     // grid limits are line-line
     InitHiLo(&chkVr, &VinpRY, &GridVoltageHL, &GridVoltageLL, &GridVoltageHL_delay, &GridVoltageLL_delay);
     InitHiLo(&chkVy, &VinpYB, &GridVoltageHL, &GridVoltageLL, &GridVoltageHL_delay, &GridVoltageLL_delay);
     InitHiLo(&chkVb, &VinpBR, &GridVoltageHL, &GridVoltageLL, &GridVoltageHL_delay, &GridVoltageLL_delay);

     InitHiLo(&chkIr, &IinpR, &IinpHL, &LowValue, &IinpHL_delay, &LowDelay);
     InitHiLo(&chkIy, &IinpY, &IinpHL, &LowValue, &IinpHL_delay, &LowDelay);
     InitHiLo(&chkIb, &IinpB, &IinpHL, &LowValue, &IinpHL_delay, &LowDelay);

     InitHiLo(&chkdcLink, &DcLinkVoltage, &DCLinkHL, &DCLinkLL, &DCLinkHL_delay, &DCLinkLL_delay);

     // 1-phase inverter, bypass, battery
     InitHiLo(&chkVinv, &Vinv, &InvVoltageHL, &InvVoltageLL, &InvVoltHL_delay, &InvVoltLL_delay);
     InitHiLo(&chkVbyp, &Vbyp, &BypVoltageHL, &BypVoltageLL, &BypVoltHL_delay, &BypVoltLL_delay);
     InitHiLo(&chkIout, &Iout, &InvOverloadA, &LowValue, &InvOverload_delay, &LowDelay);
     InitHiLo(&chkVbat, &Vbat, &VbatHL, &VbatLL, &VbatHL_delay, &VbatLL_delay);
 }


 void    CheckHiLo(MIN_MAX *x)
 {
     if(*x->val>*x->Max) { if(x->maxTimer< (N32) (*x->maxDelay*1000)) x->maxTimer++; }
     else { if(x->maxTimer) x->maxTimer--; }
     if(*x->val<*x->Min) { if(x->minTimer< (N32)(*x->minDelay*1000)) x->minTimer++; }
     else { if(x->minTimer) x->minTimer--; }

     if(x->maxTimer== (N32)(*x->maxDelay*1000)) x->stat.bit.HIGH = TRUE;
     if(x->stat.bit.ARH && (x->maxTimer==0)) x->stat.bit.HIGH = FALSE;

     if(x->minTimer== (N32)(*x->minDelay*1000)) x->stat.bit.LOW = TRUE;
     if(x->stat.bit.ARL && (x->minTimer==0)) x->stat.bit.LOW = FALSE;
     if(x->stat.bit.LOW  || x->stat.bit.HIGH /*|| *x->val<*x->Min*/) x->stat.bit.OK = FALSE;
     else x->stat.bit.OK = TRUE;
 }

 void  InitHiLo(MIN_MAX *x, float32_t *v1, float32_t *hlmt, float32_t *llmt, float32_t *hdly, float32_t *ldly)
 {
     x->val = v1;
     x->Max = hlmt;
     x->Min = llmt;
     x->maxDelay = hdly;
     x->minDelay = ldly;
     x->stat.all = 0;
     x->maxTimer = 0;
     x->minTimer = 0;
     x->stat.bit.ARH = 1;
     x->stat.bit.ARL = 1;
 }

 void InitVariables(void) {

 N16 i = 0;
     for(i=0;i<NPARAMS;i++)
         ArrayParamsF[i]= 0;

     for(i=0;i<NPARAMS;i++)
         ArrayParamsI[i]= 0;

     for(i=0;i<8;i++)
         DItimers[i]= 0;

     for(i=0;i<20;i++)
         offsets[i]= 0;

     PowerOnDelay = POWERONDELAY;

     ref_inc = (0.0002);        // DC-link soft start, p.u. per ms (RBC)
     SystemFlag.all = 0;

     // RBC defaults; gains and limits are overwritten from the parameters
     // (updateRAMvalues(0)) once they are restored from the EEPROM
     SetPfcPI(&pid_dclink, 0.5, 0.01, 0.4, -0.02);
     SetPfcPI(&pfc.pid_de, 2.0, 0.01, 0.4, -0.4);
     SetPfcPI(&pfc.pid_qe, 2.0, 0.01, 0.4, -0.2);
     setLowPassFilter(&pfc.lpf_qe, 0.2, 0.0);
     setLowPassFilter(&pfc.lpf_de, 0.2, 0.0);
     PR_Reset();

     setLowPassFilter(&lpf_vR, (16000.0*SAMPLING_TIME), (0.0));
     setLowPassFilter(&lpf_vY, (16000.0*SAMPLING_TIME), (0.0));
     setLowPassFilter(&lpf_vB, (16000.0*SAMPLING_TIME), (0.0));

     setLowPassFilter(&lpf_iR, (10000.0*SAMPLING_TIME), (0.0));
     setLowPassFilter(&lpf_iY, (10000.0*SAMPLING_TIME), (0.0));
     setLowPassFilter(&lpf_iB, (10000.0*SAMPLING_TIME), (0.0));

     setLowPassFilter(&lpf_dclink, (10000.0*SAMPLING_TIME), (0.0));
     setLowPassFilter(&lpf_vbat, (2000.0*SAMPLING_TIME), (0.0));
     setLowPassFilter(&lpf_ibat, (2000.0*SAMPLING_TIME), (0.0));
     setLowPassFilter(&lpf_temp1, (100.0*SAMPLING_TIME), (0.0));
     setLowPassFilter(&lpf_temp2, (100.0*SAMPLING_TIME), (0.0));
     setLowPassFilter(&lpf_bias, (50.0*SAMPLING_TIME), (0.0)); //Bias Voltage
     setLowPassFilter(&lpf_omega, (100.0*SAMPLING_TIME), (OMEGADEFAULT)); //Omega

     initRmsMeter(&volt_meter_R);
     initRmsMeter(&volt_meter_Y);
     initRmsMeter(&volt_meter_B);
     initRmsMeter(&volt_meter_RY);
     initRmsMeter(&volt_meter_YB);
     initRmsMeter(&volt_meter_BR);

     initRmsMeter(&cur_meter_R);
     initRmsMeter(&cur_meter_Y);
     initRmsMeter(&cur_meter_B);

     initRmsMeter(&kw_meter_R);
     initRmsMeter(&kw_meter_Y);
     initRmsMeter(&kw_meter_B);

     // 1-phase inverter, bypass
     setLowPassFilter(&lpf_vinv, (10000.0*SAMPLING_TIME), (0.0));
     setLowPassFilter(&lpf_vout, (10000.0*SAMPLING_TIME), (0.0));
     setLowPassFilter(&lpf_vbyp, (10000.0*SAMPLING_TIME), (0.0));
     setLowPassFilter(&lpf_iinv, (10000.0*SAMPLING_TIME), (0.0));
     setLowPassFilter(&lpf_iout, (10000.0*SAMPLING_TIME), (0.0));
     setLowPassFilter(&lpf_omegaByp, (100.0*SAMPLING_TIME), (OMEGADEFAULT));
     initRmsMeter(&vinv_meter);
     initRmsMeter(&vout_meter);
     initRmsMeter(&vbyp_meter);
     initRmsMeter(&iinv_meter);
     initRmsMeter(&iout_meter);
     initRmsMeter(&kwo_meter);
     PR_RES_CLEAR(prV1); PR_RES_CLEAR(prV3);
     PR_RES_CLEAR(prV5); PR_RES_CLEAR(prV7);
     InitPLL(&pllByp);
     omegaInv = OMEGADEFAULT;
     angleInv = 0.0;
     theta_inv.sin_val = 0.0;
     theta_inv.cos_val = 1.0;

     // battery buck-boost (gains from the parameters in BatteryControl)
     SetPfcPI(&pid_vbat, 0.01, 0.0005, 0.95, 0.0);
     SetPfcPI(&pid_ibat, 0.01, 0.0005, 0.95, 0.0);
     SetPfcPI(&pid_boost, 0.005, 0.0002, 0.6, 0.0);

     theta.sin_val = 0.0;
     theta.cos_val = 1.0;
     InitPLL(&pllGrid);
     SVGEN_CLA_INIT_MACRO(svgen_dq1);
     CardID = CARD_ID;
     swVersion = SWVERSION_SET;
     getCompiledDate();

 }



N8	findMonth(char c1, char c2, char c3)
		{
			N8 m = 1;
			c1 &= 0xdf; c2 &= 0xdf; c3 &= 0xdf;
			switch(c1)
				{
					case 'A':
						if(c2 == 'P') m = 4;
						else	m = 8;
						break;
					case 'D':
						m = 12;
						break;
					case 'F':
						m = 2;
						break;
					case 'J':
						if((c3 == 'N') && (c2=='U')) m = 6;
						else if((c3 == 'L') && (c2=='U')) m = 7;
						else	m = 1;
						break;

					case 'M':
						if(c3 == 'Y') m = 5;
							else m = 3;
						break;
					case 'N':
						m = 11;
						break;
					case 'O':
						m = 10;
						break;
					case 'S':
						m = 9;
						break;
				}
			return(m);
	}


N8	asc2Dec(char c1, char c2)
		{
			c1 &= 0xf;	c2 &= 0xf;
			return(c1 * 10 + c2);
}
	//A D F J M N O S

void	getCompiledDate(void)
{


	char dt[12];
	char tt[12];

	strcpy(dt,__DATE__); //compiled Date
	strcpy(tt,__TIME__); //compiled Date

	swMinSec = asc2Dec(tt[3],tt[4]) * 100 + asc2Dec(tt[6],tt[7]);
	swDateHour = asc2Dec(dt[4],dt[5]) * 100 + asc2Dec(tt[0],tt[1]);
	swYearMonth = asc2Dec(dt[9],dt[10]) * 100 + findMonth(dt[0],dt[1],dt[2]);

}


//---------------------------------------------------------------------------
// setLowPassFilter:
//---------------------------------------------------------------------------
//	This function sets the time constant and initial output of Low pass Filter

void setLowPassFilter(LPF *f, float32_t T, float32_t y) {
	f->T = T;			//VCA
	f->y = y;
}



//---------------------------------------------------------------------------
// setPiRegulator:
//---------------------------------------------------------------------------
//	This function sets the parameters of the PI regulator

void setPiRegulator_old(PIreg* p, float32_t kp, float32_t ki, float32_t ui, float32_t umax, float32_t umin)

{

	p->Kp = kp;
	p->Ki = ki;
	p->ui = ui;
	p->Umax = umax;
	p->Umin = umin;

}

// PFC PI regulator (controlTypes.h PFC_PI_MACRO): gains, limits, state cleared
void SetPfcPI(PFC_PI *p, float32_t kp, float32_t ki, float32_t umax, float32_t umin)
{
    p->Kp = kp;
    p->Ki = ki;
    p->Umax = umax;
    p->Umin = umin;
    p->Ref = 0.0;
    p->Fbk = 0.0;
    p->Err = 0.0;
    p->Out = 0.0;
    p->ui = 0.0;
}


void initRmsMeter(rms_meter *r)
{

	r->mean_square = 0.0;
	r->sample_count = 0;
	r->RMS = 0.0;

}


//===========================================================================
// 							End of file
//===========================================================================
