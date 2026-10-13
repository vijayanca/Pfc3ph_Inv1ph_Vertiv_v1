#ifndef CNTL_PI_F_H_
#define CNTL_PI_F_H_



//*********** Structure Definition ********//

// PI control law with programmable output saturation.
// This macro uses CNTL_PI_F structures to store coefficients & internal values.
// The structures should be initialized with the supplied CNTL_PI_F_INIT macro.
// Within the structure the Max & Min parameters are the output bounds

typedef struct {
	float  Ref;   			// Input: reference set-point
	float  Fbk;   			// Input: feedback
	float  Out;   			// Output: controller output
	float  Kp;				// Parameter: proportional loop gain
	float  Ki;			    // Parameter: integral gain
	float  Umax;				// Parameter: upper saturation limit
	float  Umin;				// Parameter: lower saturation limit
	float  up;				// Data: proportional term
	float  ui;				// Data: integral term
	float  v1;				// Data: pre-saturated controller output
	float  i1;				// Data: integrator storage: ui(k-1)
	float  w1;				// Data: saturation record: [u(k-1) - v(k-1)]
} CNTL_PI_F;

//*********** Function Declarations *******//
void CNTL_PI_F_init(CNTL_PI_F *k);
void CNTL_PI_F_FUNC(CNTL_PI_F *v);

//*********** Macro Definition ***********//
#define CNTL_PI_F_MACRO(v)								\
	/* proportional term */ 							\
	v.up = ((v.Ref - v.Fbk)*v.Kp);						\
														\
	/* integral term */ 								\
	v.ui = (v.Out == v.v1)?((v.Ki* v.up)+ v.i1) : v.i1;	\
	v.i1 = v.ui;										\
														\
	/* control output */ 								\
	v.v1 = (v.up + v.ui);								\
	v.Out= (v.v1> v.Umax)?v.Umax:v.v1;					\
	v.Out= (v.Out< v.Umin)?v.Umin:v.Out;				\
	

#endif /* CNTL_PI_F_H_ */
