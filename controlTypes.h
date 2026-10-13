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
// EPWM_INV_ISR: 0. Same code and data either way, for A/B checks.
#ifndef CONTROL_ON_CLA
#define CONTROL_ON_CLA      1
#endif

#define SHIFTED_FBACK                           // feedback angle = inverter angle + Angle30 (theta_fb)

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

typedef struct
{
    LPF lpf_de, lpf_qe;
    PIreg pi_de, pi_qe;
    float32_t ref_d, ref_q;
}
harmonic;


#define PIREGULATOR(x)  \
    x.Err = x.Ref - x.Fbk; \
    x.ui = x.ui + (x.Ki * x.Err); \
    if(x.ui >x.Umax) x.ui = x.Umax; \
    else if(x.ui < x.Umin) x.ui = x.Umin; \
    x.Out = (x.Kp * x.Err) + x.ui; \
    if(x.Out > x.Umax) x.Out = x.Umax; \
    else if(x.Out < x.Umin) x.Out = x.Umin


// Resonant (PR) voltage controller, stationary alpha-beta frame
#define PR_NHARM        4           // 5th, 7th, 11th, 13th
#define PR_FUND_LIMIT   0.96

// Open-loop test (VoltCtrlMode = 2): fixed balanced modulation, no voltage feedback,
// ramped 0 -> OL_MOD_MAX in 1 s. Same limit as the PI/PR controller outputs.
#define OL_MOD_MAX      0.96
#define OL_MOD_STEP     (OL_MOD_MAX * SAMPLING_TIME / 1.0)

typedef struct {
    float32_t Y;        // in-phase state
    float32_t X;        // quadrature state
    float32_t out;
} PR_STATE;

typedef struct {
    float32_t a;        // 2*sin(h*w*Ts/2) -> exact discrete resonance at h*w
    float32_t c, s;     // cos/sin of phase lead (delay compensation)
} PR_COEF;

// Two-integrator resonator: Y += k*e - wd*Y - a*X ; X += a*Y ; out = c*Y - s*X
#define PR_RES_MACRO(r,k,e,cf,wd,lim) \
    r.Y = r.Y + ((k) * (e)) - ((wd) * r.Y) - (cf.a * r.X); \
    if(r.Y > (lim)) r.Y = (lim); \
    else if(r.Y < -(lim)) r.Y = -(lim); \
    r.X = r.X + (cf.a * r.Y); \
    if(r.X > (lim)) r.X = (lim); \
    else if(r.X < -(lim)) r.X = -(lim); \
    r.out = (cf.c * r.Y) - (cf.s * r.X)



// For Current Limiter
typedef struct                                      //VCA
{
    float32_t mean_square;
    uint16_t sample_count;
    float32_t savedSum;
    float RMS;
} rms_meter;
/*************************** STRUCTURE DECLARATIONS *************************/



typedef struct
{
        float32_t a, b, c;
        float32_t d, q, z;
        float32_t de, qe;
        LPF lpf_de, lpf_qe;
        PID_GRANDO_F_CONTROLLER pid_de, pid_qe;
        float32_t ref_d, ref_q;
        float32_t ref_a, ref_b, ref_c;
} DQControl;


typedef struct                                      //VCA
{
    DQControl vol;
    float32_t deo;
    float32_t qeo;
    float32_t alpha;
    float32_t beta;
    float32_t refa;
    float32_t refb;
    float32_t refc;
    sincos theta;
    sincos theta_o;
    float32_t omeaga;
}
VolControl;

#endif /* CONTROLTYPES_H_ */
