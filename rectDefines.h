#ifndef __RECTDEFINESH__
#define __RECTDEFINESH__
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

#define SWFREQ128KC    4688
#define SWFREQ10KC     6000
#define SWFREQ8KC      7500

#define MS_ONE_PERIOD  15000L
#define SEQFAILDELAY   50
#define INV_PWM_PERIOD SWFREQ128KC

#define     RECORDNUMLOC    4
#define     IDLOC           10

//#define INV_HALFPERIOD  (INV_PWM_PERIOD >>1)


#define MSG_DATA_LENGTH     8
#define TX_MSG_OBJ_ID       1
#define RX_MSG_OBJ_ID       1


#define DATAADDR    0x300
#define EVENTADDR   0x380
#define SETADDR     0x101
#define OMEGA600        (0.188495559)

#define STOP_TIMER0 CpuTimer0Regs.TCR.bit.TSS = 1
#define STOP_TIMER1 CpuTimer1Regs.TCR.bit.TSS = 1
#define START_TIMER0 CpuTimer0Regs.TCR.bit.TSS = 0
#define START_TIMER1 CpuTimer1Regs.TCR.bit.TSS = 0

#define LOAD_TIMER0(x) CpuTimer0Regs.PRD.all  = x;  \
         CpuTimer0Regs.TCR.bit.TRB = 1
#define LOAD_TIMER1(x) CpuTimer1Regs.PRD.all  = x;  \
         CpuTimer1Regs.TCR.bit.TRB = 1

#define DISABLE_PWM()   __asm(" EALLOW");  \
                        EPwm1Regs.TZFRC.bit.OST = 0x1; \
                        EPwm2Regs.TZFRC.bit.OST = 0x1; \
                        EPwm3Regs.TZFRC.bit.OST = 0x1; \
                        EPwm4Regs.TZFRC.bit.OST = 0x1; \
                        EPwm5Regs.TZFRC.bit.OST = 0x1; \
                        EPwm6Regs.TZFRC.bit.OST = 0x1; \
                        __asm(" EDIS")
#define ENABLE_PWM()    __asm(" EALLOW");  \
                        EPwm1Regs.TZCLR.bit.OST = 0x1; \
                        EPwm2Regs.TZCLR.bit.OST = 0x1; \
                        EPwm3Regs.TZCLR.bit.OST = 0x1; \
                        EPwm4Regs.TZCLR.bit.OST = 0x1; \
                        EPwm5Regs.TZCLR.bit.OST = 0x1; \
                        EPwm6Regs.TZCLR.bit.OST = 0x1; \
                        __asm(" EDIS")


// Common Controller v2: no RTC (MCP79522) on the board. GPIO40 is now the
// RS485 #1 (SCIB) TXEN, so the RTC chip-select macros must not touch it.
// Set RTC_PRESENT to 1 (and give the RTC its own /CS) on a board with an RTC.
#define RTC_PRESENT         0

#if RTC_PRESENT
#define ENABLE_RTC()       GpioDataRegs.GPBSET.bit.GPIO57  = 1;   \
                           GpioDataRegs.GPBCLEAR.bit.GPIO40 = 1
#define DISABLE_RTC()      GpioDataRegs.GPBSET.bit.GPIO40 = 1
#else
#define ENABLE_RTC()       GpioDataRegs.GPBSET.bit.GPIO57  = 1    // EEPROM deselected only
#define DISABLE_RTC()      Nop()
#endif

#define ENABLE_EEPR()      GpioDataRegs.GPBCLEAR.bit.GPIO57 = 1   // 25LC512 /CS (GPIO57)
#define DISABLE_EEPR()     GpioDataRegs.GPBSET.bit.GPIO57 = 1

#define DISABLE_SPI()      GpioDataRegs.GPBSET.bit.GPIO57 = 1

#define EnableTX()  GpioDataRegs.GPBSET.bit.GPIO34  = 1     // RS485 #2 (SCIA) TXEN = transmit
#define DisableTX() GpioDataRegs.GPBCLEAR.bit.GPIO34    = 1 // RS485 #2 (SCIA) TXEN = receive
#define EnableTX1()  GpioDataRegs.GPBSET.bit.GPIO40  = 1    // RS485 #1 (SCIB) TXEN = transmit
#define DisableTX1() GpioDataRegs.GPBCLEAR.bit.GPIO40   = 1 // RS485 #1 (SCIB) TXEN = receive

#define EnableRLY1()   GpioDataRegs.GPBSET.bit.GPIO60  = 1  // RLY1 = GPIO60 (board_pin_map.h)
#define DisableRLY1()  GpioDataRegs.GPBCLEAR.bit.GPIO60    = 1

//#define RESET_FAULTS()  Nop()

#define ALL_DRIVES_OFF()    \
        EPwm1Regs.AQCTLA.bit.CAD = AQ_CLEAR;    \
        EPwm1Regs.AQCTLB.bit.CBD = AQ_CLEAR;    \
        EPwm2Regs.AQCTLA.bit.CAD = AQ_CLEAR;    \
        EPwm2Regs.AQCTLB.bit.CBD = AQ_CLEAR;    \
        EPwm3Regs.AQCTLA.bit.CAD = AQ_CLEAR;    \
        EPwm3Regs.AQCTLB.bit.CBD = AQ_CLEAR;    \
        EPwm4Regs.AQCTLA.bit.CAD = AQ_CLEAR;    \
        EPwm4Regs.AQCTLB.bit.CBD = AQ_CLEAR;    \
        EPwm5Regs.AQCTLA.bit.CAD = AQ_CLEAR;    \
        EPwm5Regs.AQCTLB.bit.CBD = AQ_CLEAR;    \
        EPwm6Regs.AQCTLA.bit.CAD = AQ_CLEAR;    \
        EPwm6Regs.AQCTLB.bit.CBD = AQ_CLEAR



#define DATAADDR_2    0x300
#define EVENTADDR_2   0x380
#define SETADDR_2     0x301

#define DATAADDR_1   0x200
#define EVENTADDR_1  0x280
#define SETADDR_1    0x201

#define     DI_UNITON   0x0001
#define     DI_RESET    0x0002
#define     DI_CPCC     0x0004
#define     DI_DGMODE   0x0008
#define     DI_FLOAT    0x0010
#define     DI_BOOST    0x0020
#define     DI_BT       0x0040
#define     DI_EQ       0x0080
#define     DI_AUTO     0x0100

#define     SPI_TIME            3
#define     EP_PARAM_START      0x0200
#define     EP_PARAM_START2     0x0400
#define     EP_PARAM_START3     0x0600

#define     EP_INTPARAM_START   0x0800
#define     EP_INTPARAM_START2  0x0A00
#define     EP_INTPARAM_START3  0x0C00

#define     EP_PACKET_NUM       0x0012
#define     EP_SERIAL           0x0030
#define     EP_IDLOC_F          0x0000
#define     EP_IDLOC_I          0x0010
#define     MAGIC               0x4142
#define     MAGIC1              0x4344

#define     LOGSTART            0x2000
#define     MAXPACKETS          192
#define     RECSIZE             32

#define     SYSCLOCK            150000000
#define     SYSTEM_FREQUENCY    150

#define     TWOPI               6.2831853072
#define	    SNAP_SIZE			100
#define	    PI2					1.570796327
#define	    PI					3.141592654
#define     TWO3 				0.66666666667
#define     BYROOT3				0.577350269
#define     BYROOT3N			-0.577350269
#define	    ONE3				0.3333333333

#define     INV_SAMPLING_TIME	20000
#define		POWERONDELAY		5000
#define		CANOKDELAY			5
#define		SEC1				1000
#define     SEC2S               2000

#define		SEC10				10000
#define     DI_DELAY            100

#define     SAT_LIMIT    0.999
#define     SAT_LIMITN  -0.999

#define SAMPLING_TIME       0.0000500
#define OMEGAFACTOR         1591.549431

#define CMD_DEFAULT         999

#define RESET_ALL() \
        SystemFlag.all = 0; \
        LimitFlag.all = 0;  \
        DigitalInputFlag1 &= cfgAutoResetFlag;  \
        chkFrequency.maxTimer = 0; \
        chkVr.maxTimer = 0; \
        chkVy.maxTimer = 0; \
        chkVb.maxTimer = 0; \
        chkIr.maxTimer = 0; \
        chkIy.maxTimer = 0; \
        chkIb.maxTimer = 0; \
        chkVdc.maxTimer = 0; \
        chkVdc.minTimer = 0; \
        chkIdc.maxTimer = 0; \
        chkIbat.maxTimer = 0; \
        chkIbat.minTimer = 0; \
        chkVbat.maxTimer = 0; \
        chkFrequency.stat.bit.HIGH = 0; \
        chkFrequency.stat.bit.LOW = 0; \
        chkVr.stat.bit.HIGH = 0; \
        chkVr.stat.bit.LOW = 0;\
        chkVy.stat.bit.HIGH = 0; \
        chkVy.stat.bit.LOW = 0;\
        chkVb.stat.bit.HIGH = 0; \
        chkVb.stat.bit.LOW = 0;\
        chkVbat.stat.bit.HIGH = 0; \
        chkVbat.stat.bit.LOW = 0; \
        chkIr.stat.bit.HIGH = 0; \
        chkIy.stat.bit.HIGH = 0; \
        chkIb.stat.bit.HIGH = 0; \
        chkVdc.stat.bit.HIGH = 0; \
        chkVdc.stat.bit.LOW = 0; \
        chkIdc.stat.bit.HIGH = 0; \
        chkIbat.stat.bit.HIGH = 0; \
        TripBitsFlag = 0; \
        TripCount = 0

#define AUTO_RESET_FAULTS() \
        SystemFlag.all = 0; \
        DigitalInputFlag1 &= cfgAutoResetFlag;  \
        LimitFlag.all = 0;  \
        chkFrequency.maxTimer = 0; \
        chkVr.maxTimer = 0; \
        chkVy.maxTimer = 0; \
        chkVb.maxTimer = 0; \
        chkIr.maxTimer = 0; \
        chkIy.maxTimer = 0; \
        chkIb.maxTimer = 0; \
        chkVdc.maxTimer = 0; \
        chkVdc.minTimer = 0; \
        chkIdc.maxTimer = 0; \
        chkIbat.maxTimer = 0; \
        chkIbat.minTimer = 0; \
        chkVbat.maxTimer = 0; \
        chkFrequency.stat.bit.HIGH = 0; \
        chkFrequency.stat.bit.LOW = 0; \
        chkVr.stat.bit.HIGH = 0; \
        chkVr.stat.bit.LOW = 0;\
        chkVy.stat.bit.HIGH = 0; \
        chkVy.stat.bit.LOW = 0;\
        chkVb.stat.bit.HIGH = 0; \
        chkVb.stat.bit.LOW = 0;\
        chkVbat.stat.bit.HIGH = 0; \
        chkVbat.stat.bit.LOW = 0; \
        chkIr.stat.bit.HIGH = 0; \
        chkIy.stat.bit.HIGH = 0; \
        chkIb.stat.bit.HIGH = 0; \
        chkVdc.stat.bit.HIGH = 0; \
        chkVdc.stat.bit.LOW = 0; \
        chkIdc.stat.bit.HIGH = 0; \
        TripBitsFlag = 0; \
        chkIbat.stat.bit.HIGH = 0

#define SET_DO(DO_PIN)  \
    GPIO_SetupPinMux(DO_PIN,GPIO_MUX_CPU1,0);   \
    GPIO_SetupPinOptions(DO_PIN,GPIO_OUTPUT,GPIO_OUTPUT);   \
    GPIO_WritePin(DO_PIN,0)

#define SET_DI(DI_PIN)  \
    GPIO_SetupPinMux(DI_PIN,GPIO_MUX_CPU1,0);   \
    GPIO_SetupPinOptions(DI_PIN,GPIO_INPUT,GPIO_INPUT)

#define checkDI_NORM(b,t,d) \
        if (b) { if (t < d) t++;    } \
        else {  if (t) t--;     } \
        Nop()
#define checkDI(b,t,d)  \
        if (GPIO_ReadPin(b)) { if (t < d) t++;  } \
        else {  if (t) t--;     } \
        Nop()
#define checkDI_NO(b,t,d)  \
        if (GPIO_ReadPin(b)) {  if (t) t--;     } \
        else { if (t < d) t++;  } \
        Nop()


#define Nop()   __asm(" NOP")


#define BAUD9600            487
#define BAUD19200           243
#define BAUD38400           121
#define BAUD57600            80
#define BAUD115200           40


#define	MIN5COUNT			300
#define	MIN1COUNT			60
#define	MIN2COUNT			120
#define	RESTARTDELAY		20
#define	SHORTCIRCUITDELAY	50

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
#define  	MB_SETDOM       0x0F
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

#define DIDELAY    100
#define	DI1DELAY	100
#define	DI2DELAY	100
#define	DI3DELAY	100
#define	DI4DELAY	100
#define	DI5DELAY	100
#define DI6DELAY    100
#define DI7DELAY    100
#define DI8DELAY    100
#define DI9DELAY    100
#define DI10DELAY   100
#define DI11DELAY   100
#define DI12DELAY   100


#define	CON1DELAY	4

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

    #define 	Nop()	__asm(" NOP")

typedef struct PARAM {
    float32_t MinValue;
    float32_t MaxValue;
    float32_t DefaultValue;
} PARAM;

typedef struct PARAM_INT  {
     int MinValue;
     int MaxValue;
     int DefaultValue;
} PARAM_INT;

// For Low pass filter
typedef struct {
	float32_t T;
	float32_t u;
	float32_t y;
} LPF;

typedef struct {
	float32_t sin_val;
	float32_t cos_val;
} sincos;

typedef struct {
	int sec;
	int min;
	int hour;
	int day;
	int date;
	int month;
	int year;
} date_time;

// For PI Regulator
typedef struct {
    float32_t Ref,Fbk;
    float32_t Kp, Ki;
    float32_t Err, Out;
    float32_t ui;
    float32_t Umax, Umin;
} PIreg;


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
    float32_t *MinRel;
    float32_t *MaxRel;
    float32_t *val;
    float32_t *minDelay;
    float32_t *maxDelay;
    N32 minTimer;
    N32 maxTimer;
    MINMAX stat;
}
MIN_MAX;

// For Current Limiter
typedef struct                                      //VCA
{
    float mean_square;
    Uint16 sample_count;
    float savedSum;
    float RMS;
} rms_meter;
/*************************** STRUCTURE DECLARATIONS *************************/


typedef struct
{
    N16 pinNo;
    int *BitNum;
    int *Delay;
    N16 Timer;
    N16 Enable;
    N16 RelayOn;
} RELAY;


typedef struct
{
    float32_t larray[32];
    N16 lptr;
    float32_t lsum;
    float32_t y;
} LPF32;
//Enable Flag
struct SYSTEM_BITS {
      Uint16 POWERINGUP :1;       // 0
      Uint16 RECTON :1;           // 1
      Uint16 SOFTSTART :1;        // 2
      Uint16 WRONGSEQ:1;          // 3
      Uint16 IACUNBAL:1;          // 4
      Uint16 SINGLEPH:1;          // 5
      Uint16 BATTFAIL:1;          // 6
      Uint16 INPOVERLOAD:1;       // 7
      Uint16 CVMODE:1;            // 8
      Uint16 CCMODE:1;            // 9
      Uint16 UNITON:1;            // 10
      Uint16 IBATMODE:1;          // 11
      Uint16 NZD:1;               //12
      Uint16 TRIPFAULT :1;        // 13
      Uint16 LATCHFAULT :1;       // 14
      Uint16 BITSVALID :1;        // 15
  };
typedef union {
    N16 all;
    struct SYSTEM_BITS bit;
} SYSMODE;


struct LIMIT_BITS {             // bits  description
    Uint16 MAINSLOW :1;         // 0
    Uint16 MAINSHIGH :1;        // 1
    Uint16 COMMONALARM :1;          // 2
    Uint16 VDCHIGH :1;          // 3
    Uint16 IDCHIGH:1;           // 4
    Uint16 IBATHIGH:1;          // 5
    Uint16 VLOADHIGH:1;         // 6
    Uint16 VBATHIGH:1;          // 7
    Uint16 VDCLOW:1;            // 8
    Uint16 VBATLOW:1;           // 9
    Uint16 OPENLOOP:1;          // 10
    Uint16 CANFAIL:1;           // 11
    Uint16 CURMODE:1;           // 12
    Uint16 VOLMODE:1;           // 13
    Uint16 IBATMODE:1;          // 14
    Uint16 BITSVALID :1;        // 15
};

typedef union {
    N16 all;
    struct LIMIT_BITS bit;
} LMTMODE;


struct SYSOK_BITS {             // bits  description
    Uint16 VROK :1;             // 0
    Uint16 VYOK :1;             // 1
    Uint16 VBOK :1;             // 2
    Uint16 MAINSOK :1;          // 3
    Uint16 SYNCOK :1;          // 4
    Uint16 MAINSFAIL :1;          // 5
    Uint16 CHRGRFAIL :1;          // 6

    Uint16 NOTUSED01 :8;         // 7-14
    Uint16 BITSVALID :1;        // 15
};
typedef union {
    N16 all;
    struct SYSOK_BITS bit;
} OKMODE;

enum {
        NoModeSelected = 0,
        modeFLOAT,
        modeBOOST,
        modeEQUALIZE,
        modeDG,
        modeTEST
    };

struct BATMODE_BITS
{                             // bits  description
    Uint16 FLOATMODE :1;      // 0
    Uint16 BOOSTMODE :1;      // 1
    Uint16 EQUALIZE :1;       // 3
    Uint16 TESTMODE:1;        // 4
    Uint16 DGMODE :1;         // 2
    Uint16 DISCHARGE :1;      // 4
    Uint16 CHARGE:1;          // 5
    Uint16 BATFAIL:1;         // 7
    Uint16 NZD1:7;            // 8-14
    Uint16 BITSVALID :1;      // 15
};

typedef union
{
    N16 all;
    struct BATMODE_BITS bit;
} BATMODE;


typedef struct eventPacket {
	N16 type;
	N16	minsec;
	N16	dthr;
	N16	yrmon;
	N16 alarm1;
	N16 alarm2;
	N16 alarm3;
	N16 alarm4;
    N16 eVinpR;
    N16 eVinpY;
    N16 eVinpB;
    N16 eIinpR;
    N16 eIinpY;
    N16 eIinpB;
    N16 eVdc;
    N16 eIdc;
    N16 eVbat;
    N16 eIbat;
    N16 eVload;
    N16 eIload;
    N16 eIbrdg1;
    N16 eIbrdg2;
    N16 kVA_R;
    N16 kVA_Y;
    N16 kVA_B;
    N16 kW_R;
    N16 kW_Y;
    N16 kW_B;
    N16 eKVA;
    N16 eKW;
    N16 ePF;
    N16 eFrequency;
} eventPacket;

/* future plan:
 * max of all analog values, 10 x 10 x 3
 * for last 10 cycles
 *
 */



//Function Protoytpes

void		ModbusProcessing(void);
void        ModbusProcessing1(void);
N16 		pi_com(void);
void update_datablock(N16 st_adr, N16 nreg, N8 *rxbuf );
N16 		update_one(unsigned int, unsigned int);

void		GpioInit(void);
void		EPwmInit();
void		AdcInit();
void		InitTimers();
void		InitSCI(void);
void		UpdateDigitalIO(void);
void		scia_xmit(int a);
void		scia_msg(char * msg);

void		scib_xmit(int Ch);
void		scib_msg(char *msg);

void		InitVariables(void);
inline		void	CurrentLimit(void);
inline		void	rmsCal(void);
void		InverterApplication(void);
void		ePWMInit(void);
interrupt	void	EPwmISR(void);
interrupt	void	AdcISR(void);
interrupt	void	SCIRxISR(void);

void		setPiRegulator(PIreg* p, float32_t kp, float32_t ki, float32_t ui, float32_t umax, float32_t umin);
void		setLowPassFilter(LPF *f, float32_t T, float32_t y);

void		spi_init(void);
N8          write_enable(void);
void        storeArrayToEEPROM(void);
#define     MB_PORT_A   1       // SCI-A, modbus.c
#define     MB_PORT_B   2       // SCI-B, modbus2.c
void        sendLog(N16 nPkt, N16 portNumber);
void        storeEvents(void);
void        saveRecordNumber(void);
void        EraseEventLog(void);

void		AdcOffsetSelfCal(void);
void        snapShot(N16 st_addr, N16 nreg, N16 portNumber);
void		initRmsMeter(rms_meter *r);
void		update_rmsmeters(void);
void		updateStatus(void);
void 		InitStructs(void);

N8			toHex(N8 k);
void		RestoreSettings(void);
N16			UpdateOne(N16 adr, float32_t val);
N8 			StoreValue(N16 adr, N16 Value);
N16			readCANmsg();
void		InitECAN(void);
void		CanbusProcessing(void);
N16			TableCRC(BYTE *nData, WORD wLength);
N16			txCanPacket(N16 packetNo);
void		EventLogging(void);
void		updateRAMvalues(N16 adr);
void		SendDummyCanPacket(void);
void		SetFactoryDefaults(void);
void		updateReference(N16 adr, int x);
void 		SetPWM_Zero();

void		getCompiledDate(void);
void		updateSerial(void);
void 		InitADC(void);

N8		asc2Dec(char c1, char c2);
N8		findMonth(char c1, char c2, char c3);

void 	eepromWriteArray(BYTE addr, BYTE* vData, WORD wLen);
void 	eepromReadArray(BYTE addr, BYTE *vData, BYTE wLength);
N8		ReadRTCCStatus(void);
void 	SPIFlashInit(void);
N16 	ReadRecordPointer(void);
void 	WriteRecordPointer(N16 nrecord);
N16		readEEPROM(N8 adr);
N16		writeEEPROM(N8 adr, N16 val);
void 	ReadSerial(void);
void	RTCwriteByte(BYTE addr, BYTE val);
void 	SaveSerialArray2EEPROM(void);
void 	delay_ms(N16 n1);
N16 	RestoreValue(N8 adr, N16 Max, N16 Min, N16 defval);
void    InitMCAN(void);
float32_t 	phaseLockedLoop(float32_t va, float32_t vb, float32_t vc);

N8		findBit(N8 start, N16 flag);
void	SelectMessage3(void);
void    InitPhaseLockedLoop(void);
void    ePWMInitBuck(void);
void    SetPiRegulator( PI_CONTROLLER *p, float32_t kp, float32_t ki, float32_t umax, float32_t umin);
void    ConfigureADC(void);
void    setupADC(uint16_t trig_sel, uint16_t acqps);
void  InitHiLo(MIN_MAX *x, float32_t *v1, float32_t *hlmt, float32_t *hrecover, float32_t *llmt, float32_t *lrecover, float32_t *hdly, float32_t *ldly);
void    CheckHi(MIN_MAX *x);
void    CheckHiLo(MIN_MAX *x);
void    InitHiLoParams(void);
inline void getReferenceCurrent(void);
void    ePWMInit_INV(N16 ch1, N16 ch2, N16 ch3 );
void    ePWMInit_SMPS(N16 chnl1, N16 chnl2);
N16     CRC16(N16 *dat, N16 nbytes);
void update_datablock_Int(N16 st_adr, N16 nreg, N8 *rxbuf);
N16     UpdateOneInt(N16 adr, int val);
N16     RestoreParamsInt(N16 start);
void    StoreParamsInt(N16 start);
void    RestoreSettingsInt(void) ;
void    storeArrayToEEPROMInt(void) ;
void    SetFactoryDefaultsInt(void) ;
N16     UpdateOneInt(N16 adr, int val) ;
float32_t Average32(LPF32 *x, float32_t val);
void    InitSCIA(N16 Baud);
void    InitSCIB(void);
void    BuckPWMInit(N16 channel);
void    ePWMInit_PFC(N16 ch1, N16 ch2, N16 ch3 );
N16     eeprom_read_word(Uint16 addr);
N16     eeprom_write_word(Uint16 addr, N16 val);
void    StoreParams(N16 start);
N16     RestoreParams(N16 start);
void    SM_FloatBoost(void);
void    InitDrives(N16 ch1);
void    EnableRTCClock(void);
void    SetTime(int *buf);
void    ReadTime(void);
void    updateRelays(void);
void    MCANBusProcessing(void);
void    RTCCReadArray(BYTE dwAddress, BYTE *vData, BYTE wLength);
void    RTCCWriteEnable(void);
void    RTCCWriteDisable(void);
void    TxCanMsg(long id, N16 val1, N16 val2, N16 val3, N16 val4);
void    UpdateBits(void);
void    InitRelays(void);
void    initZCD_ECAP1_GPIO47(void);
void    EPWM8_TimebaseInit_WithCmpInt(void);
static inline void InitDriveGateMode(volatile struct EPWM_REGS *p);
void    InitCapture(void);
void InitPI(PI_CONTROLLER *v);
void InitPID(PI_CONTROLLER *v);
void setPIRegulator(PI_CONTROLLER *p, float32_t kp, float32_t ki, float32_t umax, float32_t umin);
void     WriteRecordPointer(N16 nrecord);
Uint16 eeprom_read_words(Uint16 addr, Uint16 *buf, Uint16 words);
Uint16 eeprom_write_words(Uint16 addr, Uint16 *buf, Uint16 words);
void setupADCSOCs(void);
void configureADC(uint32_t base);
#endif	//__RECTDEFINESH__
