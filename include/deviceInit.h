
//###########################################################################
//
// FILE:   DeviceInit.h
//
// TITLE:  Device Initialisation Definitions (Specific to C3591)
//
//###########################################################################
// $PEI elease: 2803x C/C++ Header Files $
// $Release Date: August 1, 2012 $
//###########################################################################

#ifndef DEVICE_INIT_H
#define DEVICE_INIT_H


void GpioInit(void);
void EPwmInit();
void AdcInit();
void InitTimers();
void InitSCI(void);
void InitLIN(void);


#endif
