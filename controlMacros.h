/*
 * controlMacros.h
 *
 *  Created on: 13-Aug-2023
 *      Author: vijay
 */

#ifndef CONTROLMACROS_H_
#define CONTROLMACROS_H_


#define checkLimitHN(var,lmt,dly,tmr)    \
    if(var>lmt) { if(tmr<dly) tmr++; } \
    else if(tmr) tmr--;  \
    if(tmr>dly) tmr=dly; \
    Nop()


#define checkLimitL(var,lmt,dly,tmr)    \
    if(var<lmt) { if(tmr<dly) tmr++; } \
    else if( (var >= (lmt+vdc_hysteresis)) && tmr) tmr--;   \
    if(tmr>dly) tmr=dly; \
    Nop()

#define checkLimitH(var,lmt,dly,tmr)    \
    if(var>lmt) { if(tmr<dly) tmr++; } \
    else if( (var<(lmt-vdc_hysteresis)) && tmr) tmr--;  \
    if(tmr>dly) tmr=dly; \
    Nop()

#define checkOK(x1,x2,dly,tmr)  \
    if(x1 || x2 ) {if(tmr) tmr--;} \
    else { if(tmr<dly) tmr++; } \
    Nop()

#define checkRange(var,llmt,hlmt,dly,tmr)   \
    if((var<llmt) || (var>hlmt)) { if(tmr<dly) tmr++; } \
    else {if(tmr) tmr--;}   \
    if(tmr>dly) tmr=dly; \
    Nop()

#define updateBit(dly,tmr,bit)  \
    if(tmr==dly) bit = TRUE;    \
    else if(tmr==0) bit = FALSE

#define checkDI_ANALOG(b,t,d)   \
        if (b>2000) { if (t < d) t++;   } \
        else {  if (t) t--;     } \
        Nop()

#define SetBit(dly,tmr,bit) \
        if(tmr==dly) bit = TRUE



#define RYB2AB_F(alpha,beta,z,r,y,b)         \
    alpha = 0.6666666666f * r - 0.3333333333f*y - 0.3333333333f*b;    \
    beta  = 0.57735026913f * (y - b);          \
    z     = 0.33333333333f*(r + y + b)

#define AB2DQN_F(d,q,a,b,s)   \
    d =  -a*s.sin_val - b*s.cos_val;  \
    q = a*s.cos_val - b*s.sin_val

#define DQ2ABN_F(a,b,d,q,s)        \
    a =  -d*s.sin_val + q*s.cos_val;\
    b =  -q*s.sin_val - d*s.cos_val

#define AB2DQ_F(d,q,a,b,s)   \
    d =  a*s.sin_val - b*s.cos_val;  \
    q = a*s.cos_val + b*s.sin_val

#define DQ2AB_F(a,b,d,q,s)        \
    a =  d*s.sin_val + q*s.cos_val;\
    b =  q*s.sin_val - d*s.cos_val

#define AB2RYB_F(a,b,c,alpha,beta)        \
    a = alpha;                           \
    b = -0.5f*alpha + 0.86602540378f*beta; \
    c = -0.5f*alpha - 0.86602540378f*beta

#define CLARKE_LINEV_MACRO_old(alpha,beta,vr,vy) \
    alpha =  vr*0.66666666667 + vy*0.3333333333; \
    beta  =  vy*-0.577350269

#define CLARKE_LINEV_MACRO_old1(alpha,beta,vr,vy) \
    alpha =  vr*1.154700538 + vy*0.577350269; \
    beta  =  -vy



//--------------------------------------------------------------------------
//  Macro HARMONICOSCILLATOR : Implementation of a PI Regulator
//--------------------------------------------------------------------------
#define HARMONICOSCILLATOR(omega, theta) \
    theta.sin_val += (omega * theta.cos_val); \
    theta.cos_val -= (omega * theta.sin_val)

//--------------------------------------------------------------------------
//  Macro UPDATE_RMS : Calculates the mean square for RMS meters
//--------------------------------------------------------------------------
#define UPDATE_RMS(c,value) \
    c.mean_square += ((value * value)); \
    c.sample_count++

//#define UPDATE_KW(kw_meter_R, lpf_vRph.y, lpf_iR.y, VinpRYSF, IinpRSF);

#define UPDATE_KW(m,v,i,vsf,isf) \
        m.mean_square += (v*vsf*i*isf)

#define SAVE_METER(m)   \
        m.savedSum = m.mean_square; \
        m.mean_square = 0; \
        m.sample_count = 0
//--------------------------------------------------------------------------
//  Macro LOWPASSFILTER : Implementation of a Low pass Filter
//--------------------------------------------------------------------------
#define LOWPASSFILTER(f) \
    f.y = f.y + (((f.u - f.y) * f.T))


#define PIREG_MACRO(x)  \
    x.Err = x.Ref - x.Fbk;  \
    x.ui = x.ui + x.Ki*x.Err; \
    if(x.ui>x.Umax) x.ui = x.Umax; \
    if(x.ui<x.Umin) x.ui = x.Umin; \
    x.Out = x.Kp*x.Err + x.ui; \
    if(x.Out>x.Umax) x.Out = x.Umax; \
    if(x.Out<x.Umin) x.Out = x.Umin

#endif /* CONTROLMACROS_H_ */
