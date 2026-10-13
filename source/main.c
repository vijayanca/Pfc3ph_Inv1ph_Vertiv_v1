/***********************************************************************************
 Orion Technologies. Copyright (C) Feb 2023
 139, Shanti Industrial Esate, Mulund West, Mumbai-80
 -----------------------------------------------------------------------------------
 Project Name	: PFC 3PH
 Work Order	  	: MAGNAFLEX
 Software Code 	: 01??
 File Name 	  	: Main.C
 Description	: This is the main file for the Texas Instruments' TMS320F28035 DSP
 controller. This  file  contains  the  scheduler and functions for
 interfacing	to the peripheral cards.
 Date Created  	: 01 Aug 2023
 Authors		: Vijayan C A
 History Log	: Following Changes Carried out by Vijayan C A
 ----------------------------------------------------------------------------------\
 | Date	      | Ver	   | 			Description of Modification		 	 | Bug Nbr |
 |------------|--------|-------------------------------------------------|---------|
 | 20/06/2024 | 00AB   | Paralleling with output current                 |         |
 |            |        |                                                 |         |
 | 06/08/2023 | 00AA   | Initial Project                                 |         |
 |            |        |                                                 |         |
 |            |        |        										 |		   |
 **********************************************************************************/


/*------------------------------------------------------------------------------
 External Function Declarations here
 ------------------------------------------------------------------------------*/
#define	CCEXTERN
#define	INIT0	= 0
#define	INITARRAY0 = {0}
#define MATH_TYPE FLOAT_MATH

#include "f28x_project.h"
#include "../ccVariables.h"
#include "device.h"
#include "math.h"
extern __interrupt void MCANIntr1ISR(void);    // CAN-B paralleling RX
extern __interrupt void MCANARxISR(void);      // CAN-A display writes
extern __interrupt void AdcISR(void);
extern __interrupt void SCIRxISR(void);
extern __interrupt void SCIBRx_ISR(void);
extern __interrupt void cpu_timer0_isr(void);
extern __interrupt void cpu_timer1_isr(void);
extern __interrupt void cpu_timer2_isr(void);
extern __interrupt void EPWM_INV_ISR(void);
void storeEvents(void);
void EventLogging(void);


#define scaleREF(x,y,z) z = x*y
N16 rebirth = 0;

extern const PARAM defaultValues[];
char tchr[16];

#ifdef _FLASH
#pragma DATA_SECTION(ArrayParamsF, "BIG_ARRAYS")
#pragma DATA_SECTION(ArrayParamsI, "BIG_ARRAYS")
#pragma DATA_SECTION(waveBuffer, "BIG_ARRAYS")
#pragma DATA_SECTION(rx_buffer, "BIG_ARRAYS")
#endif

#define DI_UNITON           0x0001
#define DI_RESET            0x0002
#define DI_MAINSFAIL        0x0004
#define DI_MANUALBYP        0x0008
#define DI_OUTMCB           0x0010
#define DI_BYPMCB           0x0020
#define DI_HSTEMP           0x0040
#define DI_EPO              0x0080


 void main(void){

	//Device_init();

    InitSysCtrl(); // Initialize System Control....PLL, WatchDog, enable Peripheral Clocks
	InitPieCtrl(); // Initialize the PIE control registers to their default state.
	InitCpuTimers();

	DINT;
	//--------------------------------------------------------------------------
	    EALLOW;	// Clear all interrupts and initialize PIE vector table:
	    IER = 0x0000; // Disable CPU interrupts and clear all CPU interrupt flags:
	    IFR = 0x0000;
	EDIS;
	//--------------------------------------------------------------------------

	InitPieVectTable(); //Initialize the PIE vector table with pointers
	                    // to the shell Interrupt	 Service Routines (ISR)
	GpioInit();             // Common Controller v2: relays off first, PWM pins, DIs (BoardInit.c)
	InitFaultXbar();        // VCESAT1 -> INPUTXBAR1, VCESAT2 -> INPUTXBAR2
    SysCtl_setMCANClk(SYSCTL_MCANA, SYSCTL_MCANCLK_DIV_3);
    SysCtl_setMCANClk(SYSCTL_MCANB, SYSCTL_MCANCLK_DIV_3);
	InitVariables();

    // Bridge 1 (EPWM1-3, U30 -> CON15): the inverter
    ePWMInit_INV(1, 2, 3);
    DISABLE_PWM();
    DesatTripInit();        // VCESAT1 (active high) -> one-shot trip of EPWM1-3

    // Bridge 2 (EPWM4-6, U33 -> CON19): not used, outputs held low
    InitDrives(4);
    InitDrives(5);
    InitDrives(6);
    BRIDGE2_OFF();

    StaticSwitchDrivesInit();   // EPWM7A STAT_INV / EPWM7B STAT_BYP gate pulse trains, off
    STATIC_DRIVES_OFF();
    AdcTriggerInit();           // EPWM8 SOCA, 20 kHz ADC trigger

	LimitFlagInv.all = 0x7;
	SystemFlag.all = 0x01; // POWERINGUP + INPUTDC FAULT + BOOSTER1&2 DISABLE + INVRTER DISABLE

	EALLOW;
	// Interrupts are re-mapped to ISR functions

	PieVectTable.TIMER0_INT = &cpu_timer0_isr;
	PieVectTable.TIMER1_INT = &cpu_timer1_isr;
	PieVectTable.TIMER2_INT = &cpu_timer2_isr;

    PieVectTable.EPWM1_INT = &EPWM_INV_ISR;
    PieVectTable.ADCA1_INT = &AdcISR;
    PieVectTable.SCIA_RX_INT = &SCIRxISR;
    PieVectTable.SCIB_RX_INT = &SCIBRx_ISR;
    PieVectTable.MCANB_0_INT = &MCANIntr1ISR;   // paralleling RX on CAN-B
    PieVectTable.MCANB_1_INT = &MCANIntr1ISR;
    PieVectTable.MCANA_0_INT = &MCANARxISR;     // display writes on CAN-A
    PieVectTable.MCANA_1_INT = &MCANARxISR;

#ifdef SLOW_SAMPLING
	ConfigCpuTimer(&CpuTimer0, SYSTEM_FREQUENCY, 100);
#else
	ConfigCpuTimer(&CpuTimer0, SYSTEM_FREQUENCY, 50);
#endif
	ConfigCpuTimer(&CpuTimer1, SYSTEM_FREQUENCY, 1000);
	ConfigCpuTimer(&CpuTimer2, SYSTEM_FREQUENCY, 10000);

	CpuTimer0Regs.TCR.all = 0x4000; // Use write-only instruction to set TSS bit = 0
	CpuTimer1Regs.TCR.all = 0x4000; // Use write-only instruction to set TSS bit = 0
	CpuTimer2Regs.TCR.all = 0x4000; // Use write-only instruction to set TSS bit = 0

    EDIS;

    ConfigureADC();
    setupADC(ADC_TRIGGER_EPWM8_SOCA, 44);   // CCv2 SOC map; ADCA INT1 on EOC4

EALLOW;
	//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
	PieCtrlRegs.PIEIER1.bit.INTx1 = 1;   // 1.1 ADCA1
    PieCtrlRegs.PIEIER1.bit.INTx7 = 1;   // 1.7 TIMER0
	PieCtrlRegs.PIEIER3.bit.INTx1 = 1;   // 3.1 EPWM1 (inverter modulator)
	PieCtrlRegs.PIEIER9.bit.INTx1 = 1;   // 9.1 SCIA RX
    PieCtrlRegs.PIEIER9.bit.INTx3 = 1;   // 9.3 SCIB RX
    PieCtrlRegs.PIEIER9.bit.INTx8 = 1;   // 9.8 MCANA_1: CAN-A display writes
    PieCtrlRegs.PIEIER9.bit.INTx10 = 1;  // 9.10 MCANB_1: CAN-B paralleling RX

    EPwm1Regs.ETSEL.bit.INTEN = 1; // Enable EPWM1INT generation
    EPwm1Regs.ETSEL.bit.INTSEL = 1; // Enable interrupt CNT_zero event
    EPwm1Regs.ETPS.bit.INTPRD = 1; // Generate interrupt on the 1st event
    EPwm1Regs.ETCLR.bit.INT = 1; // Enable more interrupts

    //Enable CPU INT3 which is connected to EPwm1-3 INT:trip

    IER |= M_INT3; // Enable CPU INT3 for PWM
    IER |= M_INT1; // Enable CPU Interrupt 0
    IER |= M_INT9; //serial + CAN
    IER |= M_INT13; // Enable CPU Timer 1;
    IER |= M_INT14; // Enable CPU Timer 2;
EDIS;

	EINT;
	// Enable global Interrupts and higher priority
	ERTM;
	// real-time debug events:

	//Initialising Params
	address = 1;
	address1 = 1;
	rxChar = 0;

    InitSCIA(0);    // SCIA RS485 #2, fixed 115200 (BoardInit.c)
    InitSCIB();     // SCIB RS485 #1, GUI port
    DisableTX();
    DisableTX1();

    spi_init();

	InitVariables();

	msDelayCount = 500;
	while (msDelayCount); // wait for 0.5 sec

	RestoreSettings();
	updateRAMvalues(0);	//setting parameters
	RestoreSettingsInt();

	updateRAMvalues(0); //setting parameters

	if((UnitAddress>0) && (UnitAddress<7))
	                address = UnitAddress;
	address1 = address;     // same Modbus address on both ports

	if(address==1) PowerOnDelay = 3000;

//	if(address==1) PowerOnDelay = 3000;
//    else PowerOnDelay = address*1000 + 2000;

	InitHiLoParams();
    InitMCANPins();     // MCANA GPIO30/31, MCANB GPIO18/19
    InitMCAN();         // CAN-A display, CAN-B paralleling (mcanInterface.c)
    MCAN_enableIntr(MCANB_DRIVER_BASE, MCAN_INTR_MASK_ALL, 1U);
    MCAN_selectIntrLine(MCANB_DRIVER_BASE, MCAN_INTR_MASK_ALL, MCAN_INTR_LINE_NUM_1);
    MCAN_enableIntrLine(MCANB_DRIVER_BASE, MCAN_INTR_LINE_NUM_1, 1U);
    MCAN_enableIntr(MCANA_DRIVER_BASE, MCAN_INTR_SRC_RX_FIFO1_NEW_MSG, 1U);
    MCAN_selectIntrLine(MCANA_DRIVER_BASE, MCAN_INTR_MASK_ALL, MCAN_INTR_LINE_NUM_1);
    MCAN_enableIntrLine(MCANA_DRIVER_BASE, MCAN_INTR_LINE_NUM_1, 1U);
	SystemFlag.bit.POWERINGUP = TRUE;
	UPDATE_INV_FSW();
	OffsetNotDone = TRUE;

	while (1)
	{
	    InverterApplication();
		updateStatus();

		ModbusProcessing();
		ModbusProcessing1();
		MCANBusProcessing();

        if (rms_update_flag)
            {
                update_rmsmeters();
            }

		if (seconds!= seconds_old)
		{
            seconds_old = seconds;

		    if(seconds==30) { InitSCIA(0); InitSCIB(); }
		    if(errorRx) InitSCIA(0);

		    // Re-initialise a CAN module only if it went bus-off: a periodic
		    // re-init would interrupt the master's angle stream on CAN-B
		    if(MCANBusOff()) { InitMCAN(); }
		}
	    EventLogging();
	    storeEvents();
//-----------------------------------------------------------------------------

	    if(specialCommnad == CMD_DEFAULT)
	        {
	            SetFactoryDefaults();
	            SetFactoryDefaultsInt();
	            specialCommnad = 0;
	        }

	    if(autoSaveFlag)
	    {
	        UpdateOneInt(ADR_COMMIT, 1);
	        autoSaveFlag = FALSE;
	    }

	   UpdateRelayFlag();
	   UpdateRelays();

	   Pair_StatusFlag = PairStatusFlag.all ;

	} //End of Infinite for loop
} // End Of Main

#define DI_BTEST    0x04

void InverterApplication(void) //sequencing
{
//float32_t tempVar = 0;

    if (SystemFlag.bit.POWERINGUP || EmergencyStop)
        {
            DISABLE_PWM();
            pwmEnabled = FALSE;
            reason= 1;
            SystemFlag.bit.INVRTRDISABL = TRUE;
            SystemFlag.bit.OVERLOAD = FALSE;
            return;
        }


    if (SystemFlag.bit.TRIPFAULT)
    {
        DISABLE_PWM();
        pwmEnabled = FALSE;
        SystemFlag.bit.INVRTRDISABL = TRUE;
        reason= 2;
        return;
    }

       if((UnitOnCommand || unitOnCommand) && !EmergencyStop && !SystemFlag.bit.MANUALBYP &&
               (LimitFlag.bit.DCLINKOK || TestMode) && ((DigitalInputFlag1 & TripBits)==0))
            {

            if(pwmEnabled == FALSE)
                  {
                      //initialise variables, set softstart
                      UPDATE_INV_FSW();
                      soft_VdRef = 0.0;
                      soft_VqRef = 0.0;
                      volRef_Id = (RatedVoltage*TrxFactor)/VinvRSF;

                      defaultOmega = OMEGADEFAULT;//tempVar >> 10;
                      InitPID(&vcontrol.vol.pid_de);
                      InitPID(&vcontrol.vol.pid_qe);
                      PR_Reset();

//                      StartupTimer = SEC10;
                      pwmEnabled = TRUE;
//                      startTimer = 1000;
                     // ENABLE_PWM();
                      SystemFlag.bit.SOFTSTART = TRUE;
                      SystemFlag.bit.INVRTRDISABL = FALSE;
                      reason = 10;
                      systemRunningTimer = 0;
                      // (DSAT: hardware trip, DesatTripInit)
                  }
              }
             else
             {
                 InverterOK_timer = 0;
                 DISABLE_PWM();
               //  UnitOnCommand = FALSE;
                 pwmEnabled = FALSE;
                 SystemFlag.bit.INVRTRDISABL = TRUE;

                 if(reason==0) reason= 6;
                 if(SystemFlag.bit.TRIPFAULT)
                 {
                     reason= 3;
                 }

             }

        return;
    }



#define MIN_VOLTAGE 850

void updateStatus(void)
{
    N16 flag1 = 0/*, flag2 = 0*/;

    N16 tempiFlag = 0;
    LimitFlagInv.bit.INVRLOW = chkVr.stat.bit.LOW;
    LimitFlagInv.bit.INVRHIGH = chkVr.stat.bit.HIGH;
    LimitFlagInv.bit.INVYLOW = chkVy.stat.bit.LOW;
    LimitFlagInv.bit.INVYHIGH = chkVy.stat.bit.HIGH;
    LimitFlagInv.bit.INVBLOW = chkVb.stat.bit.LOW;
    LimitFlagInv.bit.INVBHIGH = chkVb.stat.bit.HIGH;

    LimitFlagInv.bit.OC_LEVEL1_R = chkIr1.stat.bit.HIGH;
    LimitFlagInv.bit.OC_LEVEL1_Y = chkIy1.stat.bit.HIGH;
    LimitFlagInv.bit.OC_LEVEL1_B = chkIb1.stat.bit.HIGH;

    LimitFlagInv.bit.OC_LEVEL2_R = chkIr2.stat.bit.HIGH;
    LimitFlagInv.bit.OC_LEVEL2_Y = chkIy2.stat.bit.HIGH;
    LimitFlagInv.bit.OC_LEVEL2_B = chkIb2.stat.bit.HIGH;

    LimitFlagInv.bit.OC_LEVEL3_R = chkIr3.stat.bit.HIGH;
    LimitFlagInv.bit.OC_LEVEL3_Y = chkIy3.stat.bit.HIGH;
    LimitFlagInv.bit.OC_LEVEL3_B = chkIb3.stat.bit.HIGH;

    Lvl1_TimerR = chkIr1.maxTimer/1000;
    Lvl1_TimerY = chkIy1.maxTimer/1000;
    Lvl1_TimerB = chkIb1.maxTimer/1000;
    Lvl2_TimerR = chkIr2.maxTimer/1000;
    Lvl2_TimerY = chkIy2.maxTimer/1000;
    Lvl2_TimerB = chkIb2.maxTimer/1000;
    Lvl3_TimerR = chkIr3.maxTimer/1000;
    Lvl3_TimerY = chkIy3.maxTimer/1000;
    Lvl3_TimerB = chkIy3.maxTimer/1000;


    if(chkIr1.maxTimer || chkIy1.maxTimer || chkIb1.maxTimer)   SystemFlag.bit.OCLVL1ACTIVE = TRUE;
    else SystemFlag.bit.OCLVL1ACTIVE = FALSE;

    if(chkIr2.maxTimer || chkIy2.maxTimer || chkIb2.maxTimer)   SystemFlag.bit.OCLVL2ACTIVE = TRUE;
    else SystemFlag.bit.OCLVL2ACTIVE = FALSE;

    if(LimitFlagInv.all & 0x7fc0) SystemFlag.bit.OVERLOAD = TRUE;
    else SystemFlag.bit.OVERLOAD = FALSE;

    LimitFlag.bit.BYPRLOW = chkVbypr.stat.bit.LOW;
    LimitFlag.bit.BYPRHIGH = chkVbypr.stat.bit.HIGH;
    LimitFlag.bit.BYPYLOW = chkVbypy.stat.bit.LOW;
    LimitFlag.bit.BYPYHIGH = chkVbypy.stat.bit.HIGH;
    LimitFlag.bit.BYPBLOW = chkVbypb.stat.bit.LOW;
    LimitFlag.bit.BYPBHIGH = chkVbypb.stat.bit.HIGH;

     SysOkFlag.bit.VINVROK = chkVr.stat.bit.OK;
     SysOkFlag.bit.VINVYOK = chkVy.stat.bit.OK;
     SysOkFlag.bit.VINVBOK = chkVb.stat.bit.OK;


     SysOkFlag.bit.VBYPROK = chkVbypr.stat.bit.OK;
     SysOkFlag.bit.VBYPYOK = chkVbypy.stat.bit.OK;
     SysOkFlag.bit.VBYPBOK = chkVbypb.stat.bit.OK;

     LimitFlag.bit.BYPASSOK = chkVbypr.stat.bit.OK && chkVbypy.stat.bit.OK && chkVbypb.stat.bit.OK;
     LimitFlag.bit.INVRTROK = chkVr.stat.bit.OK && chkVy.stat.bit.OK && chkVb.stat.bit.OK;

     LimitFlag.bit.DCLINKLOW = chkdcLink.stat.bit.LOW;
     LimitFlag.bit.DCLINKHIGH = chkdcLink.stat.bit.HIGH;
     LimitFlag.bit.DCLINKOK = chkdcLink.stat.bit.OK;

     LimitFlag.bit.PREALARM = chkVbatpre.stat.bit.LOW;
     LimitFlag.bit.VBATLOW = chkVbat.stat.bit.LOW;
     LimitFlag.bit.VBATHIGH = chkVbat.stat.bit.HIGH;


     if( (DigitalInputFlag1 & DI_MAINSFAIL) &&   SysOkFlag.bit.VINVROK) SystemFlag.bit.LOADONBAT = TRUE;
     else SystemFlag.bit.LOADONBAT = FALSE;

     // Hardware desaturation trip (VCESAT1 -> DCAEVT1, DesatTripInit): the pulses
     // are already off; flag it so the trip / restart / latch logic sees it
     if(pwmEnabled && EPwm1Regs.TZFLG.bit.DCAEVT1) SystemFlag.bit.VCESAT = TRUE;

     if (SystemFlag.bit.OVERLOAD || SystemFlag.bit.VCESAT ||
              (LimitFlagInv.all & 0x7ff8) || EmergencyStop || dcOV)
         {
             pwmEnabled = FALSE;
             if(!SystemFlag.bit.TRIPFAULT && !SystemFlag.bit.POWERINGUP)
                 {
                     if(TripCount<MaxRestartCount) TripCount++;
                     if((DigitalInputFlag1 & TripBits) || EmergencyStop || SystemFlag.bit.VCESAT) TripCount = MaxRestartCount;
                     systemRunningTimer = 0;
                     restartTimer = RestartDelay;
                     SystemFlag.bit.TRIPFAULT = TRUE;
                     if(TripCount == MaxRestartCount) SystemFlag.bit.LATCHFAULT = TRUE;
                     chkIr1.maxTimer = 0;
                     chkIr2.maxTimer = 0;
                     chkIr3.maxTimer = 0;
                 }
       }

     tempiFlag = DigitalInputFlag1;

     if (DI1timer == DI_DELAY)   tempiFlag |= BIT00;
     else if (DI1timer == 0)     tempiFlag &= BITC0;
     if (DI2timer == DI_DELAY)   tempiFlag |= BIT01;
     else if (DI2timer == 0)     tempiFlag &= BITC1;
     if (DI3timer == DI_DELAY)   tempiFlag |= BIT02;
     else if (DI3timer == 0)     tempiFlag &= BITC2;
     if (DI4timer == DI_DELAY)   tempiFlag |= BIT03;
     else if (DI4timer == 0)     tempiFlag &= BITC3;
     if (DI5timer == DI_DELAY)   tempiFlag |= BIT04;
     else if (DI5timer == 0)     tempiFlag &= BITC4;
     if (DI6timer == DI_DELAY)   tempiFlag |= BIT05;
     else if (DI6timer == 0)     tempiFlag &= BITC5;
     if (DI7timer == DI_DELAY)   tempiFlag |= BIT06;
     else if (DI7timer == 0)     tempiFlag &= BITC6;
     if (DI8timer == DI_DELAY)   tempiFlag |= BIT07;
     else if (DI8timer == 0)     tempiFlag &= BITC7;
     if (DI9timer == DI_DELAY)   tempiFlag |= BIT08;
     else if (DI9timer == 0)     tempiFlag &= BITC8;
     if (DI10timer == DI_DELAY)   tempiFlag |= BIT09;
     else if (DI10timer == 0)     tempiFlag &= BITC9;

     DigitalInputFlag1 = tempiFlag;

     if(DigitalInputFlag1 & DI_UNITON) unitOnCommand = FALSE;
     else  unitOnCommand = TRUE;

     if(DigitalInputFlag1 & DI_OUTMCB) UPSFlag.bit.OUTMCBOFF = TRUE;
     else  UPSFlag.bit.OUTMCBOFF = FALSE;

    if(DigitalInputFlag1 & DI_BYPMCB) UPSFlag.bit.BYPMCBOFF = TRUE;
     else UPSFlag.bit.BYPMCBOFF = FALSE;


     if(DigitalInputFlag1 & DI_MANUALBYP) SystemFlag.bit.MANUALBYP = TRUE;
     else SystemFlag.bit.MANUALBYP = FALSE;

     if(DigitalInputFlag1 & DI_EPO) EmergencyStop = 1; //Latch fault

     if(DigitalInputFlag1 & DI_MAINSFAIL) { mainsFailTimer = MainsOKDelay; chkdcLink.stat.bit.ARL = FALSE; }
     else
         {
             if(mainsFailTimer == 0) chkdcLink.stat.bit.ARL = TRUE ;
         }

     if((DigitalInputFlag1 & DI_RESET)==0)
             {
                 RESET_FAULTS();
             }

     UPSFlag.bit.POWERINGUP = SystemFlag.bit.POWERINGUP;
     UPSFlag.bit.INVRTRDISABL = SystemFlag.bit.INVRTRDISABL;
     UPSFlag.bit.LOADONBAT = SystemFlag.bit.LOADONBAT;
     UPSFlag.bit.LOADONBYP = SystemFlag.bit.LOADONBYP;
     UPSFlag.bit.LOADONINV = SystemFlag.bit.LOADONINV;
     UPSFlag.bit.MANUALBYP = SystemFlag.bit.MANUALBYP;
     //UPSFlag.bit.INVFAILURE = SystemFlag.bit.INVFAILURE;

     UPSFlag.bit.NOTSYNCED = SystemFlag.bit.NOTSYNCED;

     if(!UPSFlag.bit.INVOK && LimitFlag.bit.INVRTROK) xfr2InvTimer = xfrToInverterDelay;
     UPSFlag.bit.INVOK = LimitFlag.bit.INVRTROK;
     UPSFlag.bit.SOFTSTART = SystemFlag.bit.SOFTSTART;
     UPSFlag.bit.BYPASSOK = LimitFlag.bit.BYPASSOK;
     UPSFlag.bit.TRIPFAULT = SystemFlag.bit.TRIPFAULT;
     UPSFlag.bit.BITSVALID = TRUE;

    updateBit(DCOV_Inst_Delay,DCOVTimer,  LimitFlag.bit.DCLINKHIGH);

    if(Master) UPSFlag.bit.MASTER = TRUE;
     else UPSFlag.bit.MASTER = FALSE;

     SystemFlag.bit.BITSVALID = TRUE;
     AlarmFlag1= SystemFlag.all;

     LimitFlag.bit.BITSVALID = TRUE;
     AlarmFlag2= LimitFlag.all;

     LimitFlagInv.bit.BITSVALID = TRUE;
     AlarmFlag3 = LimitFlagInv.all;

     flag1 = SysOkFlag.all & 0xff00;
     AlarmFlag4 = flag1 | (DigitalInputFlag1 & 0xff);
     AlarmFlag5 = PairStatusFlag.all;

     CommStatus.bit.BITSVALID = TRUE;
     CommStatusFlag = CommStatus.all;

}



float32_t  FinMax3(float32_t a,float32_t b, float32_t c)
    {
        if((a>b) && (a>c)) return(a);
        if(b>c) return(b);
        return(c);
    }

float32_t Average32(LPF32 *x, float32_t val)
{
    N16 c=0;
    x->larray[x->lptr] = val;
    x->lptr++;
    x->lptr &= 0x1f;
    x->lsum = 0;
    for(c=0;c<32;c++)
    x->lsum += x->larray[c];
    x->y = (x->lsum/32);
    return(x->y);
}

//#define DUMMYVALS

#define UPDATE_RMS_F(x,y,z)  x.RMS = sqrt(x.savedSum/rms_samplecount); \
                              z = x.RMS*y
#define UPDATE_KW_F(x,z)  z = (x.savedSum/rms_samplecount)*.001
#define SCALEF(x,y,z)  z = x*y
#define scaleKVA(v,i,k) k = v*i*.001

float rawval1 = 0,rawval2 = 0,rawval3 = 0;

#define SCALE_RMSVAL_FLT(meter, sf,flt,v)   \
        meter.RMS = sqrt(meter.savedSum/rms_samplecount)*sf; \
        v = Average32(&flt, meter.RMS)

#define SCALE_DCVAL_FLT(lpf, sf,flt,v)   \
        rawval1 = lpf.y*sf; \
        v = Average32(&flt, rawval1)

void update_rmsmeters(void)
{
    SCALE_RMSVAL_FLT(volt_meter_R,VinvRSF,lpf32_vinvr,VinvR);
    SCALE_RMSVAL_FLT(volt_meter_Y,VinvYSF,lpf32_vinvy,VinvY);
    SCALE_RMSVAL_FLT(volt_meter_B,VinvBSF,lpf32_vinvb,VinvB);

    SCALE_RMSVAL_FLT(cur_meter_R,IinvRSF,lpf32_iinvr,IinvR);
    SCALE_RMSVAL_FLT(cur_meter_Y,IinvYSF,lpf32_iinvy,IinvY);
    SCALE_RMSVAL_FLT(cur_meter_B,IinvBSF,lpf32_iinvb,IinvB);

    SCALE_RMSVAL_FLT(vout_meter_R,VoutRSF,lpf32_voutr,VoutR);
    SCALE_RMSVAL_FLT(vout_meter_Y,VoutYSF,lpf32_vouty,VoutY);
    SCALE_RMSVAL_FLT(vout_meter_B,VoutBSF,lpf32_voutb,VoutB);

    SCALE_RMSVAL_FLT(iout_meter_R,IoutRSF,lpf32_ioutr,IoutR);
    SCALE_RMSVAL_FLT(iout_meter_Y,IoutYSF,lpf32_iouty,IoutY);
    SCALE_RMSVAL_FLT(iout_meter_B,IoutBSF,lpf32_ioutb,IoutB);

    SCALE_RMSVAL_FLT(vll_meter_R,VoutRSF,lpf32_vllr,VoutRY);
    SCALE_RMSVAL_FLT(vll_meter_Y,VoutYSF,lpf32_vlly,VoutYB);
    SCALE_RMSVAL_FLT(vll_meter_B,VoutBSF,lpf32_vllb,VoutBR);

    SCALE_RMSVAL_FLT(vbyp_meter_R,VbypRSF,lpf32_vbypr,VbypR);
    SCALE_RMSVAL_FLT(vbyp_meter_Y,VbypYSF,lpf32_vbypy,VbypY);
    SCALE_RMSVAL_FLT(vbyp_meter_B,VbypBSF,lpf32_vbypb,VbypB);

    SCALE_RMSVAL_FLT(vllb_meter_R,VbypRSF,lpf32_vbypry,VbypRY);
    SCALE_RMSVAL_FLT(vllb_meter_Y,VbypYSF,lpf32_vbypyb,VbypYB);
    SCALE_RMSVAL_FLT(vllb_meter_B,VbypBSF,lpf32_vbypbr,VbypBR);

    UPDATE_KW_F(kw_meter_R, kW_Rph);
    UPDATE_KW_F(kw_meter_Y, kW_Yph);
    UPDATE_KW_F(kw_meter_B, kW_Bph);

//KVA Calculation:
    scaleKVA(VoutR, IoutR, kVA_Rph);
    scaleKVA(VoutY, IoutY, kVA_Yph);
    scaleKVA(VoutB, IoutB, kVA_Bph);


    //POWERFACTOR SCALING:
    if (kW_Rph >kVA_Rph) kW_Rph = kVA_Rph;
    if (kW_Yph >kVA_Yph) kW_Yph = kVA_Yph;
    if (kW_Bph >kVA_Bph) kW_Bph = kVA_Bph;

    powerfactorR = kW_Rph/ kVA_Rph;
    powerfactorY = kW_Yph/ kVA_Yph;
    powerfactorB = kW_Bph/ kVA_Bph;

    kVA_Total = kVA_Rph + kVA_Yph + kVA_Bph;
    kW_Total = kW_Rph + kW_Yph + kW_Bph;

    SCALEF(lpf_dclink.y,DcLinkVoltageSF, DcLinkVoltage);

    SCALEF(lpf_vbat.y,VbatSF, Vbat);
    SCALEF(lpf_ibat.y,IbatSF, Ibat);
    SCALEF(lpf_temp1.y,Temp1SF, Temp1);
    SCALEF(lpf_temp2.y,Temp2SF, Temp2);


    GridFrequency = OMEGA_FACTOR * pllBypass.omega;
    InvFrequency = OMEGA_FACTOR * omega_inv;


    rms_update_flag = FALSE;

    Inv_vol_de_ref = vcontrol.vol.pid_de.term.Ref;
    Inv_vol_de_fback = vcontrol.vol.lpf_de.y;
    Inv_vol_de_pidout  = vcontrol.vol.pid_de.term.Out;
    Inv_vol_qe_ref = vcontrol.vol.pid_qe.term.Ref;
    Inv_vol_qe_fback = vcontrol.vol.lpf_qe.y;
    Inv_vol_qe_pidout  = vcontrol.vol.pid_qe.term.Out;

    current_de_ref =  cur.pi_de.Ref;
    current_qe_ref =  cur.pi_qe.Ref;
    current_de_fback =  cur.pi_de.Fbk;
    current_qe_fback =  cur.pi_qe.Fbk;
    current_de_pidout =  cur.pi_de.Out;
    current_qe_pidout =  cur.pi_qe.Out;

    Cur_deMax =  cur.pi_de.Umax;
    Cur_deMin =  cur.pi_de.Umin;
    Cur_qeMax =  cur.pi_qe.Umax;

    cur_kp = cur.pi_de.Kp;
    cur_ki = cur.pi_de.Ki;

    IoutMax = FinMax3(IinvR, IinvR, IinvR);
}


static N16 NUMBITS[] = {    0xffff, 0xfffe, 0xfffc, 0xfff8, 0xfff0, 0xffe0, 0xffc0,0xff80,
                                0xff00, 0xfe00, 0xfc00, 0xf800, 0xf000, 0xe000, 0xc000,0x08000  };

N8  findBit(N8 start, N16 flag)
    {
     N8 msgcount;
     N16 bitptr;

     bitptr = flag & NUMBITS[start];

     if(bitptr == 0) return(0);

     bitptr = 1L << start;

        msgcount = 0;

    while (!(flag & bitptr))
    {
      bitptr <<= 1;
    }

    do
        {
              bitptr >>= 1;
              ++msgcount;
        }
            while(bitptr);

        return(msgcount--);
}


void delay_ms(N16 n1)
    {
        msDelayCount = n1;
        while(msDelayCount);
        return;
    }
/*
void  SetFactoryDefaults(void) {
    Uint16 parAddr;
    Uint16 val, x;

    spiWriteBuffer[0] = MAGIC;
    spiWriteBuffer[1] = 0;
    eeprom_write(EP_IDLOC, 2);
    DELAY_US(10000);

    for (x = 0; x < 10; x++) {
        val = *(defaultSerial + x + x + 0);
        val <<= 8;
        val |= *(defaultSerial + x + x + 1);
        spiWriteBuffer[x ] = val;
        ArrayParam[SERIAL_START + x] = val;
    }

    eeprom_write(EP_SERIAL, READLEN);
    DELAY_US(10000);

    for (parAddr = 0; parAddr < NUMWPARAMS; parAddr++)
        ArrayParamF[parAddr + WR_START] = defaultValues[parAddr].DefaultValue;
        storeArrayToEEPROM();
}
*/
    void        updateRAMvalues(N16 adr)
    {
        switch(adr)
        {
        case 0:
            cur.pi_de.Kp = cur_kp;
            cur.pi_qe.Kp = cur_kp;

            volRef_Id = (RatedVoltage*TrxFactor)/VinvRSF;
            InvCurHL_Lvl1a = RatedCurrent*InvCurHL_Lvl1/100.0;
            InvCurHL_Lvl2a = RatedCurrent*InvCurHL_Lvl2/100.0;
            InvCurHL_Lvl3a = RatedCurrent*InvCurHL_Lvl3/100.0;

            inverterFailLevel = (InvVoltageLL/VinvRSF)*0.9;
            bypassFailLevel = (BypVoltageLL/VbypRSF)*0.9;

            vcontrol.vol.pid_de.param.Kp = inv_kp;
            vcontrol.vol.pid_qe.param.Kp = inv_kp;
            vcontrol.vol.pid_de.param.Ki = inv_ki;
            vcontrol.vol.pid_qe.param.Ki = inv_ki;
            cur.pi_de.Umax = Cur_deMax;
            cur.pi_de.Umin = Cur_deMin;
            cur.pi_qe.Umax = Cur_qeMax;
            cur.pi_qe.Umin = -Cur_qeMax;

            if(VacPeakLimit<RatedVoltage)  vacPeakLimit = 1.1 * RatedVoltage;

            vacPeakLimit = 1.414 * VacPeakLimit;
            dcOV_Inst_Level = DCOV_Inst_Level/DcLinkVoltageSF;

            if((UnitAddress>0) && (UnitAddress<33)) address = UnitAddress;
            else address = 1;
           // Master = MasterSet;
            break;

        case 100:
            volRef_Id = (RatedVoltage*TrxFactor)/VinvRSF;
            ReferenceChangedFlag = TRUE;
            break;
        case 102:
            if(Cur_deMin<0.0f) cur.pi_de.Umin = Cur_deMin;
            break;
        case 103:
            cur.pi_de.Umax = Cur_deMax;
            break;
        case 104:
            cur.pi_qe.Umax = Cur_qeMax;
            cur.pi_qe.Umin = -Cur_qeMax;
            break;
        case 101:
        case 117:
        case 118:
        case 119:
            ReferenceChangedFlag = TRUE;
            InvCurHL_Lvl1a = RatedCurrent*InvCurHL_Lvl1/100.0;
            InvCurHL_Lvl2a = RatedCurrent*InvCurHL_Lvl2/100.0;
            InvCurHL_Lvl3a = RatedCurrent*InvCurHL_Lvl3/100.0;
            break;
        case 145:
             cur_neg.pi_de.Kp = cur_neg_kp;
             cur_neg.pi_qe.Kp = cur_neg_kp;
             break;

        case 146:
             cur_neg.pi_de.Ki = cur_neg_ki;
             cur_neg.pi_qe.Ki = cur_neg_ki;
             break;
        case 148:
             cur_neg.lpf_de.T = cur_neg_lpf;
             cur_neg.lpf_qe.T = cur_neg_lpf;
             break;
//
//
//            {0.01,.5,0.1},  /*  cur_neg_kp 145 */
//            {0.001,0.5,0.01},  /*  cur_neg_ki 146 */
//            {-0.5,1.047197551, 0.523598776},  /*  Angle30 147 */
//            {0.0001,0.1,0.001}, /* cur_neg_lpf  148  */

        case 155:
            vcontrol.vol.pid_de.param.Kp = inv_kp;
            vcontrol.vol.pid_qe.param.Kp = inv_kp;
            vcontrol.vol.pid_de.data.i1 = vcontrol.vol.pid_de.data.i1 * 0.6;
            vcontrol.vol.pid_qe.data.i1 = vcontrol.vol.pid_qe.data.i1 * 0.6;
            break;
        case 156:
            vcontrol.vol.pid_de.param.Ki = inv_ki;
            vcontrol.vol.pid_qe.param.Ki = inv_ki;
            vcontrol.vol.pid_de.data.i1 = vcontrol.vol.pid_de.data.i1 * 0.6;
            vcontrol.vol.pid_qe.data.i1 = vcontrol.vol.pid_qe.data.i1 * 0.6;
            break;
        case 153:
             cur.pi_de.Kp = cur_kp;
             cur.pi_qe.Kp = cur_kp;
             break;

        case 154:
             cur.pi_de.Ki = cur_ki;
             cur.pi_qe.Ki = cur_ki;
             break;
        }

        return;
}
#define CALARM_PFC_FLAG1    3840
#define CALARM_PFC_FLAG2    32767
#define CALARM_PFC_FLAG3    15
#define CALARM_PFC_FLAG4    0

#define CALARM_INV_FLAG1    7112
#define CALARM_INV_FLAG2    2047
#define CALARM_INV_FLAG3    4681
#define CALARM_INV_FLAG4    0

N16 BITS[] = {BIT0, BIT1, BIT2, BIT3, BIT4, BIT5, BIT6, BIT7, BIT8, BIT9, BIT10, BIT11, BIT12, BIT13, BIT14, BIT15};

N16 Fault_Bits(void)
    {
        N16 Count = 0;
        N16 i = 0;
        N16 pfc_flag3_mask = 0;

        pfc_flag3_mask = CALARM_PFC_FLAG3 | (DI_AlarmBits_PFC << 8);

        for(i=0;i<16;i++)
            if(Pfc_AlarmFlag1 & BITS[i] & CALARM_PFC_FLAG1 ) Count++;
        for(i=0;i<16;i++)
            if(Pfc_AlarmFlag2 & BITS[i] & CALARM_PFC_FLAG2) Count++;
        for(i=0;i<16;i++)
            if(Pfc_AlarmFlag3 & BITS[i] & pfc_flag3_mask) Count++;
        for(i=0;i<16;i++)
            if(Pfc_AlarmFlag4 & BITS[i] & CALARM_PFC_FLAG4) Count++;

        for(i=0;i<16;i++)
            if(AlarmFlag1 & BITS[i] & CALARM_INV_FLAG1) Count++;
        for(i=0;i<16;i++)
            if(AlarmFlag2 & BITS[i] & CALARM_INV_FLAG2) Count++;
        for(i=0;i<16;i++)
            if(AlarmFlag3 & BITS[i] & CALARM_INV_FLAG3) Count++;
        for(i=0;i<16;i++)
            if(AlarmFlag4 & BITS[i] & (DigitalInputFlag1 & DI_AlarmBits)) Count++;

     return(Count);
    }

//    INPUT AC FAILURE
//    DC GROUND FAULT
//    BATTERY CB OPEN
//    BYPASS FAILURE
//    OVERLOAD
//    COMMON FAULT
//    UPS ON BATTERY
//    INVERTER HEALTHY
//    CHARGER FIALURE
//    LOAD ON BYPASS

//2016 1791 0x7fff + DI Alarms and DI alarms FF //INV
//3840 0x7fff 15 + DI alarms  (ff00)  //PFC


N16 oldBits = 0;
void    UpdateRelayFlag(void)
{
    N16 pfc_flag3_mask = 0;

        pfc_flag3_mask = CALARM_PFC_FLAG3 | (DI_AlarmBits_PFC << 8);

        if((Pfc_AlarmFlag1 & 0x800) || (Pfc_AlarmFlag2 & 0x003f)) RelayFlag |= BIT00;
        else RelayFlag &= BITC0; //INPUT AC FAILURE

        if(Pfc_AlarmFlag3 & 0x8000) RelayFlag |= BIT01; //GROUND FAULT
        else RelayFlag &= BITC1;

        if(Pfc_AlarmFlag3 & 0x2000) RelayFlag |= BIT02;
        else RelayFlag &= BITC2; //BAT CB

        if(AlarmFlag2 & 0x003f) RelayFlag |= BIT03;
        else RelayFlag &= BITC3;
        if(AlarmFlag1 & 0x1880) RelayFlag |= BIT04; //Overload + active
        else RelayFlag &= BITC4;

        if( (Pfc_AlarmFlag1 & CALARM_PFC_FLAG1) || (Pfc_AlarmFlag2 & CALARM_PFC_FLAG2) ||
                (Pfc_AlarmFlag3 & pfc_flag3_mask) || (Pfc_AlarmFlag4 & CALARM_PFC_FLAG4) ||
                (AlarmFlag1 & CALARM_INV_FLAG1) || (AlarmFlag2 & CALARM_INV_FLAG2) ||
                                (AlarmFlag3 & CALARM_INV_FLAG3) || (AlarmFlag4 & (DigitalInputFlag1 & DI_AlarmBits) )) { RelayFlag |= BIT05; BuzzerOn = TRUE; }
         else { RelayFlag &= BITC5;  BuzzerOn = FALSE; }//common

        FaultBits = Fault_Bits();
        if(FaultBits> oldBits) MuteFlag = FALSE;
        oldBits = FaultBits;

        if(AlarmFlag1 & 0x1808) BeepDuration = 200;
        else BeepDuration = 900;


        if(AlarmFlag1 & 0x0008) RelayFlag |= BIT06; //load on batt
        else RelayFlag &= BITC6;

        if(AlarmFlag4 & 0x0800) RelayFlag |= BIT07; //Inverter Healthy
        else RelayFlag &= BITC7;

        if(Pfc_AlarmFlag1 & 0x2008) RelayFlag |= BIT08; //Charger Failure
        else RelayFlag  &= BITC8;

        if(AlarmFlag1 & 0x0020) RelayFlag |= BIT09; //load on bypass
        else RelayFlag &= BITC9;


}
 void UpdateRelays(void)
    {
        if(RelayFlag & 0x0001)  GPIO_WritePin(24, 1);
        else GPIO_WritePin(24, 0);
        if(RelayFlag & 0x0002)  GPIO_WritePin(25, 1);
        else GPIO_WritePin(25, 0);
        if(RelayFlag & 0x0004)  GPIO_WritePin(26, 1);
        else GPIO_WritePin(26, 0);
        if(RelayFlag & 0x0008)  GPIO_WritePin(27, 1);
        else GPIO_WritePin(27, 0);

        if(RelayFlag & 0x0010)  GPIO_WritePin(32, 1);
        else GPIO_WritePin(32, 0);
        if(RelayFlag & 0x0020)  GPIO_WritePin(58, 1);
        else GPIO_WritePin(58, 0);
        if(RelayFlag & 0x0040)  GPIO_WritePin(22, 1);
        else GPIO_WritePin(22, 0);
        if(RelayFlag & 0x0080)  GPIO_WritePin(44, 1);
        else GPIO_WritePin(44, 0);

        if(RelayFlag & 0x0100)  GPIO_WritePin(61, 1);
        else GPIO_WritePin(61, 0);
        if(RelayFlag & 0x0200)  GPIO_WritePin(59, 1);
        else GPIO_WritePin(59, 0);

    }


 void storeEvents(void) {
     N16 *byteptr;
     N16 eventAddr = 0;
     Uint16 _ccount = 0;

     if (msgPending && ((milliseconds- last_stored_time) > 10 ))
     {
         RecordNumber++;
         if(RecordNumber > MAXPACKETS) RecordNumber = 0;
         WriteRecordPointer();
         //store

         eventAddr = (RecordNumber << 6) + LOGSTART;
         byteptr = (N16 *) &EventTX;

         for(_ccount=0;_ccount < RECSIZE;_ccount++)
         spiWriteBuffer[_ccount] = *(byteptr + _ccount);

         eeprom_write(eventAddr, RECSIZE);
         msgPending = FALSE;
         last_stored_time = milliseconds;
         recordWriteTimer = SEC1;
     }

 }

 //EventLogging Functions
 N16 j = 0,Event = 0, AlarmFlag = 0, ev = 0;

 void EventLogging(void)
 {

#define MSGMASK1 6601  //EventFlag
#define MSGMASK2 2047 // LimitFlag
#define MSGMASK3 32767 // LimitFlag
#define MSGMASK4 125 // LimitFlag
#define MSGMASK5 4095 // Pair Status


     j = 0,Event = 0, AlarmFlag = 0, ev = 0;
 Event = 0;
 j= 0;

         AlarmFlag = AlarmFlag1;
         ev = AlarmFlag & MSGMASK1;
         j = ev & (~AlarmFlag1_old);
         if (j) Event = 1;
         AlarmFlag1_old = ev;

         AlarmFlag = AlarmFlag2;
         ev = AlarmFlag & MSGMASK2;
         j = ev & (~AlarmFlag2_old);
         if (j) Event = 1;
         AlarmFlag2_old = ev;

         AlarmFlag = AlarmFlag3;
         ev = AlarmFlag & MSGMASK3;
         j = ev & (~AlarmFlag3_old);
         if (j) Event = 1;
         AlarmFlag3_old = ev;

         AlarmFlag = AlarmFlag4;
         ev = AlarmFlag & MSGMASK4;
         j = ev & (~AlarmFlag4_old);
         if (j) Event = 1;
         AlarmFlag4_old = ev;

         AlarmFlag = AlarmFlag5;
         ev = AlarmFlag & MSGMASK5;
         j = ev & (~AlarmFlag5_old);
         if (j) Event = 1;
         AlarmFlag5_old = ev;

         if (Event && (eventCount < 3))
         {
             Events[eventCount].type = 0x1973;
             Events[eventCount].minsec = RTCMinute*256+RTCSecond;
             Events[eventCount].dthr = RTCDate*256+RTCHour;
             Events[eventCount].yrmon = RTCYear*256 + RTCMonth;
             Events[eventCount].alarm1 = AlarmFlag1 & MSGMASK1;
             Events[eventCount].alarm2 = AlarmFlag2 & MSGMASK2;
             Events[eventCount].alarm3 = AlarmFlag3 & MSGMASK3;
             Events[eventCount].alarm4 = AlarmFlag4 & MSGMASK4;
             Events[eventCount].alarm5 = AlarmFlag5 & MSGMASK5;

                Events[eventCount].eVinvR = (int) (VinvR*10);
                Events[eventCount].eVinvY = (int) (VinvY*10);
                Events[eventCount].eVinvB = (int) (VinvB*10) ;
                Events[eventCount].eVbypR = (int) (VbypR*10) ;
                Events[eventCount].eVbypR = (int) (VbypY*10) ;
                Events[eventCount].eVbypR = (int) (VbypB*10) ;
                Events[eventCount].eIinvR = (int) (IoutR*10) ;
                Events[eventCount].eIinvY = (int) (IoutY*10) ;
                Events[eventCount].eIinvB = (int) (IoutB*10) ;
                Events[eventCount].ekVA_R = (int) (kVA_Rph*10) ;
                Events[eventCount].ekVA_Y = (int) (kVA_Yph*10) ;
                Events[eventCount].ekVA_B = (int) (kVA_Bph*10);
                Events[eventCount].ekW_R =  (int) (kW_Rph*10);
                Events[eventCount].ekW_Y =  (int) (kW_Yph*10);
                Events[eventCount].ekW_B =  (int) (kW_Bph*10);
                Events[eventCount].InvFreq = (int) (InvFrequency*100);
                Events[eventCount].BypFreq =  (int) (GridFrequency*100);
                Events[eventCount].eVbat =  (int) (Vbat*10);
                Events[eventCount].DcLinkV =  (int) (DcLinkVoltage*10);
                Events[eventCount].eIbat =  (int) (Ibat*10);;

             eventSerialNumber++;
             eventSerialNumber &= 0xfff;
             eventCount++;
         }   //Event Occured

          if ((msgPending == 0) && eventCount)    //last event transmission complete
                         {
                             EventTX = Events[0];
                             Events[0] = Events[1];
                             Events[1] = Events[2];
                             Events[2] = Events[3];
                             eventCount--;
                             msgPending = TRUE;
                         }

     }       //function END

 //----------- NO MORE ----------------

