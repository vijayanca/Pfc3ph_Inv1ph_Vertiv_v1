/***********************************************************************************
 Orion Technologies. Copyright (C) Feb 2023
 139, Shanti Industrial Esate, Mulund West, Mumbai-80
 -----------------------------------------------------------------------------------
 Project Name	: 12 Pulse Rectifier - 3 Phase
 Work Order	  	: VERTIV
 Software Code 	: 01??
 File Name 	  	: Main.C
 Description	: This is the main file for the Texas Instruments' TMS320F280025 DSP
 controller. This  file  contains  the  scheduler and functions for
 interfacing	to the peripheral cards.
 Date Created  	: 10 January 2023
 Authors		: Vijayan C A
 History Log	: Following Changes Carried out by Vijayan C A
 ----------------------------------------------------------------------------------\
 | Date	      | Ver	   | 			Description of Modification		 	 | Bug Nbr |
 |------------|--------|-------------------------------------------------|---------|
 |            |        |                                                 |         |
 | 14/06/2024 | 00AC   | New Board with 12 DIs & 16 DOs                  |         |
 |            |        |                                                 |         |
 | 21/03/2023 | 00AB   | 12 PULSE CONTROL                                |         |
 |            |        |     											 |		   |
 | 10/01/2023 | 00AA   | Initial Version								 |		   |
 |            |        |        										 |		   |
 **********************************************************************************/

/*------------------------------------------------------------------------------
 External Function Declarations here
------------------------------------------------------------------------------*/

#define	CCEXTERN
#define	INIT0	    = 0
#define	INITARRAY0  = {0}
#define	rbuf	    spiReadBuffer
#define	wbuf	    spiWriteBuffer

#include "f28x_project.h"
#include "../ccVariables.h"
#include "device.h"
#include "math.h"
#include "board_pin_map.h"


extern __interrupt void MCANIntr1ISR(void);
extern interrupt void AdcISR(void);
extern __interrupt void SCIARx_ISR(void);
extern __interrupt void SCIBRx_ISR(void);
extern __interrupt void cpu_timer0_isr(void);
extern __interrupt void cpu_timer1_isr(void);
extern __interrupt void cpu_timer2_isr(void);

#pragma DATA_SECTION(ArrayParamsI, "BIG_ARRAYS")
#pragma DATA_SECTION(ArrayParamsF, "BIG_ARRAYS")
#pragma DATA_SECTION(waveBuffer, "BIG_ARRAYS")
#pragma DATA_SECTION(rx_buffer, "BIG_ARRAYS")

void    UpdateBatteryLevels(void);
//inline void ControlLoop(void);
extern const PARAM *paramptr;
//------------------------------------------------------------------------------
/*------------------------------------------------------------------------------
 Functions that will be run from FLASH linker cmd file.
 ------------------------------------------------------------------------------*/
// 12345678901234567890

extern char *defaultSerial;
volatile Uint16 oldSec = 0;
volatile Uint16 result = 0;
uint32_t base = SPIA_BASE;

N16 a =10, b =20, c= 30, d = 0;

Uint16 notEnabled = TRUE;
float vdcRef_old = 0;
float ibatref_old = 0;

#define PULSEFILL_COUNT 5000

 extern void ePWM7_Init();
  N16    secNow = 0;
 void main(void)
 {
	//Device_init();
	InitSysCtrl(); // Initialize System Control....PLL, WatchDog, enable Peripheral Clocks
	InitGpio();
	InitVariables();
	cyclePoints = 200;

	InitPieCtrl(); // Initialize the PIE control registers to their default state.
	InitCpuTimers();

	DINT;
	//--------------------------------------------------------------------------
	EALLOW;       // Clear all interrupts and initialize PIE vector table:
        IER = 0x0000; // Disable CPU interrupts and clear all CPU interrupt flags:
        IFR = 0x0000;
	EDIS;
	//--------------------------------------------------------------------------

	InitPieVectTable(); // Initialize the PIE vector table with pointers
    GpioInit(); // Initialize GPIO 00 for Yphase PWM output
    SysCtl_setMCANClk(SYSCTL_MCANA, SYSCTL_MCANCLK_DIV_3);
    SysCtl_setMCANClk(SYSCTL_MCANB, SYSCTL_MCANCLK_DIV_3);
    ePWM7_Init();
  //  EPWM8_TimebaseInit_WithCmpInt();
    initZCD_ECAP1_GPIO47();

	EALLOW;
	// Interrupts are re-mapped to ISR functions

	PieVectTable.TIMER0_INT = &cpu_timer0_isr;
	PieVectTable.TIMER1_INT = &cpu_timer1_isr;
	PieVectTable.TIMER2_INT = &cpu_timer2_isr;
	PieVectTable.ADCA1_INT = &AdcISR;

	PieVectTable.SCIA_RX_INT = &SCIARx_ISR;
	PieVectTable.SCIB_RX_INT = &SCIBRx_ISR;
	PieVectTable.MCANA_0_INT = &MCANIntr1ISR;
	PieVectTable.MCANA_1_INT = &MCANIntr1ISR;

    AdcaRegs.ADCINTSEL1N2.bit.INT1E = 1;
    AdcaRegs.ADCINTSEL1N2.bit.INT1SEL = 6;

    //ConfigCpuTimer(&CpuTimer0, SYSTEM_FREQUENCY, 50.0);
    ConfigCpuTimer(&CpuTimer2, SYSTEM_FREQUENCY, 1000);
    //ConfigCpuTimer(&CpuTimer2, SYSTEM_FREQUENCY, 10000);

	CpuTimer0Regs.TCR.all = 0x4000;
	CpuTimer1Regs.TCR.all = 0x4000;
	CpuTimer2Regs.TCR.all = 0x4000;

	// Enable EPwm INTn in the PIE: Group 3 interrupt 1-3
	PieCtrlRegs.PIEIER1.bit.INTx1 = 1; // Enable INT 1.1 ADCA
    PieCtrlRegs.PIEIER9.bit.INTx1 = 1; // PIE group 9 - for SCIA RX
    PieCtrlRegs.PIEIER9.bit.INTx3 = 1; // PIE group 9 - for SCIB RX
    PieCtrlRegs.PIEIER9.bit.INTx10 = 1;  //MCAN A RX

	PieCtrlRegs.PIEIER1.bit.INTx7 = 1; // Timer0

    InitDrives(1);
    InitDrives(2);
    InitDrives(3);
    InitDrives(4);
    InitDrives(5);
    InitDrives(6);

    ALL_DRIVES_OFF();
    DISABLE_PWM();

    IER |= M_INT1;  // Enable CPU Interrupt 0
    IER |= M_INT3;  // Enable CPU INT3 for PWM
	IER |= M_INT9;  //SCI-A CANA
	IER |= M_INT13; // Enable CPU Timer 1;
	IER |= M_INT12; // Enable CPU Timer 1;
	IER |= M_INT14; // Enable CPU Timer 2;
	EDIS;

    ConfigureADC();
    setupADC(ADC_TRIGGER_EPWM7_SOCA, 44); //EPM1 Trigger

	XintRegs.XINT1CR.bit.ENABLE = FALSE;    // Enable XINT1#
	XintRegs.XINT2CR.bit.ENABLE = FALSE;    // Enable XINT2
	XintRegs.XINT3CR.bit.ENABLE = FALSE;    // Enable XINT3

	EINT;	// Enable global Interrupts and higher priority
	ERTM;

	//Initialising Params
	delay_ms(500);
    spi_init();
    delay_ms(100);

	RestoreSettings();

	msDelayCount = 20;
    while (msDelayCount); // wait for 0.05 sec

    RestoreSettingsInt();
    updateRAMvalues(0); //setting parameters

    InitSCIA(BaudRate);
    InitSCIB();
    if((DeviceID>0) && (DeviceID<3))
    {
        address = DeviceID;
        address1 = DeviceID;
    }
    else { address = 1; address1 = 1; }

    InitHiLoParams();
    InitRelays();

    DisableTX();
    InitPLL(&pllGrid);

    LimitFlag.all  = 0x0000;    //setting Low
    SystemFlag.bit.POWERINGUP = TRUE;
 //   CAN_enableGlobalInterrupt(CANA_BASE, CAN_GLOBAL_INT_CANINT0);
 //   UnitOnCommand = FALSE;
    Batt_TestMode = FALSE;
    DO_TestFlag = FALSE;

#if RTC_PRESENT
    ReadTime();
    secNow = RTCSecond;
    delay_ms(1050);
    ReadTime();
    if(secNow == RTCSecond) EnableRTCClock();
#else
    delay_ms(1050);     // no RTC (mcp79522.c excluded): keep the start-up timing
#endif

    InitMCAN();
    MCAN_enableIntr(MCANA_DRIVER_BASE, MCAN_INTR_MASK_ALL, 1U);
    MCAN_selectIntrLine(MCANA_DRIVER_BASE, MCAN_INTR_MASK_ALL, MCAN_INTR_LINE_NUM_1);
    MCAN_enableIntrLine(MCANA_DRIVER_BASE, MCAN_INTR_LINE_NUM_1, 1U);
    freqNotSet = TRUE;
 //   InitCapture();
    vdc_inc = 0.004;
    idc_inc = 0.004;
    ManualModeSelect = 0;
    OpenLoop = 0;

    count30_SP1 = 2083;
    count30_SP2 = 2083;
    count_60 = 4166;
    count_30 = count_60 >> 1;

    if(GridFrequency == 60)
    {
        count30_SP1 = 1736;
        count30_SP2 = 1736;
        count_60 = 3472;
        count_30 = count_60 >> 1;
    }

    OffsetNotDone = TRUE;
    FilledPulse = TRUE;

	while (1)
	{
		InverterApplication();
		UpdateDigitalIO();
		updateStatus();
		EventLogging();
		storeEvents();

        ModbusProcessing();
        ModbusProcessing1();
        MCANBusProcessing();


        if (rms_update_flag)
            {
                update_rmsmeters();
                rms_update_flag = 0;
            }

        if (seconds!= seconds_old)
        {
            seconds_old = seconds;
#if RTC_PRESENT
            ReadTime();
#endif
          //  updateRelays();

            if(Vdc_Ref> 0.05) vdc_inc = Vdc_Ref * 0.01;
            if(Idc_Ref> 0.05) idc_inc = Idc_Ref * 0.01;

            if((seconds%30)==0) {
                                    InitSCIA(BaudRate);
                                    InitECAN();
                                    InitMCAN();
                                    InitSCIB();
                                }

            if(errorRx) InitSCIA(BaudRate); //BaudRate

            if(LimitFlag.bit.CANFAIL && (canInitTimer==0))
            {
                InitECAN();
                InitMCAN();
                canInitTimer = 5000;
            }

                if(specialCommnad == CMD_DEFAULT)
                    {
                        SetFactoryDefaults();
                        SetFactoryDefaultsInt();
                        specialCommnad = 0;
                    }
                if(address==1) CAN_TxInterval = 1;
                else CAN_TxInterval = 20;
        } //seconds

        if(autoSaveFlag)
        {
            UpdateOneInt(ADR_COMMIT, 1);
            autoSaveFlag = FALSE;
        }

//        if(doPID)
//        { ControlLoop(); doPID = FALSE; }

    }   //End of Infinite for loop
}       // End Of Main

Uint16 noInit = 0;
 void InverterApplication(void)
 {
     if (SystemFlag.bit.POWERINGUP)
     {
         notEnabled = TRUE;
         reason = 1;
         pwmEnabled = FALSE;
         return;
     }

     if(SystemFlag.bit.RECTON && (WaitDelayTimer == 0) ) SystemFlag.bit.UNITON = TRUE;
     else SystemFlag.bit.UNITON = FALSE;

     if(!SystemFlag.bit.RECTON)
     {
             GridFrequency = 50.0;
             OmegaDefault = 0.015707963;
             PLL_Lock_Min = 0.014922565;
             PLL_Lock_Max = 0.016493361;
             FullCycle = 400;
             MinPoints = 360;
             MinPeriod = 16;

        freqNotSet = FALSE;
     }


     if(runModeSelect) { SM_FloatBoost(); runModeSelect = FALSE; }

     if((SysOkFlag.bit.MAINSOK || TestMode) && !SystemFlag.bit.WRONGSEQ &&
             (UnitOnCommand || unitOncmd) && !SystemFlag.bit.TRIPFAULT )
     {

         if(!SystemFlag.bit.RECTON)
         {
             soft_VdcRef = vdcFbk;
             soft_IdcRef = IdcFback;
             soft_IbatRef = ibatFbk;

       //  if(soft_VdcRef < (Vdc_Ref * 0.25))
         {
             InitPID(&pid_vdc);
             InitPID(&pid_idc);
             InitPID(&pid_idc1);
             InitPID(&pid_idc2);
             InitPID(&pid_ibat);
         }
             CycleCount = 0;
             pwmEnabled = TRUE;
             ncycle = 0;
             SystemFlag.bit.SOFTSTART = TRUE;
             reason = 2;
             SystemFlag.bit.RECTON = TRUE;
             opMode = modeFLOAT;
             BattFlag.bit.FLOATMODE = TRUE;
         }
         reason = 3;
     }
     else
     {
         SystemFlag.bit.RECTON = FALSE;
         pwmEnabled = FALSE;
         soft_VdcRef = vdcFbk;
         soft_IdcRef = IdcFback;
         soft_IbatRef = ibatFbk;
         DISABLE_PWM();
         RP1 = RN1 = YP1 = YN1 = BP1 = BN1 = 0;
         RP2 = RN2 = YP2 = YN2 = BP2 = BN2 = 0;
     }


     return;
 }
 N16 tmpRelay1 = 0;
 N16 tmpRelay2 = 0;
 N16 tmpRelay3 = 0;
 N16 tmpTripRelay1 = 0;

void    UpdateRelayBits(void)
{
    N16 u=0;
    tmpRelay1 = 0;
    tmpRelay2 = 0;
    tmpRelay3 = 0;

    for(u=4;u<12;u++)
         {
            if(ArrayParamsI[MAIN_RLY_SIG_START + u])
            {
              if(DOParams[ArrayParamsI[MAIN_RLY_SIG_START + u]-1])
                 {
                     MainConRLY[u].Enable = TRUE;
                 }
              else
                  {
                     MainConRLY[u].Enable = FALSE;
                     MainConRLY[u].RelayOn = FALSE;
                  }
            }
            else {
                    MainConRLY[u].Enable = FALSE;
                    MainConRLY[u].RelayOn = FALSE;
                }

             if(MainConRLY[u].RelayOn) tmpRelay1 |= (1 << u);
         }



    for(u=0;u<16;u++)
         {
        if(ArrayParamsI[IOM1_RLY_SIG_START + u])
        {
            if(DOParams[ArrayParamsI[IOM1_RLY_SIG_START + u]-1]) IOM1RLY[u].Enable = TRUE;
             else { IOM1RLY[u].Enable = FALSE; IOM1RLY[u].RelayOn = FALSE; }
        }
        else {
                IOM1RLY[u].Enable = FALSE;
                IOM1RLY[u].RelayOn = FALSE;
            }

        if(ArrayParamsI[IOM2_RLY_SIG_START + u])
        {
             if(DOParams[ArrayParamsI[IOM2_RLY_SIG_START + u]-1]) IOM2RLY[u].Enable = TRUE;
             else {IOM2RLY[u].Enable = FALSE;  IOM2RLY[u].RelayOn = FALSE; }
         }
         else { IOM2RLY[u].Enable = FALSE; IOM2RLY[u].RelayOn = FALSE; }

             if(IOM1RLY[u].RelayOn) tmpRelay2 |= (1 << u);
             if(IOM2RLY[u].RelayOn) tmpRelay3 |= (1 << u);

         }

    tmpTripRelay1 = 0;

    for(u=0;u<10;u++)
            {
              if(ArrayParamsI[TRIP_SIG_START + u])
              {
               if(DOParams[ArrayParamsI[TRIP_SIG_START + u]-1]) tmpTripRelay1 |= (1 << u);
              }
            }
    TripBitsFlag = tmpTripRelay1;

    if(Vdc >= Rly1_On_Level) RlyLevelFlag |= BIT0;
    if(Vdc < Rly1_Off_Level) RlyLevelFlag &= BITC0;
    if(Vdc >= Rly2_On_Level) RlyLevelFlag |= BIT1;
    if(Vdc < Rly2_Off_Level) RlyLevelFlag &= BITC1;
    if(Vdc >= Rly3_On_Level) RlyLevelFlag |= BIT2;
    if(Vdc < Rly3_Off_Level) RlyLevelFlag &= BITC2;
    if(Vdc >= Rly4_On_Level) RlyLevelFlag |= BIT3;
    if(Vdc < Rly4_Off_Level) RlyLevelFlag &= BITC3;

    RelayFlag = tmpRelay1 | (RlyLevelFlag & 0x000f);
    iom1_relayflag1 = tmpRelay2 & 0xff;
    iom1_relayflag2 = tmpRelay2 >> 8;

    iom2_relayflag1 = tmpRelay3 & 0xff;
    iom2_relayflag2 = tmpRelay3 >> 8;

}


volatile N16 TimerRunning = 0;
 void updateStatus(void)
 {
     LimitFlag.bit.MAINSLOW = chkVr.stat.bit.LOW || chkVy.stat.bit.LOW || chkVb.stat.bit.LOW;
     LimitFlag.bit.MAINSHIGH = chkVr.stat.bit.HIGH || chkVy.stat.bit.HIGH || chkVb.stat.bit.HIGH;

     LimitFlag.bit.VDCLOW = chkVdc.stat.bit.LOW;
     LimitFlag.bit.VDCHIGH = chkVdc.stat.bit.HIGH;


     LimitFlag.bit.VBATHIGH = chkVbat.stat.bit.HIGH;
     LimitFlag.bit.VBATLOW = chkVbat.stat.bit.LOW;

     LimitFlag.bit.IDCHIGH = chkIdc.stat.bit.HIGH;
     LimitFlag.bit.IBATHIGH = chkIbat.stat.bit.HIGH;

     LimitFlag.all &= 0x3ff;
     if(LmtMode==1) LimitFlag.bit.CURMODE = TRUE;
     else if(LmtMode==2) LimitFlag.bit.IBATMODE = TRUE;
     else if(LmtMode==3) LimitFlag.bit.CURMODE = TRUE;
     else if(LmtMode==4) LimitFlag.bit.CURMODE = TRUE;
     else LimitFlag.bit.VOLMODE = TRUE;

     SysOkFlag.bit.VROK = chkVr.stat.bit.OK;
     SysOkFlag.bit.VYOK = chkVy.stat.bit.OK;
     SysOkFlag.bit.VBOK = chkVb.stat.bit.OK;

     GoodPhases = 0;
     if(SysOkFlag.bit.VROK) GoodPhases++;
     if(SysOkFlag.bit.VYOK) GoodPhases++;
     if(SysOkFlag.bit.VBOK) GoodPhases++;
     if(GoodPhases == 2) SystemFlag.bit.SINGLEPH = TRUE;
     else SystemFlag.bit.SINGLEPH = FALSE;

     if((seqFailTimer > 40) && SysOkFlag.bit.MAINSOK) SystemFlag.bit.WRONGSEQ = TRUE;
     if((seqFailTimer < 5) || !SysOkFlag.bit.MAINSOK) SystemFlag.bit.WRONGSEQ = FALSE;

     SysOkFlag.bit.MAINSOK = chkVr.stat.bit.OK && chkVy.stat.bit.OK && chkVb.stat.bit.OK;
     SysOkFlag.bit.MAINSFAIL = !SysOkFlag.bit.MAINSOK;
     SysOkFlag.bit.CHRGRFAIL = LimitFlag.bit.VDCHIGH || LimitFlag.bit.VDCLOW;

     SystemFlag.bit.INPOVERLOAD = chkIr.stat.bit.HIGH || chkIy.stat.bit.HIGH || chkIb.stat.bit.HIGH;
     if(CANFailDelay == canFailureCount) LimitFlag.bit.CANFAIL = TRUE;
     else LimitFlag.bit.CANFAIL = FALSE;
     SystemFlag.bit.BITSVALID = TRUE;

     if(iom1_canFailTimer==0) {IOM1_DI_Flag1 = 0; IOM1_DI_Flag2 = 0;}
     if(iom2_canFailTimer==0) {IOM2_DI_Flag1 = 0; IOM2_DI_Flag2 = 0;}

     if((AlarmFlag1 & CommonAlarmBits1) || (AlarmFlag2 & CommonAlarmBits2) || (AlarmFlag3 & CommonAlarmBits3) ||
             (AlarmFlag4 & CommonAlarmBits4) || (AlarmFlag5 & CommonAlarmBits5)
             || (AlarmFlag6 & CommonAlarmBits6)) LimitFlag.bit.COMMONALARM = TRUE;
             else LimitFlag.bit.COMMONALARM = FALSE;

     if(!SystemFlag.bit.RECTON) SystemFlag.bit.SOFTSTART = FALSE;
     AlarmFlag1 = SystemFlag.all;
     AlarmFlag2 = LimitFlag.all;
     AlarmFlag3 = DigitalInputFlag1 & 0xfff;
     AlarmFlag4 = SysOkFlag.all;AlarmFlag4 <<= 8;
     AlarmFlag4 |=BattFlag.all;
     AlarmFlag5 = IOM1_DI_Flag2; AlarmFlag5 <<= 8; AlarmFlag5 |= IOM1_DI_Flag1;
     AlarmFlag6 = IOM2_DI_Flag2; AlarmFlag6 <<= 8; AlarmFlag6 |= IOM2_DI_Flag1;
     AlarmFlag7 = RelayFlag;

     UpdateBits();
   //  UpdateRelayBits();

     if (TripBitsFlag || (DigitalInputFlag1 & cfgTripFlag))
         {
             if(!SystemFlag.bit.TRIPFAULT)
                 {
                     if(TripCount<MaxRestartCount) TripCount++;
                     //if(AlarmFlag3 & TripBits) TripCount = MaxRestartCount;
                     systemRunningTimer = 0;
                     restartTimer = RestartDelay;
                     SystemFlag.bit.TRIPFAULT = TRUE;
                     unitOncmd = FALSE;
                     if(TripCount == MaxRestartCount) SystemFlag.bit.LATCHFAULT = TRUE;
                 }
         }

     if (SystemFlag.bit.TRIPFAULT)
     {
         SystemFlag.bit.RECTON = FALSE;
         DISABLE_PWM();
         pwmEnabled = FALSE;
         reason= 2;
         return;
     }


  //   AlarmFlag4 = SysOkFlag.all;
 }

N16 NewPress = 1;
N16 oldBattFlag = 1;
void UpdateDigitalIO(void)
{
    N16 tempiFlag = 0;

    tempiFlag = DigitalInputFlag1;

    if (DI1timer == DI1DELAY)   tempiFlag |= BIT00;
    else if (DI1timer == 0)     tempiFlag &= BITC0;
    if (DI2timer == DI2DELAY)   tempiFlag |= BIT01;
    else if (DI2timer == 0)     tempiFlag &= BITC1;

    if (DI3timer == DI3DELAY)   tempiFlag |= BIT02;
    if((cfgAutoResetFlag & BIT02) && (DI3timer == 0)) tempiFlag &= BITC2;
    if (DI4timer == DI4DELAY)   tempiFlag |= BIT03;
    else if ((cfgAutoResetFlag & BIT03) && (DI4timer == 0)) tempiFlag &= BITC3;
    if (DI5timer == DI5DELAY)   tempiFlag |= BIT04;
    else if ((cfgAutoResetFlag & BIT04) && (DI5timer == 0))     tempiFlag &= BITC4;
    if (DI6timer == DI6DELAY)   tempiFlag |= BIT05;
    else if ((cfgAutoResetFlag & BIT05) && (DI6timer == 0))     tempiFlag &= BITC5;
    if (DI7timer == DI7DELAY)   tempiFlag |= BIT06;
    else if ((cfgAutoResetFlag & BIT06) && (DI7timer == 0))     tempiFlag &= BITC6;
    if (DI8timer == DI8DELAY)   tempiFlag |= BIT07;
    else if ((cfgAutoResetFlag & BIT07) && (DI8timer == 0))     tempiFlag &= BITC7;
    if (DI9timer == DI9DELAY)   tempiFlag |= BIT08;
    else if ((cfgAutoResetFlag & BIT08) && (DI9timer == 0))     tempiFlag &= BITC8;
    if (DI10timer == DI10DELAY) tempiFlag |= BIT09;
    else if ((cfgAutoResetFlag & BIT09) && (DI10timer == 0))    tempiFlag &= BITC9;

    if (DI11timer == DI11DELAY) tempiFlag |= BIT10;
    else if ((cfgAutoResetFlag & BIT10) && (DI11timer == 0))    tempiFlag &= BITC10;
    if (DI12timer == DI12DELAY) tempiFlag |= BIT11;
    else if ((cfgAutoResetFlag & BIT11) && (DI12timer == 0))    tempiFlag &= BITC11;

    DigitalInputFlag1 = tempiFlag;


if(DI_ModeSelect)
{
    if(DigitalInputFlag1 & DI_AUTO) ManualModeSelect = FALSE;
    else ManualModeSelect = TRUE;

    if(DigitalInputFlag1 & DI_FLOAT) ForcedFloat = TRUE;
    else ForcedFloat = FALSE;
    if(DigitalInputFlag1 & DI_BOOST) ForcedBoost = TRUE;
    else ForcedBoost = FALSE;
    if(DigitalInputFlag1 & DI_BT) ForcedBatTest = TRUE;
    else ForcedBatTest = FALSE;
    if(DigitalInputFlag1 & DI_EQ) ForcedEqualize = TRUE;
    else ForcedEqualize = FALSE;
}

    if(DigitalInputFlag1 & DI_UNITON) unitOncmd = TRUE;
    else unitOncmd = FALSE;

    if((DigitalInputFlag1 & DI_RESET) && NewPress)
        {
            RESET_ALL();
            NewPress = FALSE;
        }

    if((DigitalInputFlag1 & DI_RESET)==0) NewPress = TRUE;

    if(ExtRefEnable==1)
    {
        if(DigitalInputFlag1 & DI_CPCC)
        {
            SystemFlag.bit.CCMODE = TRUE;
            SystemFlag.bit.CVMODE = FALSE;
        }
        else
        {
            SystemFlag.bit.CCMODE = FALSE;
            SystemFlag.bit.CVMODE = TRUE;

        }
    }

    if(ManualModeSelect)
    {
        if(BattFlag.all ^ oldBattFlag)
            updateRAMvalues(0);

        oldBattFlag = BattFlag.all;
    }



}

void updateRelays(void)
{
    // Common Controller v2: RelayFlag bit n -> RLY_(n+1) (board_pin_map.h)
    if(RelayFlag & 0x0001)    GPIO_WritePin(RLY1_GPIO, 1);
    else                    GPIO_WritePin(RLY1_GPIO, 0);
    if(RelayFlag & 0x0002)    GPIO_WritePin(RLY2_GPIO, 1);
    else                    GPIO_WritePin(RLY2_GPIO, 0);
    if(RelayFlag & 0x0004)    GPIO_WritePin(RLY3_GPIO, 1);
    else                    GPIO_WritePin(RLY3_GPIO, 0);
    if(RelayFlag & 0x0008)    GPIO_WritePin(RLY4_GPIO, 1);
    else                    GPIO_WritePin(RLY4_GPIO, 0);
    if(RelayFlag & 0x0010)    GPIO_WritePin(RLY5_GPIO, 1);
    else                    GPIO_WritePin(RLY5_GPIO, 0);
    if(RelayFlag & 0x0020)    GPIO_WritePin(RLY6_GPIO, 1);
    else                    GPIO_WritePin(RLY6_GPIO, 0);
    if(RelayFlag & 0x0040)    GPIO_WritePin(RLY7_GPIO, 1);
    else                    GPIO_WritePin(RLY7_GPIO, 0);
    if(RelayFlag & 0x0080)    GPIO_WritePin(RLY8_GPIO, 1);
    else                    GPIO_WritePin(RLY8_GPIO, 0);
    if(RelayFlag & 0x0100)    GPIO_WritePin(RLY9_GPIO, 1);
    else                    GPIO_WritePin(RLY9_GPIO, 0);
    if(RelayFlag & 0x0200)    GPIO_WritePin(RLY10_GPIO, 1);
    else                    GPIO_WritePin(RLY10_GPIO, 0);
    if(RelayFlag & 0x0400)    GPIO_WritePin(RLY11_GPIO, 1);
    else                    GPIO_WritePin(RLY11_GPIO, 0);
    if(RelayFlag & 0x0800)    GPIO_WritePin(RLY12_GPIO, 1);
    else                    GPIO_WritePin(RLY12_GPIO, 0);

}


void    updateEEPROM_8(N16 start_addr)
{
    spiWriteBuffer[0] = ArrayParamsI[st_addr+0];
    spiWriteBuffer[1] = ArrayParamsI[st_addr+1];
    spiWriteBuffer[2] = ArrayParamsI[st_addr+2];
    spiWriteBuffer[3] = ArrayParamsI[st_addr+3];
    spiWriteBuffer[4] = ArrayParamsI[st_addr+4];
    spiWriteBuffer[5] = ArrayParamsI[st_addr+5];
    spiWriteBuffer[6] = ArrayParamsI[st_addr+6];
    spiWriteBuffer[7] = ArrayParamsI[st_addr+7];
    eeprom_write_words(EP_PARAM_START + ((start_addr - WR_START)<<1), spiWriteBuffer, 8);
    DELAY_US(10000);
}


N8 toHex(N8 k) {
	N8 s;
	s = k;
	return ((s % 10) | ((s / 10) << 4));
}

float Average32(LPF32 *x, float val)
    {
        N16 c=0;
        x->larray[x->lptr] = val;
        x->lptr++;
        x->lptr &= 0x1f;
        x->lsum = 0;
        for(c=0;c<32;c++)
        x->lsum += x->larray[c];
        x->y =  (x->lsum /32);
        return(x->y);
    }

float Average8(LPF32 *x, float val)
    {
        N16 c=0;
        x->larray[x->lptr] = val;
        x->lptr++;
        x->lptr &= 0x7;
        x->lsum = 0;
        for(c=0;c<8;c++)
        x->lsum += x->larray[c];
        x->y =  (x->lsum * 0.125);
        return(x->y);
    }


#define GET_RMS(x)  \
        tempVar = x.savedSum / rms_samplecount; \
        x.RMS = sqrt(tempVar)

int  FinMax3(int a,int b, int c)
    {
        if((a>b) && (a>c)) return(a);
        if((b>a) && (b>c)) return(b);
        return(c);
    }

#define UPDATE_QRMS_F(x,y,z)  z = sqrt(x.savedSum/qrtrCycle)*y
#define UPDATE_RMS_F(x,y,z)  z = sqrt(x.savedSum/rms_samplecount)*y
#define UPDATE_KW_F(x,z)  z = (x.savedSum/rms_samplecount)*.001
#define SCALEF(x,y,z)  z = x*y
#define scaleKVA(v,i,k) k = v*i*.001

#define SCALE_RMSVAL_FLT(meter, sf,flt,v)   \
        meter.RMS = sqrt(meter.savedSum/rms_samplecount); \
        rawval1 = meter.RMS * sf; \
        v = Average32(&flt, rawval1)

#define SCALE_DCVAL_FLT(lpf, sf,flt,v)   \
        rawval1 = lpf*sf; \
        v = Average32(&flt, rawval1)

#define SCALE_DCVAL_FLT8(lpf, sf,flt,v)   \
        rawval1 = lpf*sf; \
        v = Average8(&flt, rawval1)

long count_diff  = 0;

void update_rmsmeters(void)
{
            float32_t   rawval1 = 0;
            int count_60_1 = 0;
            int manualBalCount = 0;

            count_60_1 = (int) (periodCount/(6*120));
            count_60_Sum += count_60_1;
            count60_count++;

#define ECAP_CLK_HZ  150000000.0f      // eCAP timebase = SYSCLK; confirm on your part

            if(periodCount<100000L) MainsFrequency = 0;
            else
            {
                MainsFrequency = ECAP_CLK_HZ / (float)periodCount;
            }

            if(count60_count >=32)
            {
                count_60 = count_60_Sum >> 5;
                count_60_Sum = 0;
                count60_count = 0;
                count_30 = count_60 >> 1;
                count30SP = count_30;// - PulseWidth;

            }

       if(ManualBalancing)
       {
           // Bridge2AngleAdjust (deg, bipolar): +ve delays bridge 2,
           // -ve advances it; SP1+SP2 stays 60 deg
           manualBalCount = (int) (Bridge2AngleAdjust * count_30 / 30.0);
           count30_SP1 = count_30 + manualBalCount;
           count30_SP2 = count_30 - manualBalCount;
       }
       else
       {
        if(BalancingDirection)
        {
            count30_SP1 = count_30 + Bridge2LagCount - Bridge2LeadCount;
            count30_SP2 = count_30 - Bridge2LagCount + Bridge2LeadCount;
        }
        else
        {
            count30_SP1 = count_30 - Bridge2LagCount + Bridge2LeadCount;
            count30_SP2 = count_30 + Bridge2LagCount - Bridge2LeadCount;
        }
            }

            if(rms_samplecount >MinPoints)
            {
                SCALE_RMSVAL_FLT(volt_meter_R,VinpRSF,lpf32_vr,VinpR);
                SCALE_RMSVAL_FLT(volt_meter_Y,VinpYSF,lpf32_vy,VinpY);
                SCALE_RMSVAL_FLT(volt_meter_B,VinpBSF,lpf32_vb,VinpB);

                SCALE_RMSVAL_FLT(vll_meter_R,VinpRSF,lpf32_vry,VinpRY);
                SCALE_RMSVAL_FLT(vll_meter_Y,VinpYSF,lpf32_vyb,VinpYB);
                SCALE_RMSVAL_FLT(vll_meter_B,VinpBSF,lpf32_vbr,VinpBR);

                SCALE_RMSVAL_FLT(cur_meter_R,IinpRSF,lpf32_ir,IinpR);
                SCALE_RMSVAL_FLT(cur_meter_Y,IinpYSF,lpf32_iy,IinpY);
                SCALE_RMSVAL_FLT(cur_meter_B,IinpBSF,lpf32_ib,IinpB);


                //vdcRipple_meter.RMS = sqrt(vdcRipple_meter.savedSum/rms_samplecount);
                //VdcRipple = vdcRipple_meter.RMS * VdcSF;

                SCALE_RMSVAL_FLT(vdcRipple_meter,VdcSF,lpf32_vripple,VdcRipple);

                if(VinpRY<MinVoltage) VinpRY = 0;
                if(VinpYB<MinVoltage) VinpYB = 0;
                if(VinpBR<MinVoltage) VinpBR = 0;

                if(IinpR<MinCurrent) IinpR = 0;
                if(IinpY<MinCurrent) IinpY = 0;
                if(IinpB<MinCurrent) IinpB = 0;

                UPDATE_KW_F(kw_meter_R, kW_Rph);
                UPDATE_KW_F(kw_meter_Y, kW_Yph);
                UPDATE_KW_F(kw_meter_B, kW_Bph);

            //KVA Calculation:
            scaleKVA(VinpR, IinpR, kVA_Rph);
            scaleKVA(VinpY, IinpY, kVA_Yph);
            scaleKVA(VinpB, IinpB, kVA_Bph);


        //POWERFACTOR SCALING:
            if (kVA_Rph<kW_Rph) kVA_Rph = kW_Rph;
            if (kVA_Yph<kW_Yph) kVA_Yph = kW_Yph;
            if (kVA_Bph<kW_Bph) kVA_Bph = kW_Bph;
            if(kVA_Rph > 2.0) powerfactorR = kW_Rph/ kVA_Rph;
            else powerfactorR = 1.0;
            if(kVA_Yph > 2.0) powerfactorY = kW_Yph/ kVA_Yph;
            else powerfactorY = 1.0;
            if(kVA_Bph > 2.0) powerfactorB = kW_Bph/ kVA_Bph;
            else powerfactorB = 1.0;

            kVA_Total = kVA_Rph + kVA_Yph + kVA_Bph;
            kW_Total = kW_Rph + kW_Yph + kW_Bph;


        } //min points

        SCALE_DCVAL_FLT(lpf_vbat.y,VbatSF, lpf32_vbat, Vbat);
        SCALE_DCVAL_FLT(ibatFbk,IbatSF, lpf32_ibat, Ibat);
        SCALE_DCVAL_FLT(lpf_vload.y,VloadSF, lpf32_vload, LoadVoltage);

        SCALE_DCVAL_FLT(vdcFbk,VdcSF, lpf32_vdc, Vdc);
        SCALE_DCVAL_FLT(idc1Fbk,Idc1SF, lpf32_idc1, Idc1);
        SCALE_DCVAL_FLT(idc2Fbk,Idc2SF, lpf32_idc2, Idc2);

        //Average8
        if(CurrentSensingMode) IdcTotal = Idc1 + Idc2;
        else IdcTotal = Idc1;

        if(IdcTotal>Ibat) LoadCurrent  = IdcTotal - Ibat;

        SCALEF(lpf_temp1.y,Temp1SF, BattTemp);
        SCALEF(lpf_temp2.y,Temp2SF, Temp2);
        SCALEF(lpf_temp3.y,Temp3SF, Temp3);     // ambient, display only

//        MainsFrequency =  OMEGAFACTOR * pllGrid.omega;
//        if((MainsFrequency >= 47) && (MainsFrequency <= 62)) SysOkFlag.bit.SYNCOK = TRUE;
//        else SysOkFlag.bit.SYNCOK  = FALSE;

        vdc_ref        = pid_vdc.Ref;
        vdc_fback      = pid_vdc.Fbk;
        vdc_pidout     = pid_vdc.Out;

        idc_ref        = pid_idc.Ref;
        idc_fback      = pid_idc.Fbk;
        idc_pidout     = pid_idc.Out;

        ibat_ref       = pid_ibat.Ref;
        ibat_fback     = pid_ibat.Fbk;
        ibat_pidout    = pid_ibat.Out;

        if(ExtRefEnable==1)
           {
               if(SystemFlag.bit.CCMODE)
               {
                   fact1 = extVref * 1.5;
                   if(fact1> 0.99) fact1 = 0.99;
                   ExtIbatRef = MinIbatRef * FloatIbatRef * 0.01 + (100 - MinIbatRef) * fact1 * FloatIbatRef * 0.01;
                   ExtVdcRef = FloatVRef;
               }
               else //CV MODE
               {
                   fact1 = extVref * 1.5;
                   if(fact1> 0.99) fact1 = 0.99;
                   ExtVdcRef = MinVdcRef * FloatVRef * 0.01 + (100 - MinVdcRef) * fact1 * FloatVRef * 0.01;
                   ExtIbatRef = FloatIbatRef;
               }
           }
        else if(ExtRefEnable==2)
        {
            fact1 = extIbatref * 1.5;
            if(fact1> 0.99) fact1 = 0.99;
            ExtIbatRef = MinIbatRef * FloatIbatRef * 0.01 + (100 - MinIbatRef) * fact1 * FloatIbatRef * 0.01;

            fact1 = extVref * 1.5;
            if(fact1> 0.99) fact1 = 0.99;
            ExtVdcRef = MinVdcRef * FloatVRef * 0.01 + (100 - MinVdcRef) * fact1 * FloatVRef * 0.01;
        }
}

void        updateRAMvalues(N16 adr)
{

   switch(adr)
    {
    case 0:
        if(DeviceID == 1)
        {
            DataAddr = DATAADDR_1;
            EventAddr = EVENTADDR_1;
            SetAddr = SETADDR_1;
            PidAddr1 = DATAADDR_2;
            PidAddr2 = DATAADDR_2 + 1;
        }
        else
        {
            DataAddr = DATAADDR_2;
            EventAddr = EVENTADDR_2;
            SetAddr = SETADDR_2;
            PidAddr1 = DATAADDR_1;
            PidAddr2 = DATAADDR_1 + 1;
        }

         pid_ibat.Kp = ibat_kp;
         pid_ibat.Ki = ibat_ki;
         pid_idc.Kp = idc_kp;
         pid_idc.Ki = idc_ki;
         pid_vdc.Kp = vdc_kp;
         pid_vdc.Ki = vdc_ki;

         lpf_vdc.T = Lpf_T_Vdc;
         lpf_idc1.T = Lpf_T_Idc;
         lpf_idc2.T = Lpf_T_Idc;
         lpf_ibat.T = Lpf_T_Idc;

         sfFactor = Idc2SF/ Idc1SF;

         Vdc_Ref = VdcRef /VdcSF;
         Idc_Ref = IdcRef /Idc1SF;
         Idc1_2_Ref = Idc_Ref * 0.60;

         Ibat_Ref = IbatRef /IbatSF;
         mainsFailLevel = VacLL/VinpRSF;
         ocLevel = IacHL/IinpRSF;
         Idc1_2_Ref = Idc_Ref * 0.6;
         break;

    case 120:
    case 123:
    case 169:
        Vdc_Ref = VdcRef /VdcSF;
        ReferenceChangedFlag = TRUE;
        break;

    case 121:
    case 170:
    case 171:
        Idc_Ref = IdcRef /Idc1SF;
        Idc1_2_Ref = Idc_Ref * 0.60;
        sfFactor = Idc2SF/ Idc1SF;
        ReferenceChangedFlag = TRUE;
        break;
    case 122:
        Ibat_Ref = IbatRef /IbatSF;;
        break;
    case 184://LPF IDC
//        lpf_idc1.T = LpfT_Idc;
//        lpf_idc2.T = LpfT_Idc;
        break;
    case 185:
        pid_vdc.Kp = vdc_kp;
        break;
    case 186:
        pid_vdc.Ki = vdc_ki;
        break;
    case 187:
        pid_idc.Kp = idc_kp;
        break;
    case 188:
        pid_idc.Ki = idc_ki;
        break;
    case 189:
        pid_ibat.Kp = ibat_kp;
        break;
    case 190:
        pid_ibat.Ki = ibat_ki;
        break;
    case 228:
        lpf_ifback.T = Lpf_T_Iref;
        lpf_pid.T = Lpf_T_Iref;
        break;
    case 229:
        lpf_vdc.T = Lpf_T_Vdc;
    case 230:
        lpf_idc1.T = Lpf_T_Idc;
        lpf_idc2.T = Lpf_T_Idc;
        lpf_ibat.T = Lpf_T_Idc;
        break;

    }

    return;
}


void storeEvents(void) {
    N16 *byteptr;
    N16 eventAddr = 0;
    Uint16 _ccount = 0;

    if (msgPending && ((milliseconds- last_stored_time) > 10 ))
    {
        RecordNumber++;
        if(RecordNumber > MAXPACKETS) RecordNumber = 0;
#if RTC_PRESENT
        WriteRecordPointer(RecordNumber);   // record pointer was kept in the RTC SRAM
#endif
        //store

        eventAddr = (RecordNumber << 6) + LOGSTART;
        byteptr = (N16 *) &EventTX;

        for(_ccount=0;_ccount < RECSIZE;_ccount++)
        spiWriteBuffer[_ccount] = *(byteptr + _ccount);

        eeprom_write_words(eventAddr, spiWriteBuffer, RECSIZE);
        msgPending = FALSE;
        last_stored_time = milliseconds;
      //  recordWriteTimer = SEC1;
    }

}


void EventLogging(void) {

    N16  j, ev = 0;
    N16 Event = 0;

    Event = 0;

    ev = AlarmFlag1 & EventMask1;
    j = ev & (~AlarmFlag1_old);
    if (j) Event = 1;
    AlarmFlag1_old = ev;

    ev = AlarmFlag2 & EventMask2;
    j = ev & (~AlarmFlag2_old);
    if (j) Event = 1;
    AlarmFlag2_old = ev;

    ev = AlarmFlag3 & EventMask3;
    j = ev & (~AlarmFlag3_old);
    if (j) Event = 1;
    AlarmFlag3_old = ev;



    if (Event && (eventCount < 3))
    {
        Events[eventCount].type = 0x1973;
        Events[eventCount].alarm1 = AlarmFlag1 & MsgMask1;
        Events[eventCount].alarm2 = AlarmFlag2 & MsgMask2;
        Events[eventCount].alarm3 = AlarmFlag3 & MsgMask3;
        Events[eventCount].alarm4 = AlarmFlag5 ;

        Events[eventCount].eVinpR = VinpRY;
        Events[eventCount].eVinpY = VinpYB;
        Events[eventCount].eVinpB = VinpBR;
        Events[eventCount].eIinpR = IinpR;
        Events[eventCount].eIinpY = IinpY;
        Events[eventCount].eIinpB = IinpB;

        Events[eventCount].eVdc = Vdc;
        Events[eventCount].eIdc = Idc1;
        Events[eventCount].eVbat = Vbat;
        Events[eventCount].eIbat = Ibat;
        Events[eventCount].eVload = LoadVoltage;
        Events[eventCount].eIload = LoadCurrent;
        Events[eventCount].eIbrdg1 = Idc1;
        Events[eventCount].eIbrdg1 = Idc2;
        Events[eventCount].kVA_R = kVA_Rph;
        Events[eventCount].kVA_Y = kVA_Yph;
        Events[eventCount].kVA_B = kVA_Bph;
        Events[eventCount].kW_R = kW_Rph;
        Events[eventCount].kW_Y = kW_Yph;
        Events[eventCount].kW_B = kW_Bph;
//        Events[eventCount].eKVA = InputkVA;
//        Events[eventCount].eKW = InputkW;
//        Events[eventCount].ePF = powerFactor;
//        Events[eventCount].eFrequency = Frequency;

        eventSerialNumber++;
        if(eventSerialNumber>= 1000) eventSerialNumber = 0;
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


void delay_ms(N16 n1)
    {
        msDelayCount = n1;
        while(msDelayCount);
        return;
    }

/*enum {
        F_NOFORCE = 0,
        F_EQUALIZE,
        F_BATTEST,
        F_BOOST,
        F_FLOAT
};
N16 ManualChargerMode = 0;
*/

float CalculateVdcRef(float BaseVRef) {
    float Temp_Delta = 0 ;
    float Temp_Compensation_V = 0 ;
    if (Batt_TempComp) {
        // Temp_Delta (degrees) * Batt_Drop_perDegree (-mV/degree) / 1000 (mV->V)
        Temp_Delta = Batt_Rated_Temp - BattTemp;
        Temp_Compensation_V = (Temp_Delta * Batt_Drop_perDegree) / 1000.0;
        return BaseVRef + Temp_Compensation_V;
    } else {
        return BaseVRef;
    }
}

// Charger States (Enumerate all 6 distinct phases)
typedef enum {
    STATE_IDLE,             // Waiting for a connection or command
    STATE_FLOAT,            // Normal maintenance charge
    STATE_BOOST,       // Main charging phase
    STATE_EQUALIZING,       // High-voltage cleaning phase
    STATE_DG_MODE,          // Dedicated generator battery maintenance
    STATE_BATTERY_TEST      // Running a discharge/recharge test
} ChargerState;

// Current State Tracking
ChargerState CurrentState = STATE_IDLE;

void SM_FloatBoost(void)
 {
    if(Auto_SetRefLevels)
    {
        Vfloat = NumberOfCells * CellFloatLevel;
        Vboost = NumberOfCells * CellBoostLevel;
        Veq = NumberOfCells * CellEqualizerLevel;
        Vdg = NumberOfCells * CellNoChargeLevel;
        Vtest = Batt_SetLevel;
        Ibatref = BatteryAH * ChargingLevel * 0.01;
    }
    else
    {
        Vfloat = FloatVRef;
        Vboost = BoostVRef;
        Veq = EqualiseVRef;
        Vdg = DGModeVRef;
        Vtest = Batt_SetLevel;
        Ibatref = FloatIbatRef;
    }

    if(opMode == modeBOOST) Ibatref = BoostIbatRef;

    if(ExtRefEnable) {Vfloat = ExtVdcRef; Ibatref = ExtIbatRef; }

    if(Auto_BoostEnable) AutoBoostCurLevel = Ibatref * BoostChangeoverLevel * 0.01;
    else AutoBoostCurLevel = Ibatref;

switch (CurrentState)
    {
            case STATE_IDLE:

                VdcRef = 0.0; // Output off
                IbatRef = 0.0; // Output off

                if(!SystemFlag.bit.POWERINGUP) CurrentState = STATE_FLOAT;
                break;

            case STATE_FLOAT:
                VdcRef = CalculateVdcRef(Vfloat);
                IbatRef = Ibatref; // IbatRef is minimal/maintenance current limit

                // Transition to Boost (Manual or Auto)
                if (Auto_BoostEnable && (Ibat > AutoBoostCurLevel))
                {
                    CurrentState = STATE_BOOST;
                    BoostTimer = BoostPeriod; // Reset timer on transition
                    ForcedBoost = false; // Consume manual command
                }
                opMode = modeFLOAT;
                break;

            case STATE_BOOST:
                VdcRef = CalculateVdcRef(Vboost);
                IbatRef = Ibatref; // High current limit

                if (Ibat < (AutoBoostCurLevel - IbatHysteresis))
                {
                    CurrentState = STATE_FLOAT;
                }
                // Boost Termination Condition 2: Safety Timer Exceeded
                if (BoostTimer == 0)
                { // Convert hours to ms
                    CurrentState = STATE_FLOAT;
                }
                opMode = modeBOOST;
                break;
        }


if(ManualModeSelect)
{
    BattFlag.all &= 0xfff0; //clearing mode bits
    opMode = 0;

    if(ForcedFloat) { opMode = modeFLOAT;
                      VdcRef = CalculateVdcRef(Vfloat);
                      IbatRef = Ibatref;
                      BattFlag.bit.FLOATMODE = TRUE;
                    }
      else if(ForcedBoost) {
                              if(opMode!=modeBOOST)
                              {
                                  opMode = modeBOOST;
                                  BattFlag.bit.BOOSTMODE = TRUE;
                                  BoostTimer = BoostPeriod;
                              }
                              if (BoostTimer == 0) ForcedBoost = FALSE;
                              VdcRef = CalculateVdcRef(Vboost);
                              IbatRef = Ibatref;
                         }
      else if(ForcedEqualize) {
                                  if(opMode != modeEQUALIZE) //initial ops
                                      {
                                          EqualizeTimer = EqualizerTime;
                                          opMode = modeEQUALIZE;
                                          IbatEqRelease = EqualiseIbatRef * 0.05;
                                          BattFlag.bit.EQUALIZE = TRUE;
                                      }
                                  if(Ibat < IbatEqRelease) { if(eqrelTimer<100) eqrelTimer++; }
                                  else { if(eqrelTimer) eqrelTimer--; }
                                  if((EqualizeTimer==0) || (eqrelTimer==100))ForcedEqualize = FALSE;
                                  VdcRef = EqualiseVRef;
                                  IbatRef = EqualiseIbatRef;
                              }

  else if(ForcedBatTest) {
                              if(opMode == modeTEST) //check for exit
                              {
                                  if((Vbat<Batt_FailLevel) || (Batt_TestTimer==0)) ForcedBatTest = FALSE;
                              }
                              else
                              {
                                  opMode = modeTEST;
                                  BattFlag.bit.TESTMODE = TRUE;
                                  Batt_TestTimer = Batt_Duration;
                              }
                              VdcRef = Batt_SetLevel;
                         }
}

    else if(DigitalInputFlag1 & DI_DGMODE)
    {
        opMode = modeDG;
        VdcRef = DGModeVRef;
        BattFlag.bit.DGMODE = TRUE;
    }

//---------------------------------------------------------------------------------

    BattFlag.bit.FLOATMODE = FALSE;
    BattFlag.bit.BOOSTMODE = FALSE;
    BattFlag.bit.DGMODE = FALSE;
    BattFlag.bit.EQUALIZE = FALSE;
    BattFlag.bit.TESTMODE = FALSE;

    if(Ibat>1.0)  { BattFlag.bit.CHARGE = TRUE; BattFlag.bit.DISCHARGE = FALSE; }
    if(Ibat< -1.0)  { BattFlag.bit.CHARGE = FALSE; BattFlag.bit.DISCHARGE = TRUE; }
    if(opMode == modeFLOAT) BattFlag.bit.FLOATMODE = TRUE;
    if(opMode == modeBOOST) BattFlag.bit.BOOSTMODE = TRUE;
    if(opMode == modeEQUALIZE) BattFlag.bit.EQUALIZE = TRUE;
    if(opMode == modeTEST) BattFlag.bit.TESTMODE = TRUE;
    if(opMode == modeDG) BattFlag.bit.DGMODE = TRUE;

    if(VdcRef != vdcRef_old)
    {
        Vdc_Ref = VdcRef /VdcSF;
        vdcRef_old = VdcRef;
        ReferenceChangedFlag = TRUE;
    }

    if(IbatRef != ibatref_old)
    {
        Ibat_Ref = IbatRef /IbatSF;
        ibatref_old = IbatRef;
    }

}

void    InitCapture(void)
{

        ECAP_setEventPrescaler(ECAP1_BASE, 1);
        ECAP_setCaptureMode(ECAP1_BASE, ECAP_CONTINUOUS_CAPTURE_MODE, ECAP_EVENT_1);
        ECAP_setEventPolarity(ECAP1_BASE, ECAP_EVENT_1, ECAP_EVNT_RISING_EDGE);
        ECAP_enableCounterResetOnEvent(ECAP1_BASE, ECAP_EVENT_1);   // optional OFF/ON
        ECAP_disableCounterResetOnEvent(ECAP1_BASE, ECAP_EVENT_1);  // keep TSCTR free-running
        ECAP_enableLoadCounter(ECAP1_BASE); // keeps TSCTR running
        ECAP_enableTimeStampCapture(ECAP1_BASE);
        ECAP_startCounter(ECAP1_BASE);
}





//----------- NO MORE ----------------



