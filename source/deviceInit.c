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
#include "board_pin_map.h"

#define	    CARD_ID			1010
#define	    SWVERSION_SET	126

void  InitHiLo0(MIN_MAX *x, float32_t *v1, float32_t *hlmt, float32_t *llmt, float32_t *hdly, float32_t *ldly);

void InitRelays(void)
{
    N16  i = 0;
    for(i=0;i<12;i++)
    {
        MainConRLY[i].Delay = &ArrayParamsI[MAIN_RLY_DLY_START + i];
        MainConRLY[i].pinNo = DOpins[i];
        MainConRLY[i].BitNum = &ArrayParamsI[MAIN_RLY_SIG_START + i];
        MainConRLY[i].Timer = 0;
        Nop();
        Nop();
    }

    for(i=0;i<16;i++)
    {
        IOM1RLY[i].Delay = &ArrayParamsI[IOM1_RLY_DLY_START + i];
        IOM1RLY[i].pinNo = 1;
        IOM1RLY[i].BitNum = &ArrayParamsI[IOM1_RLY_SIG_START + i];
        IOM1RLY[i].Timer = 0;

        IOM2RLY[i].Delay = &ArrayParamsI[IOM2_RLY_DLY_START + i];
        IOM2RLY[i].pinNo = 1;
        IOM2RLY[i].BitNum = &ArrayParamsI[IOM2_RLY_SIG_START + i];
        IOM2RLY[i].Timer = 0;
    }

    for(i=0;i<3;i++)
    {
        MainConRLY[i].BitNum = &ArrayParamsI[MAIN_RLY_SIG_START + i];
        MainConRLY[i].Delay = &ArrayParamsI[MAIN_RLY_DLY_START + i];
        MainConRLY[i].pinNo = DOpins[i];

        MainConRLY[i].Timer = 0;
        Nop();
        Nop();
    }



}
void    InitHiLoParams(void)
{
    chkVr.stat.bit.HIGH = 0x0; chkVr.stat.bit.LOW = 0x0;
    chkVy.stat.bit.HIGH = 0x0; chkVy.stat.bit.LOW = 0x0;
    chkVb.stat.bit.HIGH = 0x0; chkVb.stat.bit.LOW = 0x0;
    chkIb.stat.bit.HIGH = 0x0; chkIb.stat.bit.LOW = 0x0;
    chkIb.stat.bit.HIGH = 0x0; chkIb.stat.bit.LOW = 0x0;
    chkIb.stat.bit.HIGH = 0x0; chkIb.stat.bit.LOW = 0x0;
    chkVdc.stat.bit.HIGH = 0x0; chkVdc.stat.bit.LOW = 0x0;
    chkIdc.stat.bit.HIGH = 0x0; chkIdc.stat.bit.LOW = 0x0;
    chkIbat.stat.bit.HIGH = 0x0; chkIbat.stat.bit.LOW = 0x0;

    InitHiLo0(&chkFrequency, &GridFrequency, &FrequencyHL, &FrequencyLL, &FrequencyHL_delay, &FrequencyLL_delay );

    InitHiLo(&chkVr, &VinpRY, &VacHL, &VacHL_recover, &VacLL, &VacLL_recover, &VacHL_delay, &VacLL_delay);
    InitHiLo(&chkVy, &VinpYB, &VacHL, &VacHL_recover, &VacLL, &VacLL_recover, &VacHL_delay, &VacLL_delay);
    InitHiLo(&chkVb, &VinpBR, &VacHL, &VacHL_recover, &VacLL, &VacLL_recover, &VacHL_delay, &VacLL_delay);

    InitHiLo(&chkIr, &IinpR, &IacHL, &IacHL_recover, &LowValue, &LowValue, &IacHL_delay, &LowDelay);
    InitHiLo(&chkIy, &IinpY, &IacHL, &IacHL_recover, &LowValue, &LowValue, &IacHL_delay, &LowDelay);
    InitHiLo(&chkIb, &IinpB, &IacHL, &IacHL_recover, &LowValue, &LowValue, &IacHL_delay, &LowDelay);

    InitHiLo(&chkIdc,  &IdcTotal, &IdcHL, &IdcHL_recover, &LowValue, &LowValue, &IdcHL_delay, &LowDelay);
    InitHiLo(&chkIbat, &Ibat,     &IbatHL,&IbatHL_recover, &LowValue, &LowValue, &IbatHL_delay, &LowDelay);

    InitHiLo(&chkVdc, &Vdc, &VdcHL, &VdcHL_recover,  &VdcLL, &VdcLL_recover, &VdcHL_delay, &VdcLL_delay);
    InitHiLo(&chkVbat, &Vbat, &VbatHL, &VbatHL_recover,  &VbatLL, &VbatLL_recover, &VbatHL_delay, &VbatLL_delay);
    InitHiLo(&chkVload, &LoadVoltage, &VloadHL, &VloadHL_recover,  &VloadLL, &VloadLL_recover, &VloadHL_delay, &VloadLL_delay);
}

void    CheckHiLo(MIN_MAX *x)
{
    if(*x->val>*x->Max) { if(x->maxTimer< (N32) (*x->maxDelay*1000)) x->maxTimer++; }
    else if(*x->val < (*x->MaxRel)) { if(x->maxTimer) x->maxTimer--; }
    if(*x->val<*x->Min) { if(x->minTimer< (N32)(*x->minDelay*1000)) x->minTimer++; }
    else if (*x->val> (*x->MinRel)) { if(x->minTimer) x->minTimer--; }

    if(*x->val<(*x->Min * 0.5f)) x->minTimer = (N32)(*x->minDelay*1000);
    if(x->maxTimer== (N32)(*x->maxDelay*1000)) x->stat.bit.HIGH = TRUE;
    if(x->stat.bit.ARH && (x->maxTimer==0)) x->stat.bit.HIGH = FALSE;

    if(x->minTimer== (N32)(*x->minDelay*1000)) x->stat.bit.LOW = TRUE;
    if(x->stat.bit.ARL && (x->minTimer==0)) x->stat.bit.LOW = FALSE;
    if(x->stat.bit.LOW  || x->stat.bit.HIGH) x->stat.bit.OK = FALSE;
    else x->stat.bit.OK = TRUE;
}

void    CheckHi(MIN_MAX *x)
{
    if(*x->val>*x->Max) { if(x->maxTimer< (N32) (*x->maxDelay*1000)) x->maxTimer++; }
    else if(*x->val < (*x->MaxRel)) { if(x->maxTimer) x->maxTimer--; }
    if(x->maxTimer== (N32)(*x->maxDelay*1000)) x->stat.bit.HIGH = TRUE;
    if(x->stat.bit.ARH && (x->maxTimer==0)) x->stat.bit.HIGH = FALSE;
}


 void  InitHiLo(MIN_MAX *x, float32_t *v1, float32_t *hlmt, float32_t *hrecover, float32_t *llmt, float32_t *lrecover, float32_t *hdly, float32_t *ldly)
 {
     x->val = v1;
     x->Max = hlmt;
     x->Min = llmt;
     x->MaxRel= hrecover;
     x->MinRel = lrecover;
     x->maxDelay = hdly;
     x->minDelay = ldly;
     x->stat.all = 0;
     x->maxTimer = 0;
     x->minTimer = 0;
     x->stat.bit.ARH = 1;
     x->stat.bit.ARL = 1;
 }

 void  InitHiLo0(MIN_MAX *x, float32_t *v1, float32_t *hlmt, float32_t *llmt, float32_t *hdly, float32_t *ldly)
 {
     x->val = v1;
     x->Max = hlmt;
     x->Min = llmt;
     x->MaxRel= hlmt - 5;
     x->MinRel = llmt + 5;
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
    {
        ArrayParamsI[i]= 0;
        ArrayParamsF[i]= 0.0;
    }

      // Common Controller v2: DI01..DI10 (board_pin_map.h). DI11/DI12 do not
      // exist on this board and are no longer scanned (interrupt.c).
      DIpins[0]=  DI1_GPIO;     // GPIO247
      DIpins[1]=  DI2_GPIO;     // GPIO20
      DIpins[2]=  DI3_GPIO;     // GPIO21
      DIpins[3]=  DI4_GPIO;     // GPIO63
      DIpins[4]=  DI5_GPIO;     // GPIO48
      DIpins[5]=  DI6_GPIO;     // GPIO49
      DIpins[6]=  DI7_GPIO;     // GPIO50
      DIpins[7]=  DI8_GPIO;     // GPIO51
      DIpins[8]=  DI9_GPIO;     // GPIO52
      DIpins[9]=  DI10_GPIO;    // GPIO53

      // RLY_01..RLY_12
      DOpins[0]=  RLY1_GPIO;    // GPIO60
      DOpins[1]=  RLY2_GPIO;    // GPIO33
      DOpins[2]=  RLY3_GPIO;    // GPIO24
      DOpins[3]=  RLY4_GPIO;    // GPIO25
      DOpins[4]=  RLY5_GPIO;    // GPIO26
      DOpins[5]=  RLY6_GPIO;    // GPIO27
      DOpins[6]=  RLY7_GPIO;    // GPIO32
      DOpins[7]=  RLY8_GPIO;    // GPIO58
      DOpins[8]=  RLY9_GPIO;    // GPIO22
      DOpins[9]=  RLY10_GPIO;   // GPIO44
      DOpins[10]=  RLY11_GPIO;  // GPIO61
      DOpins[11]=  RLY12_GPIO;  // GPIO59

      InitStructs();

        for(i=0;i<16;i++)
            offsets[i] = 0;

        OffsetNotDone = TRUE;
        LowDelay = 1000;

for(i=0;i<32;i++)
{
        lpf32Vdc.larray[i] = 0;
        lpf32Idc.larray[i] = 0;
        lpf32Ibat.larray[i] = 0;
        lpf32Vbat.larray[i] = 0;

        lpf32IBridge1.larray[i] = 0;
        lpf32IBridge2.larray[i] = 0;
}


        InitStructs();
        SysOkFlag.all = 0;

        vdc_inc = 0.004;
        idc_inc = 0.004;

        SystemFlag.all = 0;
        waveptr = &lpf_vInpR.y;

        SystemFlag.all = 0;

        InitPI(&pid_vdc);
        InitPI(&pid_idc);
        InitPI(&pid_idc1);
        InitPI(&pid_idc2);
        InitPI(&pid_ibat);
        InitPI(&pid_iac);

        setPIRegulator(&pid_vdc, 0.5, 0.01,  0.98, 0.0);
        setPIRegulator(&pid_idc, 0.5, 0.01,  0.98, 0.0);
        setPIRegulator(&pid_idc1, 0.5, 0.01, 0.98, 0.0);
        setPIRegulator(&pid_idc2, 0.5, 0.01, 0.98, 0.0);
        setPIRegulator(&pid_iac , 0.1, 0.01, 0.98, 0.0);
        setPIRegulator(&pid_ibat, 0.5, 0.01, 0.98, 0.0);

        setLowPassFilter(&lpf_ifback, (1000.0*SAMPLING_TIME), (0.0));
        setLowPassFilter(&lpf_remVref, (8000.0*SAMPLING_TIME), (0.0));
        setLowPassFilter(&lpf_pid, (1000.0*SAMPLING_TIME), (0.0));

            setLowPassFilter(&lpf_vdc, (1000.0*SAMPLING_TIME), (0.0));
            setLowPassFilter(&lpf_idc1, (1000.0*SAMPLING_TIME), (0.0));
            setLowPassFilter(&lpf_idc2, (1000.0*SAMPLING_TIME), (0.0));
            setLowPassFilter(&lpf_ibat, (1000.0*SAMPLING_TIME), (0.0));

            setLowPassFilter(&lpf_vbat, (100.0*SAMPLING_TIME), (0.0));
            setLowPassFilter(&lpf_vload, (100.0*SAMPLING_TIME), (0.0));
            setLowPassFilter(&lpf_temp1, (100.0*SAMPLING_TIME), (0.0));
            setLowPassFilter(&lpf_temp2, (100.0*SAMPLING_TIME), (0.0));
            setLowPassFilter(&lpf_vref, (100.0*SAMPLING_TIME), (0.0));
            setLowPassFilter(&lpf_iref, (100.0*SAMPLING_TIME), (0.0));

            setLowPassFilter(&lpf_vInpR, (2500.0*SAMPLING_TIME), (0.0));
            setLowPassFilter(&lpf_vInpY, (2500.0*SAMPLING_TIME), (0.0));
            setLowPassFilter(&lpf_vInpB, (2500.0*SAMPLING_TIME), (0.0));
            setLowPassFilter(&lpf_iInpR, (2500.0*SAMPLING_TIME), (0.0));
            setLowPassFilter(&lpf_iInpY, (2500.0*SAMPLING_TIME), (0.0));
            setLowPassFilter(&lpf_iInpB, (2500.0*SAMPLING_TIME), (0.0));

            setLowPassFilter(&lpf_bias, (50.0*SAMPLING_TIME), (0.0)); //Bias Voltage


        initRmsMeter(&volt_meter_R);
        initRmsMeter(&volt_meter_Y);
        initRmsMeter(&volt_meter_B);
        initRmsMeter(&vll_meter_R);
        initRmsMeter(&vll_meter_Y);
        initRmsMeter(&vll_meter_B);

        initRmsMeter(&cur_meter_R);
        initRmsMeter(&cur_meter_Y);
        initRmsMeter(&cur_meter_B);
        initRmsMeter(&kw_meter_R);
        initRmsMeter(&kw_meter_Y);
        initRmsMeter(&kw_meter_B);

        CardID = CARD_ID;
        swVersion = SWVERSION_SET;
        getCompiledDate();
        InitPLL(&pllGrid);
    }


N8  asc2Dec(char c1, char c2)
        {
            c1 &= 0xf;  c2 &= 0xf;
            return(c1 * 10 + c2);
}

N8  findMonth(char c1, char c2, char c3)
        {
            N8 m = 1;
            c1 &= 0xdf; c2 &= 0xdf; c3 &= 0xdf;
            switch(c1)
                {
                    case 'A':
                        if(c2 == 'P') m = 4;
                        else    m = 8;
                        break;
                    case 'D':
                        m = 12;
                        break;
                    case 'F':
                        m = 2;
                        break;
                    case 'J':
                        if(c3 == 'L') m = 7;
                        else if(c2=='U') m = 6;
                        else m = 1;
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

void setLowPassFilter(LPF *f, float32_t T, float32_t y) {
    f->T = T;           //VCA
    f->y = y;
}

//---------------------------------------------------------------------------
// setPiRegulator:
//---------------------------------------------------------------------------
//  This function sets the parameters of the PI regulator

void setPiRegulator(PIreg* p, float32_t kp, float32_t ki, float32_t ui, float32_t umax, float32_t umin)

{

    p->Kp = kp;
    p->Ki = ki;
    p->ui = ui;
    p->Umax = umax;
    p->Umin = umin;

}

void setPIRegulator(PI_CONTROLLER *p, float32_t kp, float32_t ki, float32_t umax, float32_t umin)

{
    p->Kp = kp;
    p->Ki = ki;
    p->Umax = umax;
    p->Umin = umin;
    InitPID(p);

}
/*void setPIDRegulator(PID_GRANDO_F_CONTROLLER *p, float32_t kp, float32_t ki, float32_t kd,  float32_t kr, float32_t km, float32_t umax, float32_t umin)

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

*/
	void InitStructs(void)
	{
	        return;




	}

//---------------------------------------------------------------------------
// setHarmonicOscillator:
//---------------------------------------------------------------------------
//	This function sets the initial values of Harmonic Oscillator

void setHarmonicOscillator(sincos* t) {

	t->sin_val = (0.0);
	t->cos_val = (1.0);

}



void initRmsMeter(rms_meter *r)
{

	r->mean_square = (0.0);
	r->sample_count = 0;
	r->RMS = (0.0);

}


void InitPI(PI_CONTROLLER *v)
{
        v->Ref = 0;
        v->Fbk = 0;
        v->Out = 0;
        v->Kp = (0.5);
        v->Ki = (0.01);
        v->Umax = (1.0);
        v->Umin = (0.0);
        v->up = (0.0);
        v->ui = (0.0);
        v->v1 = (0.0);
}


void InitPID(PI_CONTROLLER *v)
{
        v->Ref = 0;
        v->Fbk = 0;
        v->Out = 0;
        v->up = (0.0);
        v->ui = (0.0);
        v->v1 = (0.0);
}

/*
void PID_GRANDO_F_init0(PID_GRANDO_F_CONTROLLER *v)
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

void InitPID0(PID_GRANDO_F_CONTROLLER *v)
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
*/
//===========================================================================
// 							End of file
//===========================================================================
