#ifndef NOTCH_FLTR_F_H_
#define NOTCH_FLTR_F_H_

/*********** Structure Definitions ***********/
typedef struct{
	float	B2_notch;
	float	B1_notch;
	float	B0_notch;
	float	A2_notch;
	float	A1_notch;
}NOTCH_COEFF_F;

typedef struct{
	float	Out1;
	float	Out2;
	float	In;
	float	In1;
	float In2;
	float	Out;
}NOTCH_VARS_F;

//*********** Function Declarations *******//
void NOTCH_FLTR_F_VARS_init(NOTCH_VARS_F *v);
void NOTCH_FLTR_F_run(NOTCH_VARS_F *v,NOTCH_COEFF_F *w);
void NOTCH_FLTR_F_ASM(NOTCH_VARS_F *v,NOTCH_COEFF_F *w);
void NOTCH_FLTR_F_COEFF_Update(float delta_T, float wn,float c2, float c1, NOTCH_COEFF_F *notch_obj);

//*********** Macro Definition ***********//
#define NOTCH_FLTR_F_MACRO(v, w)		\
	v.Out=(w.A1_notch*v.Out1)+(w.A2_notch*v.Out2)+(w.B0_notch*v.In)+(w.B1_notch*v.In1)+(w.B2_notch*v.In2); \
	v.Out2=v.Out1;	\
	v.Out1=v.Out;	\
	v.In2=v.In1;	\
	v.In1=v.In;

#endif
