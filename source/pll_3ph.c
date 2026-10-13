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

void NOTCH_FLTR_F_COEFF_Update(float delta_T, float wn,float c2, float c1, NOTCH_COEFF_F *notch_obj);
void NOTCH_FLTR_F_run(NOTCH_VARS_F *v,NOTCH_COEFF_F *w);
void NOTCH_FLTR_F_VARS_init(NOTCH_VARS_F *v);

#pragma CODE_SECTION(phaseLockedLoopB, ".TI.ramfunc");
//VARIABLES:

 //---------------------------------------------------------------------------
 //  phaseLockedLoop:
 //---------------------------------------------------------------------------
 //  This function is the implementation of Phase Locked Loop
 //  This takes the three phase grid voltage as input and determines omega
 //  required for reference frame transformations
volatile N16 bypfCount = 0;
volatile N16 bypokcounter = 0;

// DSOGI-PLL: SOGI filters on alpha/beta, positive sequence extraction,
// amplitude normalised SRF-PLL. No filter inside the loop.
#define SOGI_K      1.41421356      // SOGI gain (sqrt 2)
#define PLL_AMP_MIN 0.02            // below this, phase error is not normalised further

 float32_t phaseLockedLoopB(pllBlock *pll)
 {
     float32_t a, d, q, s, c;

     RYB2AB_F(pll->alpha, pll->beta, pll->zero, pll->va, pll->vb, pll->vc);

     // SOGI, frequency adaptive (a = w*Ts, w in rad/sample)
     a = pll->omega * (1.0 - pll->omega*pll->omega*0.0416666667);

     pll->va1  += a*(SOGI_K*(pll->alpha - pll->va1) - pll->qva1);
     pll->qva1 += a*pll->va1;
     pll->vb1  += a*(SOGI_K*(pll->beta - pll->vb1) - pll->qvb1);
     pll->qvb1 += a*pll->vb1;

     // positive sequence: v+ = 1/2 [1 -q; q 1] v'
     pll->alphaP = 0.5*(pll->va1 - pll->qvb1);
     pll->betaP  = 0.5*(pll->qva1 + pll->vb1);

     AB2DQ_F(d, q, pll->alphaP, pll->betaP, pll->theta_0);

     pll->amp = sqrtf(pll->alphaP*pll->alphaP + pll->betaP*pll->betaP);
     if(pll->amp > PLL_AMP_MIN) pll->qn = q/pll->amp;
     else pll->qn = q/PLL_AMP_MIN;

     pll->pi_pll.Ref = 0;
     pll->pi_pll.Fbk = -pll->qn;
     PIREG_MACRO(pll->pi_pll);
     pll->omega   = pll->pi_pll.Out;

     // filtered d/q for monitoring and bypass-fail detection (outside the loop)
     pll->lpf2.u = d;
     LOWPASSFILTER(pll->lpf2);
     vol_de = pll->lpf2.y;
     pll->lpf1.u = q;
     LOWPASSFILTER(pll->lpf1);
     vol_qe = pll->lpf1.y;

     pll->wAngle += pll->omega;
     if(pll->wAngle>= TWOPI) pll->wAngle -= TWOPI;
     pll->theta_0.sin_val = sinf(pll->wAngle);
     pll->theta_0.cos_val = cosf(pll->wAngle);

     // remove the discrete SOGI phase lead of 1.25 samples (small angle rotation)
     a = 1.25*pll->omega;
     d = 1.0 - 0.5*a*a;
     s = pll->theta_0.sin_val*d - pll->theta_0.cos_val*a;
     c = pll->theta_0.cos_val*d + pll->theta_0.sin_val*a;

#ifndef SHIFTED_FBACK
     pll->theta.sin_val = s;
     pll->theta.cos_val = c;
#else
     // rotate by -Angle30 (prRotC/prRotS = cos/sin(Angle30), from PR_UpdateCoeffs)
     pll->theta.sin_val = s*prRotC - c*prRotS;
     pll->theta.cos_val = c*prRotC + s*prRotS;
#endif

     if(vol_de < bypassFailLevel)
     {
             if(bypfCount<5) bypfCount++;
             if(bypfCount==5)
                 {
                     bypassFailed = TRUE;
                     bypokcounter = 0;
                     SystemFlag.bit.NOTSYNCED = TRUE;
                     Synced = FALSE;
                 }
     }

     else
         {
             if(bypfCount) bypfCount--;
             if(bypokcounter<50) bypokcounter++;
             if(bypokcounter==50) { bypassFailed = FALSE; bypfCount = 0; }
         }


     return(pll->omega);
 }


 void    InitPLL(pllBlock *pll)
 {
     pll->va = 0;
     pll->vb = 0;
     pll->vc = 0;
     pll->omega = OMEGADEFAULT;
     pll->wAngle = 0;
     pll->theta.cos_val = (1.0);
     pll->theta.sin_val = (0.0);
     pll->theta_0.cos_val = (1.0);
     pll->theta_0.sin_val = (0.0);
     pll->va1 = 0; pll->qva1 = 0;
     pll->vb1 = 0; pll->qvb1 = 0;
     pll->alphaP = 0; pll->betaP = 0;
     pll->amp = 0; pll->qn = 0;

     setLowPassFilter( &pll->lpf1, 50.0*SAMPLING_TIME, 0.0);
     setLowPassFilter( &pll->lpf2, 50.0*SAMPLING_TIME, 0.0);
     // normalised loop: wn = 2*pi*5 rad/s, zeta = 0.707
     // Kp = 2*zeta*wn*Ts, Ki = wn^2*Ts^2 (output in rad/sample); integrator starts at nominal
     setPiRegulator_old( &pll->pi_pll, 0.00222, 0.00000247, OMEGADEFAULT, 1.05*2*PI*GRID_FREQ*SAMPLING_TIME, 0.9*2*PI*GRID_FREQ*SAMPLING_TIME);

 }





