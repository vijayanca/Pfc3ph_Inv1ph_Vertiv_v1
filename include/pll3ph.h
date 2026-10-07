/*
 * pll3ph.h
 *
 *  Created on: 17-Mar-2023
 *      Author: vijay
 */

#ifndef INCLUDE_PLL3PH_H_
#define INCLUDE_PLL3PH_H_
#include "../include/NOTCH_FLTR_F.h"


typedef struct {
    float32_t va, vb, vc;
    float32_t alpha, beta, zero;
    float32_t de, qe;
    float32_t omega;
    float32_t wAngle;
    LPF lpf1,lpf2;
    PIreg pi_pll;
    NOTCH_COEFF_F notchTGF;
    NOTCH_VARS_F notchBV;
    sincos theta_0;
    sincos theta;

} pllBlock;

float32_t     phaseLockedLoopB(pllBlock *pll);
void    InitPLL(pllBlock *pll);

#endif /* INCLUDE_PLL3PH_H_ */
