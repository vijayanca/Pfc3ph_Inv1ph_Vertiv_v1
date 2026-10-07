/*
 * BoardMacros.h
 *
 *  Created on: 31-Oct-2023
 *      Author: vijay
 */

#ifndef BOARDMACROS_H_
#define BOARDMACROS_H_

#define MSG_DATA_LENGTH     8
#define TX_MSG_OBJ_ID       1
#define RX_MSG_OBJ_ID       1

#define ENABLE_EEPR()       GpioDataRegs.GPBCLEAR.bit.GPIO57 = 1
#define DISABLE_EEPR()      GpioDataRegs.GPBSET.bit.GPIO57 = 1
#define DISABLE_SPI()       GpioDataRegs.GPBSET.bit.GPIO57 = 1

#define EnableTX()  GpioDataRegs.GPBSET.bit.GPIO34  = 1 // TXEN cleared
#define DisableTX() GpioDataRegs.GPBCLEAR.bit.GPIO34    = 1 // TXEN cleared

#define EnableRLY1()   GpioDataRegs.GPBSET.bit.GPIO33  = 1
#define DisableRLY1()  GpioDataRegs.GPBCLEAR.bit.GPIO33    = 1




#endif /* BOARDMACROS_H_ */
