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
volatile N16 gridfCount = 0;
volatile N16 gridokcounter = 0;

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

     // filtered d/q for monitoring, grid-fail detection and the q feed-forward
     // of the current controller (outside the loop)
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

     pll->theta.sin_val = s;
     pll->theta.cos_val = c;

     // fast mains failure: positive sequence below GridFailLevel for 5 samples,
     // back after 50 samples (2.5 ms) above it
     if(vol_de < mainsFailLevel)
     {
             if(gridfCount<5) gridfCount++;
             if(gridfCount==5)
                 {
                     gridFailed = TRUE;
                     gridokcounter = 0;
                 }
     }

     else
         {
             if(gridfCount) gridfCount--;
             if(gridokcounter<50) gridokcounter++;
             if(gridokcounter==50) { gridFailed = FALSE; gridfCount = 0; }
         }


     return(pll->omega);
 }


#pragma CODE_SECTION(phaseLockedLoop1ph, ".TI.ramfunc");

 //---------------------------------------------------------------------------
 //  phaseLockedLoop1ph: single-phase SOGI-PLL (bypass voltage)
 //---------------------------------------------------------------------------
 //  The SOGI gives the in-phase (alpha) and the 90 degree lagging (beta) parts
 //  of v = V sin(wt): alpha = V sin, beta = -V cos, the same convention as the
 //  3-phase alpha-beta, so theta has sin in phase with v. Loop as phaseLockedLoopB.
 float32_t phaseLockedLoop1ph(pllBlock *pll, float32_t v)
 {
     float32_t a, d, q, s, c;

     a = pll->omega * (1.0 - pll->omega*pll->omega*0.0416666667);

     pll->va1  += a*(SOGI_K*(v - pll->va1) - pll->qva1);
     pll->qva1 += a*pll->va1;
     pll->alpha = pll->va1;
     pll->beta  = pll->qva1;

     AB2DQ_F(d, q, pll->alpha, pll->beta, pll->theta_0);

     pll->amp = sqrtf(pll->alpha*pll->alpha + pll->beta*pll->beta);
     if(pll->amp > PLL_AMP_MIN) pll->qn = q/pll->amp;
     else pll->qn = q/PLL_AMP_MIN;

     pll->pi_pll.Ref = 0;
     pll->pi_pll.Fbk = -pll->qn;
     PIREG_MACRO(pll->pi_pll);
     pll->omega   = pll->pi_pll.Out;

     pll->lpf2.u = d;
     LOWPASSFILTER(pll->lpf2);
     pll->de = pll->lpf2.y;
     pll->lpf1.u = q;
     LOWPASSFILTER(pll->lpf1);
     pll->qe = pll->lpf1.y;

     pll->wAngle += pll->omega;
     if(pll->wAngle>= TWOPI) pll->wAngle -= TWOPI;
     pll->theta_0.sin_val = sinf(pll->wAngle);
     pll->theta_0.cos_val = cosf(pll->wAngle);

     // remove the discrete SOGI phase lead of 1.25 samples (small angle rotation)
     a = 1.25*pll->omega;
     d = 1.0 - 0.5*a*a;
     s = pll->theta_0.sin_val*d - pll->theta_0.cos_val*a;
     c = pll->theta_0.cos_val*d + pll->theta_0.sin_val*a;
     pll->theta.sin_val = s;
     pll->theta.cos_val = c;

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





