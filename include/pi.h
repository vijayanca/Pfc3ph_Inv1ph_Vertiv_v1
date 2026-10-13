/* =================================================================================
File name:       PI.H 
===================================================================================*/
#ifndef __PI_H__
#define __PI_H__

typedef struct {  float32_t  Ref;   			// Input: reference set-point
				  float32_t  Fbk;   			// Input: feedback
				  float32_t  Out;   			// Output: controller output
				  float32_t  Kp;				// Parameter: proportional loop gain
				  float32_t  Ki;			    // Parameter: integral gain
				  float32_t  Umax;			// Parameter: upper saturation limit
				  float32_t  Umin;			// Parameter: lower saturation limit
				  float32_t  up;				// Data: proportional term
				  float32_t  ui;				// Data: integral term
				  float32_t  v1;				// Data: pre-saturated controller output
				  float32_t  i1;				// Data: integrator storage: ui(k-1)
				  float32_t  w1;				// Data: saturation record: [u(k-1) - v(k-1)]
				} PI_CONTROLLER;


/*-----------------------------------------------------------------------------
Default initalisation values for the PI_GRANDO objects
-----------------------------------------------------------------------------*/                     

#define PI_CONTROLLER_DEFAULTS {		\
						   0, 			\
                           0, 			\
						   0, 			\
                           float32_t(1.0),	\
                           float32_t(0.0),	\
                           float32_t(1.0),	\
                           float32_t(-1.0), 	\
                           float32_t(0.0),	\
                           float32_t(0.0), 	\
                           float32_t(0.0),	\
                           float32_t(0.0),	\
                           float32_t(1.0) 	\
              			  }


/*------------------------------------------------------------------------------
 	PI_GRANDO Macro Definition
------------------------------------------------------------------------------*/
#define PI_MACRO_CLA(v) \
				/* proportional term */							    \
				v.up = v.Kp* (v.Ref - v.Fbk);						\
				/* integral term */ 								\
				v.ui = (v.Out == v.v1)?((v.Ki* v.up) + v.i1) : v.i1;	\
				v.i1 = v.ui;										\
				/* control output */ 						    	\
				v.v1 = v.up + v.ui;									\
				/*v.Out= __fmax((__fmin(v.Umax,v.v1)),v.Umin)*/

#define PI_MACRO(v)                                                 \
			    /* proportional term */                                     \
			    v.up = v.Ref - v.Fbk;                                       \
			    v.ui = (v.Out == v.v1)?((v.Ki* v.up)+ v.i1) : v.i1;         \
			    v.i1 = v.ui;                                                \
			    v.v1 = (v.Kp* (v.up + v.ui));                               \
			    v.Out= __fmax((__fmin(v.Umax,v.v1)),v.Umin);          \
			    v.w1 = (v.Out == v.v1) ? (1.0) : (0.0)

#endif // __PI_H__
