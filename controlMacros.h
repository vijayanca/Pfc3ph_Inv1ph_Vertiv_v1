/*
 * controlMacros.h
 *
 *  Created on: 13-Aug-2023
 *      Author: vijay
 */

#ifndef CONTROLMACROS_H_
#define CONTROLMACROS_H_

#define MAX3(a, b, c)  ((a > b) ? ((a > c) ? a : c) : ((b > c) ? b : c))

#define checkLimitHN(var,lmt,dly,tmr)    \
    if(var>lmt) { if(tmr<dly) tmr++; } \
    else if(tmr) tmr--;  \
    if(tmr>dly) tmr=dly; \
    Nop()

#define checkLimitLN(var,lmt,dly,tmr)    \
    if(var<lmt) { if(tmr<dly) tmr++; } \
    else if( (var > lmt) && tmr) tmr--;   \
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

#define UpdateBit(dly,tmr,bit)  \
    if(tmr==dly) bit = TRUE;    \
    else if(tmr==0) bit = FALSE

#define checkDI_ANALOG(b,t,d)   \
        if (b>2000) { if (t < d) t++;   } \
        else {  if (t) t--;     } \
        Nop()

#define checkDI_ANALOG_NO(b,t,d)   \
        if (b<2000) { if (t < d) t++;   } \
        else {  if (t) t--;     } \
        Nop()

#define SetBit(dly,tmr,bit) \
        if(tmr==dly) bit = TRUE



#define RYB2AB_F(alpha,beta,z,r,y,b)         \
    alpha = 0.6666666666 * r - 0.3333333333*y - 0.3333333333*b;    \
    beta  = 0.57735026913 * (y - b);          \
    z     = 0.33333333333*(r + y + b)

#define CLARKE_F_MACRO(alpha,beta,z,r,y,b)                                   \
    alpha=((0.66666666667)*(r - ((0.5)*(y+b))));    \
    beta =((0.57735026913)*(y - b));                  \
    z =((0.33333333333)*(r + y + b))

#define AB2DQ_F(d,q,a,b,s)   \
    d =  a*s.sin_val - b*s.cos_val;  \
    q = a*s.cos_val + b*s.sin_val

#define DQ2AB_F(a,b,d,q,s)        \
    a =  d*s.sin_val + q*s.cos_val;\
    b =  q*s.sin_val - d*s.cos_val;\

#define AB2RYB_F(a,b,c,alpha,beta)        \
    a = alpha;                           \
    b = -0.5*alpha + 0.86602540378*beta; \
    c = -0.5*alpha - 0.86602540378*beta

#define CLARKE_LINEV_MACRO(alpha,beta,vr,vy) \
    alpha =  vr*0.66666666667 + vy*0.3333333333; \
    beta  =  vy*0.577350269

#define CLARKE_LINE_MACRO(alpha,beta,vr,vy) \
    alpha =  vr*1.154700538 + vy*0.577350269; \
    beta  =  -vy

//#define CLARKE_LINE_MACRO(alpha,beta,vr,vy) \
//    alpha = alpha * TWO3 + vy * ONE3; \
//    beta  = beta * -0.577350269

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



//---------STATIC SWITCH CONTROL----------------
#define STATIC_DRIVES_OFF()    \
        EPwm1Regs.AQCTLA.bit.CAD = AQ_CLEAR;    \
        EPwm1Regs.AQCTLB.bit.CBD = AQ_CLEAR;    \
        EPwm2Regs.AQCTLA.bit.CAD = AQ_CLEAR;    \
        EPwm2Regs.AQCTLB.bit.CBD = AQ_CLEAR;    \
        EPwm3Regs.AQCTLA.bit.CAD = AQ_CLEAR;    \
        EPwm3Regs.AQCTLB.bit.CBD = AQ_CLEAR;    \
        EPwm4Regs.AQCTLA.bit.CAD = AQ_CLEAR;    \
        EPwm4Regs.AQCTLB.bit.CBD = AQ_CLEAR;    \
        EPwm5Regs.AQCTLA.bit.CAD = AQ_CLEAR;    \
        EPwm5Regs.AQCTLB.bit.CBD = AQ_CLEAR;    \
        EPwm6Regs.AQCTLA.bit.CAD = AQ_CLEAR;    \
        EPwm6Regs.AQCTLB.bit.CBD = AQ_CLEAR

#define EnableInvSSR()  EPwm4Regs.AQCTLA.bit.CAD = AQ_SET
#define EnableInvSSY()  EPwm4Regs.AQCTLB.bit.CBD = AQ_SET
#define EnableInvSSB()  EPwm5Regs.AQCTLA.bit.CAD = AQ_SET

#define EnableBypSSR()  EPwm5Regs.AQCTLB.bit.CBD = AQ_SET
#define EnableBypSSY()  EPwm6Regs.AQCTLA.bit.CAD = AQ_SET
#define EnableBypSSB()  EPwm6Regs.AQCTLB.bit.CBD = AQ_SET

#define DisableInvSSR()  EPwm4Regs.AQCTLA.bit.CAD = AQ_CLEAR
#define DisableInvSSY()  EPwm4Regs.AQCTLB.bit.CBD = AQ_CLEAR
#define DisableInvSSB()  EPwm5Regs.AQCTLA.bit.CAD = AQ_CLEAR

#define DisableBypSSR()  EPwm5Regs.AQCTLB.bit.CBD = AQ_CLEAR
#define DisableBypSSY()  EPwm6Regs.AQCTLA.bit.CAD = AQ_CLEAR
#define DisableBypSSB()  EPwm6Regs.AQCTLB.bit.CBD = AQ_CLEAR

#define EnableInvSS() EnableInvSSR(); \
                       EnableInvSSY(); \
                       EnableInvSSB()
#define EnableBypSS() EnableBypSSR(); \
                       EnableBypSSY(); \
                       EnableBypSSB()

#define DisableInvSS() DisableInvSSR(); \
                       DisableInvSSY(); \
                       DisableInvSSB()
#define DisableBypSS() DisableBypSSR(); \
                       DisableBypSSY(); \
                       DisableBypSSB()

#endif /* CONTROLMACROS_H_ */
