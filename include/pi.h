/* =================================================================================
File name:       PI.H 
===================================================================================*/


#ifndef __PI_H__
#define __PI_H__

typedef struct {  float32_t  Ref;   			// Input: reference set-point
				  float32_t  Fbk;   			// Input: feedback
				  float32_t  err;               // Input: feedback
				  float32_t  Out;   			// Output: controller output
				  float32_t  Kp;				// Parameter: proportional loop gain
				  float32_t  Ki;			    // Parameter: integral gain
				  float32_t  Umax;			// Parameter: upper saturation limit
				  float32_t  Umin;			// Parameter: lower saturation limit
				  float32_t  up;				// Data: proportional term
				  float32_t  ui;				// Data: integral term
				  float32_t  v1;				// Data: pre-saturated controller output
				} PI_CONTROLLER;


/*-----------------------------------------------------------------------------
Default initalisation values for the PI_GRANDO objects
-----------------------------------------------------------------------------*/                     

#define PI_CONTROLLER_DEFAULTS {		\
						   0, 			\
                           0, 			\
						   0, 			\
                           (1.0),	\
                           (0.0),	\
                           (1.0),	\
                           (-1.0), 	\
                           (0.0),	\
                           (0.0), 	\
                           (0.0),	\
                           (0.0),	\
                           (1.0) 	\
              			  }


#define PI_MACRO(v)                                               \
                                                                  \
    /* error */                                                   \
    v.err = v.Ref - v.Fbk;                                        \
                                                                  \
    /* proportional */                                            \
    v.up = v.Kp * v.err;                                         \
    /* integral (conditional) */                                  \
    if (v.Out < v.Umax && v.Out > v.Umin)                         \
        v.ui = v.i1 + v.Ki * v.err;                               \
    else  ui = v.i1;                                              \
    v.i1 = v.ui;                                                  \
    v.v1 = v.up + v.ui;                                           \
    if (v.v1 > v.Umax) v.Out = v.Umax;                            \
    else if (v.v1 < v.Umin) v.Out = v.Umin;                       \
    else v.Out = v.v1

/*------------------------------------------------------------------------------
 	PI_GRANDO Macro Definition
------------------------------------------------------------------------------*/

#define PI_MACRO0(v)												\
																\
				/* proportional term */							\
				v.up = float32_tmpy(v.Kp, (v.Ref - v.Fbk));						\
																			\
				/* integral term */ 										\
				v.ui = (v.Out == v.v1)?(float32_tmpy(v.Ki, v.up)+ v.i1) : v.i1;	\
				v.i1 = v.ui;												\
																			\
				/* control output */ 										\
				v.v1 = v.up + v.ui;											\
				v.Out= v.v1/*, v.Umax, v.Umin*/;
#endif // __PI_H__

