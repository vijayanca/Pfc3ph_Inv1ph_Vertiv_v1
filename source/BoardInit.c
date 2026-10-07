/*
 * BoardInit.c
 *
 *  Created on: 24-Oct-2023
 *      Author: Vijayan
 *
 *  Board : Common Controller (Common_Board_3ph_Inv_pfc_rect12p_v2)
 *  MCU   : TMS320F28P550SG9PZR (100-pin PZ)
 *  Pin numbers / signal names follow board_pin_map.h.
 *
 *  12-pulse rectifier use of the common board:
 *   - ePWM1..6 A/B (GPIO0..11) : SCR firing pulses, bridge 1 and bridge 2
 *   - ePWM7 module             : ADC trigger timebase only (ePWM7_Init in pwm.c).
 *                                Its pins GPIO12/13 are the static-switch
 *                                outputs (STAT_INV / STAT_BYP) as plain GPIO.
 */

#define CCEXTERN    extern
#define INIT0
#define INITARRAY0

#include "f28x_project.h"
#include "../ccVariables.h"
#include "string.h"
#include "../include/device.h"

#include "driverlib.h"
#include "device.h"
#include "board_pin_map.h"

#define CARD_ID         1021
#define SWVERSION_SET   100

//--------------------------------------------------------------------------
// RS485 direction control (ISO3082: DE and /RE tied, 1 = transmit)
//   RS485 #1 : SCIB, TXEN1 = GPIO40
//   RS485 #2 : SCIA, TXEN  = GPIO34  (EnableTX()/DisableTX() in rectDefines.h)
//--------------------------------------------------------------------------
#define SCIA_TX_ENABLE()    GPIO_WritePin(RS485_2_TXEN_GPIO, 1)
#define SCIA_TX_DISABLE()   GPIO_WritePin(RS485_2_TXEN_GPIO, 0)
#define SCIB_TX_ENABLE()    GPIO_WritePin(RS485_1_TXEN_GPIO, 1)
#define SCIB_TX_DISABLE()   GPIO_WritePin(RS485_1_TXEN_GPIO, 0)

/*--------------------------------------------------------------------------
Function Name:  GpioInit
Description:    Initializes all general purpose I/O and special I/O channels.
Takes       :   None
Returns     :   None
Funcs Called:   None

Author      :   Vijayan C A
History Log:
---------------------------------------------------------------------------
Date        | Modified By |         Comments
---------------------------------------------------------------------------
10/02/2023    VCA               Created
02/10/2026    VCA               IO re-allocated for Common Controller v2
---------------------------------------------------------------------------*/

void GpioInit(void)
{
    //---------------------------------------------------------------------
    // Relay outputs first: RLY3 (GPIO24) and RLY7 (GPIO32) have boot-mode
    // pull-ups, so drive every relay low as early as possible.
    //---------------------------------------------------------------------
    SET_DO(RLY1_GPIO);      // GPIO60  pin 44
    SET_DO(RLY2_GPIO);      // GPIO33  pin 53
    SET_DO(RLY3_GPIO);      // GPIO24  pin 56  (boot pin, 3K3 pull-up)
    SET_DO(RLY4_GPIO);      // GPIO25  pin 57
    SET_DO(RLY5_GPIO);      // GPIO26  pin 58
    SET_DO(RLY6_GPIO);      // GPIO27  pin 59
    SET_DO(RLY7_GPIO);      // GPIO32  pin 64  (boot pin, 3K3 pull-up)
    SET_DO(RLY8_GPIO);      // GPIO58  pin 67
    SET_DO(RLY9_GPIO);      // GPIO22  pin 83
    SET_DO(RLY10_GPIO);     // GPIO44  pin 85
    SET_DO(RLY11_GPIO);     // GPIO61  pin 91  (also K1/K2 analog select)
    SET_DO(RLY12_GPIO);     // GPIO59  pin 92  (also K1/K2 analog select)

    GPIO_WritePin(RLY1_GPIO,  0);
    GPIO_WritePin(RLY2_GPIO,  0);
    GPIO_WritePin(RLY3_GPIO,  0);
    GPIO_WritePin(RLY4_GPIO,  0);
    GPIO_WritePin(RLY5_GPIO,  0);
    GPIO_WritePin(RLY6_GPIO,  0);
    GPIO_WritePin(RLY7_GPIO,  0);
    GPIO_WritePin(RLY8_GPIO,  0);
    GPIO_WritePin(RLY9_GPIO,  0);
    GPIO_WritePin(RLY10_GPIO, 0);
    GPIO_WritePin(RLY11_GPIO, 0);
    GPIO_WritePin(RLY12_GPIO, 0);

    EALLOW;

    //-------------- PWM Out (via 74ACT573, 10K pull-downs on board) -------
    // Bridge 1 -> U30 -> CON15
    GpioCtrlRegs.GPAPUD.bit.GPIO0       = 1;    // RTOP   EPWM1A  pin 79
    GpioCtrlRegs.GPAPUD.bit.GPIO1       = 1;    // RBOT   EPWM1B  pin 78
    GpioCtrlRegs.GPAMUX1.bit.GPIO0      = 1;
    GpioCtrlRegs.GPAMUX1.bit.GPIO1      = 1;

    GpioCtrlRegs.GPAPUD.bit.GPIO2       = 1;    // YTOP   EPWM2A  pin 77
    GpioCtrlRegs.GPAPUD.bit.GPIO3       = 1;    // YBOT   EPWM2B  pin 76
    GpioCtrlRegs.GPAMUX1.bit.GPIO2      = 1;
    GpioCtrlRegs.GPAMUX1.bit.GPIO3      = 1;

    GpioCtrlRegs.GPAPUD.bit.GPIO4       = 1;    // BTOP   EPWM3A  pin 75
    GpioCtrlRegs.GPAPUD.bit.GPIO5       = 1;    // BBOT   EPWM3B  pin 89
    GpioCtrlRegs.GPAMUX1.bit.GPIO4      = 1;
    GpioCtrlRegs.GPAMUX1.bit.GPIO5      = 1;

    // Bridge 2 -> U33 -> CON19
    GpioCtrlRegs.GPAPUD.bit.GPIO6       = 1;    // RTOP'  EPWM4A  pin 97
    GpioCtrlRegs.GPAPUD.bit.GPIO7       = 1;    // RBOT'  EPWM4B  pin 84
    GpioCtrlRegs.GPAMUX1.bit.GPIO6      = 1;
    GpioCtrlRegs.GPAMUX1.bit.GPIO7      = 1;

    GpioCtrlRegs.GPAPUD.bit.GPIO8       = 1;    // YTOP'  EPWM5A  pin 74
    GpioCtrlRegs.GPAPUD.bit.GPIO9       = 1;    // YBOT'  EPWM5B  pin 90
    GpioCtrlRegs.GPAMUX1.bit.GPIO8      = 1;
    GpioCtrlRegs.GPAMUX1.bit.GPIO9      = 1;

    GpioCtrlRegs.GPAPUD.bit.GPIO10      = 1;    // BTOP'  EPWM6A  pin 93
    GpioCtrlRegs.GPAPUD.bit.GPIO11      = 1;    // BBOT'  EPWM6B  pin 52
    GpioCtrlRegs.GPAMUX1.bit.GPIO10     = 1;
    GpioCtrlRegs.GPAMUX1.bit.GPIO11     = 1;

    // Static switch commands -> U33 Q6/Q7 -> CON24
    // Set STAT_USE_EPWM7 to 1 if they need to be PWM (EPWM7A/B).
    // Keep 0 for the rectifier: the ePWM7 module is the ADC trigger timebase.
#ifndef STAT_USE_EPWM7
#define STAT_USE_EPWM7  0
#endif
    GpioCtrlRegs.GPAPUD.bit.GPIO12      = 1;    // STAT_INV  pin 51
    GpioCtrlRegs.GPAPUD.bit.GPIO13      = 1;    // STAT_BYP  pin 50
#if STAT_USE_EPWM7
    GpioCtrlRegs.GPAMUX1.bit.GPIO12     = 1;    // EPWM7A
    GpioCtrlRegs.GPAMUX1.bit.GPIO13     = 1;    // EPWM7B
#else
    GpioDataRegs.GPACLEAR.bit.GPIO12    = 1;    // off
    GpioDataRegs.GPACLEAR.bit.GPIO13    = 1;
    GpioCtrlRegs.GPAMUX1.bit.GPIO12     = 0;    // GPIO
    GpioCtrlRegs.GPAMUX1.bit.GPIO13     = 0;
    GpioCtrlRegs.GPADIR.bit.GPIO12      = 1;    // output
    GpioCtrlRegs.GPADIR.bit.GPIO13      = 1;
#endif

    //-------------- Mains zero cross (R phase only on this board) ---------
    GpioCtrlRegs.GPBPUD.bit.GPIO47      = 0;    // ZCD_R  pin 6
    GpioCtrlRegs.GPBMUX1.bit.GPIO47     = 0;
    GpioCtrlRegs.GPBDIR.bit.GPIO47      = 0;
    GpioCtrlRegs.GPBQSEL1.bit.GPIO47    = 2;
    EDIS;

    GPIO_setQualificationPeriod(ZCD_R_GPIO, 510);
    GPIO_setQualificationMode(ZCD_R_GPIO, GPIO_QUAL_6SAMPLE);
    // NOTE: initZCD_ECAP1_GPIO47() re-configures GPIO47 to async for eCAP.

    //-------------- Analog / AGPIO pin selection --------------------------
    EALLOW;
    // AGPIO pins used as ADC inputs -> analog mode ON
    GPIO_setPinConfig(GPIO_228_GPIO228);            // pin 14  Vr_inv
    GPIO_setAnalogMode(228U, GPIO_ANALOG_ENABLED);
    GPIO_setPinConfig(GPIO_226_GPIO226);            // pin 15  Vy_inv
    GPIO_setAnalogMode(226U, GPIO_ANALOG_ENABLED);
    GPIO_setPinConfig(GPIO_242_GPIO242);            // pin 16  Vb_inv
    GPIO_setAnalogMode(242U, GPIO_ANALOG_ENABLED);
    GPIO_setPinConfig(GPIO_224_GPIO224);            // pin 17  Vr_byp
    GPIO_setAnalogMode(224U, GPIO_ANALOG_ENABLED);
    GPIO_setPinConfig(GPIO_227_GPIO227);            // pin 38  Temp1
    GPIO_setAnalogMode(227U, GPIO_ANALOG_ENABLED);
    GPIO_setPinConfig(GPIO_236_GPIO236);            // pin 39  Temp2
    GPIO_setAnalogMode(236U, GPIO_ANALOG_ENABLED);
    GPIO_setPinConfig(GPIO_230_GPIO230);            // pin 40  Temp3
    GPIO_setAnalogMode(230U, GPIO_ANALOG_ENABLED);
    GPIO_setPinConfig(GPIO_253_GPIO253);            // pin 41  Vref1.5
    GPIO_setAnalogMode(253U, GPIO_ANALOG_ENABLED);

    // AIO / AGPIO pins used as digital inputs -> analog mode OFF
    GPIO_setPinConfig(GPIO_247_GPIO247);            // pin 42  DI_1
    GPIO_setAnalogMode(247U, GPIO_ANALOG_DISABLED);
    GPIO_setPinConfig(GPIO_20_GPIO20);              // pin 48  DI_2
    GPIO_setAnalogMode(20U, GPIO_ANALOG_DISABLED);
    GPIO_setPinConfig(GPIO_21_GPIO21);              // pin 49  DI_3
    GPIO_setAnalogMode(21U, GPIO_ANALOG_DISABLED);
    EDIS;

    //------------ DIGITAL INPUTS (opto DI01..DI10) --------------------------
    SET_DI(DI1_GPIO);       // GPIO247 pin 42
    SET_DI(DI2_GPIO);       // GPIO20  pin 48
    SET_DI(DI3_GPIO);       // GPIO21  pin 49
    SET_DI(DI4_GPIO);       // GPIO63  pin 47
    SET_DI(DI5_GPIO);       // GPIO48  pin 7
    SET_DI(DI6_GPIO);       // GPIO49  pin 8
    SET_DI(DI7_GPIO);       // GPIO50  pin 9
    SET_DI(DI8_GPIO);       // GPIO51  pin 10
    SET_DI(DI9_GPIO);       // GPIO52  pin 11
    SET_DI(DI10_GPIO);      // GPIO53  pin 12

    //------------ Gate driver fault inputs ----------------------------------
    SET_DI(VCESAT1_GPIO);   // GPIO54  pin 13  bridge 1 (CON15)
    SET_DI(VCESAT2_GPIO);   // GPIO55  pin 43  bridge 2 (CON19)

    //------------ Other outputs ---------------------------------------------
    SET_DO(SYSLED_GPIO);        // GPIO62 pin 46, LED active low
    SET_DO(RS485_2_TXEN_GPIO);  // GPIO34 pin 94, SCIA RS485 DE
    SET_DO(RS485_1_TXEN_GPIO);  // GPIO40 pin 80, SCIB RS485 DE
    SET_DO(EEPROM_CS_GPIO);     // GPIO57 pin 66, 25LC512 /CS

    GPIO_WritePin(SYSLED_GPIO, 1);          // LED off
    GPIO_WritePin(RS485_2_TXEN_GPIO, 0);    // receive
    GPIO_WritePin(RS485_1_TXEN_GPIO, 0);    // receive
    GPIO_WritePin(EEPROM_CS_GPIO, 1);       // EEPROM deselected

    // GPIO23 / GPIO41 (USB) left at reset default - not used yet.
}

//----------------------------------------------------------------------------
//  Gate-driver fault -> Input X-BAR (for EPWM trip zones)
//  INPUT1 -> TZ1 : VCESAT1 (bridge 1)
//  INPUT2 -> TZ2 : VCESAT2 (bridge 2)
//  INPUT4 is used by ZCD -> eCAP1.
//  Set TZ polarity in the EPWM trip-zone setup to match the driver fault
//  output (most drivers pull FAULT low).
//  Not called yet: the rectifier has no trip-zone setup for these inputs.
//----------------------------------------------------------------------------
void InitFaultXbar(void)
{
    GPIO_setQualificationMode(VCESAT1_GPIO, GPIO_QUAL_ASYNC);
    GPIO_setQualificationMode(VCESAT2_GPIO, GPIO_QUAL_ASYNC);
    XBAR_setInputPin(INPUTXBAR_BASE, XBAR_INPUT1, VCESAT1_GPIO);
    XBAR_setInputPin(INPUTXBAR_BASE, XBAR_INPUT2, VCESAT2_GPIO);
}

//----------------------------------------------------------------------------
//          Initialize SCI
//  SCIA -> RS485 #2 (U31), GPIO28 RX / GPIO29 TX, mux 1, TXEN GPIO34
//  SCIB -> RS485 #1 (U28), GPIO15 RX / GPIO14 TX, mux 2, TXEN GPIO40
//----------------------------------------------------------------------------

void InitSCIA(N16 Baud)
{
    // SCIA fixed at 115200 baud; the Baud argument (BaudRate parameter) is ignored.
    N16 baudCount = BAUD115200;
    (void)Baud;

    EALLOW;
        GpioCtrlRegs.GPAPUD.bit.GPIO28 = 0;    // pull-up on RXD
        GpioCtrlRegs.GPAPUD.bit.GPIO29 = 0;    // pull-up on TXD
        GpioCtrlRegs.GPAQSEL2.bit.GPIO28 = 3;  // async input

        GpioCtrlRegs.GPAGMUX2.bit.GPIO28 = (1 >> 2);
        GpioCtrlRegs.GPAMUX2.bit.GPIO28  = (1 & 0x03);   // SCIA_RX
        GpioCtrlRegs.GPAGMUX2.bit.GPIO29 = (1 >> 2);
        GpioCtrlRegs.GPAMUX2.bit.GPIO29  = (1 & 0x03);   // SCIA_TX
    EDIS;

    SciaRegs.SCICCR.all = 0x0007;           // 1 stop, no parity, 8 bits
    SciaRegs.SCICTL1.all = 0x0003;          // enable TX, RX, internal SCICLK
    SciaRegs.SCICTL2.all = 0x0003;
    SciaRegs.SCICTL2.bit.TXINTENA = 1;
    SciaRegs.SCICTL2.bit.RXBKINTENA = 1;
    SciaRegs.SCIHBAUD.all = baudCount >> 8;
    SciaRegs.SCILBAUD.all = baudCount & 0xff;
    SciaRegs.SCICTL1.all = 0x0023;          // relinquish SCI from reset
}

void InitSCIB(void)
{
    EALLOW;
        GpioCtrlRegs.GPAPUD.bit.GPIO15 = 0;    // pull-up on RXD1
        GpioCtrlRegs.GPAPUD.bit.GPIO14 = 0;    // pull-up on TXD1
        GpioCtrlRegs.GPAQSEL1.bit.GPIO15 = 3;  // async input

        GpioCtrlRegs.GPAGMUX1.bit.GPIO15 = (2 >> 2);
        GpioCtrlRegs.GPAMUX1.bit.GPIO15  = (2 & 0x03);   // SCIB_RX
        GpioCtrlRegs.GPAGMUX1.bit.GPIO14 = (2 >> 2);
        GpioCtrlRegs.GPAMUX1.bit.GPIO14  = (2 & 0x03);   // SCIB_TX
    EDIS;

    ScibRegs.SCICCR.all = 0x0007;
    ScibRegs.SCICTL1.all = 0x0003;
    ScibRegs.SCICTL2.all = 0x0003;
    ScibRegs.SCICTL2.bit.TXINTENA = 1;
    ScibRegs.SCICTL2.bit.RXBKINTENA = 1;
    ScibRegs.SCIHBAUD.all = 0x0000;
    ScibRegs.SCILBAUD.all = BAUD115200;
    ScibRegs.SCICTL1.all = 0x0023;
}

void scia_xmit(int a)
{
    while(!SciaRegs.SCICTL2.bit.TXEMPTY);
    SciaRegs.SCITXBUF.bit.TXDT = a;
    while(!SciaRegs.SCICTL2.bit.TXEMPTY);
}

void scia_msg(char * msg)
{
    int i = 0;
    SCIA_TX_ENABLE();
    while(msg[i] != '\0')
    {
        scia_xmit(msg[i]);
        i++;
    }
    SCIA_TX_DISABLE();      // TXEMPTY already waited for in scia_xmit
}

void scib_xmit(int a)
{
    while(!ScibRegs.SCICTL2.bit.TXEMPTY);
    ScibRegs.SCITXBUF.bit.TXDT = a;
    while(!ScibRegs.SCICTL2.bit.TXEMPTY);
}

void scib_msg(char * msg)
{
    int i = 0;
    SCIB_TX_ENABLE();
    while(msg[i] != '\0')
    {
        scib_xmit(msg[i]);
        i++;
    }
    SCIB_TX_DISABLE();
}

//----------------------------------------------------------------------------
//          ADC
//  External 3.0 V reference (TL431) on VREFHI for all five ADCs.
//----------------------------------------------------------------------------
#define ADC_RESOLUTION_12BIT 0
#define ADC_MODE_SINGLE_ENDED 0
int *adc_cal = 0;

void ConfigureADC(void)
{
    SetVREF(ADC_ADCA, ADC_EXTERNAL, ADC_REFERENCE_VREFHI);
    SetVREF(ADC_ADCB, ADC_EXTERNAL, ADC_REFERENCE_VREFHI);
    SetVREF(ADC_ADCC, ADC_EXTERNAL, ADC_REFERENCE_VREFHI);
    SetVREF(ADC_ADCD, ADC_EXTERNAL, ADC_REFERENCE_VREFHI);
    SetVREF(ADC_ADCE, ADC_EXTERNAL, ADC_REFERENCE_VREFHI);

    EALLOW;
    AdcaRegs.ADCCTL2.bit.PRESCALE = 6;      // ADCCLK = SYSCLK/4
    AdcaRegs.ADCCTL1.bit.INTPULSEPOS = 1;
    AdcaRegs.ADCCTL1.bit.ADCPWDNZ = 1;

    AdcbRegs.ADCCTL2.bit.PRESCALE = 6;
    AdcbRegs.ADCCTL1.bit.INTPULSEPOS = 1;
    AdcbRegs.ADCCTL1.bit.ADCPWDNZ = 1;

    AdccRegs.ADCCTL2.bit.PRESCALE = 6;
    AdccRegs.ADCCTL1.bit.INTPULSEPOS = 1;
    AdccRegs.ADCCTL1.bit.ADCPWDNZ = 1;

    AdcdRegs.ADCCTL2.bit.PRESCALE = 6;
    AdcdRegs.ADCCTL1.bit.INTPULSEPOS = 1;
    AdcdRegs.ADCCTL1.bit.ADCPWDNZ = 1;

    AdceRegs.ADCCTL2.bit.PRESCALE = 6;
    AdceRegs.ADCCTL1.bit.INTPULSEPOS = 1;
    AdceRegs.ADCCTL1.bit.ADCPWDNZ = 1;
    EDIS;

    DELAY_US(2000);         // ADC power-up

    // ADC trigger: EPWM7 SOCA at period (main: setupADC(ADC_TRIGGER_EPWM7_SOCA, ..)).
    // The rectifier uses ePWM1..6 for SCR firing, so the ePWM7 module stays the
    // ADC timebase (ePWM7_Init in pwm.c). Only its pins (GPIO12/13) are used
    // elsewhere on this board, as plain GPIO (STAT_USE_EPWM7 = 0).
    EALLOW;
    EPwm7Regs.ETSEL.bit.SOCASEL = ET_CTR_PRD;
    EPwm7Regs.ETPS.bit.SOCAPRD  = ET_1ST;
    EPwm7Regs.ETSEL.bit.SOCAEN  = 1;
    EDIS;
}

/*
 * SOC map of the common board (all five ADCs on the same trigger, so SOCn on
 * every ADC is sampled at the same instant; the three phases of each group
 * are on different ADCs):
 *
 *        SOC0        SOC1        SOC2        SOC3          SOC4
 *  A   Ir_ind A0   Ir_out A5   Ib_out A8   Vb_byp/Vld A14  Temp1 A9
 *  B   Vb_inv B3   Iy_out B8   Vy_byp B9   Vref1.5 B0      Temp3 B1
 *  C   Iy_ind C5   VdcLink C0  Vr_byp C9   Vbat C2         Temp2 C8
 *  D   Vr_inv D14  Vb_out D15  Vy_out D12  Ibat D11        -
 *  E   Ib_ind E11  Vy_inv E12  Vr_out E16  -               -
 *
 * 12-pulse rectifier assignment (NU = not used, SOC not configured):
 *
 *        SOC0          SOC1          SOC2          SOC3          SOC4
 *  A   Idc1   A0     Iinp_R A5     Iinp_B A8     Vload  A14    Temp1 A9
 *  B   Vinp_B B3     Iinp_Y B8     NU     B9     Vref1.5 B0    Temp3 B1
 *  C   Idc2   C5     Vdc    C0     NU     C9     Vbat   C2     Temp2 C8
 *  D   Vinp_R D14    NU     D15    NU     D12    Ibat   D11    -
 *  E   NU     E11    Vinp_Y E12    NU     E16    -             -
 *
 * Results are read in AdcISR (interrupt.c) into AnalogInput1..18 (labels in
 * RectParams.h). EOC: ADCA INT1 on EOC4 (ADC-A has the most conversions).
 */
#define SET_SOC(REGS, N, CH)                        \
    REGS.ADCSOC##N##CTL.bit.CHSEL   = (CH);         \
    REGS.ADCSOC##N##CTL.bit.ACQPS   = acqps;        \
    REGS.ADCSOC##N##CTL.bit.TRIGSEL = trig_sel

void setupADC(uint16_t trig_sel, uint16_t acqps)
{
    EALLOW;
    //---------- ADC-A ----------------------------------------------------
    SET_SOC(AdcaRegs, 0, IR_IND_ADC_CH);        // A0   Idc1   (bridge 1)
    SET_SOC(AdcaRegs, 1, IR_OUT_ADC_CH);        // A5   Iinp_R
    SET_SOC(AdcaRegs, 2, IB_OUT_ADC_CH);        // A8   Iinp_B
    SET_SOC(AdcaRegs, 3, VB_BYP_VLOAD_ADC_CH);  // A14  Vload  (selected by a link on the board, not by RLY11/RLY12)
    SET_SOC(AdcaRegs, 4, TEMP1_ADC_CH);         // A9   Temp1

    //---------- ADC-B ----------------------------------------------------
    SET_SOC(AdcbRegs, 0, VB_INV_ADC_CH);        // B3   Vinp_B
    SET_SOC(AdcbRegs, 1, IY_OUT_ADC_CH);        // B8   Iinp_Y
    SET_SOC(AdcbRegs, 3, VREF1P5_ADC_CH);       // B0   Vref1.5
    SET_SOC(AdcbRegs, 4, TEMP3_ADC_CH);         // B1   Temp3

    //---------- ADC-C ----------------------------------------------------
    SET_SOC(AdccRegs, 0, IY_IND_ADC_CH);        // C5   Idc2   (bridge 2)
    SET_SOC(AdccRegs, 1, VDCLINK_ADC_CH);       // C0   Vdc
    SET_SOC(AdccRegs, 3, VBAT_ADC_CH);          // C2   Vbat
    SET_SOC(AdccRegs, 4, TEMP2_ADC_CH);         // C8   Temp2

    //---------- ADC-D ----------------------------------------------------
    SET_SOC(AdcdRegs, 0, VR_INV_ADC_CH);        // D14  Vinp_R
    SET_SOC(AdcdRegs, 3, IBAT_ADC_CH);          // D11  Ibat

    //---------- ADC-E ----------------------------------------------------
    SET_SOC(AdceRegs, 1, VY_INV_ADC_CH);        // E12  Vinp_Y

    //---------- End of conversion interrupt --------------------------------
    AdcaRegs.ADCINTSEL1N2.bit.INT1SEL = 4;      // EOC4 on ADC-A
    AdcaRegs.ADCINTSEL1N2.bit.INT1E   = 1;
    AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;
    EDIS;
}

//----------------------------------------------------------------------------
//          SPI-A -> 25LC512 EEPROM
//  GPIO16 SIMO, GPIO17 SOMI, GPIO56 CLK (mux 1), GPIO57 /CS as GPIO
//----------------------------------------------------------------------------
void spi_init(void)
{
    EALLOW;
        GpioCtrlRegs.GPAPUD.bit.GPIO16 = 0;     // SIMO
        GpioCtrlRegs.GPAPUD.bit.GPIO17 = 0;     // SOMI
        GpioCtrlRegs.GPBPUD.bit.GPIO56 = 0;     // CLK

        GpioCtrlRegs.GPAQSEL2.bit.GPIO16 = 3;   // async
        GpioCtrlRegs.GPAQSEL2.bit.GPIO17 = 3;
        GpioCtrlRegs.GPBQSEL2.bit.GPIO56 = 3;

        GpioCtrlRegs.GPAGMUX2.bit.GPIO16 = (1 >> 2);
        GpioCtrlRegs.GPAMUX2.bit.GPIO16  = (1 & 0x03);  // SPIA_SIMO
        GpioCtrlRegs.GPAGMUX2.bit.GPIO17 = (1 >> 2);
        GpioCtrlRegs.GPAMUX2.bit.GPIO17  = (1 & 0x03);  // SPIA_SOMI
        GpioCtrlRegs.GPBGMUX2.bit.GPIO56 = (1 >> 2);
        GpioCtrlRegs.GPBMUX2.bit.GPIO56  = (1 & 0x03);  // SPIA_CLK
    EDIS;

    SpiaRegs.SPIFFTX.all = 0xE040;
    SpiaRegs.SPIFFRX.all = 0x204f;
    SpiaRegs.SPIFFCT.all = 0x0;

    SpiaRegs.SPICCR.all = 0x47;     // reset on, rising edge, 8-bit
    SpiaRegs.SPICTL.all = 0x06;     // master, normal phase
    SpiaRegs.SPIBRR.all = 9;

    SpiaRegs.SPICCR.all = 0x0C7;    // relinquish from reset
    DELAY_US(100);
    DISABLE_SPI();                  // /CS (GPIO57) high
}

//----------------------------------------------------------------------------
//          MCAN pins
//  MCANA : GPIO30 RX / GPIO31 TX, mux 10 (GMUX = 2, MUX = 2)  -> U32/U34
//  MCANB : GPIO18 RX / GPIO19 TX, mux 3  (GMUX = 0, MUX = 3)  -> X1/X2 pins
//  Module setup (clock, bit timing, message RAM) is done in the MCAN driver
//  (InitMCAN in mcanInterface.c).
//  NOTE: GPIO18/19 are the X1/X2 crystal pins - the board has no crystal,
//  the PLL runs from INTOSC2 (USE_PLL_SRC_INTOSC in f28p55x_sysctrl.c/device.h).
//----------------------------------------------------------------------------
void InitMCANPins(void)
{
    EALLOW;
    // MCANA
    GpioCtrlRegs.GPAPUD.bit.GPIO30   = 0;
    GpioCtrlRegs.GPAQSEL2.bit.GPIO30 = 3;           // async
    GpioCtrlRegs.GPAGMUX2.bit.GPIO30 = (10 >> 2);
    GpioCtrlRegs.GPAMUX2.bit.GPIO30  = (10 & 0x03); // MCANA_RX
    GpioCtrlRegs.GPAPUD.bit.GPIO31   = 0;
    GpioCtrlRegs.GPAGMUX2.bit.GPIO31 = (10 >> 2);
    GpioCtrlRegs.GPAMUX2.bit.GPIO31  = (10 & 0x03); // MCANA_TX

    // MCANB
    GpioCtrlRegs.GPAPUD.bit.GPIO18   = 0;
    GpioCtrlRegs.GPAQSEL2.bit.GPIO18 = 3;           // async
    GpioCtrlRegs.GPAGMUX2.bit.GPIO18 = (3 >> 2);
    GpioCtrlRegs.GPAMUX2.bit.GPIO18  = (3 & 0x03);  // MCANB_RX
    GpioCtrlRegs.GPAPUD.bit.GPIO19   = 0;
    GpioCtrlRegs.GPAGMUX2.bit.GPIO19 = (3 >> 2);
    GpioCtrlRegs.GPAMUX2.bit.GPIO19  = (3 & 0x03);  // MCANB_TX
    EDIS;
}

// Kept for the existing callers in main.c (start-up and the 30 s re-init).
void    InitECAN(void)
{
    InitMCANPins();
}

//----------------------------------------------------------------------------
//  ZCD_R (GPIO47) -> Input X-BAR INPUT4 -> eCAP1
//----------------------------------------------------------------------------
void initZCD_ECAP1_GPIO47(void)
{
    // 1) GPIO47 as plain input, async qualification (required for eCAP)
    GPIO_setPinConfig(GPIO_47_GPIO47);
    GPIO_setDirectionMode(ZCD_R_GPIO, GPIO_DIR_MODE_IN);
    GPIO_setPadConfig(ZCD_R_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(ZCD_R_GPIO, GPIO_QUAL_ASYNC);

    // 2) Route GPIO47 -> Input X-BAR INPUT4 -> eCAP1
    XBAR_setInputPin(INPUTXBAR_BASE, XBAR_INPUT4, ZCD_R_GPIO);
    ECAP_selectECAPInput(ECAP1_BASE, ECAP_INPUT_INPUTXBAR4);

    // 3) eCAP config
    EALLOW;
    ECap1Regs.ECCTL1.all = 0;
    ECap1Regs.ECCTL2.all = 0;

    ECap1Regs.ECCTL1.bit.CAPLDEN   = 1;   // load capture regs on event
    ECap1Regs.ECCTL1.bit.PRESCALE  = 0;   // /1, capture every edge
    ECap1Regs.ECCTL1.bit.CAP1POL   = 0;   // event1 = rising
    ECap1Regs.ECCTL1.bit.CAP2POL   = 1;   // event2 = falling
    ECap1Regs.ECCTL1.bit.CTRRST1   = 0;   // free-running counter
    ECap1Regs.ECCTL1.bit.CTRRST2   = 0;
    ECap1Regs.ECCTL1.bit.CTRRST3   = 0;
    ECap1Regs.ECCTL1.bit.CTRRST4   = 0;

    ECap1Regs.ECCTL2.bit.CAP_APWM    = 0;  // capture mode
    ECap1Regs.ECCTL2.bit.CONT_ONESHT = 0;  // continuous
    ECap1Regs.ECCTL2.bit.STOP_WRAP   = 1;  // wrap after event2
    ECap1Regs.ECCTL2.bit.SYNCI_EN    = 0;

    ECap1Regs.TSCTR = 0;
    ECap1Regs.ECCTL2.bit.TSCTRSTOP = 1;    // start counter

    ECap1Regs.ECCLR.all = 0xFFFF;
    ECap1Regs.ECEINT.all = 0;
    ECap1Regs.ECEINT.bit.CEVT1 = 1;        // interrupt on rising edge
    EDIS;

    ECAP_clearInterrupt(ECAP1_BASE, ECAP_ISR_SOURCE_CAPTURE_EVENT_1);
}
