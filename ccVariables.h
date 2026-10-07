#ifndef __CCVARIABLESH__
#define __CCVARIABLESH__

#include "pi.h"
#include "../include/pi.h"
#include "rectDefines.h"
#include "RectParams.h"
#include "controlMacros.h"
#include "pll3ph.h"
#include "can.h"
#include "can1.h"
#include "mcan.h"

//CCEXTERN    float vdcCycleFbk INIT0;
CCEXTERN    float idc1CycleFbk INIT0;
CCEXTERN    float idc2CycleFbk INIT0;
CCEXTERN    float idcCycleFbk INIT0;
CCEXTERN    float ibatCycleFbk INIT0;

CCEXTERN    float    idcSignal INIT0;
CCEXTERN    volatile float    angle INIT0;
CCEXTERN    float    sin600 INIT0;

CCEXTERN    volatile N32 alpha_R_start INIT0;
CCEXTERN    volatile N32 alpha_Y_start INIT0;
CCEXTERN    volatile N32 alpha_B_start INIT0;


CCEXTERN    volatile N16 temp_AdcIsrCount INIT0;
CCEXTERN    volatile N16 temp_T0IsrCount INIT0;
CCEXTERN    volatile N16 temp_T1IsrCount INIT0;
CCEXTERN    volatile N16 temp_T2IsrCount INIT0;
CCEXTERN    volatile N16 temp_zcdRCount INIT0;
CCEXTERN    volatile N16 temp_zcdYCount INIT0;
CCEXTERN    volatile N16 temp_zcdBCount INIT0;
CCEXTERN    volatile N16 temp_canRxCount INIT0;
CCEXTERN    volatile N16 canInitTimer INIT0;
CCEXTERN    volatile N16 AutoSaveTimer INIT0;
CCEXTERN    volatile N16 freqNotSet INIT0;
CCEXTERN    volatile N16 tmp_cycle3count INIT0;
CCEXTERN    volatile N16 ncycle INIT0;

CCEXTERN    volatile Uint16 last_stored_time INIT0;
//CCEXTERN    volatile Uint16 recordWriteTimer INIT0;
CCEXTERN    volatile Uint16 old_RecordNumber INIT0;

CCEXTERN    volatile float    extVref INIT0;
CCEXTERN    volatile float    extIbatref INIT0;
CCEXTERN    volatile float    pll_omega INIT0;

CCEXTERN    volatile float fact1 INIT0;
CCEXTERN    volatile float fact2 INIT0;

CCEXTERN    volatile long count_60_Sum INIT0;
CCEXTERN    volatile long count_60_Sum_1 INIT0;
CCEXTERN    volatile N16 count60_count INIT0;

CCEXTERN    float      RP1    INIT0;
CCEXTERN    float      RN1    INIT0;
CCEXTERN    float      YP1    INIT0;
CCEXTERN    float      YN1    INIT0;
CCEXTERN    float      BP1    INIT0;
CCEXTERN    float      BN1    INIT0;
CCEXTERN    float      RP2    INIT0;
CCEXTERN    float      RN2    INIT0;
CCEXTERN    float      YP2    INIT0;
CCEXTERN    float      YN2    INIT0;
CCEXTERN    float      BP2    INIT0;
CCEXTERN    float      BN2    INIT0;

CCEXTERN    float      ZCDP    INIT0;
CCEXTERN    float      ZCDN    INIT0;

CCEXTERN    volatile float      referenceV    INIT0;
CCEXTERN    volatile float      referenceI    INIT0;
CCEXTERN    volatile float      mainsFailLevel    INIT0;
CCEXTERN    volatile float      ocLevel    INIT0;

CCEXTERN    volatile int setAddr INIT0;
CCEXTERN    volatile N16 paramUpdate INIT0;

CCEXTERN    MIN_MAX chkFrequency;
CCEXTERN    MIN_MAX chkVr;
CCEXTERN    MIN_MAX chkVy;
CCEXTERN    MIN_MAX chkVb;
CCEXTERN    MIN_MAX chkIr;
CCEXTERN    MIN_MAX chkIy;
CCEXTERN    MIN_MAX chkIb;
CCEXTERN    MIN_MAX chkVdc;
CCEXTERN    MIN_MAX chkIdc;
CCEXTERN    MIN_MAX chkIbat;
CCEXTERN    MIN_MAX chkVbat;
CCEXTERN    MIN_MAX chkVload;

CCEXTERN    RELAY MainConRLY[12];
CCEXTERN    RELAY IOM1RLY[16];
CCEXTERN    RELAY IOM2RLY[16];

CCEXTERN    N32 CanID INIT0;
CCEXTERN    N16 InitCANFlag INIT0;
CCEXTERN    N16 msgPending INIT0;
CCEXTERN    N16 txinProgress  INIT0;
CCEXTERN    tCANMsgObject sTXCANMessage;
CCEXTERN    tCANMsgObject sRXCANMessage;
CCEXTERN    N16 oldmsCount  INIT0;
CCEXTERN    N16 oldmsCount1  INIT0;
CCEXTERN    N16 volatile ms_old  INIT0;
CCEXTERN    N16 canTxPakcetCount  INIT0;
CCEXTERN    N16 canTxPakcetCount1  INIT0;
CCEXTERN    unsigned char txMsgData[8];
CCEXTERN    unsigned char ucRXMsgData[8];

CCEXTERN    Uint16  eventCounter INIT0;
CCEXTERN    Uint16  msgCounter INIT0;
CCEXTERN    Uint16  CANDataAddress INIT0;
CCEXTERN    Uint16  CANRXAddress INIT0;
CCEXTERN    Uint16  errorRx INIT0;      // SCIA (RS485 #2) receive error
CCEXTERN    Uint16  errorRx1 INIT0;     // SCIB (RS485 #1) receive error count
//CCEXTERN    Uint16  LedFlag INIT0;

CCEXTERN    Uint16  R_msCount INIT0;
CCEXTERN    Uint16  Y_msCount INIT0;
CCEXTERN    Uint16  B_msCount INIT0;

CCEXTERN    LPF32 lpf32Vdc, lpf32Vload;
CCEXTERN    LPF32 lpf32Idc;
CCEXTERN    LPF32 lpf32Ibat;
CCEXTERN    LPF32 lpf32Vbat;

CCEXTERN    LPF32 lpf32Ir, lpf32Iy, lpf32Ib;

CCEXTERN    LPF32 lpf32IBridge1;
CCEXTERN    LPF32 lpf32IBridge2;

CCEXTERN    volatile Uint16 StartupTimer INIT0;
CCEXTERN    volatile Uint16 PrechargeTimer INIT0;
CCEXTERN    volatile Uint16 WaitDelayTimer INIT0;

CCEXTERN    volatile Uint16 VDCLowLimitTimer INIT0;
CCEXTERN    volatile Uint16 VDCHighLimitTimer INIT0;
CCEXTERN    volatile Uint16 IDCLowLimitTimer INIT0;
CCEXTERN    volatile Uint16 IDCHighLimitTimer INIT0;
CCEXTERN    volatile Uint16 tmp_zcdCount    INIT0;
CCEXTERN    volatile Uint16 tmp_zcdnCount    INIT0;
CCEXTERN    volatile Uint16 VInpRLowLimitTimer INIT0;
CCEXTERN    volatile Uint16 VInpRHighLimitTimer INIT0;
CCEXTERN    volatile Uint16 VInpYLowLimitTimer INIT0;
CCEXTERN    volatile Uint16 VInpYHighLimitTimer INIT0;
CCEXTERN    volatile Uint16 VInpBLowLimitTimer INIT0;
CCEXTERN    volatile Uint16 VInpBHighLimitTimer INIT0;
CCEXTERN    volatile Uint16 IdcHLTimer INIT0;
CCEXTERN    volatile Uint16 tmp_pidcount INIT0;
CCEXTERN    volatile long periodCount INIT0;

CCEXTERN    volatile Uint16 FreqR_mscount INIT0;
CCEXTERN    volatile Uint16 FreqY_mscount INIT0;
CCEXTERN    volatile Uint16 FreqB_mscount INIT0;

CCEXTERN    volatile Uint16 rp_count INIT0;
CCEXTERN    volatile Uint16 bp_count INIT0;
CCEXTERN    volatile Uint16 rseqCount INIT0;
CCEXTERN    volatile Uint16 zcdCount INIT0;
CCEXTERN    volatile long halfWidth INIT0;
CCEXTERN    volatile Uint16 ZCDR INIT0;

CCEXTERN    volatile Uint16 VinprOK_timer INIT0;
CCEXTERN    volatile Uint16 VinpyOK_timer INIT0;
CCEXTERN    volatile Uint16 VinpbOK_timer INIT0;

CCEXTERN    volatile N16    InputOKtimer INIT0;
CCEXTERN    volatile N16    triggered INIT0;

CCEXTERN    volatile Uint16 doRead INIT0;
CCEXTERN    volatile Uint16 wptr INIT0;
CCEXTERN    volatile Uint16 cycleptr INIT0;
CCEXTERN    Uint16 samplesPerCycle INIT0;
CCEXTERN    volatile Uint16    nPoints INIT0;

CCEXTERN	N16 spiUpdate INIT0;
CCEXTERN	volatile N16 epr_read_enable INIT0;
CCEXTERN	volatile N16 epr_delay INIT0;
CCEXTERN	volatile N16 spi_timeout INIT0;
CCEXTERN	volatile N16 spi_error INIT0;
CCEXTERN	volatile N16 inc INIT0;
CCEXTERN	N16 spiTimeOutCount INIT0;
CCEXTERN	N16 spiTimeout INIT0;
CCEXTERN	Uint16 spiReadBuffer[32] INITARRAY0;
CCEXTERN	Uint16 spiWriteBuffer[32] INITARRAY0;


CCEXTERN    unsigned char rx_buffer[220] INITARRAY0;
CCEXTERN    unsigned char rx_buffer1[220] INITARRAY0;
CCEXTERN    unsigned char tx_buffer[420] INITARRAY0;

CCEXTERN    volatile Uint16 interchar_silent_timer INIT0;
CCEXTERN    volatile Uint16 char_waiting INIT0;
CCEXTERN    volatile Uint16 chars_rxd INIT0;
CCEXTERN    volatile Uint16 address INIT0;
CCEXTERN    volatile Uint16 vptr INIT0;
CCEXTERN	unsigned char *rxbuffer;
CCEXTERN    unsigned char *rxbuffer1;

CCEXTERN	unsigned char *datablock;
CCEXTERN	int ArrayParamsI[NPARAMS + NPARAMS] INITARRAY0;
CCEXTERN    float ArrayParamsF[NPARAMS + 50] INITARRAY0;
CCEXTERN    int DOParams[NPARAMS] INITARRAY0;
CCEXTERN    int DIParams[NPARAMS] INITARRAY0;
CCEXTERN	volatile float waveBuffer[SNAP_SIZE] INITARRAY0;

CCEXTERN    volatile float vdc_buf[150] INITARRAY0;
CCEXTERN    volatile float idc1_buf[150] INITARRAY0;
CCEXTERN    volatile float idc2_buf[150] INITARRAY0;
CCEXTERN    volatile float ibat_buf[150] INITARRAY0;

CCEXTERN    volatile int setAddrF INIT0;
CCEXTERN    volatile float setValueF INIT0;
CCEXTERN    volatile int setValueI INIT0;
CCEXTERN    volatile int setAddrI INIT0;

CCEXTERN    volatile N16 dummyEvent INIT0;
CCEXTERN    volatile N16 iom1_canFailTimer INIT0;
CCEXTERN    volatile N16 iom2_canFailTimer INIT0;

CCEXTERN	N16 _RTCSecond INIT0;
CCEXTERN	N16 _RTCMinute INIT0;
CCEXTERN	N16 _RTCHour INIT0;
CCEXTERN	N16 _RTCDate INIT0;
CCEXTERN	N16 _RTCMonth INIT0;
CCEXTERN	N16 _RTCYear INIT0;

CCEXTERN	volatile Uint16 rms_samplecount INIT0;
CCEXTERN	volatile Uint16 rms_update_flag INIT0;
CCEXTERN	volatile Uint16 frame_complete INIT0;
CCEXTERN	volatile Uint16 live_frame INIT0;
CCEXTERN	volatile Uint16 rsync INIT0;
CCEXTERN	volatile Uint16 old_ms INIT0;
CCEXTERN	volatile Uint16 delayCount INIT0;
CCEXTERN	volatile Uint16 delayCount1 INIT0;

CCEXTERN    volatile Uint16 frame_complete1 INIT0;
CCEXTERN    volatile Uint16 live_frame1 INIT0;
CCEXTERN    volatile Uint16 interchar_silent_timer1 INIT0;
CCEXTERN    volatile Uint16 char_waiting1 INIT0;
CCEXTERN    volatile Uint16 chars_rxd1 INIT0;
CCEXTERN    volatile Uint16 address1 INIT0;
CCEXTERN    volatile Uint16 st_addr1 INIT0;
CCEXTERN    volatile Uint16 nregs1 INIT0;

CCEXTERN    RtcType     RTC;
CCEXTERN	volatile Uint16 status_flag INIT0;
CCEXTERN	volatile Uint16 msDelayCounter INIT0;
CCEXTERN	volatile Uint16 transmitNow INIT0;
CCEXTERN	volatile Uint16 st_addr INIT0;
CCEXTERN	volatile Uint16 nregs INIT0;
CCEXTERN	volatile Uint16 mainLoopCount INIT0;

CCEXTERN	volatile Uint16 seconds_old INIT0;
CCEXTERN	volatile Uint16 rxChar INIT0;
CCEXTERN	volatile Uint16 txNow INIT0;
CCEXTERN	volatile Uint16 clearFlag INIT0;
CCEXTERN	volatile Uint16 saveCounter INIT0;
CCEXTERN	volatile Uint16 lastCount INIT0;

CCEXTERN	volatile Uint16 CANOKTimer INIT0;
CCEXTERN    volatile Uint16 ReferenceChangedFlag INIT0;


CCEXTERN	volatile Uint16 DI1timer INIT0;
CCEXTERN	volatile Uint16 DI2timer INIT0;
CCEXTERN	volatile Uint16 DI3timer INIT0;
CCEXTERN    volatile Uint16 DI4timer INIT0;
CCEXTERN    volatile Uint16 DI5timer INIT0;
CCEXTERN    volatile Uint16 DI6timer INIT0;
CCEXTERN    volatile Uint16 DI7timer INIT0;
CCEXTERN    volatile Uint16 DI8timer INIT0;
CCEXTERN    volatile Uint16 DI9timer INIT0;
CCEXTERN    volatile Uint16 DI10timer INIT0;
CCEXTERN    volatile Uint16 DI11timer INIT0;
CCEXTERN    volatile Uint16 DI12timer INIT0;

CCEXTERN	volatile Uint16 msDelayCount INIT0;
CCEXTERN    volatile Uint16 CycleCount INIT0;
CCEXTERN	volatile Uint16 maxUnbalanceCurrent INIT0;
CCEXTERN    volatile float vdcAvg INIT0;
CCEXTERN    volatile float vAlpha INIT0;
CCEXTERN    volatile float vR_rectified INIT0;
CCEXTERN    volatile float vY_rectified INIT0;
CCEXTERN    volatile float vB_rectified INIT0;
CCEXTERN    float vOutput INIT0;
CCEXTERN    volatile float valpha INIT0;
CCEXTERN    volatile float vbeta INIT0;

CCEXTERN    volatile float vBeta INIT0;
CCEXTERN     float vol_Rph INIT0;
CCEXTERN     float vol_Yph INIT0;
CCEXTERN     float vol_Bph INIT0;
CCEXTERN    volatile float freqRcount INIT0;
CCEXTERN    volatile float freqYcount INIT0;
CCEXTERN    volatile float freqBcount INIT0;
CCEXTERN    volatile Uint16 seqFailTimer  INIT0;
CCEXTERN    volatile float FreqCountArray_R[16];
CCEXTERN    volatile Uint16 fptr INIT0;

CCEXTERN	float minVref INIT0;
CCEXTERN    float minIref INIT0;


CCEXTERN   volatile N16 samplingCount_tmp INIT0;
//CCEXTERN   volatile N16 samplingCount INIT0;

//CCEXTERN    float idc2CycleSum INIT0;
//CCEXTERN    float ibatCycleSum INIT0;
CCEXTERN    float vdcCycleSum INIT0;
//
//CCEXTERN    float vdcCycleFbk INIT0;
//CCEXTERN    float idc1CycleFbk INIT0;
//CCEXTERN    float idc2CycleFbk INIT0;
//CCEXTERN    float idcCycleFbk INIT0;
//CCEXTERN    float ibatCycleFbk INIT0;

CCEXTERN    float LowValue INIT0;
CCEXTERN    float LowDelay INIT0;
CCEXTERN    volatile Uint16 nBytes INIT0;
CCEXTERN    volatile Uint16    zcdptr INIT0;

CCEXTERN	float *waveptr;

CCEXTERN	char UnitSerial[20] INITARRAY0;
CCEXTERN	N16 specialCommnad INIT0;
CCEXTERN	N16 eventCount INIT0;

CCEXTERN	N16 AlarmFlag1_old INIT0;
CCEXTERN	N16 AlarmFlag2_old INIT0;
CCEXTERN	N16 AlarmFlag3_old INIT0;
CCEXTERN    N16 AlarmFlag4_old INIT0;
CCEXTERN    N16 softcounter INIT0;
CCEXTERN    N16 prevCountR INIT0;
CCEXTERN    N16 prevCountY INIT0;
CCEXTERN    N16 prevCountB INIT0;

CCEXTERN    volatile Uint16 dataLogTimer INIT0;
CCEXTERN    volatile Uint16 dataLog INIT0;
CCEXTERN    volatile Uint16 LastWriteTimer INIT0;
CCEXTERN    eventPacket Events[4];
CCEXTERN    eventPacket EventTX;

CCEXTERN	volatile N16	snap_start INIT0;
CCEXTERN	volatile N16	snap_end INIT0;
CCEXTERN	volatile N16 snap_ptr INIT0;
CCEXTERN    volatile N16 updateLED INIT0;
CCEXTERN	Uint16 mainLoopCount_prev INIT0;

CCEXTERN    Uint16 equalizeTimer INIT0;
CCEXTERN    Uint16 IbatEqRelease INIT0;
CCEXTERN    Uint16 eqReleaseTimer INIT0;
CCEXTERN    Uint16 transferCount INIT0;
CCEXTERN    Uint16 boostReleaseCount INIT0;
CCEXTERN    Uint16 BattTestTimer INIT0;


CCEXTERN	Uint16 Error INIT0;
CCEXTERN	SYSMODE SystemFlag;
CCEXTERN    SYSMODE SystemFlag_Rem;
CCEXTERN	LMTMODE LimitFlag;
CCEXTERN    OKMODE  SysOkFlag;
CCEXTERN    BATMODE BattFlag;


CCEXTERN    N16 DIpins[16];
CCEXTERN    N16 DOpins[16];
CCEXTERN    sincos pll_theta;
CCEXTERN	float *waveptr;

CCEXTERN    volatile N16    doPID INIT0;
CCEXTERN    volatile N16    capcntr INIT0;
CCEXTERN    volatile N16    Blinkflag INIT0;
CCEXTERN    volatile N16    LedFlag_old INIT0;
CCEXTERN    volatile N16    countPtr INIT0;
CCEXTERN    volatile Uint16 OffsetNotDone INIT0;
CCEXTERN    volatile Uint16 ShiftedFlag INIT0;

CCEXTERN    volatile long offsets[20] INITARRAY0;

CCEXTERN    LPF lpf_vInpR, lpf_vInpY, lpf_vInpB;
CCEXTERN    LPF lpf_iInpR, lpf_iInpY, lpf_iInpB;
CCEXTERN    LPF lpf_vdc, lpf_vload, lpf_idc1, lpf_idc2, lpf_ibat;

CCEXTERN    LPF lpf_vload;
CCEXTERN    LPF lpf_vbat;
CCEXTERN    LPF lpf_bias;
CCEXTERN    LPF lpf_vref;
CCEXTERN    LPF lpf_iref;
CCEXTERN    LPF lpf_temp1;
CCEXTERN    LPF lpf_temp2;
CCEXTERN    LPF lpf_temp3;      // ambient (Common Controller v2, B1)
CCEXTERN    LPF lpf_ifback;
CCEXTERN    LPF lpf_ibatfback;
CCEXTERN    LPF lpf_remVref;
CCEXTERN    LPF lpf_pid;

CCEXTERN    rms_meter vdcRipple_meter;
CCEXTERN    rms_meter volt_meter_R, volt_meter_Y, volt_meter_B;
CCEXTERN    rms_meter cur_meter_R, cur_meter_Y, cur_meter_B;
CCEXTERN    rms_meter kw_meter_R, kw_meter_Y, kw_meter_B;
CCEXTERN    rms_meter vll_meter_R, vll_meter_Y, vll_meter_B;


CCEXTERN    PI_CONTROLLER  pid_vdc;
CCEXTERN    PI_CONTROLLER  pid_idc;
CCEXTERN    PI_CONTROLLER  pid_idc1;
CCEXTERN    PI_CONTROLLER  pid_idc2;
CCEXTERN    PI_CONTROLLER  pid_ibat;
CCEXTERN    PI_CONTROLLER  pid_iac;
/*
CCEXTERN    PID_GRANDO_F_CONTROLLER  pid_vdc;
CCEXTERN    PID_GRANDO_F_CONTROLLER  pid_idc;
CCEXTERN    PID_GRANDO_F_CONTROLLER  pid_idc1;
CCEXTERN    PID_GRANDO_F_CONTROLLER  pid_idc2;
CCEXTERN    PID_GRANDO_F_CONTROLLER  pid_ibat;
CCEXTERN    PID_GRANDO_F_CONTROLLER  pid_iac;
*/
CCEXTERN    volatile long    FreqHiCount INIT0;
CCEXTERN    volatile long    FreqLoCount INIT0;

CCEXTERN    LPF32 lpf32_idc1, lpf32_idc2, lpf32_ibat;
CCEXTERN    LPF32 lpf8_idc1, lpf8_idc2;

CCEXTERN    LPF32 lpf32_vry,lpf32_vyb, lpf32_vbr, lpf32_vripple;
CCEXTERN    LPF32 lpf32_vr,lpf32_vy, lpf32_vb;
CCEXTERN    LPF32 lpf32_ir,lpf32_iy, lpf32_ib, lpf32_freq;
CCEXTERN    LPF32 lpf32_vdc, lpf32_vbat,lpf32_vload;
CCEXTERN    pllBlock  pllGrid;

CCEXTERN    volatile int    GridFrequencyLL INIT0;
CCEXTERN    volatile int    GridFrequencyHL INIT0;
CCEXTERN    volatile int    GrdFrqncyL_HL_delay INIT0;
CCEXTERN    volatile int    piCounter INIT0;


CCEXTERN    volatile float    vdcReference INIT0;
CCEXTERN    volatile float    idcReference INIT0;
CCEXTERN    volatile float    ibatReference INIT0;
CCEXTERN    volatile float    iacRef INIT0;

CCEXTERN    MCAN_RxBufElement rxMsg[1];
CCEXTERN    MCAN_RxBufElement rxMsg1;
CCEXTERN    MCAN_TxBufElement txMsg[1];
CCEXTERN    MCAN_RxFIFOStatus RxFS;


#endif //__CCVARIABLESH__
