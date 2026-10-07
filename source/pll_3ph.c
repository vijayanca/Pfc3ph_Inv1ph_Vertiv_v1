/*
 * pll_3ph.c
 *
 *  Created on: 17-Mar-2023
 *      Author: vijay
 */

#define CCEXTERN    extern
#define INIT0
#define INITARRAY0

#include "f28x_project.h"
#include "../ccVariables.h"
#include "inc/hw_memmap.h"
#include "../include/pll3ph.h"
#include "NOTCH_FLTR_F.h"
#include "math.h"

void NOTCH_FLTR_F_COEFF_Update(float delta_T, float wn,float c2, float c1, NOTCH_COEFF_F *notch_obj);
void NOTCH_FLTR_F_run(NOTCH_VARS_F *v,NOTCH_COEFF_F *w);
void NOTCH_FLTR_F_VARS_init(NOTCH_VARS_F *v);
#ifdef _FLASH
#pragma CODE_SECTION(phaseLockedLoopB, ".TI.ramfunc");
#endif
//VARIABLES:

 //---------------------------------------------------------------------------
 //  phaseLockedLoop:
 //---------------------------------------------------------------------------
 //  This function is the implementation of Phase Locked Loop
 //  This takes the three phase grid voltage as input and determines omega
 //  required for reference frame transformations
volatile N16 mainFailCount = 0;
float adjustAngle = 0;

#define ang30 5.7595865316

 float32_t phaseLockedLoopB(pllBlock *pll)
 {

     CLARKE_F_MACRO/*RYB2AB_F*/(pll->alpha, pll->beta, pll->zero, pll->va, pll->vb, pll->vc);
     AB2DQ_F(pll->lpf2.u, pll->lpf1.u,pll->alpha,pll->beta,pll->theta_0);

 // Low pass filtering of qe-component
     LOWPASSFILTER(pll->lpf2);
     vol_de = pll->lpf2.y;
     LOWPASSFILTER(pll->lpf1);
     vol_qe = pll->lpf1.y;
     pll->notchBV.In = pll->lpf1.y;
     NOTCH_FLTR_F_run(&pll->notchBV, &pll->notchTGF);
     pll->pi_pll.Ref = 0;
     pll->pi_pll.Fbk = -pll->notchBV.Out;
     PIREG_MACRO(pll->pi_pll);
     pll->omega   = pll->pi_pll.Out;

     /*** sine and cosine generation using Harmonic Oscillator ***/
     pll->wAngle += pll->omega;
     if(pll->wAngle>= TWOPI) pll->wAngle -= TWOPI;

     pll->theta_0.sin_val = sinf(pll->wAngle);
     pll->theta_0.cos_val = cosf(pll->wAngle);
#define ANGLE2RAD   0.0174532925
     if(SyncAdjustAngle<0.0) adjustAngle = (SyncAdjustAngle + 360) * ANGLE2RAD;
     else adjustAngle = (SyncAdjustAngle * ANGLE2RAD);
     pll->theta.sin_val = sinf(pll->wAngle + adjustAngle) ;
     pll->theta.cos_val = cosf(pll->wAngle + adjustAngle);

     return(pll->omega);
 }

 void    InitPLL(pllBlock *pll)
 {
     pll->va = 0;
     pll->vb = 0;
     pll->vc = 0;
     pll->omega = OmegaDefault;
     pll->theta.cos_val = (1.0);
     pll->theta.sin_val = (0.0);

     setLowPassFilter( &pll->lpf1, 50.0*SAMPLING_TIME, 0.0);
     setLowPassFilter( &pll->lpf2, 50.0*SAMPLING_TIME, 0.0);
     //setPiRegulator( &pll->pi_pll, 0.005,  0.00000340, 0.0, 1.05*2*PI*GridFrequency*SAMPLING_TIME, 0.9*2*PI*GridFrequency*SAMPLING_TIME);

     float32_t w0 = 2.0f*PI*GridFrequency*SAMPLING_TIME;
     setPiRegulator(&pll->pi_pll, 0.005, 0.00000340, w0, 1.05f*w0, 0.9f*w0);

     NOTCH_FLTR_F_VARS_init(&pll->notchBV);
     NOTCH_FLTR_F_COEFF_Update(SAMPLING_TIME, (float)(2.0*PI*GridFrequency*2.0),0.00001,0.1, &pll->notchTGF);

         FullCycle = 400;
         MinPoints = 360;
         count30_SP1 = 1666;
         count30_SP2 = 1666;
         count_60 = 3333;
         count_30 = count_60 >> 1;


 }





