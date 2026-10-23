#ifndef __PFC3PHDEFINES_H__
#define __PFC3PHDEFINES_H__

//Type casts

typedef unsigned int N16;
typedef unsigned char N8;
typedef unsigned int WORD;
typedef unsigned char BYTE;
typedef unsigned long N32;
typedef unsigned long DWORD;
typedef signed long int S32;
typedef signed int S16;
typedef signed char S8;


#define PHASE3PH
//#define SLOW_SAMPLING
#define    CAN_GLB_INT_CANINT0  0x1
#define    CAN_GLB_INT_CANINT1  0x2

#define    TWOPI               6.2831853072
#define	    SNAP_SIZE			100
#define	    PI2					1.570796327
#define	    PI					3.141592654
#define    TWO3 				0.66666666667
#define    BYROOT3				0.577350269
#define    BYROOT3N			-0.577350269
#define	   ONE3				0.3333333333
#define    PFC_SAMPLING_FREQ	20000					//20.0KHz
#define    SYSTEM_FREQUENCY    150                     // TMS320F28P550, 150 MHz
#define		POWERONDELAY		5000
#define		CANOKDELAY			5
#define		SEC1				1000
#define		SEC10				10000
#define    SYSCLOCK            150000000
#define    DI_DELAY           10
#define    DELAY10            10

#define     MAXPACKETS      192
#define     RECSIZE         32

// Common Controller v2: no RTC (MCP79522) on the board
#define     RTC_PRESENT     0

// Display unit on CAN-A: the PFC is unit 2 of the inverter / PFC display
// protocol (broadcast 0x300.., writes 0x102 / 0x103), mcanInterface.c
#define     DISP_UNIT_PFC   2

// Digital inputs DI01..DI10 (PC817), board_pin_map.h DIx_GPIO.
// DI1 is AIO GPIO247 (analog mode off in GpioInit). Read through GPIO_ReadPin()
// so the port register (GPA/GPB/GPH) does not have to be named per pin.
#define DINP1       GPIO_ReadPin(247U)      // DI_1  pin 42  Unit ON
#define DINP2       GPIO_ReadPin(20U)       // DI_2  pin 48  Reset
#define DINP3       GPIO_ReadPin(21U)       // DI_3  pin 49  Main contactor aux
#define DINP4       GPIO_ReadPin(63U)       // DI_4  pin 47  spare
#define DINP5       GPIO_ReadPin(48U)       // DI_5  pin 7   spare
#define DINP6       GPIO_ReadPin(49U)       // DI_6  pin 8   spare
#define DINP7       GPIO_ReadPin(50U)       // DI_7  pin 9   Heatsink temp
#define DINP8       GPIO_ReadPin(51U)       // DI_8  pin 10  EPO
#define DINP9       GPIO_ReadPin(52U)       // DI_9  pin 11  spare
#define DINP10      GPIO_ReadPin(53U)       // DI_10 pin 12  spare

// DigitalInputFlag1 bits (bit set = contact open, see DI_NO_Flag)
#define DI_UNITON           0x0001
#define DI_RESET            0x0002
#define DI_MAINCONAUX       0x0004
#define DI_HSTEMP           0x0040
#define DI_EPO              0x0080

// Relay outputs (RelayFlag bits), board_pin_map.h RLYx_GPIO
#define DO_PRECHARGE        0x0001      // RLY11 (GPIO61, onboard) precharge contactor
#define DO_MAINSCON         0x0002      // RLY12 (GPIO59, onboard) main contactor
#define DO_PFCOK            0x0004      // RLY3  PFC OK
#define DO_COMMONFAULT      0x0008      // RLY4  common fault

// PFC carrier, up-down count at TBCLK = SYSCLK = 150 MHz:
// TBPRD = 150 MHz / (2 x fsw) = 75000 / fsw[kHz]
#define UPDATE_PFC_FSW() \
    if((FswPFC<6) || (FswPFC>24)) FswPFC = 10; \
    PFCPwmPeriod = (N16) (75000.0/FswPFC); \
    PFC_HalfPeriod =  PFCPwmPeriod >>1; \
    EPwm1Regs.TBPRD = PFCPwmPeriod; \
    EPwm2Regs.TBPRD = PFCPwmPeriod; \
    EPwm3Regs.TBPRD = PFCPwmPeriod

//#define HZ60
#define SAMPLING_TIME       0.0000500

#ifdef HZ60
    #define GRID_FREQ           60
    #define OMEGADEFAULT        (0.0188684243)
    #define PLL_LOCK_MIN        (0.017925003)
    #define PLL_LOCK_MAX        (0.019811846)
    #define QUARTERCYCLE        83
    #define FULLCYCLE           333
    #define MINPOINTS           300
    #define MAXPOINTS           360
    #define OMEGA_FACTOR        (3183.098916)
#else
#define GRID_FREQ           50
#define OMEGADEFAULT        (0.015707963)
#define PLL_LOCK_MIN        (0.014922565)
#define PLL_LOCK_MAX        (0.016493361)
#define QUARTERCYCLE        100
#define FULLCYCLE           400
#define MINPOINTS           360
#define MAXPOINTS           440
#define OMEGA_FACTOR        (3183.098916)
#endif


#define CMD_DEFAULT     555

#define RESET_FAULTS()  \
              chkVr.stat.bit.HIGH = FALSE;      \
              chkVy.stat.bit.HIGH = FALSE;      \
              chkVb.stat.bit.HIGH = FALSE;      \
              chkIr.stat.bit.HIGH = FALSE;      \
              chkIy.stat.bit.HIGH = FALSE;      \
              chkIb.stat.bit.HIGH = FALSE;      \
              chkdcLink.stat.bit.HIGH = FALSE;  \
              SystemFlag.bit.TRIPFAULT = FALSE;   \
              SystemFlag.bit.LATCHFAULT = FALSE;   \
              SystemFlag.bit.VCESAT = FALSE;   \
              SystemFlag.bit.OVERLOAD = FALSE;   \
              SystemFlag.bit.OCTRIP = FALSE;   \
              SystemFlag.bit.DCOVTRIP = FALSE;   \
              SystemFlag.bit.DITRIP = FALSE;   \
              ACLimitFlag.all &= 0x71FF;  \
              DCLimitFlag.bit.DCOV_TRIP = FALSE;  \
              EmergencyStop = 0; \
              TripCount = 0

#define AUTO_RESET_FAULTS()  \
              chkVr.stat.bit.HIGH = FALSE;      \
              chkVy.stat.bit.HIGH = FALSE;      \
              chkVb.stat.bit.HIGH = FALSE;      \
              chkIr.stat.bit.HIGH = FALSE;      \
              chkIy.stat.bit.HIGH = FALSE;      \
              chkIb.stat.bit.HIGH = FALSE;      \
              chkdcLink.stat.bit.HIGH = FALSE;  \
              SystemFlag.bit.TRIPFAULT = FALSE;   \
              SystemFlag.bit.VCESAT = FALSE;      \
              SystemFlag.bit.OVERLOAD = FALSE;    \
              SystemFlag.bit.OCTRIP = FALSE;   \
              SystemFlag.bit.DCOVTRIP = FALSE;   \
              SystemFlag.bit.DITRIP = FALSE;   \
              ACLimitFlag.all &= 0x71FF;  \
              DCLimitFlag.bit.DCOV_TRIP = FALSE

#define SET_DO(DO_PIN)  \
    GPIO_SetupPinMux(DO_PIN,GPIO_MUX_CPU1,0);   \
    GPIO_SetupPinOptions(DO_PIN,GPIO_OUTPUT,GPIO_OUTPUT);   \
    GPIO_WritePin(DO_PIN,0)

#define SET_DI(DI_PIN)  \
    GPIO_SetupPinMux(DI_PIN,GPIO_MUX_CPU1,0);   \
    GPIO_SetupPinOptions(DI_PIN,GPIO_INPUT,GPIO_INPUT)

#define checkDI_NC(b,t,d) \
        if (b) { if (t < d) t++;    } \
        else {  if (t) t--;     } \
        Nop()

#define checkDI_NO(b,t,d) \
        if (b) {  if (t) t--;     } \
        else  { if (t < d) t++;   } \
        Nop()

#define EnableTX()  GpioDataRegs.GPBSET.bit.GPIO34  = 1     // RS485 #2 (SCIA) TXEN = transmit
#define DisableTX() GpioDataRegs.GPBCLEAR.bit.GPIO34    = 1 // RS485 #2 (SCIA) TXEN = receive
#define EnableTX1()  GpioDataRegs.GPBSET.bit.GPIO40  = 1    // RS485 #1 (SCIB) TXEN = transmit
#define DisableTX1() GpioDataRegs.GPBCLEAR.bit.GPIO40   = 1 // RS485 #1 (SCIB) TXEN = receive

#define ENABLE_RTC()       GpioDataRegs.GPBSET.bit.GPIO57  = 1    // no RTC: EEPROM deselected only
#define DISABLE_RTC()
#define ENABLE_EEPR()      GpioDataRegs.GPBCLEAR.bit.GPIO57 = 1   // 25LC512 /CS (GPIO57)
#define DISABLE_EEPR()     GpioDataRegs.GPBSET.bit.GPIO57 = 1
#define DISABLE_SPI()      GpioDataRegs.GPBSET.bit.GPIO57 = 1

// Bridge 1 (EPWM1-3) = PFC bridge, on/off through the one-shot trip. ENABLE also
// clears a latched hardware desaturation trip (DCAEVT1, VCESAT1 - DesatTripInit())
// and an instantaneous trip forced by the control sample (ctl_protect).
#define DISABLE_PFCPWM()   __asm(" EALLOW");  \
                        EPwm1Regs.TZFRC.bit.OST = 0x1; \
                        EPwm2Regs.TZFRC.bit.OST = 0x1; \
                        EPwm3Regs.TZFRC.bit.OST = 0x1; \
                        pulseON = 0;                   \
                        __asm(" EDIS")
#define ENABLE_PFCPWM()    __asm(" EALLOW");  \
                        EPwm1Regs.TZCLR.bit.DCAEVT1 = 0x1; \
                        EPwm2Regs.TZCLR.bit.DCAEVT1 = 0x1; \
                        EPwm3Regs.TZCLR.bit.DCAEVT1 = 0x1; \
                        EPwm1Regs.TZCLR.bit.OST = 0x1; \
                        EPwm2Regs.TZCLR.bit.OST = 0x1; \
                        EPwm3Regs.TZCLR.bit.OST = 0x1; \
                        pulseON = 1;                   \
                        __asm(" EDIS")

// Bridge 2 (EPWM4-6, CON19) is not used by the PFC: held off permanently,
// by a one-shot trip (TZ force low) and a continuous software force low on A and B
#define BRIDGE2_OFF()   EPwm4Regs.AQSFRC.bit.RLDCSF = 3; EPwm4Regs.AQCSFRC.all = 0x5; \
                        EPwm5Regs.AQSFRC.bit.RLDCSF = 3; EPwm5Regs.AQCSFRC.all = 0x5; \
                        EPwm6Regs.AQSFRC.bit.RLDCSF = 3; EPwm6Regs.AQCSFRC.all = 0x5; \
                        __asm(" EALLOW");  \
                        EPwm4Regs.TZFRC.bit.OST = 0x1; \
                        EPwm5Regs.TZFRC.bit.OST = 0x1; \
                        EPwm6Regs.TZFRC.bit.OST = 0x1; \
                        __asm(" EDIS")

#define Nop()   __asm(" NOP")

// SCI baud registers, LSPCLK = 37.5 MHz: BRR = LSPCLK / (baud x 8) - 1
#define	BAUD9600			487
#define	BAUD19200			243
#define	BAUD38400			121
#define	BAUD57600			 80
#define	BAUD115200			 40

#define     SPI_TIME        0x2    //for spi_error

//DEFINES FOR SPI MEM
#define     EP_IDLOC_F          0x00
#define     EP_IDLOC_I          0x10

#define     EP_PARAM_START      0x200
#define     EP_PARAM_START2     0x400
#define     EP_PARAM_START3     0x600

#define     EP_INTPARAM_START   0x800
#define     EP_INTPARAM_START2  0xA00
#define     EP_INTPARAM_START3  0xC00


#define     EP_PACKET_NUM   0x12    //location 0x20 & 0x21 in RTC NVRAM
#define     EP_SERIAL       0x30    //location 0x20 & 0x21 in RTC NVRAM
#define     LOGSTART        0x2000  // 25LC512: event log above the parameter areas

// EEPROM layout IDs. Not the inverter's 0x4943 / 0x4944 or the rectifier's: a
// board that held other firmware must load PFC defaults, not foreign values.
#define     MAGIC           0x5046  // "PF" PFC float params
#define     MAGIC1          0x5047  // "PG" PFC int params
#define     NPARAMS         200
#define     CANRXID         0x200

#define		TRUE			1
#define		FALSE			0
#define		INTERCHARDELAY	5
#define  	RXDELAY         3               // Set 2ms delay count
#define  	MB_READDO       0x1
#define  	MB_READDI       0x2
#define  	MB_READREG      0x3
#define  	MB_READIREG     0x4
#define  	MB_SETDO        0x5
#define  	MB_SETREG       0x6
#define  	MB_SETDOM       0x15
#define  	MB_SETREGM      0x10
#define  	MB_REPORT       0x11

#define	 	DEVICEID		0x01

//Global Constants

#define	BIT00     	0x0001
#define	BIT01	    0x0002
#define	BIT02	    0x0004
#define	BIT03		0x0008
#define	BIT04	    0x0010
#define	BIT05	    0x0020
#define	BIT06	    0x0040
#define	BIT07       0x0080
#define	BIT08	    0x0100
#define	BIT09	    0x0200

#define	BITC0	    0xFFFE
#define	BITC1	    0xFFFD
#define	BITC2	    0xFFFB
#define	BITC3		0xFFF7
#define	BITC4		0xFFEF
#define	BITC5    	0xFFDF
#define	BITC6		0xFFBF
#define	BITC7		0xFF7F
#define	BITC8	    0xFEFF
#define	BITC9	    0xFDFF
#define	BITC10    	0xFBFF
#define	BITC11    	0xF7FF
#define	BITC12    	0xEFFF
#define	BITC13      0xDFFF
#define	BITC14    	0xBFFF
#define	BITC15    	0x7FFF


	typedef struct RtcType {
		N8 Second;
		N8 Minute;
		N8 Hour;
		N8 Day;
		N8 Date;
		N8 Month;
		N8 Year;
	} RtcType;
	//Bit Definitions

typedef struct PARAM {
    float32_t MinValue;      	// 0
    float32_t MaxValue;      	// 1
    float32_t DefaultValue;      // 2

} PARAM;


typedef struct PARAM_INT  {
     int MinValue;
     int MaxValue;
     int DefaultValue;
} PARAM_INT;

#include "controlTypes.h"


typedef struct {
	int sec;
	int min;
	int hour;
	int day;
	int date;
	int month;
	int year;
} date_time;


struct MINMAX_BITS {
      Uint16 LOW :1;       // 0
      Uint16 HIGH :1;           // 1
      Uint16 OK :1;        // 2
      Uint16 ARH:1;
      Uint16 ARL:1;
      Uint16 NUX:13;
  };
typedef union {
    N16 all;
    struct MINMAX_BITS bit;
} MINMAX;

typedef struct
{
    float32_t *Min;
    float32_t *Max;
    float32_t *val;
    float32_t *minDelay;
    float32_t *maxDelay;
    N32 minTimer;
    N32 maxTimer;
    MINMAX stat;
}
MIN_MAX;


// AlarmFlag1 (STAT1)
struct SYSTEM_BITS {
    Uint16 POWERINGUP :1;           // 0
    Uint16 PFCDISABLE :1;           // 1
    Uint16 PFCSOFTSTART :1;         // 2
    Uint16 PRECHARGE :1;            // 3
    Uint16 MAINCON :1;              // 4
    Uint16 WRONGSEQ :1;             // 5
    Uint16 GRIDFAIL :1;             // 6
    Uint16 OVERLOAD :1;             // 7
    Uint16 VCESAT :1;               // 8
    Uint16 OCTRIP :1;               // 9
    Uint16 DCOVTRIP :1;             // 10
    Uint16 EPO :1;                  // 11
    Uint16 DITRIP :1;               // 12
    Uint16 TRIPFAULT :1;            // 13
    Uint16 LATCHFAULT:1;            // 14
    Uint16 BITSVALID :1;            // 15
};

typedef union {
    N16 all;
    struct SYSTEM_BITS bit;
} SYSMODE;

typedef struct
{
    float32_t larray[32];
    N16 lptr;
    float32_t lsum;
    float32_t y;
} LPF32;

union canPacket {
    char chars[8];
    int ints[4];
    long longs[2];
    float floats[2];
};

// AlarmFlag2 (STAT2), grid side
struct ACLIMIT_BITS {             // bits  description
    Uint16 VINPRLOW :1;          // 0
    Uint16 VINPYLOW :1;          // 1
    Uint16 VINPBLOW :1;          // 2
    Uint16 VINPRHIGH :1;         // 3
    Uint16 VINPYHIGH :1;         // 4
    Uint16 VINPBHIGH :1;         // 5
    Uint16 IINPRHIGH :1;         // 6
    Uint16 IINPYHIGH :1;         // 7
    Uint16 IINPBHIGH :1;         // 8
    Uint16 OC_INSTTRIP_R :1;     // 9
    Uint16 OC_INSTTRIP_Y :1;     // 10
    Uint16 OC_INSTTRIP_B :1;     // 11
    Uint16 FREQLOW :1;           // 12
    Uint16 FREQHIGH :1;          // 13
    Uint16 NU :1;                // 14
    Uint16 BITSVALID :1;         // 15
};
typedef union {
    N16 all;
    struct ACLIMIT_BITS bit;
} ACLMTMODE;

// AlarmFlag3 (STAT3), DC side
struct DCLIMIT_BITS {             // bits  description
    Uint16 DCLINKLOW :1;         // 0
    Uint16 DCLINKHIGH :1;        // 1
    Uint16 DCLINKOK :1;          // 2
    Uint16 DCOV_TRIP :1;         // 3
    Uint16 NU :11;               // 4-14
    Uint16 BITSVALID :1;         // 15
};
typedef union {
    N16 all;
    struct DCLIMIT_BITS bit;
} DCLMTMODE;

// AlarmFlag4 high byte (STAT4 bits 8-15)
struct SYSOK_BITS {             // bits  description
    Uint16 VROK :1;          // 0
    Uint16 VYOK :1;          // 1
    Uint16 VBOK :1;          // 2
    Uint16 INPUTOK :1;       // 3
    Uint16 DCLINKOK :1;      // 4
    Uint16 PRECHARGEOK :1;   // 5
    Uint16 SYNCOK :1;        // 6
    Uint16 MAINCONCLOSED :1; // 7
    Uint16 NU :8;            // 8-15
};
typedef union {
    N16 all;
    struct SYSOK_BITS bit;
} OKMODE;

// AlarmFlag5 (STAT5)
struct STAT_BITS {
    Uint16 PFCENABLE :1;     // 0
    Uint16 TEST_MODE :1;     // 1
    Uint16 RLYPRECHRG :1;    // 2
    Uint16 RLYMAINCON :1;    // 3
    Uint16 RLYPFCOK :1;      // 4
    Uint16 RLYFAULT :1;      // 5
    Uint16 DMAXRAMP :1;      // 6
    Uint16 PRMODE :1;        // 7
    Uint16 NU :7;            // 8-14
    Uint16 BITSVALID :1;     // 15
};
typedef union {
    N16 all;
    struct STAT_BITS bit;
} STATMODE;

struct COMMOK_BITS {             // bits  description
    Uint16 CAN_DISP :1;  // 0
    Uint16 NZ1 :2;       // 1-2
    Uint16 RS485_LIVE :1;  // 3
    Uint16 NZD :11;  // 4-14

    Uint16 BITSVALID :1;       // 15
};
typedef union {
    N16 all;
    struct COMMOK_BITS bit;
} COMMMODE;


typedef struct eventPacket {
	N16 type;
	N16	minsec;
	N16	dthr;
	N16	yrmon;
	N16 alarm1;
	N16 alarm2;
	N16 alarm3;
	N16 alarm4;
	N16 alarm5;
	N16 alarm6;
	N16	eVinpRY;
	N16	eVinpYB;
	N16	eVinpBR;
	N16 eIinpR;
	N16 eIinpY;
	N16 eIinpB;
	N16	eDcLink;
	N16	eDcLinkRef;
	N16	ekW_Total;
	N16	ekVA_Total;
	N16	eFreq;
	N16	eVbat;
	N16	eIbat;
	N16	eTemp1;
	N16	eTemp2;
	N16	eReason;
	N16	notUsed00;
	N16	notUsed01;
	N16	notUsed02;
	N16	notUsed03;
	N16	notUsed04;
	N16	notUsed05;
} eventPacket;



//Function Protoytpes

void		ModbusProcessing(void);         // SCIA, RS485 #2
void		ModbusProcessing1(void);        // SCIB, RS485 #1 (GUI)
N16 		pi_com(N16 port);
void		update_datablock(void);
N16 		update_one(unsigned int, unsigned int);

void		GpioInit(void);
void		InitSCIA(N16 Baud);
void        InitSCIB(void);
void        InitFaultXbar(void);
void        InitMCANPins(void);
void        DesatTripInit(void);
void        AdcTriggerInit(void);
void		UpdateDigitalIO(void);
void		scia_xmit(int a);
void		scia_msg(char * msg);

void		scib_xmit(int Ch);
void		scib_msg(char *msg);

void		InitVariables(void);
void		PfcApplication(void);
interrupt	void	AdcISR(void);
interrupt	void	SCIRxISR(void);

void		setLowPassFilter(LPF *f, float32_t T, float32_t y);
void		eeprom_write(Uint16 addr, N8 nbytes);
void		eeprom_read(Uint16 addr, N8 nbytes);
void		spi_init(void);
N8          write_enable(void);
void        storeArrayToEEPROM(void);
void        storeEvents(void);
void        EraseEventLog(void);

void		snapShot(N16 st_addr);
void		initRmsMeter(rms_meter *r);
void		update_rmsmeters(void);
void		updateStatus(void);

void		RestoreSettings(void);
N16			UpdateOne(N16 adr, float32_t val);
void		InitECAN(void);
N16			TableCRC(BYTE *nData, WORD wLength);
void		EventLogging(void);
void		updateRAMvalues(N16 adr);
void		SetFactoryDefaults(void);

void		getCompiledDate(void);

N8		asc2Dec(char c1, char c2);
N8		findMonth(char c1, char c2, char c3);

N16 	ReadRecordPointer(void);
void 	delay_ms(N16 n1);
void    InitMCAN(void);

void    ConfigureADC(void);
void    setupADC(uint16_t trig_sel, uint16_t acqps);
void    InitHiLo(MIN_MAX *x, float32_t *v1, float32_t *hlmt, float32_t *llmt, float32_t *hdly, float32_t *ldly);
void    CheckHiLo(MIN_MAX *x);
void    InitHiLoParams(void);
void    ePWMInit_PFC(N16 ch1, N16 ch2, N16 ch3 );
N16     CRC16(N16 *dat, N16 nbytes);
void    update_datablock_Int(void);
void    InitDrives(N16 ch1);
void    setPiRegulator_old(PIreg *p, float32_t kp, float32_t ki, float32_t ui, float32_t umax, float32_t umin);
void    SetPfcPI(PFC_PI *p, float32_t kp, float32_t ki, float32_t umax, float32_t umin);
void    RestoreSettingsInt(void);
void    storeArrayToEEPROMInt(void);
void    SetFactoryDefaultsInt(void);
N16     UpdateOneInt(N16 adr, int val);
N16     eeprom_read_word(Uint16 addr);
N16     eeprom_write_word(Uint16 addr, N16 val);
void    UpdateRelayFlag(void);
void    UpdateRelays(void);
void    WriteRecordPointer(void);
void    MCANBusProcessing(void);
N16     MCANBusOff(void);
void    PR_Reset(void);
#endif	//__PFC3PHDEFINES_H__
