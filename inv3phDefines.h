#ifndef __INV3PHDEFINES_H__
#define __INV3PHDEFINES_H__

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

#define FLOAT_CONVERTER_DC    0.000244140625
#define FLOAT_CONVERTER_AC    0.00048828125

#define ANGLE120 4.1887902048
#define ANGLE240 2.0943951024
//#define ANGLE30  0.523598776

#define PHASE3PH
#define SHIFTED_FBACK
//#define DUMMYSIG
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
#define    INV_SAMPLING_TIME	20000					//20.0KHz
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

// Digital inputs DI01..DI10 (PC817), board_pin_map.h DIx_GPIO.
// DI1 is AIO GPIO247 (analog mode off in GpioInit). Read through GPIO_ReadPin()
// so the port register (GPA/GPB/GPH) does not have to be named per pin.
#define DINP1       GPIO_ReadPin(247U)      // DI_1  pin 42  Unit ON
#define DINP2       GPIO_ReadPin(20U)       // DI_2  pin 48  Reset
#define DINP3       GPIO_ReadPin(21U)       // DI_3  pin 49  Mains fail
#define DINP4       GPIO_ReadPin(63U)       // DI_4  pin 47  Manual bypass
#define DINP5       GPIO_ReadPin(48U)       // DI_5  pin 7   Output MCB
#define DINP6       GPIO_ReadPin(49U)       // DI_6  pin 8   Bypass MCB
#define DINP7       GPIO_ReadPin(50U)       // DI_7  pin 9   Heatsink temp
#define DINP8       GPIO_ReadPin(51U)       // DI_8  pin 10  EPO
#define DINP9       GPIO_ReadPin(52U)       // DI_9  pin 11  spare
#define DINP10      GPIO_ReadPin(53U)       // DI_10 pin 12  spare

// Inverter carrier, up-down count at TBCLK = SYSCLK = 150 MHz:
// TBPRD = 150 MHz / (2 x fsw) = 75000 / fsw[kHz]
#define UPDATE_INV_FSW() \
    if((FSW_Inverter<6) || (FSW_Inverter>24)) FSW_Inverter = 12; \
    InvPwmPeriod = (N16) (75000.0/FSW_Inverter); \
    Inv_HalfPeriod =  InvPwmPeriod >>1; \
    EPwm1Regs.TBPRD = InvPwmPeriod; \
    EPwm2Regs.TBPRD = InvPwmPeriod; \
    EPwm3Regs.TBPRD = InvPwmPeriod

//#define HZ60
#define SAMPLING_TIME       0.0000500

#ifdef HZ60
    #define SAMPLING_TIME       0.0000500
    #define GRID_FREQ           60
    #define OMEGADEFAULT        (0.0188684243)
    #define PLL_LOCK_MIN        (0.017925003)
    #define PLL_LOCK_MAX        (0.019811846)
    #define QUARTERCYCLE        83
    #define FULLCYCLE           333
    #define MINPOINTS           300
    #define SLEWRATE_FACT       2200
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
#define SLEWRATE_FACT       2200
#define MAXPOINTS           440
#define OMEGA_FACTOR        (3183.098916)
#endif

#define SAT_LIMIT    0.999
#define SAT_LIMITN  -0.999

#define	OMEGAFACTOR			107944
#define CMD_DEFAULT     555

#define RESET_FAULTS()  \
              chkVr.stat.bit.HIGH = FALSE;      \
              chkIr1.stat.bit.HIGH = FALSE;      \
              chkIr2.stat.bit.HIGH = FALSE;      \
              chkIr3.stat.bit.HIGH = FALSE;      \
              chkFrequency.stat.bit.HIGH = FALSE;      \
              chkVbypr.stat.bit.HIGH = FALSE;      \
              chkVbat.stat.bit.HIGH = FALSE;      \
              chkIbat.stat.bit.HIGH = FALSE;      \
              SystemFlag.bit.TRIPFAULT = FALSE;   \
              SystemFlag.bit.LATCHFAULT = FALSE;   \
              SystemFlag.bit.VCESAT = FALSE;   \
              SystemFlag.bit.OVERLOAD = FALSE;   \
              LimitFlag.all &= 0x8800;  \
              EmergencyStop = 0; \
              TripCount = 0

#define AUTO_RESET_FAULTS()  \
              chkVr.stat.bit.HIGH = FALSE;      \
              chkIr1.stat.bit.HIGH = FALSE;     \
              chkIr2.stat.bit.HIGH = FALSE;     \
              chkIr3.stat.bit.HIGH = FALSE;     \
              chkFrequency.stat.bit.HIGH = FALSE; \
              chkVbypr.stat.bit.HIGH = FALSE;     \
              chkVbat.stat.bit.HIGH = FALSE;      \
              chkIbat.stat.bit.HIGH = FALSE;      \
              SystemFlag.bit.TRIPFAULT = FALSE;   \
              SystemFlag.bit.VCESAT = FALSE;      \
              SystemFlag.bit.OVERLOAD = FALSE;    \
              SystemFlag.bit.TRIPFAULT = FALSE;   \
              SystemFlag.bit.VCESAT = FALSE;      \
              SystemFlag.bit.OVERLOAD = FALSE;    \
              EmergencyStop = 0; \
              LimitFlag.all = 0

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

#define EnableRLY1()   GpioDataRegs.GPBSET.bit.GPIO60  = 1  // RLY1 = GPIO60 (board_pin_map.h)
#define DisableRLY1()  GpioDataRegs.GPBCLEAR.bit.GPIO60    = 1

#define ENABLE_RTC()       GpioDataRegs.GPBSET.bit.GPIO57  = 1    // no RTC: EEPROM deselected only
#define DISABLE_RTC()
#define ENABLE_EEPR()      GpioDataRegs.GPBCLEAR.bit.GPIO57 = 1   // 25LC512 /CS (GPIO57)
#define DISABLE_EEPR()     GpioDataRegs.GPBSET.bit.GPIO57 = 1
#define DISABLE_SPI()      GpioDataRegs.GPBSET.bit.GPIO57 = 1

// Bridge 1 (EPWM1-3) on/off through the one-shot trip. ENABLE also clears a
// latched hardware desaturation trip (DCAEVT1, VCESAT1 - see DesatTripInit()).
#define DISABLE_PWM()   __asm(" EALLOW");  \
                        EPwm1Regs.TZFRC.bit.OST = 0x1; \
                        EPwm2Regs.TZFRC.bit.OST = 0x1; \
                        EPwm3Regs.TZFRC.bit.OST = 0x1; \
                        pulseON = 0;                   \
                        __asm(" EDIS")
#define ENABLE_PWM()    __asm(" EALLOW");  \
                        EPwm1Regs.TZCLR.bit.DCAEVT1 = 0x1; \
                        EPwm2Regs.TZCLR.bit.DCAEVT1 = 0x1; \
                        EPwm3Regs.TZCLR.bit.DCAEVT1 = 0x1; \
                        EPwm1Regs.TZCLR.bit.OST = 0x1; \
                        EPwm2Regs.TZCLR.bit.OST = 0x1; \
                        EPwm3Regs.TZCLR.bit.OST = 0x1; \
                        pulseON = 1;                   \
                        __asm(" EDIS")

// Bridge 2 (EPWM4-6, CON19) is not used by the inverter: held off permanently,
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

#define	MIN5COUNT			300
#define	MIN1COUNT			60
#define	MIN2COUNT			120
#define	RESTARTDELAY		20
#define	SHORTCIRCUITDELAY	50



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

// EEPROM layout IDs. Not the rectifier's 0x4142 / 0x4344: a board that held the
// 12-pulse rectifier firmware must load inverter defaults, not rectifier values.
#define     MAGIC           0x4943  // "IC" inverter float params
#define     MAGIC1          0x4944  // "ID" inverter int params
#define     NPARAMS         200
#define     CANRXID         0x200

#define POS_MSEC	0
#define POS_SEC		1
#define POS_MIN		2
#define POS_HR		3
#define POS_DOW		4
#define POS_DATE	5
#define POS_MONTH	6
#define POS_YEAR	7


#define	    RECORDNUMLOC	4
#define     IDLOC			10
#define     E_PARSTART		0x10
#define     E_SERSTART		0xA0
#define	    F_MSGSTART		4096

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

#define	DI0DELAY	2000
#define	DI1DELAY	2000
#define	DI2DELAY	2000
#define	DI3DELAY	1000
#define	DI4DELAY	1000
#define	DI5DELAY	100
#define	KBDDELAY	100

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
    float32_t MinValue;      	// 0
    float32_t MaxValue;      	// 1
    float32_t DefaultValue;      // 2

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
    float32_t Ref,Fbk;
    float32_t Kp, Ki;
    float32_t Err, Out;
    float32_t ui;
    float32_t Umax, Umin;
} PIreg;

typedef struct
{
    LPF lpf_de, lpf_qe;
    PIreg pi_de, pi_qe;
    float32_t ref_d, ref_q;
}
harmonic;


#define PIREGULATOR(x)  \
    x.Err = x.Ref - x.Fbk; \
    x.ui = x.ui + (x.Ki * x.Err); \
    if(x.ui >x.Umax) x.ui = x.Umax; \
    else if(x.ui < x.Umin) x.ui = x.Umin; \
    x.Out = (x.Kp * x.Err) + x.ui; \
    if(x.Out > x.Umax) x.Out = x.Umax; \
    else if(x.Out < x.Umin) x.Out = x.Umin


// Resonant (PR) voltage controller, stationary alpha-beta frame
#define PR_NHARM        4           // 5th, 7th, 11th, 13th
#define PR_FUND_LIMIT   0.96

// Open-loop test (VoltCtrlMode = 2): fixed balanced modulation, no voltage feedback,
// ramped 0 -> OL_MOD_MAX in 1 s. Same limit as the PI/PR controller outputs.
#define OL_MOD_MAX      0.96
#define OL_MOD_STEP     (OL_MOD_MAX * SAMPLING_TIME / 1.0)

typedef struct {
    float32_t Y;        // in-phase state
    float32_t X;        // quadrature state
    float32_t out;
} PR_STATE;

typedef struct {
    float32_t a;        // 2*sin(h*w*Ts/2) -> exact discrete resonance at h*w
    float32_t c, s;     // cos/sin of phase lead (delay compensation)
} PR_COEF;

// Two-integrator resonator: Y += k*e - wd*Y - a*X ; X += a*Y ; out = c*Y - s*X
#define PR_RES_MACRO(r,k,e,cf,wd,lim) \
    r.Y = r.Y + ((k) * (e)) - ((wd) * r.Y) - (cf.a * r.X); \
    if(r.Y > (lim)) r.Y = (lim); \
    else if(r.Y < -(lim)) r.Y = -(lim); \
    r.X = r.X + (cf.a * r.Y); \
    if(r.X > (lim)) r.X = (lim); \
    else if(r.X < -(lim)) r.X = -(lim); \
    r.out = (cf.c * r.Y) - (cf.s * r.X)



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

// For Current Limiter
typedef struct                                      //VCA
{
    float32_t mean_square;
    Uint16 sample_count;
    float32_t savedSum;
    float RMS;
} rms_meter;
/*************************** STRUCTURE DECLARATIONS *************************/



typedef struct
{
        float32_t a, b, c;
        float32_t d, q, z;
        float32_t de, qe;
        LPF lpf_de, lpf_qe;
        PID_GRANDO_F_CONTROLLER pid_de, pid_qe;
        float32_t ref_d, ref_q;
        float32_t ref_a, ref_b, ref_c;
} DQControl;


typedef struct                                      //VCA
{
    DQControl vol;
    float32_t deo;
    float32_t qeo;
    float32_t alpha;
    float32_t beta;
    float32_t refa;
    float32_t refb;
    float32_t refc;
    sincos theta;
    sincos theta_o;
    float32_t omeaga;
}
VolControl;


struct PARSTAT_BITS {
    Uint16 POWERINGUP :1;           // 0
    Uint16 INVRTRDISABL:1;          // 1
    Uint16 SOFTSTART :1;            // 2
    Uint16 LOADONBAT :1;            // 3
    Uint16 LOADONINV:1;             // 4
    Uint16 LOADONBYP:1;             // 5
    Uint16 MANUALBYP:1;             // 6
    Uint16 WAITMASTER:1;            // 7
    Uint16 NOTSYNCED :1;            // 8
    Uint16 TRIPFAULT :1;            // 9
    Uint16 OUTMCBOFF :1;            // 10
    Uint16 BYPMCBOFF :1;            //
    Uint16 INVOK:1;                 // 12
    Uint16 BYPASSOK:1;              // 13
    Uint16 MASTER :1;               // 14
    Uint16 BITSVALID :1;            // 15
};

typedef union {
    N16 all;
    struct PARSTAT_BITS bit;
} PARSTAT;

//Enable Flag
struct SYSTEM_BITS {
    Uint16 POWERINGUP :1;           // 0
    Uint16 INVRTRDISABL:1;          // 1
    Uint16 SOFTSTART :1;            // 2
    Uint16 LOADONBAT :1;               // 3
    Uint16 LOADONINV:1;             // 4
    Uint16 LOADONBYP:1;             // 5
    Uint16 MANUALBYP:1;             // 6
    Uint16 OVERLOAD :1;             // 7
    Uint16 VCESAT :1;               // 8
    Uint16 INVFAILURE:1;             // 9
    Uint16 NOTSYNCED :1;            // 10
    Uint16 OCLVL1ACTIVE:1;       // 11
    Uint16 OCLVL2ACTIVE:1;       // 12
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

struct LIMIT_BITS {             // bits  description
    Uint16 BYPRLOW :1;           // 0
    Uint16 BYPYLOW :1;           // 1
    Uint16 BYPBLOW :1;           // 2
    Uint16 BYPRHIGH :1;          // 3
    Uint16 BYPYHIGH :1;          // 4
    Uint16 BYPBHIGH :1;          // 5
    Uint16 VBATLOW    :1;        //6
    Uint16 VBATHIGH :1;          // 7
    Uint16 PREALARM :1;          // 8
    Uint16 DCLINKLOW :1;         // 9
    Uint16 DCLINKHIGH :1;        // 10
    Uint16 DCLINKOK :1; // 11
    Uint16 INVRTROK:1; // 12
    Uint16 CURMODE:1;       // 13
    Uint16 BYPASSOK :1; // 14
    Uint16 BITSVALID :1;         // 15
};

#define     INVRTRHILOBITS  0x003F
#define     INVRTRHIBITS    0x0038
typedef union {
    N16 all;
    struct LIMIT_BITS bit;
} LMTMODE;

struct LIMIT_BITS1 {                 // bits  description
    Uint16 INVRLOW :1;            // 0
    Uint16 INVYLOW :1;            // 1
    Uint16 INVBLOW :1;            // 2
    Uint16 INVRHIGH :1;           // 3
    Uint16 INVYHIGH :1;           // 4
    Uint16 INVBHIGH :1;           // 5
    Uint16 OC_LEVEL1_R :1;        // 6
    Uint16 OC_LEVEL1_Y :1;        // 7
    Uint16 OC_LEVEL1_B :1;        // 8
    Uint16 OC_LEVEL2_R:1;         // 9
    Uint16 OC_LEVEL2_Y:1;         //10
    Uint16 OC_LEVEL2_B:1;         //11
    Uint16 OC_LEVEL3_R :1;        // 12
    Uint16 OC_LEVEL3_Y :1;        // 13
    Uint16 OC_LEVEL3_B :1;        // 14
    Uint16 BITSVALID :1;          // 15
};
typedef union {
    N16 all;
    struct LIMIT_BITS1 bit;
} LMTMODE1;

struct SYSOK_BITS {             // bits  description
    Uint16 DINPU1 :1;  // 0
    Uint16 DINPU2 :1;  // 1
    Uint16 DINPU3 :1;  // 2
    Uint16 DINPU4 :1;  // 3
    Uint16 DINPU5 :1;  // 4
    Uint16 DINPU6 :1;  // 5
    Uint16 DINPU7 :1;  // 6
    Uint16 DINPU8 :1;  // 7
    Uint16 DINPU9 :1;  // 8
    Uint16 DINPU10 :1;  // 9
    Uint16 VINVROK :1;  // 10
    Uint16 VINVYOK :1;  // 11
    Uint16 VINVBOK :1;  // 12
    Uint16 VBYPROK :1;  // 13
    Uint16 VBYPYOK :1;  // 14
    Uint16 VBYPBOK :1;  // 15
};
typedef union {
    N16 all;
    struct SYSOK_BITS bit;
} OKMODE;

struct COMMOK_BITS {             // bits  description
    Uint16 CAN_PFC :1;  // 0
    Uint16 CAN_MASTER :1;  // 1
    Uint16 CAN_SLAVE :1;  // 2
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
	N16	eVinvR;
	N16	eVinvY;
	N16	eVinvB;
	N16	eVbypR;
	N16	eVbypY;
	N16	eVbypB;
	N16 eIinvR;
	N16 eIinvY;
	N16 eIinvB;
	N16	ekVA_R;
	N16	ekVA_Y;
	N16	ekVA_B;
	N16	ekW_R;
	N16	ekW_Y;
	N16	ekW_B;
	N16	InvFreq;
	N16	BypFreq;
	N16	eVbat;
	N16	DcLinkV;
	N16	eIbat;
	N16	notUsed00;
	N16	notUsed01;
} eventPacket;

/* future plan:
 * max of all analog values, 10 x 10 x 3
 * for last 10 cycles
 *
 */



//Function Protoytpes

void		ModbusProcessing(void);         // SCIA, RS485 #2
void		ModbusProcessing1(void);        // SCIB, RS485 #1 (GUI)
N16 		pi_com(N16 port);
void		update_datablock(void);
N16 		update_one(unsigned int, unsigned int);

void		GpioInit(void);
void		EPwmInit();
void		AdcInit();
void		InitTimers();
void		InitSCIA(N16 Baud);
void        InitSCIB(void);
void        InitFaultXbar(void);
void        InitMCANPins(void);
void        DesatTripInit(void);
void        StaticSwitchDrivesInit(void);
void        AdcTriggerInit(void);
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

void		setPiRegulator(PI_CONTROLLER *p, float32_t kp, float32_t ki, float32_t ui, float32_t umax, float32_t umin);
void		setLowPassFilter(LPF *f, float32_t T, float32_t y);
N16			eepr_status(N16 chipnum);
void		eeprom_write(Uint16 addr, N8 nbytes);
void		eeprom_read(Uint16 addr, N8 nbytes);
void		spi_init(void);
N8          write_enable(void);
void        storeArrayToEEPROM(void);
void        sendLog(N16 nPkt);
void        storeEvents(void);
void        saveRecordNumber(void);
void        EraseEventLog(void);

void		AdcOffsetSelfCal(void);
void		snapShot(N16 st_addr);
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

void 		setHarmonicOscillator(sincos* t);

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
void    InitHiLo(MIN_MAX *x, float32_t *v1, float32_t *hlmt, float32_t *llmt, float32_t *hdly, float32_t *ldly);
void    CheckHi(MIN_MAX *x);
void    CheckHiLo(MIN_MAX *x);
void    CheckHiLoH(MIN_MAX *x);
void    InitHiLoParams(void);
inline void getReferenceVoltage(void);
void    ePWMInit_INV(N16 ch1, N16 ch2, N16 ch3 );
void    ePWMInit_SMPS(N16 chnl1, N16 chnl2);
N16     CRC16(N16 *dat, N16 nbytes);
void    update_datablock_Int(void);
inline void StaticSwitchControl(void);
void    InitDrives(N16 ch1);
void    PID_GRANDO_F_init(PID_GRANDO_F_CONTROLLER *v);
void    InitPID(PID_GRANDO_F_CONTROLLER *v);
void    setPIDRegulator(PID_GRANDO_F_CONTROLLER *p, float32_t kp, float32_t ki, float32_t kd,  float32_t kr, float32_t km, float32_t umax, float32_t umin);
void    setPiRegulator_old(PIreg *p, float32_t kp, float32_t ki, float32_t ui, float32_t umax, float32_t umin);
void    RestoreSettingsInt(void);
void    storeArrayToEEPROMInt(void);
void    SetFactoryDefaultsInt(void);
N16     UpdateOneInt(N16 adr, int val);
N16     eeprom_read_word(Uint16 addr);
N16     eeprom_write_word(Uint16 addr, N16 val);
void    InitPLL_1ph(void);
void    UpdateRelayFlag(void);
void    UpdateRelays(void);
void    InitDACs(void);
void    configureDAC(uint16_t dac_num);
void    UpdateDAC(N16 channel, N16 val);
void WriteRecordPointer(void);
void setPIRegulator(PI_CONTROLLER *p, float32_t kp, float32_t ki, float32_t umax, float32_t umin);
void StaticSwitchControl_Master(void);
void StaticSwitchControl_Slave(void);
void    MCANBusProcessing(void);
N16     ParTransmit(N16 id, long v1, long v2);      // CAN-B paralleling frame
N16     MCANBusOff(void);
inline void    NormalOperation(void);
void    PR_Reset(void);
void    PR_UpdateCoeffs(void);
#endif	//__USINV25KVA_H__
