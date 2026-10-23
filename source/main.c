/***********************************************************************************
 Orion Technologies. Copyright (C) Feb 2023
 139, Shanti Industrial Esate, Mulund West, Mumbai-80
 -----------------------------------------------------------------------------------
 Project Name	: PFC 3PH
 Work Order	  	: VERTIV COMMON PLATFORM
 Software Code 	: 01??
 File Name 	  	: Main.C
 Description	: This is the main file for the Texas Instruments' TMS320F28P550 DSP
 controller. This  file  contains  the  scheduler and functions for
 interfacing	to the peripheral cards.
 3-phase PFC (active front end) on the Common Controller v2, control as in the
 RBC (RBC2_KDSENG_v4): DC-link PI -> Id reference, dq PI / alpha-beta PR
 current control on the CLA (controlCore.h).
 Date Created  	: 01 Aug 2023
 Authors		: Vijayan C A
 History Log	: Following Changes Carried out by Vijayan C A
 ----------------------------------------------------------------------------------\
 | Date	      | Ver	   | 			Description of Modification		 	 | Bug Nbr |
 |------------|--------|-------------------------------------------------|---------|
 | 23/10/2026 | 01AA   | 3-phase PFC from the 3-phase inverter           |         |
 |            |        | (RBC control, sequencing and protections)       |         |
 | 20/06/2024 | 00AB   | Paralleling with output current                 |         |
 |            |        |                                                 |         |
 | 06/08/2023 | 00AA   | Initial Project                                 |         |
 **********************************************************************************/


/*------------------------------------------------------------------------------
 External Function Declarations here
 ------------------------------------------------------------------------------*/
#define	CCEXTERN
#define CLA_SHARED_DEFINE       // claShared.h: define the CPU/CLA shared variables here
#define	INIT0	= 0
#define	INITARRAY0 = {0}
#define MATH_TYPE FLOAT_MATH

#include "f28x_project.h"
#include "../ccVariables.h"
#include "device.h"
#include "math.h"
#include "board_pin_map.h"
#include "../controlCore.h"
extern __interrupt void MCANIntr1ISR(void);    // CAN-B RX (no traffic used by the PFC)
extern __interrupt void MCANARxISR(void);      // CAN-A display writes
#if CONTROL_ON_CLA
extern __interrupt void ClaTask1EndISR(void); // CLA task 1 (control sample) done
void claInit(void);
#else
extern __interrupt void AdcISR(void);
extern __interrupt void EPWM_PFC_ISR(void);
#endif
extern __interrupt void SCIRxISR(void);
extern __interrupt void SCIBRx_ISR(void);
extern __interrupt void cpu_timer0_isr(void);
extern __interrupt void cpu_timer1_isr(void);
extern __interrupt void cpu_timer2_isr(void);


extern const PARAM defaultValues[];

#ifdef _FLASH
#pragma DATA_SECTION(waveBuffer, "BIG_ARRAYS")
#pragma DATA_SECTION(rx_buffer, "BIG_ARRAYS")
#endif


 void main(void){

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

    // Bridge 1 (EPWM1-3, U30 -> CON15): the PFC bridge
    ePWMInit_PFC(1, 2, 3);
    DISABLE_PFCPWM();
    DesatTripInit();        // VCESAT1 (active high) -> one-shot trip of EPWM1-3

    // Bridge 2 (EPWM4-6, U33 -> CON19): not used, outputs held low
    InitDrives(4);
    InitDrives(5);
    InitDrives(6);
    BRIDGE2_OFF();

    AdcTriggerInit();           // EPWM8 SOCA, 20 kHz ADC trigger

	SystemFlag.all = 0x01;      // POWERINGUP

	EALLOW;
	// Interrupts are re-mapped to ISR functions

	PieVectTable.TIMER0_INT = &cpu_timer0_isr;
	PieVectTable.TIMER1_INT = &cpu_timer1_isr;
	PieVectTable.TIMER2_INT = &cpu_timer2_isr;

#if CONTROL_ON_CLA
    PieVectTable.CLA1_1_INT = &ClaTask1EndISR;  // control sample done (CLA task 1)
#else
    PieVectTable.EPWM1_INT = &EPWM_PFC_ISR;
    PieVectTable.ADCA1_INT = &AdcISR;
#endif
    PieVectTable.SCIA_RX_INT = &SCIRxISR;
    PieVectTable.SCIB_RX_INT = &SCIBRx_ISR;
    PieVectTable.MCANB_0_INT = &MCANIntr1ISR;
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
#if CONTROL_ON_CLA
    // ADCA1 and EPWM1 INT trigger CLA tasks 1 / 2 (claInit), not CPU ISRs
    PieCtrlRegs.PIEIER11.bit.INTx1 = 1;  // 11.1 CLA1_1: CLA task 1 end
    PieCtrlRegs.PIEIER1.bit.INTx7 = 1;   // 1.7 TIMER0
#else
	PieCtrlRegs.PIEIER1.bit.INTx1 = 1;   // 1.1 ADCA1
    PieCtrlRegs.PIEIER1.bit.INTx7 = 1;   // 1.7 TIMER0
	PieCtrlRegs.PIEIER3.bit.INTx1 = 1;   // 3.1 EPWM1 (PFC modulator)
#endif
	PieCtrlRegs.PIEIER9.bit.INTx1 = 1;   // 9.1 SCIA RX
    PieCtrlRegs.PIEIER9.bit.INTx3 = 1;   // 9.3 SCIB RX
    PieCtrlRegs.PIEIER9.bit.INTx8 = 1;   // 9.8 MCANA_1: CAN-A display writes
    PieCtrlRegs.PIEIER9.bit.INTx10 = 1;  // 9.10 MCANB_1: CAN-B RX

    EPwm1Regs.ETSEL.bit.INTEN = 1; // Enable EPWM1INT generation
    EPwm1Regs.ETSEL.bit.INTSEL = 1; // Enable interrupt CNT_zero event
    EPwm1Regs.ETPS.bit.INTPRD = 1; // Generate interrupt on the 1st event
    EPwm1Regs.ETCLR.bit.INT = 1; // Enable more interrupts

#if CONTROL_ON_CLA
    IER |= M_INT11; // CLA1 end-of-task
#else
    IER |= M_INT3; // Enable CPU INT3 for PWM
#endif
    IER |= M_INT1; // Enable CPU Interrupt 0
    IER |= M_INT9; //serial + CAN
    IER |= M_INT13; // Enable CPU Timer 1;
    IER |= M_INT14; // Enable CPU Timer 2;
EDIS;

#if CONTROL_ON_CLA
    claInit();      // CLA memory, task vectors, triggers; runs ctl_init() as task 8
#else
    ctl_init();
#endif

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
#if CONTROL_ON_CLA
	// InitVariables() cleared the parameter arrays: claInit()'s check again
	claLayoutOK = (ctl.claLayout == (uint16_t)sizeof(CTL_SHARED));
#endif

	msDelayCount = 500;
	while (msDelayCount); // wait for 0.5 sec

	RestoreSettings();
	RestoreSettingsInt();
	updateRAMvalues(0); //setting parameters

	if((UnitAddress>0) && (UnitAddress<33))
	                address = UnitAddress;
	address1 = address;     // same Modbus address on both ports

	InitHiLoParams();
    InitMCANPins();     // MCANA GPIO30/31, MCANB GPIO18/19
    InitMCAN();         // CAN-A display (mcanInterface.c)
    MCAN_enableIntr(MCANB_DRIVER_BASE, MCAN_INTR_MASK_ALL, 1U);
    MCAN_selectIntrLine(MCANB_DRIVER_BASE, MCAN_INTR_MASK_ALL, MCAN_INTR_LINE_NUM_1);
    MCAN_enableIntrLine(MCANB_DRIVER_BASE, MCAN_INTR_LINE_NUM_1, 1U);
    MCAN_enableIntr(MCANA_DRIVER_BASE, MCAN_INTR_SRC_RX_FIFO1_NEW_MSG, 1U);
    MCAN_selectIntrLine(MCANA_DRIVER_BASE, MCAN_INTR_MASK_ALL, MCAN_INTR_LINE_NUM_1);
    MCAN_enableIntrLine(MCANA_DRIVER_BASE, MCAN_INTR_LINE_NUM_1, 1U);
	SystemFlag.bit.POWERINGUP = TRUE;
	UPDATE_PFC_FSW();
	OffsetNotDone = TRUE;

	while (1)
	{
	    PfcApplication();
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

		    // Re-initialise a CAN module only if it went bus-off
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

	} //End of Infinite for loop
} // End Of Main


// PFC pulses off and controller held at zero (ctl_current resets it while ctl.pwmOn = 0)
static void pfcStop(void)
{
    DISABLE_PFCPWM();
    pwmEnabled = FALSE;
    ctl.pwmOn = 0;
    Dmax_Ramping = FALSE;
    ctl.dmaxRamping = 0;
    pfcSoftStart = FALSE;
    SystemFlag.bit.PFCDISABLE = TRUE;
}

/*
 Sequencing, as in the RBC:
   input OK, main contactor open      -> precharge contactor (RLY11)
   DC link precharged (PreChargeLevel
   for PreCharge_delay s)             -> main contactor (RLY12), precharge opens
   main contactor closed, input OK,
   grid synchronised, Unit ON         -> PFC on: DC-link reference ramps from the
                                         present DC-link voltage, d output released
                                         slowly (Dmax_Ramping)
 */
void PfcApplication(void) //sequencing
{
    if (SystemFlag.bit.POWERINGUP)
       {
           pfcStop();
           SystemFlag.bit.MAINCON = FALSE;
           SystemFlag.bit.PRECHARGE = FALSE;
           PrechargeTimer = 0;
           reason= 1;
           return;
       }

    if((wrongSeqCount==10) && SysOkFlag.bit.VROK && SysOkFlag.bit.VYOK) SystemFlag.bit.WRONGSEQ = TRUE;
    if((wrongSeqCount == 0) || !SysOkFlag.bit.INPUTOK) SystemFlag.bit.WRONGSEQ = FALSE;

    if ( (SysOkFlag.bit.INPUTOK || TestMode) && (MainConClosed == 0) && !EmergencyStop)
    {
        SystemFlag.bit.PRECHARGE = TRUE;
        if(AutoLevel && (VinpRY>50)) PreChargeLevel = VinpRY*1.2;
    }
    else SystemFlag.bit.PRECHARGE = FALSE;

    if(SysOkFlag.bit.PRECHARGEOK && SysOkFlag.bit.INPUTOK && !SystemFlag.bit.LATCHFAULT && !EmergencyStop)
    {
        SystemFlag.bit.MAINCON = TRUE;
    }
    else SystemFlag.bit.MAINCON = FALSE;

//----------------------------------------------------------------------------------------------------
      if (SystemFlag.bit.TRIPFAULT || EmergencyStop)
      {
              dcLinkSoftRef = lpf_dclink.y;
              pfcStop();
              reason= 2;
          return;
      }

       if( SysOkFlag.bit.INPUTOK && SysOkFlag.bit.SYNCOK && !gridFailed && (UnitOnCommand || unitOnCommand)
               && MainConClosed && !SystemFlag.bit.WRONGSEQ )
       {
           if(pwmEnabled == FALSE)
           {
               UPDATE_PFC_FSW();

               pfcSoftStart = TRUE;
               DMinVal = 0;
               Dmax_Ramping =  TRUE;
               ctl.dmaxRamping = 1;

               SystemFlag.bit.PFCDISABLE = FALSE;
               dcLinkSoftRef = lpf_dclink.y;
               // controller states are held at zero by the control sample while
               // ctl.pwmOn == 0: set it first so the control runs before the first
               // switching period
               pwmEnabled = TRUE;
               ctl.pwmOn = 1;
               ENABLE_PFCPWM();
               systemRunningTimer = 0;

               reason = 4;
             }
           else reason = 5;
       }
       else    {
                   pfcStop();
                   reason = 9;
               }
	return;
}


void updateStatus(void)
{
    N16 tempiFlag = 0;
    N16 trips = 0;
    N16 mainsHigh = 0;

    // instantaneous trips latched by the control sample (cpuPostControl)
    DINT;
    trips = tripLatch;
    tripLatch = 0;
    EINT;
    if(trips & TRIP_OC_R) ACLimitFlag.bit.OC_INSTTRIP_R = TRUE;
    if(trips & TRIP_OC_Y) ACLimitFlag.bit.OC_INSTTRIP_Y = TRUE;
    if(trips & TRIP_OC_B) ACLimitFlag.bit.OC_INSTTRIP_B = TRUE;
    if(trips & (TRIP_OC_R | TRIP_OC_Y | TRIP_OC_B)) SystemFlag.bit.OCTRIP = TRUE;
    if(trips & TRIP_DCOV) { DCLimitFlag.bit.DCOV_TRIP = TRUE; SystemFlag.bit.DCOVTRIP = TRUE; }

    //------------------------------------------------ digital inputs
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

     if(DigitalInputFlag1 & DI_EPO) EmergencyStop = 1; //Latch fault

     if((DigitalInputFlag1 & DI_RESET)==0)
             {
                 RESET_FAULTS();
             }

     // main contactor closed: aux contact (DI3 closed) or 0.5 s after the command
     if(MainConAuxUsed)
     {
         if(((DigitalInputFlag1 & DI_MAINCONAUX)==0) && SystemFlag.bit.MAINCON) MainConClosed = TRUE;
         else MainConClosed = FALSE;
     }
     else
     {
         if(SystemFlag.bit.MAINCON && (mainConTimer >= 500)) MainConClosed = TRUE;
         else MainConClosed = FALSE;
     }
     if(TestMode) MainConClosed = TRUE;

    //------------------------------------------------ grid side
    ACLimitFlag.bit.VINPRLOW = chkVr.stat.bit.LOW;
    ACLimitFlag.bit.VINPRHIGH = chkVr.stat.bit.HIGH;
    ACLimitFlag.bit.VINPYLOW = chkVy.stat.bit.LOW;
    ACLimitFlag.bit.VINPYHIGH = chkVy.stat.bit.HIGH;
    ACLimitFlag.bit.VINPBLOW = chkVb.stat.bit.LOW;
    ACLimitFlag.bit.VINPBHIGH = chkVb.stat.bit.HIGH;

    ACLimitFlag.bit.IINPRHIGH = chkIr.stat.bit.HIGH;
    ACLimitFlag.bit.IINPYHIGH = chkIy.stat.bit.HIGH;
    ACLimitFlag.bit.IINPBHIGH = chkIb.stat.bit.HIGH;

    ACLimitFlag.bit.FREQLOW = chkFrequency.stat.bit.LOW;
    ACLimitFlag.bit.FREQHIGH = chkFrequency.stat.bit.HIGH;

    mainsHigh = (chkVr.stat.bit.HIGH | chkVy.stat.bit.HIGH | chkVb.stat.bit.HIGH);
    SystemFlag.bit.OVERLOAD = (chkIr.stat.bit.HIGH | chkIy.stat.bit.HIGH | chkIb.stat.bit.HIGH);

    // complete mains loss clears the trips (as a power cycle would); EPO stays latched
    if(chkVr.stat.bit.LOW && chkVy.stat.bit.LOW && chkVb.stat.bit.LOW)
    {
        N16 epo = EmergencyStop;
        RESET_FAULTS();
        EmergencyStop = epo;
    }

    SysOkFlag.bit.VROK = chkVr.stat.bit.OK;
    SysOkFlag.bit.VYOK = chkVy.stat.bit.OK;
    SysOkFlag.bit.VBOK = chkVb.stat.bit.OK;
    if(TestMode) { SysOkFlag.bit.VROK = TRUE; SysOkFlag.bit.VYOK= TRUE; SysOkFlag.bit.VBOK= TRUE; }

    if(SysOkFlag.bit.VROK & SysOkFlag.bit.VYOK & SysOkFlag.bit.VBOK) SysOkFlag.bit.INPUTOK = TRUE;
    else SysOkFlag.bit.INPUTOK = FALSE;

    // grid synchronised: frequency inside the limits; while running a frequency
    // excursion keeps it off for 2 s
    if((GridFrequency >= FrequencyLowLimit) && (GridFrequency <= FrequencyHighLimit))
    {
        if(!pwmEnabled || (Syncfailtimer==0)) SysOkFlag.bit.SYNCOK = TRUE;
    }
    else { SysOkFlag.bit.SYNCOK  = FALSE; if(pwmEnabled) Syncfailtimer = 2000; }

    SystemFlag.bit.GRIDFAIL = gridFailed;

    //------------------------------------------------ DC side
    DCLimitFlag.bit.DCLINKLOW = chkdcLink.stat.bit.LOW;
    DCLimitFlag.bit.DCLINKHIGH = chkdcLink.stat.bit.HIGH;
    DCLimitFlag.bit.DCLINKOK = chkdcLink.stat.bit.OK;
    SysOkFlag.bit.DCLINKOK = chkdcLink.stat.bit.OK;

    updateBit(PreCharge_delay, PrechargeTimer,  SysOkFlag.bit.PRECHARGEOK);
    SysOkFlag.bit.MAINCONCLOSED = MainConClosed;

     // Hardware desaturation trip (VCESAT1 -> DCAEVT1, DesatTripInit): the pulses
     // are already off; flag it so the trip / restart / latch logic sees it
     if(pwmEnabled && EPwm1Regs.TZFLG.bit.DCAEVT1) SystemFlag.bit.VCESAT = TRUE;

     SystemFlag.bit.DITRIP = ((DigitalInputFlag1 & TripBits) != 0);
     SystemFlag.bit.EPO = EmergencyStop;
     SystemFlag.bit.PFCSOFTSTART = pfcSoftStart;

if(!SystemFlag.bit.POWERINGUP) {

     if (SystemFlag.bit.OVERLOAD || mainsHigh || SystemFlag.bit.OCTRIP || SystemFlag.bit.DCOVTRIP ||
             SystemFlag.bit.VCESAT || DCLimitFlag.bit.DCLINKHIGH || SystemFlag.bit.DITRIP || EmergencyStop)
         {
             // pulses off before the controller is released (ctl.pwmOn = 0
             // zeroes the current controllers, the bridge must not keep switching)
             DISABLE_PFCPWM();
             pwmEnabled = FALSE;
             ctl.pwmOn = 0;
             if(!SystemFlag.bit.TRIPFAULT)
                 {
                     if(TripCount<MaxRestartCount) TripCount++;
                     if(SystemFlag.bit.DITRIP || EmergencyStop || SystemFlag.bit.VCESAT) TripCount = MaxRestartCount;
                     systemRunningTimer = 0;
                     restartTimer = RestartDelay;
                     SystemFlag.bit.TRIPFAULT = TRUE;
                     if(TripCount == MaxRestartCount) SystemFlag.bit.LATCHFAULT = TRUE;
                 }
       }
}
else {SystemFlag.bit.LATCHFAULT = FALSE; SystemFlag.bit.TRIPFAULT = FALSE; }

     SystemFlag.bit.BITSVALID = TRUE;
     AlarmFlag1= SystemFlag.all;

     ACLimitFlag.bit.BITSVALID = TRUE;
     AlarmFlag2 = ACLimitFlag.all;

     DCLimitFlag.bit.BITSVALID = TRUE;
     AlarmFlag3 = DCLimitFlag.all;

     AlarmFlag4 = (SysOkFlag.all << 8);
     AlarmFlag4 |= DigitalInputFlag1 & 0xff;

     StatFlag.all  = 0x8000;
     if(pwmEnabled) StatFlag.bit.PFCENABLE = TRUE;
     if(TestMode)  StatFlag.bit.TEST_MODE = TRUE;
     if(RelayFlag & DO_PRECHARGE) StatFlag.bit.RLYPRECHRG = TRUE;
     if(RelayFlag & DO_MAINSCON) StatFlag.bit.RLYMAINCON = TRUE;
     if(RelayFlag & DO_PFCOK) StatFlag.bit.RLYPFCOK = TRUE;
     if(RelayFlag & DO_COMMONFAULT) StatFlag.bit.RLYFAULT = TRUE;
     if(Dmax_Ramping) StatFlag.bit.DMAXRAMP = TRUE;
     if(PfcCtrlMode == 1) StatFlag.bit.PRMODE = TRUE;
     AlarmFlag5 = StatFlag.all;

     CommStatus.bit.BITSVALID = TRUE;
     CommStatusFlag = CommStatus.all;
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

#define UPDATE_KW_F(x,z)  z = (x.savedSum/rms_samplecount)*.001
#define SCALEF(x,y,z)  z = x*y
#define scaleKVA(v,i,k) k = v*i*.001

#define SCALE_RMSVAL_FLT(meter, sf,flt,v)   \
        meter.RMS = sqrt(meter.savedSum/rms_samplecount)*sf; \
        v = Average32(&flt, meter.RMS)

void update_rmsmeters(void)
{
    float32_t rawval1 = 0;

    if(rms_samplecount > MINPOINTS/2)
    {
        SCALE_RMSVAL_FLT(volt_meter_RY,VinpRSF,lpf32_vry,VinpRY);
        SCALE_RMSVAL_FLT(volt_meter_YB,VinpYSF,lpf32_vyb,VinpYB);
        SCALE_RMSVAL_FLT(volt_meter_BR,VinpBSF,lpf32_vbr,VinpBR);

        SCALE_RMSVAL_FLT(volt_meter_R,VinpRSF,lpf32_vr,VinpRph);
        SCALE_RMSVAL_FLT(volt_meter_Y,VinpYSF,lpf32_vy,VinpYph);
        SCALE_RMSVAL_FLT(volt_meter_B,VinpBSF,lpf32_vb,VinpBph);

        SCALE_RMSVAL_FLT(cur_meter_R,IinpRSF,lpf32_ir,IinpR);
        SCALE_RMSVAL_FLT(cur_meter_Y,IinpYSF,lpf32_iy,IinpY);
        SCALE_RMSVAL_FLT(cur_meter_B,IinpBSF,lpf32_ib,IinpB);

        UPDATE_KW_F(kw_meter_R, kW_Rph);
        UPDATE_KW_F(kw_meter_Y, kW_Yph);
        UPDATE_KW_F(kw_meter_B, kW_Bph);

        if(kW_Rph <0) kW_Rph = 0;
        if(kW_Yph <0) kW_Yph = 0;
        if(kW_Bph <0) kW_Bph = 0;

        //KVA Calculation:
        scaleKVA(VinpRph, IinpR, kVA_Rph);
        scaleKVA(VinpYph, IinpY, kVA_Yph);
        scaleKVA(VinpBph, IinpB, kVA_Bph);

        //POWERFACTOR SCALING:
        if (kVA_Rph<kW_Rph) kVA_Rph = kW_Rph;
        if (kVA_Yph<kW_Yph) kVA_Yph = kW_Yph;
        if (kVA_Bph<kW_Bph) kVA_Bph = kW_Bph;

        powerfactorR = (kVA_Rph > 0.01) ? kW_Rph/ kVA_Rph : 0;
        powerfactorY = (kVA_Yph > 0.01) ? kW_Yph/ kVA_Yph : 0;
        powerfactorB = (kVA_Bph > 0.01) ? kW_Bph/ kVA_Bph : 0;

        kVA_Total = kVA_Rph + kVA_Yph + kVA_Bph;
        kW_Total = kW_Rph + kW_Yph + kW_Bph;
    }

    SCALEF(lpf_dclink.y,DcLinkVoltageSF, rawval1);
    DcLinkVoltage = Average32(&lpf32_dclink, rawval1);

    SCALEF(lpf_vbat.y,VbatSF, Vbat);
    SCALEF(lpf_ibat.y,IbatSF, Ibat);
    SCALEF(lpf_temp1.y,Temp1SF, Temp1);
    SCALEF(lpf_temp2.y,Temp2SF, Temp2);

    GridFrequency = OMEGA_FACTOR * lpf_omega.y;
    DcLinkRefActive = dcLinkSoftRef * DcLinkVoltageSF;

    rms_update_flag = FALSE;

    dclink_ref = pid_dclink.Ref;
    dclink_fback = pid_dclink.Fbk;
    dclink_pidout = pid_dclink.Out;

    current_de_ref =  pfc.pid_de.Ref;
    current_qe_ref =  pfc.pid_qe.Ref;
    current_de_fback =  pfc.pid_de.Fbk;
    current_qe_fback =  pfc.pid_qe.Fbk;
    current_de_pidout =  pfc.pid_de.Out;
    current_qe_pidout =  pfc.pid_qe.Out;

    ffvd_mon = ffvd;
    ccvd_mon = ccvd;
    ccvq_mon = ccvq;
    vdref_mon = ctl.vdref;
    vqref_mon = ctl.vqref;
    DMinVal_mon = DMinVal;
}


N16 BITS[] = {BIT0, BIT1, BIT2, BIT3, BIT4, BIT5, BIT6, BIT7, BIT8, BIT9, BIT10, BIT11, BIT12, BIT13, BIT14, BIT15};

#define CALARM_FLAG1    0x7FC0      // grid fail .. latched fault, wrong sequence excluded
#define CALARM_FLAG2    0x3FFF
#define CALARM_FLAG3    0x000B

N16 Fault_Bits(void)
    {
        N16 Count = 0;
        N16 i = 0;

        for(i=0;i<16;i++)
            if(AlarmFlag1 & BITS[i] & (CALARM_FLAG1 | 0x0020)) Count++;
        for(i=0;i<16;i++)
            if(AlarmFlag2 & BITS[i] & CALARM_FLAG2) Count++;
        for(i=0;i<16;i++)
            if(AlarmFlag3 & BITS[i] & CALARM_FLAG3) Count++;

     return(Count);
    }


N16 oldBits = 0;
void    UpdateRelayFlag(void)
{
        if(SystemFlag.bit.PRECHARGE) RelayFlag |= DO_PRECHARGE;
        else RelayFlag &= (~DO_PRECHARGE);
        if(SystemFlag.bit.MAINCON) RelayFlag |= DO_MAINSCON;
        else RelayFlag &= (~DO_MAINSCON);

        // PFC OK: running, DC link at its reference
        if(pwmEnabled && !pfcSoftStart && !SystemFlag.bit.TRIPFAULT) RelayFlag |= DO_PFCOK;
        else RelayFlag &= (~DO_PFCOK);

        if((AlarmFlag1 & (CALARM_FLAG1 | 0x0020)) || (AlarmFlag2 & CALARM_FLAG2) || (AlarmFlag3 & CALARM_FLAG3))
            { RelayFlag |= DO_COMMONFAULT; BuzzerOn = TRUE; }
        else { RelayFlag &= (~DO_COMMONFAULT);  BuzzerOn = FALSE; }

        FaultBits = Fault_Bits();
        if(FaultBits> oldBits) MuteFlag = FALSE;
        oldBits = FaultBits;
}

// Precharge and main contactor on the onboard relays RLY11 (GPIO61) / RLY12
// (GPIO59); PFC OK and common fault on RLY3 / RLY4. The other relays stay off.
 void UpdateRelays(void)
    {
        GPIO_WritePin(RLY11_GPIO, (RelayFlag & DO_PRECHARGE) ? 1 : 0);
        GPIO_WritePin(RLY12_GPIO, (RelayFlag & DO_MAINSCON) ? 1 : 0);
        GPIO_WritePin(RLY3_GPIO, (RelayFlag & DO_PFCOK) ? 1 : 0);
        GPIO_WritePin(RLY4_GPIO, (RelayFlag & DO_COMMONFAULT) ? 1 : 0);
    }


    void        updateRAMvalues(N16 adr)
    {
        // gains and limits are set from the parameters on every update; the
        // control sample reads them directly
        pid_dclink.Kp = dclink_kp;
        pid_dclink.Ki = dclink_ki;
        pid_dclink.Umax = PowerLimit * 0.1;
        pid_dclink.Umin = pid_DCLink_Umin;

        pfc.pid_de.Kp = current_kp;
        pfc.pid_qe.Kp = current_kp;
        pfc.pid_de.Ki = current_ki;
        pfc.pid_qe.Ki = current_ki;
        pfc.pid_de.Umax = pid_de_Umax;
        pfc.pid_de.Umin = -PowerLimit * 0.1;
        pfc.pid_qe.Umax = pfc_pid_qmax;
        pfc.pid_qe.Umin = -pfc_pid_qmax;

        pfc.lpf_de.T = current_lpfT;
        pfc.lpf_qe.T = current_lpfT;

        lpf_vR.T = InputLPF;
        lpf_vY.T = InputLPF;
        lpf_vB.T = InputLPF;
        lpf_iR.T = InputLPF;
        lpf_iY.T = InputLPF;
        lpf_iB.T = InputLPF;

        // positive sequence phase peak, p.u., from the line-line rms level
        mainsFailLevel = (GridFailLevel * 0.81649658) / VinpRSF;

        dcLinkReference = DcLinkReference / DcLinkVoltageSF;

        switch(adr)
        {
        case 0:
            if((UnitAddress>0) && (UnitAddress<33)) address = UnitAddress;
            else address = 1;
            break;

        case 100:       // DC-link reference
        case 126:       // DC-link scale factor
            if(pwmEnabled) pfcSoftStart = TRUE;
            break;

        case 142:       // switching frequency: applied while the PFC is off
            if(pwmEnabled==0) { UPDATE_PFC_FSW(); }
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

#define MSGMASK1 0x7FE2 // SystemFlag: PFC disabled, wrong sequence .. latched fault
#define MSGMASK2 0x3FFF // ACLimitFlag
#define MSGMASK3 0x000B // DCLimitFlag: low, high, inst. OV
#define MSGMASK4 0x00C1 // DI: Unit ON, heatsink temp, EPO
#define MSGMASK5 0x0001 // StatFlag: PFC enabled


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
             Events[eventCount].type = 0x1974;
             Events[eventCount].minsec = RTCMinute*256+RTCSecond;
             Events[eventCount].dthr = RTCDate*256+RTCHour;
             Events[eventCount].yrmon = RTCYear*256 + RTCMonth;
             Events[eventCount].alarm1 = AlarmFlag1 & MSGMASK1;
             Events[eventCount].alarm2 = AlarmFlag2 & MSGMASK2;
             Events[eventCount].alarm3 = AlarmFlag3 & MSGMASK3;
             Events[eventCount].alarm4 = AlarmFlag4 & MSGMASK4;
             Events[eventCount].alarm5 = AlarmFlag5 & MSGMASK5;
             Events[eventCount].alarm6 = 0;

                Events[eventCount].eVinpRY = (int) (VinpRY*10);
                Events[eventCount].eVinpYB = (int) (VinpYB*10);
                Events[eventCount].eVinpBR = (int) (VinpBR*10);
                Events[eventCount].eIinpR = (int) (IinpR*10);
                Events[eventCount].eIinpY = (int) (IinpY*10);
                Events[eventCount].eIinpB = (int) (IinpB*10);
                Events[eventCount].eDcLink = (int) (DcLinkVoltage*10);
                Events[eventCount].eDcLinkRef = (int) (DcLinkRefActive*10);
                Events[eventCount].ekW_Total = (int) (kW_Total*10);
                Events[eventCount].ekVA_Total = (int) (kVA_Total*10);
                Events[eventCount].eFreq = (int) (GridFrequency*100);
                Events[eventCount].eVbat = (int) (Vbat*10);
                Events[eventCount].eIbat = (int) (Ibat*10);
                Events[eventCount].eTemp1 = (int) (Temp1*10);
                Events[eventCount].eTemp2 = (int) (Temp2*10);
                Events[eventCount].eReason = reason;

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


void delay_ms(N16 n1)
    {
        msDelayCount = n1;
        while(msDelayCount);
        return;
    }

 //----------- NO MORE ----------------

