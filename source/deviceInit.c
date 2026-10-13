/***********************************************************************************

-----------------------------------------------------------------------------------
File Name 	  : Initilization.C
Date Created  : 8th Feb 2010
Authors		  : Vijayan C A
History Log	  :
-----------------------------------------------------------------------------------
Date	| Modified |  		Description of Modification 		| Bug Number |
		|   By	   |											|			 |
		|		   |											|			 |
************************************************************************************/

#define	CCEXTERN	extern
#define	INIT0
#define	INITARRAY0

#include "f28x_project.h"
#include "../ccVariables.h"
#include "string.h"
#include "../include/device.h"
#include "SVGEN_CLA.h"

#define	CARD_ID			1004
#define	SWVERSION_SET	100

extern pllBlock  pllGrid;
extern SVGEN_CLA svgen_dq1;
void SetPIReg(PIreg *p, float32_t kp, float32_t ki, float32_t umax, float32_t umin, float32_t ui);


void PID_GRANDO_F_init(PID_GRANDO_F_CONTROLLER *v)
{
    v->term.Ref=0;
    v->term.Fbk=0;
    v->term.Out=0;
    v->term.c1=0.1;
    v->term.c2=0.1;

    v->param.Kr = (1.0);
    v->param.Kp = (0.5);
    v->param.Ki = (0.1);
    v->param.Kd = (0.0);
    v->param.Km = (1.0);
    v->param.Umax = (1.0);
    v->param.Umin = (-1.0);

    v->data.up = (0.0);
    v->data.ui = (0.0);
    v->data.ud = (0.0);
    v->data.v1 = (0.0);
    v->data.i1 = (0.0);
    v->data.d1 = (0.0);
    v->data.d2 = (0.0);
    v->data.w1 = (1.0);
}

void InitPID(PID_GRANDO_F_CONTROLLER *v)
{
    v->term.Ref=0;
    v->term.Fbk=0;
    v->term.Out=0;
    v->term.c1=0.1;
    v->term.c2=0.1;

    v->data.up = (0.0);
    v->data.ui = (0.0);
    v->data.ud = (0.0);
    v->data.v1 = (0.0);
    v->data.i1 = (0.0);
    v->data.d1 = (0.0);
    v->data.d2 = (0.0);
    v->data.w1 = (1.0);
}

void    InitHiLoParams(void)
 {
     InitHiLo(&chkFrequency, &GridFrequency, &BypFrequencyHL, &BypFrequencyLL, &BypFreq_delay, &BypFreq_delay );
     InitHiLo(&chkVr, &VinvR, &InvVoltageHL, &InvVoltageLL, &InvVoltHL_delay, &InvVoltLL_delay);
     InitHiLo(&chkVy, &VinvY, &InvVoltageHL, &InvVoltageLL, &InvVoltHL_delay, &InvVoltLL_delay);
     InitHiLo(&chkVb, &VinvB, &InvVoltageHL, &InvVoltageLL, &InvVoltHL_delay, &InvVoltLL_delay);

     InitHiLo(&chkIr1, &IoutR, &InvCurHL_Lvl1a, &LowValue, &InCurHL_delay_Lvl1, &LowDelay);
     InitHiLo(&chkIy1, &IoutY, &InvCurHL_Lvl1a, &LowValue, &InCurHL_delay_Lvl1, &LowDelay);
     InitHiLo(&chkIb1, &IoutB, &InvCurHL_Lvl1a, &LowValue, &InCurHL_delay_Lvl1, &LowDelay);

     InitHiLo(&chkIr2, &IoutR, &InvCurHL_Lvl2a, &LowValue, &InCurHL_delay_Lvl2, &LowDelay);
     InitHiLo(&chkIy2, &IoutY, &InvCurHL_Lvl2a, &LowValue, &InCurHL_delay_Lvl2, &LowDelay);
     InitHiLo(&chkIb2, &IoutB, &InvCurHL_Lvl2a, &LowValue, &InCurHL_delay_Lvl2, &LowDelay);

     InitHiLo(&chkIr3, &IoutR, &InvCurHL_Lvl3a, &LowValue, &InCurHL_delay_Lvl3, &LowDelay);
     InitHiLo(&chkIy3, &IoutY, &InvCurHL_Lvl3a, &LowValue, &InCurHL_delay_Lvl3, &LowDelay);
     InitHiLo(&chkIb3, &IoutB, &InvCurHL_Lvl3a, &LowValue, &InCurHL_delay_Lvl3, &LowDelay);

     InitHiLo(&chkVbypr, &VbypR, &BypVoltageHL, &BypVoltageLL, &BypVoltHL_delay, &BypVoltLL_delay);
     InitHiLo(&chkVbypy, &VbypY, &BypVoltageHL, &BypVoltageLL, &BypVoltHL_delay, &BypVoltLL_delay);
     InitHiLo(&chkVbypb, &VbypB, &BypVoltageHL, &BypVoltageLL, &BypVoltHL_delay, &BypVoltLL_delay);


     InitHiLo(&chkVbat, &Vbat, &VbatHL, &VbatLL, &VbatHL_delay, &VbatLL_delay);
     InitHiLo(&chkVbatpre, &Vbat, &VbatHL, &Vbat_PreAlarm, &VbatHL_delay, &VbatLL_delay); //Vbat_PreAlarm

     InitHiLo(&chkdcLink, &DcLinkVoltage, &DCLinkHL, &DCLinkLL, &DCLinkHL_delay, &DCLinkLL_delay);
     InitHiLo(&chkIbat, &Ibat, &IbatHL, &LowValue, &IbatHL_delay, &LowDelay);

 }

 void    CheckHiLoH(MIN_MAX *x)
 {
     if(*x->val>*x->Max) { if(x->maxTimer< (N32) (*x->maxDelay*1000)) x->maxTimer++; }
     else if(*x->val < (*x->Max - vdc_hysteresis)) { if(x->maxTimer) x->maxTimer--; }
     if(*x->val<*x->Min) { if(x->minTimer< (N32)(*x->minDelay*1000)) x->minTimer++; }
     else if (*x->val> (*x->Min + vdc_hysteresis)) { if(x->minTimer) x->minTimer--; }

     if(x->maxTimer== (N32)(*x->maxDelay*1000)) x->stat.bit.HIGH = TRUE;
     if(x->stat.bit.ARH && (x->maxTimer==0)) x->stat.bit.HIGH = FALSE;

     if(x->minTimer== (N32)(*x->minDelay*1000)) x->stat.bit.LOW = TRUE;
     if(x->stat.bit.ARL && (x->minTimer==0)) x->stat.bit.LOW = FALSE;
     if(x->stat.bit.LOW  || x->stat.bit.HIGH /*|| *x->val<*x->Min*/) x->stat.bit.OK = FALSE;
     else x->stat.bit.OK = TRUE;
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

 void    CheckHi(MIN_MAX *x)
 {
     if(*x->val>*x->Max) { if(x->maxTimer<*x->maxDelay) x->maxTimer++; }
     else { if(x->maxTimer) x->maxTimer--; }

     if(x->maxTimer==*x->maxDelay) x->stat.bit.HIGH = TRUE;
     if(x->stat.bit.ARH && (x->maxTimer==0)) x->stat.bit.HIGH = FALSE;
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
     PowerOnDelay = 5000;
     for(i=0;i<NPARAMS;i++)
         ArrayParamsF[i]= 0;

     for(i=0;i<NPARAMS;i++)
         ArrayParamsI[i]= 0;

     for(i=0;i<8;i++)
         DItimers[i]= 0;

     for(i=0;i<20;i++)
         offsets[i]= 0;

     MasterPresentTimer = POWERONDELAY;
     PowerOnDelay = POWERONDELAY;

     omega_inv = OMEGADEFAULT;
     remoteOmega  = OMEGADEFAULT;

     DIpins[0] = 20;
     DIpins[1] = 55;
     DIpins[2] = 54;
     DIpins[3] = 53;
     DIpins[4] = 52;
     DIpins[5] = 51;
     DIpins[6] = 50;
     DIpins[7] = 49;


     InitStructs();
     ref_inc = (0.0002);
     SystemFlag.all = 0;

     PID_GRANDO_F_init(&vcontrol.vol.pid_de);
     PID_GRANDO_F_init(&vcontrol.vol.pid_qe);


     SetPIReg(&cur.pi_de, 2.0, 0.01, 0.2, -0.1,0.0);
     SetPIReg(&cur.pi_qe, 2.0, 0.01, 0.1, -0.1,0.0);
     setLowPassFilter(&cur.lpf_qe, 0.2, 0.0);
     setLowPassFilter(&cur.lpf_de, 0.2, 0.0);

     SetPIReg(&cur_neg.pi_de, 0.1, 0.01, 0.1, -0.1,0.0);
     SetPIReg(&cur_neg.pi_qe, 0.1, 0.01, 0.1, -0.1,0.0);
     setLowPassFilter(&cur_neg.lpf_qe, 0.001, 0.0);
     setLowPassFilter(&cur_neg.lpf_de, 0.001, 0.0);

     setPIDRegulator(&vcontrol.vol.pid_de, 0.1, 0.01, 0.0,  1.0, 1.0, 0.96,-0.10);
     setPIDRegulator(&vcontrol.vol.pid_qe, 0.1, 0.01, 0.0,  1.0, 1.0, 0.25,-0.25);
     PR_Reset();
     PR_UpdateCoeffs();

     setLowPassFilter(&vcontrol.vol.lpf_qe, 0.5, 0.0);
     setLowPassFilter(&vcontrol.vol.lpf_de, 0.5, 0.0);

     setHarmonicOscillator(&vcontrol.theta);

     setLowPassFilter(&lpf_vInvR, (10000.0*SAMPLING_TIME), (0.0));
     setLowPassFilter(&lpf_vInvY, (10000.0*SAMPLING_TIME), (0.0));
     setLowPassFilter(&lpf_vInvB, (10000.0*SAMPLING_TIME), (0.0));

     setLowPassFilter(&lpf_curRef, (1000.0*SAMPLING_TIME), (0.0));
     setLowPassFilter(&lpf_iInvR, (10000.0*SAMPLING_TIME), (0.0));
     setLowPassFilter(&lpf_iInvY, (10000.0*SAMPLING_TIME), (0.0));
     setLowPassFilter(&lpf_iInvB, (10000.0*SAMPLING_TIME), (0.0));
     setLowPassFilter(&lpf_ioutR, (10000.0*SAMPLING_TIME), (0.0));
     setLowPassFilter(&lpf_ioutY, (10000.0*SAMPLING_TIME), (0.0));
     setLowPassFilter(&lpf_ioutB, (10000.0*SAMPLING_TIME), (0.0));
     setLowPassFilter(&lpf_voutR, (1000.0*SAMPLING_TIME), (0.0));
     setLowPassFilter(&lpf_voutY, (1000.0*SAMPLING_TIME), (0.0));
     setLowPassFilter(&lpf_voutB, (1000.0*SAMPLING_TIME), (0.0));
     setLowPassFilter(&lpf_vbypR, (5000.0*SAMPLING_TIME), (0.0));
     setLowPassFilter(&lpf_vbypY, (5000.0*SAMPLING_TIME), (0.0));
     setLowPassFilter(&lpf_vbypB, (5000.0*SAMPLING_TIME), (0.0));
     setLowPassFilter(&lpf_dclink, (1000.0*SAMPLING_TIME), (0.0));
     setLowPassFilter(&lpf_vbat, (1000.0*SAMPLING_TIME), (0.0));
     setLowPassFilter(&lpf_ibat, (1000.0*SAMPLING_TIME), (0.0));
     setLowPassFilter(&lpf_temp1, (1000.0*SAMPLING_TIME), (0.0));
     setLowPassFilter(&lpf_temp2, (1000.0*SAMPLING_TIME), (0.0));
     setLowPassFilter(&lpf_bias, (50.0*SAMPLING_TIME), (0.0)); //Bias Voltage

     initRmsMeter(&volt_meter_R);
     initRmsMeter(&volt_meter_Y);
     initRmsMeter(&volt_meter_B);

     initRmsMeter(&cur_meter_R);
     initRmsMeter(&cur_meter_Y);
     initRmsMeter(& cur_meter_B);

     initRmsMeter(&kw_meter_R);
     initRmsMeter(&kw_meter_Y);
     initRmsMeter(&kw_meter_B);


     initRmsMeter(&iout_meter_R);
     initRmsMeter(&iout_meter_Y);
     initRmsMeter(&iout_meter_B);
     initRmsMeter(&vout_meter_R);
     initRmsMeter(&vout_meter_Y);
     initRmsMeter(&vout_meter_B);
     initRmsMeter(&vbyp_meter_R);
     initRmsMeter(&vbyp_meter_Y);
     initRmsMeter(&vbyp_meter_B);
     initRmsMeter(&vll_meter_R);
     initRmsMeter(&vll_meter_Y);
     initRmsMeter(&vll_meter_B);
     initRmsMeter(&vllb_meter_R);
     initRmsMeter(&vllb_meter_Y);
     initRmsMeter(&vllb_meter_B);

     InitPLL(&pllBypass);
     SVGEN_CLA_INIT_MACRO(svgen_dq1);
     defaultOmega = OMEGADEFAULT;
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


	void InitStructs(void)
	{



	    return;
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

void setPIDRegulator(PID_GRANDO_F_CONTROLLER *p, float32_t kp, float32_t ki, float32_t kd,  float32_t kr, float32_t km, float32_t umax, float32_t umin)

{
    p->param.Kp = kp;
    p->param.Ki = ki;
    p->param.Kd = kd;
    p->param.Kr = kr;
    p->param.Km = km;
    p->param.Umax = umax;
    p->param.Umin = umin;

    p->data.d1 = 0;p->data.d2 = 0;p->data.i1 = 0;p->data.ud = 0;p->data.ui = 0;p->data.up = 0;p->data.v1 = 0;p->data.w1 = 0;
    p->term.Fbk = 0;p->term.Ref = 0;p->term.Out = 0;p->term.c1 = 0;p->term.c2 = 0;

}

void setPIRegulator(PI_CONTROLLER *p, float32_t kp, float32_t ki, float32_t umax, float32_t umin)

{
    p->Kp = kp;
    p->Ki = ki;
    p->Umax = umax;
    p->Umin = umin;

    p->Ref = 0.0;
    p->Fbk = 0.0;
    p->Out = 0.0;
    p->up = 0.0;
    p->ui = 0.0;
    p->v1 = 0.0;
    p->i1 = 0.0;
    p->w1 = 1.0;
}

void SetPIReg(PIreg *p, float32_t kp, float32_t ki, float32_t umax, float32_t umin, float32_t ui)

{
    p->Kp = kp;
    p->Ki = ki;
    p->Umax = umax;
    p->Umin = umin;
    p->Ref = 0.0;
    p->Fbk = 0.0;
    p->Out = 0.0;
    p->ui = 0.0;
}
//---------------------------------------------------------------------------
// setHarmonicOscillator:
//---------------------------------------------------------------------------
//	This function sets the initial values of Harmonic Oscillator

void setHarmonicOscillator(sincos* t) {

	t->sin_val = 0.0;
	t->cos_val = 1.0;

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
