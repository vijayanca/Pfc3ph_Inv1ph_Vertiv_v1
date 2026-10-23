/*
 * controlTypes.h
 *
 * Control-loop data types and macros shared by the C28 CPU and the CLA
 * (cla_tasks.cla, controlCore.h). Compiled by both compilers: float32_t and
 * 16-bit fixed-width types only (int is 32 bits on the CLA), no pointers.
 */

#ifndef CONTROLTYPES_H_
#define CONTROLTYPES_H_

#include <stdint.h>

#ifdef __TMS320C28XX_CLA__
typedef float float32_t;    // f28p55x_cla_typedefs.h defines float32 only
#endif

#include "PID_GRANDO_F.h"
#include "SVGEN_CLA.h"

// Control loop (controlCore.h) on the CLA: 1, or on the CPU in AdcISR /
// EPWM_PFC_ISR: 0. Same code and data either way, for A/B checks.
#ifndef CONTROL_ON_CLA
#define CONTROL_ON_CLA      1
#endif

#define FLOAT_CONVERTER_DC    0.000244140625    // 1/4096, ADC counts -> p.u.
#define FLOAT_CONVERTER_AC    0.00048828125     // 2/4096

#define SAT_LIMIT    0.999                      // modulation limits
#define SAT_LIMITN  -0.999

#ifndef SAMPLING_TIME
#define SAMPLING_TIME       0.0000500
#endif


// For Low pass filter
typedef struct {
	float32_t T;
	float32_t u;
	float32_t y;
} LPF;

typedef struct {
	float32_t sin_val;
	float32_t cos_val;
} sincos;


typedef struct {
    float32_t Ref,Fbk;
    float32_t Kp, Ki;
    float32_t Err, Out;
    float32_t ui;
    float32_t Umax, Umin;
} PIreg;

#define PIREGULATOR(x)  \
    x.Err = x.Ref - x.Fbk; \
    x.ui = x.ui + (x.Ki * x.Err); \
    if(x.ui >x.Umax) x.ui = x.Umax; \
    else if(x.ui < x.Umin) x.ui = x.Umin; \
    x.Out = (x.Kp * x.Err) + x.ui; \
    if(x.Out > x.Umax) x.Out = x.Umax; \
    else if(x.Out < x.Umin) x.Out = x.Umin


// PI with clamped anti-windup (RBC pid_update): the integrator is committed
// only while the output is inside its limits, or when the error pulls it back in.
typedef struct {
    float32_t Ref, Fbk, Err;
    float32_t Kp, Ki;
    float32_t Out;
    float32_t Umax, Umin;
    float32_t ui;
} PFC_PI;

#define PFC_PI_MACRO(p)                                                     \
{                                                                           \
    float32_t i_, u_, o_;                                                   \
    p.Err = p.Ref - p.Fbk;                                                  \
    i_ = p.ui + (p.Ki * p.Err);                                             \
    u_ = (p.Kp * p.Err) + i_;                                               \
    o_ = u_;                                                                \
    if(o_ > p.Umax) o_ = p.Umax;                                            \
    else if(o_ < p.Umin) o_ = p.Umin;                                       \
    if((o_ == u_) || ((u_ > p.Umax) && (p.Err < 0.0f)) ||                   \
                     ((u_ < p.Umin) && (p.Err > 0.0f))) p.ui = i_;          \
    p.Out = o_;                                                             \
}

// Resonant integrator pair for the PR current controller (stationary frame)
//   v1' = Kr*e - w*v2 - 2*wc*v1 ,  v2' = w*v1   ->  v1 = Kr*s/(s^2 + 2wc*s + w^2) * e
typedef struct {
    float32_t v1;   // resonant output
    float32_t v2;   // quadrature state
} PR_RES;

#define PR_STATE_LIM    1.0f        // safety clamp only; anti-windup is the clamp in ctl_current

// forward Euler on v1, backward Euler on v2: poles stay on the unit circle for damp2 = 0
#define PR_RES_MACRO(r,e,kr,w,damp2)                                                    \
    r.v1 += (float32_t)SAMPLING_TIME * ((kr)*(e) - (w)*r.v2 - (damp2)*r.v1);            \
    if(r.v1 >  PR_STATE_LIM) r.v1 =  PR_STATE_LIM;                                      \
    if(r.v1 < -PR_STATE_LIM) r.v1 = -PR_STATE_LIM;                                      \
    r.v2 += (float32_t)SAMPLING_TIME * ((w)*r.v1);                                      \
    if(r.v2 >  PR_STATE_LIM) r.v2 =  PR_STATE_LIM;                                      \
    if(r.v2 < -PR_STATE_LIM) r.v2 = -PR_STATE_LIM

#define PR_RES_CLEAR(r)     r.v1 = 0.0f; r.v2 = 0.0f

// PFC current controller: dq feedback filters and PI regulators
typedef struct
{
    LPF lpf_de, lpf_qe;
    PFC_PI pid_de, pid_qe;
}
PFC_CTRL;


// For Current Limiter
typedef struct                                      //VCA
{
    float32_t mean_square;
    uint16_t sample_count;
    float32_t savedSum;
    float RMS;
} rms_meter;

#endif /* CONTROLTYPES_H_ */
