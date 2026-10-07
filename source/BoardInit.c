/*
 * BoardInit.c
 *
 *  Created on: 24-Oct-2023
 *      Author: Vijayan
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

#define CARD_ID         1021
#define SWVERSION_SET   100

#define     SET3XINT(X)    \
    GpioCtrlRegs.GPBPUD.bit.X      = 0;    \
    GpioCtrlRegs.GPBMUX1.bit.X     = 0;    \
    GpioCtrlRegs.GPBDIR.bit.X      = 0;    \
    GpioCtrlRegs.GPBQSEL1.bit.X   = 2

#define     SET3XINT2(X)    \
    GpioCtrlRegs.GPBPUD.bit.X      = 0;    \
    GpioCtrlRegs.GPBMUX2.bit.X     = 0;    \
    GpioCtrlRegs.GPBDIR.bit.X      = 0;    \
    GpioCtrlRegs.GPBQSEL2.bit.X   = 2

#define     SET3XINT2L(X)    \
    GpioCtrlRegs.GPBPUD.bit.X      = 0;    \
    GpioCtrlRegs.GPBMUX2.bit.X     = 0;    \
    GpioCtrlRegs.GPBDIR.bit.X      = 0;    \
    GpioCtrlRegs.GPBQSEL2.bit.X   = 3

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
---------------------------------------------------------------------------*/

void GpioInit(void)
{
    EALLOW;

    //-------------- PWM Out---------------------------
    GpioCtrlRegs.GPAPUD.bit.GPIO0       = 1;    // Disable pull-up on GPIO0 (EPWM1A)
    GpioCtrlRegs.GPAPUD.bit.GPIO1       = 1;    // Disable pull-up on GPIO1 (EPWM1B)
    GpioCtrlRegs.GPAMUX1.bit.GPIO0      = 1;    // Configure GPIO0 as EPWM1A
    GpioCtrlRegs.GPAMUX1.bit.GPIO1      = 1;    // Configure GPIO1 as EPWM1B

    GpioCtrlRegs.GPAPUD.bit.GPIO2       = 1;    // Disable pull-up on GPIO2
    GpioCtrlRegs.GPAPUD.bit.GPIO3       = 1;    // Disable pull-up on GPIO3
    GpioCtrlRegs.GPAMUX1.bit.GPIO2      = 1;    // Configure GPIO2 as EPWM2A
    GpioCtrlRegs.GPAMUX1.bit.GPIO3      = 1;    // Configure GPIO3 as EPWM2B

    GpioCtrlRegs.GPAPUD.bit.GPIO4       = 1;    // Disable pull-up on GPIO4
    GpioCtrlRegs.GPAPUD.bit.GPIO5       = 1;    // Disable pull-up on GPIO5
    GpioCtrlRegs.GPAMUX1.bit.GPIO4      = 1;    // Configure GPIO4 as EPWM3A
    GpioCtrlRegs.GPAMUX1.bit.GPIO5      = 1;    // Configure GPIO5 as EPWM3B

    GpioCtrlRegs.GPAPUD.bit.GPIO6       = 1;    // Disable pull-up on GPIO4
    GpioCtrlRegs.GPAPUD.bit.GPIO7       = 1;    // Disable pull-up on GPIO5
    GpioCtrlRegs.GPAMUX1.bit.GPIO6      = 1;    // Configure GPIO4 as EPWM4A
    GpioCtrlRegs.GPAMUX1.bit.GPIO7      = 1;    // Configure GPIO5 as EPWM4B

    GpioCtrlRegs.GPAPUD.bit.GPIO8       = 1;    // Disable pull-up on GPIO8
    GpioCtrlRegs.GPAPUD.bit.GPIO9       = 1;    // Disable pull-up on GPIO9
    GpioCtrlRegs.GPAMUX1.bit.GPIO8      = 1;    // Configure GPIO4 as EPWM5A
    GpioCtrlRegs.GPAMUX1.bit.GPIO9      = 1;    // Configure GPIO5 as EPWM5B

    GpioCtrlRegs.GPAPUD.bit.GPIO10      = 1;    // Disable pull-up on GPIO8
    GpioCtrlRegs.GPAPUD.bit.GPIO11      = 1;    // Disable pull-up on GPIO9
    GpioCtrlRegs.GPAMUX1.bit.GPIO10     = 1;    // Configure GPIO4 as EPWM6A
    GpioCtrlRegs.GPAMUX1.bit.GPIO11     = 1;    // Configure GPIO5 as EPWM6B

    GpioCtrlRegs.GPBPUD.bit.GPIO47      = 0;   // ZCDR
    GpioCtrlRegs.GPBMUX1.bit.GPIO47     = 0;   //
    GpioCtrlRegs.GPBDIR.bit.GPIO47      = 0;   //
    GpioCtrlRegs.GPBQSEL1.bit.GPIO47    = 2;   //

    GpioCtrlRegs.GPBPUD.bit.GPIO48      = 0;    // ZCDY
    GpioCtrlRegs.GPBMUX2.bit.GPIO48     = 0;   //
    GpioCtrlRegs.GPBDIR.bit.GPIO48      = 0;    //
    GpioCtrlRegs.GPBQSEL2.bit.GPIO48    = 2;     //

    GpioCtrlRegs.GPBPUD.bit.GPIO49      = 0;    // ZCDB
    GpioCtrlRegs.GPBMUX2.bit.GPIO49     = 0;   //
    GpioCtrlRegs.GPBDIR.bit.GPIO49      = 0;    //
    GpioCtrlRegs.GPBQSEL2.bit.GPIO49    = 2;     //

    GPIO_setQualificationPeriod(47, 510);        // Set sampling period
    GPIO_setQualificationMode(47, GPIO_QUAL_6SAMPLE);
    GPIO_setQualificationPeriod(48, 510);        // Set sampling period
    GPIO_setQualificationMode(48, GPIO_QUAL_6SAMPLE);
    GPIO_setQualificationPeriod(49, 510);        // Set sampling period
    GPIO_setQualificationMode(49, GPIO_QUAL_6SAMPLE);

#define GPIO_13_MCAN_TX                 0x00061A03U
#define GPIO_12_MCAN_RX                 0x00061803U

EALLOW;
        GpioCtrlRegs.GPAGMUX1.bit.GPIO12 = (3 >> 2);
        GpioCtrlRegs.GPAMUX1.bit.GPIO12 =  (3 & 0x03);
        GpioCtrlRegs.GPAGMUX1.bit.GPIO13 = (3 >> 2);;
        GpioCtrlRegs.GPAMUX1.bit.GPIO13 =  (3 & 0x03);
        GPIO_SetupPinOptions(12, GPIO_INPUT, GPIO_ASYNC); //RX
        GPIO_SetupPinOptions(13, GPIO_OUTPUT, GPIO_PUSHPULL); //TX

        GPIO_setPinConfig(GPIO_227_GPIO227);
        GPIO_setAnalogMode(227U, GPIO_ANALOG_DISABLED);
        GPIO_setPinConfig(GPIO_236_GPIO236);
        GPIO_setAnalogMode(236U, GPIO_ANALOG_DISABLED);
        GPIO_setPinConfig(GPIO_230_GPIO230);
        GPIO_setAnalogMode(230U, GPIO_ANALOG_DISABLED);
        GPIO_setPinConfig(GPIO_253_GPIO253);
        GPIO_setAnalogMode(253U, GPIO_ANALOG_DISABLED);
        GPIO_setPinConfig(GPIO_247_GPIO247);
        GPIO_setAnalogMode(247U, GPIO_ANALOG_DISABLED);
        GPIO_setPinConfig(GPIO_20_GPIO20);
        GPIO_setAnalogMode(20U, GPIO_ANALOG_DISABLED);
        GPIO_setPinConfig(GPIO_21_GPIO21);
        GPIO_setAnalogMode(21U, GPIO_ANALOG_DISABLED);

        GPIO_setPinConfig(GPIO_224_GPIO224);     // AGPIO -> Analog mode selected
        GPIO_setAnalogMode(224, GPIO_ANALOG_ENABLED);// Analog PinMux for A2/C9, GPIO224
        GPIO_setPinConfig(GPIO_242_GPIO242);     // AGPIO -> Analog mode selected
        GPIO_setAnalogMode(242, GPIO_ANALOG_ENABLED);     // Analog PinMux for A4/C14
        GPIO_setPinConfig(GPIO_226_GPIO226);    // Analog PinMux for C6, GPIO226
        GPIO_setAnalogMode(226, GPIO_ANALOG_ENABLED);// AGPIO -> Analog mode selected
        GPIO_setPinConfig(GPIO_228_GPIO228);// Analog PinMux for A6, GPIO228
        GPIO_setAnalogMode(228, GPIO_ANALOG_ENABLED);// AGPIO -> Analog mode selected
EDIS;



//------------NORMAL INPUTS--------------------------------------------
    SET_DI(50);
    SET_DI(51);
    SET_DI(52);
    SET_DI(53);
    SET_DI(54);
    SET_DI(55);
    SET_DI(20);
    SET_DI(21);
    SET_DI(227);
    SET_DI(236);
    SET_DI(230);
    SET_DI(253);
    SET_DI(247);


    //---------------OUTPUTS----------------------------------------------------
    SET_DO(60); //RELAY1
    SET_DO(62);
    SET_DO(33);
    SET_DO(24);
    SET_DO(25);
    SET_DO(32);
    SET_DO(58);
    SET_DO(44);
    SET_DO(61);
    SET_DO(59);
    SET_DO(15);
    SET_DO(14);
    SET_DO(26);
    SET_DO(27);

    SET_DO(41);
    SET_DO(34);
    SET_DO(57);
    SET_DO(40);

    GPIO_WritePin(40,1);//RTC
    GPIO_WritePin(57,1);//EEPROM


}
//----------------------------------------------------------------------------
//          Initialize SCI
//----------------------------------------------------------------------------

void InitSCIA(N16 Baud)
{
    // SCIA fixed at 115200 baud; the Baud argument (BaudRate parameter) is ignored.
    N16 baudCount = BAUD115200;
    (void)Baud;
//-------------------------------------------------------------------------------------------------------

        EALLOW;

            GpioCtrlRegs.GPAPUD.bit.GPIO28 = 0;    // Enable pull-up for GPIO28 (SCIRXDA)
            GpioCtrlRegs.GPAPUD.bit.GPIO29 = 0;    // Enable pull-up for GPIO29 (SCITXDA)
            GpioCtrlRegs.GPAQSEL2.bit.GPIO28 = 3;  // Asynch input GPIO28 (SCIRXDA)

            GpioCtrlRegs.GPAGMUX2.bit.GPIO28 = (1 >>2);
            GpioCtrlRegs.GPAMUX2.bit.GPIO28 =  (1 & 0x003);   // Configure GPIO28 for SCIRXDA operation
            GpioCtrlRegs.GPAGMUX2.bit.GPIO29 = (1 >>2);
            GpioCtrlRegs.GPAMUX2.bit.GPIO29 =  (1 & 0x003);   // Configure GPIO29 for SCITXDA operation

        EDIS;

    SciaRegs.SCICCR.all = 0x0007;           // 1 stop bit,  No loopback
                                            // No parity, 8 char bits,
                                            // async mode, idle-line protocol
    SciaRegs.SCICTL1.all = 0x0003;          // enable TX, RX, internal SCICLK,
                                            // Disable RX ERR, SLEEP, TXWAKE
    SciaRegs.SCICTL2.all = 0x0003;
    SciaRegs.SCICTL2.bit.TXINTENA = 1;
    SciaRegs.SCICTL2.bit.RXBKINTENA = 1;
    //
    // SCIA at 115200 baud (BAUD115200 = 40 -> HBAUD = 0x00, LBAUD = 0x28)
    //
    SciaRegs.SCIHBAUD.all = baudCount >> 8;
    SciaRegs.SCILBAUD.all = baudCount & 0xff;

    SciaRegs.SCICTL1.all = 0x0023;          // Relinquish SCI from Reset


}

void InitSCIB(void)
{
#define GPIO_15_SCIB_RX                 0x00061E02U
#define GPIO_14_SCIB_TX                 0x00061C02U

    EALLOW;
        GpioCtrlRegs.GPAPUD.bit.GPIO15 = 0;    // Enable pull-up for GPIO23 (SCIRXDA)
        GpioCtrlRegs.GPAPUD.bit.GPIO14 = 0;    // Enable pull-up for GPIO22 (SCITXDA)
        GpioCtrlRegs.GPAQSEL1.bit.GPIO15 = 3;  // Asynch input GPIO23 (SCIRXDA)

        GpioCtrlRegs.GPAGMUX1.bit.GPIO15 = (2 >>2);
        GpioCtrlRegs.GPAMUX1.bit.GPIO15 =  (2 & 0x003);   // Configure GPIO23 for SCIRXDA operation
        GpioCtrlRegs.GPAGMUX1.bit.GPIO14 = (2 >>2);
        GpioCtrlRegs.GPAMUX1.bit.GPIO14 =  (2 & 0x003);   // Configure GPIO22 for SCITXDA operation
    EDIS;

    ScibRegs.SCICCR.all = 0x0007;           // 1 stop bit,  No loopback
                                            // No parity, 8 char bits,
                                            // async mode, idle-line protocol
    ScibRegs.SCICTL1.all = 0x0003;          // enable TX, RX, internal SCICLK,
                                            // Disable RX ERR, SLEEP, TXWAKE
    ScibRegs.SCICTL2.all = 0x0003;
    ScibRegs.SCICTL2.bit.TXINTENA = 1;
    ScibRegs.SCICTL2.bit.RXBKINTENA = 1;
    ScibRegs.SCIHBAUD.all = 0x0000;
    ScibRegs.SCILBAUD.all = BAUD115200;

    ScibRegs.SCICTL1.all = 0x0023;          // Relinquish SCI from Reset

}

void scia_xmit(int a)
{
    while(!SciaRegs.SCICTL2.bit.TXEMPTY);
    SciaRegs.SCITXBUF.bit.TXDT = a;
    while(!SciaRegs.SCICTL2.bit.TXEMPTY);

}

void scia_msg(char * msg)
{
    int i;
    i = 0;
    EnableTX();
    while(msg[i] != '\0')
    {
        scia_xmit(msg[i]);
        i++;
    }
    DisableTX();
}

void scib_xmit(int a)
{
    while(!ScibRegs.SCICTL2.bit.TXEMPTY);
    ScibRegs.SCITXBUF.bit.TXDT = a;
    while(!ScibRegs.SCICTL2.bit.TXEMPTY);

}

void scib_msg(char * msg)
{
    int i;
    i = 0;
    EnableTX();
    while(msg[i] != '\0')
    {
        scib_xmit(msg[i]);
        i++;
    }
    DisableTX();
}

#define ADC_RESOLUTION_12BIT 0
#define ADC_MODE_SINGLE_ENDED 0
int *adc_cal = 0;

void ConfigureADC(void)
{
        //Write ADC configurations and power up the ADC for both ADC A

        SetVREF(ADC_ADCA, ADC_EXTERNAL, ADC_REFERENCE_VREFHI);
        SetVREF(ADC_ADCB, ADC_EXTERNAL, ADC_REFERENCE_VREFHI);
        SetVREF(ADC_ADCC, ADC_EXTERNAL, ADC_REFERENCE_VREFHI);
        SetVREF(ADC_ADCD, ADC_EXTERNAL, ADC_REFERENCE_VREFHI);
        SetVREF(ADC_ADCE, ADC_EXTERNAL, ADC_REFERENCE_VREFHI);

        EALLOW;
        AdcaRegs.ADCCTL2.bit.PRESCALE = 6; //set ADCCLK divider to /4
        AdcaRegs.ADCCTL1.bit.INTPULSEPOS = 1;
        AdcaRegs.ADCCTL1.bit.ADCPWDNZ = 1;

        AdcbRegs.ADCCTL2.bit.PRESCALE = 6; //set ADCCLK divider to /4
        AdcbRegs.ADCCTL1.bit.INTPULSEPOS = 1;
        AdcbRegs.ADCCTL1.bit.ADCPWDNZ = 1;

        AdccRegs.ADCCTL2.bit.PRESCALE = 6; //set ADCCLK divider to /4
        AdccRegs.ADCCTL1.bit.INTPULSEPOS = 1;
        AdccRegs.ADCCTL1.bit.ADCPWDNZ = 1;

        AdcdRegs.ADCCTL2.bit.PRESCALE = 6; //set ADCCLK divider to /4
        AdcdRegs.ADCCTL1.bit.INTPULSEPOS = 1;
        AdcdRegs.ADCCTL1.bit.ADCPWDNZ = 1;

        AdceRegs.ADCCTL2.bit.PRESCALE = 6; //set ADCCLK divider to /4
        AdceRegs.ADCCTL1.bit.INTPULSEPOS = 1;
        AdceRegs.ADCCTL1.bit.ADCPWDNZ = 1;

        EDIS;

        DELAY_US(1000);
        DELAY_US(1000);

    // Setting up link from EPWM to ADC
        EPwm7Regs.ETSEL.bit.SOCASEL = ET_CTR_PRD; // Select SOC from counter at ctr = 0
        EPwm7Regs.ETPS.bit.SOCAPRD  = ET_1ST;     // Generate pulse on 1st even
        EPwm7Regs.ETSEL.bit.SOCAEN  = 1;

        EDIS;
}


void setupADC(uint16_t trig_sel, uint16_t acqps)
          {
              EALLOW;
/*
//A
0-2 Ir
1-3 Iy
2-1 Idc2
3-5 Temp2
4-8 Vref1.5
//B

0-2 Vy
1-3 Vb
2-10 Vdc
3-8 Ref

//C
0-4 Ib
1-2 Idc
2-5 Ibat
3-3 Iref

//D
0-14 Vr
1-15 Temp1

//E
0-11 Vbat
1-16 Vref
*/
                  AdcaRegs.ADCSOC0CTL.bit.CHSEL = 2;
                  AdcaRegs.ADCSOC0CTL.bit.ACQPS = acqps;
                  AdcaRegs.ADCSOC0CTL.bit.TRIGSEL = trig_sel;

                  AdcaRegs.ADCSOC1CTL.bit.CHSEL = 3;
                  AdcaRegs.ADCSOC1CTL.bit.ACQPS = acqps;
                  AdcaRegs.ADCSOC1CTL.bit.TRIGSEL = trig_sel;

                  AdcaRegs.ADCSOC2CTL.bit.CHSEL = 1;
                  AdcaRegs.ADCSOC2CTL.bit.ACQPS = acqps;
                  AdcaRegs.ADCSOC2CTL.bit.TRIGSEL = trig_sel;

                  AdcaRegs.ADCSOC3CTL.bit.CHSEL = 5;
                  AdcaRegs.ADCSOC3CTL.bit.ACQPS = acqps;
                  AdcaRegs.ADCSOC3CTL.bit.TRIGSEL = trig_sel;

                  AdcaRegs.ADCSOC4CTL.bit.CHSEL = 8;
                  AdcaRegs.ADCSOC4CTL.bit.ACQPS = acqps;
                  AdcaRegs.ADCSOC4CTL.bit.TRIGSEL = trig_sel;

//----------ADC-B-------------------------------------------

                  AdcbRegs.ADCSOC0CTL.bit.CHSEL = 2;
                  AdcbRegs.ADCSOC0CTL.bit.ACQPS = acqps;
                  AdcbRegs.ADCSOC0CTL.bit.TRIGSEL = trig_sel;

                  AdcbRegs.ADCSOC1CTL.bit.CHSEL = 3;
                  AdcbRegs.ADCSOC1CTL.bit.ACQPS = acqps;
                  AdcbRegs.ADCSOC1CTL.bit.TRIGSEL = trig_sel;

                  AdcbRegs.ADCSOC2CTL.bit.CHSEL = 10;
                  AdcbRegs.ADCSOC2CTL.bit.ACQPS = acqps;
                  AdcbRegs.ADCSOC2CTL.bit.TRIGSEL = trig_sel;

                  AdcbRegs.ADCSOC3CTL.bit.CHSEL = 8;
                  AdcbRegs.ADCSOC3CTL.bit.ACQPS = acqps;
                  AdcbRegs.ADCSOC3CTL.bit.TRIGSEL = trig_sel;

//----ADC-C-----------------------------------------------------

                  AdccRegs.ADCSOC0CTL.bit.CHSEL = 4;    //A5
                  AdccRegs.ADCSOC0CTL.bit.ACQPS = acqps;
                  AdccRegs.ADCSOC0CTL.bit.TRIGSEL = trig_sel;

                  AdccRegs.ADCSOC1CTL.bit.CHSEL = 2;
                  AdccRegs.ADCSOC1CTL.bit.ACQPS = acqps;
                  AdccRegs.ADCSOC1CTL.bit.TRIGSEL = trig_sel;

                  AdccRegs.ADCSOC2CTL.bit.CHSEL = 5;
                  AdccRegs.ADCSOC2CTL.bit.ACQPS = acqps;
                  AdccRegs.ADCSOC2CTL.bit.TRIGSEL = trig_sel;

                  AdccRegs.ADCSOC3CTL.bit.CHSEL = 3;
                  AdccRegs.ADCSOC3CTL.bit.ACQPS = acqps;
                  AdccRegs.ADCSOC3CTL.bit.TRIGSEL = trig_sel;
//D ADC-----------------------------------------------------------------------
                  AdcdRegs.ADCSOC0CTL.bit.CHSEL = 14;    //A5
                  AdcdRegs.ADCSOC0CTL.bit.ACQPS = acqps;
                  AdcdRegs.ADCSOC0CTL.bit.TRIGSEL = trig_sel;

                  AdcdRegs.ADCSOC1CTL.bit.CHSEL = 15;
                  AdcdRegs.ADCSOC1CTL.bit.ACQPS = acqps;
                  AdcdRegs.ADCSOC1CTL.bit.TRIGSEL = trig_sel;
//E ADC -----------------------------------------------------------------------
                  AdceRegs.ADCSOC0CTL.bit.CHSEL = 11;    //A5
                  AdceRegs.ADCSOC0CTL.bit.ACQPS = acqps;
                  AdceRegs.ADCSOC0CTL.bit.TRIGSEL = trig_sel;

                  AdceRegs.ADCSOC1CTL.bit.CHSEL = 16;
                  AdceRegs.ADCSOC1CTL.bit.ACQPS = acqps;
                  AdceRegs.ADCSOC1CTL.bit.TRIGSEL = trig_sel;
//-----------------------------------------------------------------------
                  AdcaRegs.ADCINTSEL1N2.bit.INT1E = 1;
                  AdcaRegs.ADCINTSEL1N2.bit.INT1SEL = 4;
                  EDIS;
    }

//TODO setupADCConversion()
void setupADC0(uint16_t trig_sel, uint16_t acqps)
          {
              EALLOW;
/*
A
6 Vr
2 Ir
3 Iy
14 Ib
1 Idc2
5 Temp2
8 Vref1.5
  B
  2 Vy
  3 Vb
  10 Vdc
  12 Idc
  11 Vref
  5 Temp1
  8 Ref

  5 Ibat
  1 Vbat
  3 Iref


*/
                  AdcaRegs.ADCSOC0CTL.bit.CHSEL = 6;
                  AdcaRegs.ADCSOC0CTL.bit.ACQPS = acqps;
                  AdcaRegs.ADCSOC0CTL.bit.TRIGSEL = trig_sel;

                  AdcaRegs.ADCSOC1CTL.bit.CHSEL = 2;
                  AdcaRegs.ADCSOC1CTL.bit.ACQPS = acqps;
                  AdcaRegs.ADCSOC1CTL.bit.TRIGSEL = trig_sel;

                  AdcaRegs.ADCSOC2CTL.bit.CHSEL = 3;
                  AdcaRegs.ADCSOC2CTL.bit.ACQPS = acqps;
                  AdcaRegs.ADCSOC2CTL.bit.TRIGSEL = trig_sel;

                  AdcaRegs.ADCSOC3CTL.bit.CHSEL = 14;
                  AdcaRegs.ADCSOC3CTL.bit.ACQPS = acqps;
                  AdcaRegs.ADCSOC3CTL.bit.TRIGSEL = trig_sel;

                  AdcaRegs.ADCSOC4CTL.bit.CHSEL = 1;
                  AdcaRegs.ADCSOC4CTL.bit.ACQPS = acqps;
                  AdcaRegs.ADCSOC4CTL.bit.TRIGSEL = trig_sel;

                  AdcaRegs.ADCSOC5CTL.bit.CHSEL = 5;
                  AdcaRegs.ADCSOC5CTL.bit.ACQPS = acqps;
                  AdcaRegs.ADCSOC5CTL.bit.TRIGSEL = trig_sel;

                  AdcaRegs.ADCSOC6CTL.bit.CHSEL = 8;
                  AdcaRegs.ADCSOC6CTL.bit.ACQPS = acqps;
                  AdcaRegs.ADCSOC6CTL.bit.TRIGSEL = trig_sel;

//----------ADC-B-------------------------------------------

                  AdcbRegs.ADCSOC0CTL.bit.CHSEL = 2;
                  AdcbRegs.ADCSOC0CTL.bit.ACQPS = acqps;
                  AdcbRegs.ADCSOC0CTL.bit.TRIGSEL = trig_sel;

                  AdcbRegs.ADCSOC1CTL.bit.CHSEL = 3;
                  AdcbRegs.ADCSOC1CTL.bit.ACQPS = acqps;
                  AdcbRegs.ADCSOC1CTL.bit.TRIGSEL = trig_sel;

                  AdcbRegs.ADCSOC2CTL.bit.CHSEL = 10;
                  AdcbRegs.ADCSOC2CTL.bit.ACQPS = acqps;
                  AdcbRegs.ADCSOC2CTL.bit.TRIGSEL = trig_sel;

                  AdcbRegs.ADCSOC3CTL.bit.CHSEL = 12;
                  AdcbRegs.ADCSOC3CTL.bit.ACQPS = acqps;
                  AdcbRegs.ADCSOC3CTL.bit.TRIGSEL = trig_sel;

                  AdcbRegs.ADCSOC4CTL.bit.CHSEL = 11;
                  AdcbRegs.ADCSOC4CTL.bit.ACQPS = acqps;
                  AdcbRegs.ADCSOC4CTL.bit.TRIGSEL = trig_sel;

                  AdcbRegs.ADCSOC5CTL.bit.CHSEL = 5;
                  AdcbRegs.ADCSOC5CTL.bit.ACQPS = acqps;
                  AdcbRegs.ADCSOC5CTL.bit.TRIGSEL = trig_sel;

                  AdcbRegs.ADCSOC6CTL.bit.CHSEL = 8;
                  AdcbRegs.ADCSOC6CTL.bit.ACQPS = acqps;
                  AdcbRegs.ADCSOC6CTL.bit.TRIGSEL = trig_sel;

              //----ADC-C------------------

                  AdccRegs.ADCSOC0CTL.bit.CHSEL = 5;    //A5
                  AdccRegs.ADCSOC0CTL.bit.ACQPS = acqps;
                  AdccRegs.ADCSOC0CTL.bit.TRIGSEL = trig_sel;

                  AdccRegs.ADCSOC1CTL.bit.CHSEL = 1;
                  AdccRegs.ADCSOC1CTL.bit.ACQPS = acqps;
                  AdccRegs.ADCSOC1CTL.bit.TRIGSEL = trig_sel;

                  AdccRegs.ADCSOC2CTL.bit.CHSEL = 3;
                  AdccRegs.ADCSOC2CTL.bit.ACQPS = acqps;
                  AdccRegs.ADCSOC2CTL.bit.TRIGSEL = trig_sel;

                  AdcaRegs.ADCINTSEL1N2.bit.INT1E = 1;
                  AdcaRegs.ADCINTSEL1N2.bit.INT1SEL = 6;
                  EDIS;
    }



#define DEVICE_GPIO_PIN_SPISIMOA    16U  // GPIO number for SPI SIMOA
#define DEVICE_GPIO_PIN_SPISOMIA    17U  // GPIO number for SPI SOMIA
#define DEVICE_GPIO_PIN_SPICLKA     56U   // GPIO number for SPI CLKA

#define DEVICE_GPIO_CFG_SPISIMOA    GPIO_16_SPIA_SIMO  // "pinConfig" for SPI SIMOA
#define DEVICE_GPIO_CFG_SPISOMIA    GPIO_17_SPIA_SOMI  // "pinConfig" for SPI SOMIA
#define DEVICE_GPIO_CFG_SPICLKA     GPIO_56_SPIA_CLK    // "pinConfig" for SPI CLKA

void spi_init(void)
   {
     EALLOW;

         GpioCtrlRegs.GPAPUD.bit.GPIO16 = 0;  // Enable pull-up on GPIO16 (SPISIMOA)
         GpioCtrlRegs.GPAPUD.bit.GPIO17 = 0;  // Enable pull-up on GPIO17 (SPISOMIA)
         GpioCtrlRegs.GPBPUD.bit.GPIO56 = 0;  // Enable pull-up on GPIO18 (SPICLKA)

         GpioCtrlRegs.GPAQSEL2.bit.GPIO16 = 3; // Asynch input GPIO16 (SPISIMOA)
         GpioCtrlRegs.GPAQSEL2.bit.GPIO17 = 3; // Asynch input GPIO17 (SPISOMIA)
         GpioCtrlRegs.GPBQSEL2.bit.GPIO56 = 3; // Asynch input GPIO18 (SPICLKA)

         GpioCtrlRegs.GPAGMUX2.bit.GPIO16 = (1 >> 2);
         GpioCtrlRegs.GPAMUX2.bit.GPIO16 =  (1 & 0x03); // Configure GPIO16 as SPISIMOA

         GpioCtrlRegs.GPAGMUX2.bit.GPIO17 = (1 >> 2);;
         GpioCtrlRegs.GPAMUX2.bit.GPIO17 =  (1 & 0x03);// Configure GPIO17 as SPISOMIA

         GpioCtrlRegs.GPBGMUX2.bit.GPIO56 = (1 >>2);
         GpioCtrlRegs.GPBMUX2.bit.GPIO56 =  (1 & 0x03); // Configure GPIO18 as SPICLKA
      EDIS;
                // Initialize SPI FIFO registers
            SpiaRegs.SPIFFTX.all=0xE040;
            SpiaRegs.SPIFFRX.all=0x204f;
            SpiaRegs.SPIFFCT.all=0x0;

            SpiaRegs.SPICCR.all = 0x47;  // Reset on, rising edge, 8-bit char bits
            SpiaRegs.SPICTL.all = 0x06;  // Enable master mode, normal phase,
            SpiaRegs.SPIBRR.all = 9;   //baud rate

            SpiaRegs.SPICCR.all = 0x0C7; // Relinquish SPI from Reset // Bit 4 loop back mode disabled
            DELAY_US(100);
            DISABLE_SPI();
    }


#define MSG_DATA_LENGTH     8

void    InitECAN(void)
{

    EALLOW;
      GpioCtrlRegs.GPAQSEL2.bit.GPIO30 = 0x0; //Give CPU1 control just in case
      GpioCtrlRegs.GPAGMUX2.bit.GPIO30 = 0x0; //Set the extended mux to 0x5
      GpioCtrlRegs.GPAMUX2.bit.GPIO30 =  0x1;
      GpioCtrlRegs.GPAQSEL2.bit.GPIO30 = 0x3; //Set qualification to async just in case
      GpioCtrlRegs.GPADIR.bit.GPIO31 =  0;
      GpioCtrlRegs.GPAQSEL2.bit.GPIO31 = 0x0; //Give CPU1 control just in case
      GpioCtrlRegs.GPAGMUX2.bit.GPIO31 = 0; //Set the extended mux to bring out CANATX
      GpioCtrlRegs.GPAMUX2.bit.GPIO31 = 0x1;
      GpioCtrlRegs.GPAQSEL2.bit.GPIO31 = 0x3; //Set qualification to async just in case
    EDIS;
/*
        CANInit(CANA_BASE);

          CANClkSourceSelect(CANA_BASE, 0);
          CANBitRateSet(CANA_BASE, SYSCLOCK, 250000); //// 250kHz CAN-Clock
          CANIntEnable(CANA_BASE, CAN_INT_MASTER | CAN_INT_ERROR | CAN_INT_STATUS);

          CANEnable(CANA_BASE);
          sTXCANMessage.ui32MsgID = 0x200;
                 sTXCANMessage.ui32MsgIDMask = 0;
                 sTXCANMessage.ui32Flags = 0;
                 sTXCANMessage.ui32MsgLen = MSG_DATA_LENGTH;
                 sTXCANMessage.pucMsgData = txMsgData;

                 *(unsigned long *)ucRXMsgData = 0;

                 sRXCANMessage.ui32MsgID = 0x101;                      // CAN message ID - use 1
                 sRXCANMessage.ui32MsgIDMask = 0x0;
                 sRXCANMessage.ui32Flags = MSG_OBJ_RX_INT_ENABLE | MSG_OBJ_USE_ID_FILTER;
                 sRXCANMessage.ui32MsgLen = MSG_DATA_LENGTH;
                 sRXCANMessage.pucMsgData = ucRXMsgData;
                 CANMessageSet(CANA_BASE, 2, &sRXCANMessage, MSG_OBJ_TYPE_RX);
  */

    }

void initZCD_ECAP1_GPIO47(void)
{
    // 1) GPIO25 as plain input, async qualification (required for eCAP)
    GPIO_setPinConfig(GPIO_47_GPIO47);
    GPIO_setDirectionMode(47, GPIO_DIR_MODE_IN);
    GPIO_setPadConfig(47, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(47, GPIO_QUAL_ASYNC);

    // 2) Route GPIO25 -> Input X-BAR INPUT4 -> eCAP1
    XBAR_setInputPin(INPUTXBAR_BASE, XBAR_INPUT4, 47);   // fixed base
    ECAP_selectECAPInput(ECAP1_BASE, ECAP_INPUT_INPUTXBAR4);

    // 3) eCAP config
    EALLOW;
    ECap1Regs.ECCTL1.all = 0;
    ECap1Regs.ECCTL2.all = 0;

    ECap1Regs.ECCTL1.bit.CAPLDEN   = 1;   // load capture regs on event
    ECap1Regs.ECCTL1.bit.PRESCALE  = 0;   // /1, capture every edge
    ECap1Regs.ECCTL1.bit.CAP1POL   = 0;   // event1 = rising
    ECap1Regs.ECCTL1.bit.CAP2POL   = 1;   // event2 = falling
    ECap1Regs.ECCTL1.bit.CTRRST1   = 0;   // free-running counter, no reset on event
    ECap1Regs.ECCTL1.bit.CTRRST2   = 0;
    ECap1Regs.ECCTL1.bit.CTRRST3   = 0;
    ECap1Regs.ECCTL1.bit.CTRRST4   = 0;

    ECap1Regs.ECCTL2.bit.CAP_APWM    = 0;  // capture mode (not APWM)
    ECap1Regs.ECCTL2.bit.CONT_ONESHT = 0;  // continuous
    ECap1Regs.ECCTL2.bit.STOP_WRAP   = 1;  // wrap after event2
    ECap1Regs.ECCTL2.bit.SYNCI_EN    = 0;

    ECap1Regs.TSCTR = 0;
    ECap1Regs.ECCTL2.bit.TSCTRSTOP = 1;    // start free-running counter

    ECap1Regs.ECCLR.all = 0xFFFF;          // clear all flags
    ECap1Regs.ECEINT.all = 0;
    ECap1Regs.ECEINT.bit.CEVT1 = 1;        // interrupt on event1 (rising edge)
    EDIS;

    ECAP_clearInterrupt(ECAP1_BASE, ECAP_ISR_SOURCE_CAPTURE_EVENT_1);
}

