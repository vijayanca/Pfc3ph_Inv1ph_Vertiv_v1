
#ifndef __CCVARIABLES__
#define __CCVARIABLES__


#include "device.h"
#include "driverlib.h"
#include "include\inc\hw_types.h"
#include "include\pi.h"
#include "SVGEN_CLA.h"
#include "PID_GRANDO_F.h"
#include "pi.h"
#include "can1.h"
#include <math.h>
#include "mcan.h"
#include "inv3phDefines.h"
#include "inv3phParams.h"
#include "pll3ph.h"
#include "controlMacros.h"
#include "claShared.h"
#include "BoardMacros.h"


CCEXTERN    union canPacket rxPacket;
CCEXTERN    union canPacket txPacket;
CCEXTERN    volatile Uint16 InputOK_timer INIT0;
CCEXTERN    volatile Uint16 inverterOK INIT0;
CCEXTERN    volatile Uint16 canHWDone INIT0;

CCEXTERN   volatile N16 InvPwmPeriod INIT0;
CCEXTERN   volatile N16 OffsetNotDone INIT0;


CCEXTERN    long offsets[20] INITARRAY0;
CCEXTERN    N16 capCount INIT0;
CCEXTERN    volatile N16 temp_AdcIsrCount INIT0;
CCEXTERN    volatile N16 temp_T0IsrCount INIT0;

CCEXTERN    int DOParams[NPARAMS] INITARRAY0;
CCEXTERN    int DIParams[NPARAMS] INITARRAY0;

CCEXTERN    volatile Uint16 parTimer INIT0;

CCEXTERN    volatile Uint16 last_stored_time INIT0;
CCEXTERN    volatile Uint16 recordWriteTimer INIT0;
CCEXTERN    volatile Uint16 old_RecordNumber INIT0;
CCEXTERN    Uint16 PowerOnDelay INIT0;
CCEXTERN    eventPacket Events[4];
CCEXTERN    eventPacket EventTX;

CCEXTERN    volatile float32_t    reference_current INIT0;
CCEXTERN    volatile float32_t dummy_comp INIT0;
CCEXTERN    volatile float32_t curAlpha INIT0;
CCEXTERN    volatile float32_t curBeta INIT0;
CCEXTERN    volatile float32_t temp_deref INIT0;

CCEXTERN    LPF32 lpf32_vinvr,lpf32_vinvy, lpf32_vinvb;
CCEXTERN    LPF32 lpf32_iinvr,lpf32_iinvy, lpf32_iinvb;
CCEXTERN    LPF32 lpf32_vllr,lpf32_vlly, lpf32_vllb;

CCEXTERN    LPF32 lpf32_voutr,lpf32_vouty, lpf32_voutb;
CCEXTERN    LPF32 lpf32_ioutr,lpf32_iouty, lpf32_ioutb;
CCEXTERN    LPF32 lpf32_vbypr,lpf32_vbypy, lpf32_vbypb;
CCEXTERN    LPF32 lpf32_vbypry,lpf32_vbypyb, lpf32_vbypbr;


CCEXTERN    volatile Uint16 DItimers[16] INITARRAY0;
CCEXTERN    volatile Uint16 DIpins[16] INITARRAY0;
CCEXTERN    N32 CanID INIT0;
CCEXTERN    N16 InitCANFlag INIT0;
CCEXTERN    N16 msgPending INIT0;
CCEXTERN    N16 autoSaveFlag  INIT0;
CCEXTERN    N16 txMsgCount_tmp  INIT0;
//CCEXTERN    N16 Dmax_Ramping  INIT0;
CCEXTERN    N16 waitTimer  INIT0;


CCEXTERN    tCANMsgObject sTXCANMessage;
CCEXTERN    tCANMsgObject sRXCANMessage;

CCEXTERN    tCANMsgObject sTXMCANMessage;
CCEXTERN    tCANMsgObject sRXMCANMessage;

CCEXTERN    volatile int setAddrF INIT0;
CCEXTERN    volatile float setValueF INIT0;
CCEXTERN    volatile int setValueI INIT0;
CCEXTERN    volatile int setAddrI INIT0;


CCEXTERN    volatile int setAddr INIT0;
CCEXTERN    volatile int setValue INIT0;
CCEXTERN    N16 acceptResetCommnad  INIT0;


CCEXTERN    N16 paramUpdate  INIT0;
CCEXTERN    N16 screenNumber  INIT0;
CCEXTERN    N16 oldmsCount  INIT0;
CCEXTERN    N16 ms_old  INIT0;
CCEXTERN    N16 canTxPakcetCount  INIT0;
CCEXTERN    unsigned char txMsgData[8];
CCEXTERN    unsigned char ucRXMsgData[8];

CCEXTERN    volatile float32_t    Angle INIT0;
CCEXTERN    volatile float32_t    angleActive INIT0;

CCEXTERN    volatile float32_t    Vdc_Ref INIT0;
CCEXTERN    volatile float32_t    Idc_Ref INIT0;
CCEXTERN    volatile float32_t    soft_VdcRef INIT0;
CCEXTERN    volatile float32_t    soft_IdcRef INIT0;

CCEXTERN    float32_t    Va INIT0;
CCEXTERN    float32_t    Vb INIT0;
CCEXTERN    float32_t    Vc INIT0;

CCEXTERN    volatile float32_t    t1 INIT0;
CCEXTERN    volatile float32_t    t2 INIT0;
CCEXTERN    volatile float32_t    temp_sv1 INIT0;
CCEXTERN    volatile float32_t    temp_sv2 INIT0;
CCEXTERN    volatile float32_t    iOffset INIT0;
CCEXTERN    volatile float32_t    Vry INIT0;
CCEXTERN    volatile float32_t    Vyb INIT0;
CCEXTERN    volatile float32_t    Vbr INIT0;

CCEXTERN    volatile float32_t    Angle_000 INIT0;
CCEXTERN    volatile float32_t    Angle_120 INIT0;
CCEXTERN    volatile float32_t    Angle_240 INIT0;


CCEXTERN    float32_t fund_piout  INIT0;
CCEXTERN    float32_t volt_gain   INIT0;
CCEXTERN    float32_t reference_current_R INIT0;
CCEXTERN    float32_t reference_current_Y INIT0;
CCEXTERN    float32_t reference_current_B INIT0;
CCEXTERN    float32_t reference_voltage_R INIT0;
CCEXTERN    float32_t reference_voltage_Y INIT0;
CCEXTERN    float32_t reference_voltage_B INIT0;

CCEXTERN    volatile float32_t    dcLinkReference INIT0;
CCEXTERN    volatile float32_t    dcLinkReferenceH INIT0;
CCEXTERN    volatile float32_t    dcLinkReferenceL INIT0;
CCEXTERN    volatile float32_t    dcLinkSoftRef INIT0;
CCEXTERN    volatile float32_t    chprRef INIT0;
CCEXTERN    volatile float32_t    chprIRef INIT0;
CCEXTERN    volatile float32_t    soft_ChopperRef INIT0;
CCEXTERN    float32_t LowValue INIT0;
CCEXTERN    float32_t LowDelay INIT0;
CCEXTERN    volatile float32_t    res_Ds INIT0;
CCEXTERN    volatile float32_t    res_Qs INIT0;

CCEXTERN    float32_t harm_ref_de INIT0;

CCEXTERN    int harm_select INIT0;
CCEXTERN    float32_t  temp_comp INIT0;
CCEXTERN    float32_t temp_qe_1 INIT0;
CCEXTERN    float32_t temp_de_1 INIT0;

//CCEXTERN    volatile float32_t remoteAngle INIT0;
//CCEXTERN    volatile float32_t remoteOmega INIT0;
//CCEXTERN    volatile float32_t remoteIref INIT0;
//CCEXTERN    volatile float32_t remoteVref INIT0;

CCEXTERN    float32_t overvoltage_setting INIT0;
CCEXTERN    float32_t voltage_ramp_slope  INIT0;
CCEXTERN    float32_t breaker_close_voltage   INIT0;
CCEXTERN    float32_t voltage_buildup_level   INIT0;
CCEXTERN    float32_t capacitor_voltage_reference INIT0;
CCEXTERN    float32_t voltage_reference_setting   INIT0;
CCEXTERN    float32_t initial_capacitor_reference INIT0;
CCEXTERN    float32_t power_factor_set  INIT0;
CCEXTERN    float32_t power_factor    INIT0;
CCEXTERN    float32_t v_alpha INIT0;
CCEXTERN    float32_t v_beta  INIT0;

CCEXTERN    float32_t ibatReference  INIT0;
CCEXTERN    float32_t vbatReference  INIT0;
CCEXTERN    float32_t buckPID  INIT0;

CCEXTERN    float32_t sumDCR   INIT0;
CCEXTERN    float32_t sumDCY   INIT0;
CCEXTERN    float32_t sumDCB   INIT0;


CCEXTERN    LPF lpf_curRef;

CCEXTERN    LPF lpf_Freq;
//CCEXTERN    LPF lpf_vout_de;
//CCEXTERN    LPF lpf_vout_qe;


CCEXTERN    N16 spiUpdate INIT0;
CCEXTERN    volatile N16 epr_read_enable INIT0;
CCEXTERN    volatile N16 epr_delay INIT0;
CCEXTERN    volatile N16 spi_timeout INIT0;
CCEXTERN    volatile N16 spi_error INIT0;
CCEXTERN    volatile N16 inc INIT0;
CCEXTERN    volatile N16 kbdCol INIT0;
CCEXTERN    volatile N16 kbdRow INIT0;
CCEXTERN    volatile N16 newPress INIT0;
CCEXTERN    volatile N16 toggleCount INIT0;
CCEXTERN    volatile N16 passwordLevel INIT0;
CCEXTERN    volatile N16 keyReleased INIT0;
CCEXTERN    volatile N16 menuActive INIT0;
CCEXTERN    volatile N16 lastKey INIT0;

CCEXTERN    int Grid_Freq  INIT0;
CCEXTERN    volatile N16 kbdRowStatus INIT0;

CCEXTERN    volatile N16 newKeypress INIT0;
CCEXTERN    volatile N16 lcdBackLightTimer INIT0;
CCEXTERN    volatile N16 menuTimer INIT0;
CCEXTERN    volatile N16 shift INIT0;
CCEXTERN    volatile N16 IbatEnable_old INIT0;
CCEXTERN    volatile N16 lcdBusy INIT0;
CCEXTERN    volatile N16 beat INIT0;
CCEXTERN    volatile int pwm1Ref_old INIT0;
CCEXTERN    volatile int pwm2Ref_old INIT0;

CCEXTERN    volatile Uint16 spiReadyTimer INIT0;
CCEXTERN    volatile float32_t zcdR INIT0;
CCEXTERN    unsigned char rx_buffer[240] INITARRAY0;

// Modbus on SCIB (RS485 #1, GUI port): same register map as SCIA, own frame state
CCEXTERN    unsigned char rx_buffer1[240] INITARRAY0;
CCEXTERN    unsigned char *rxbuffer1;
CCEXTERN    volatile Uint16 frame_complete1 INIT0;
CCEXTERN    volatile Uint16 live_frame1 INIT0;
CCEXTERN    volatile Uint16 interchar_silent_timer1 INIT0;
CCEXTERN    volatile Uint16 char_waiting1 INIT0;
CCEXTERN    volatile Uint16 chars_rxd1 INIT0;
CCEXTERN    volatile Uint16 address1 INIT0;
CCEXTERN    Uint16  errorRx1 INIT0;     // SCIB receive error count

CCEXTERN    N16 spiTimeOutCount INIT0;
CCEXTERN    N16 spiTimeout INIT0;

CCEXTERN    Uint16 spiReadBuffer[32] INITARRAY0;
CCEXTERN    Uint16 spiWriteBuffer[32] INITARRAY0;
CCEXTERN    volatile Uint16 quarterCycleCount INIT0;

CCEXTERN    unsigned char *rxbuffer;
CCEXTERN    unsigned char *datablock;

#ifndef PHASE3PH
CCEXTERN    float32_t  betaI_Buffer[110];
CCEXTERN    float32_t  betaVout_Buffer[110];
CCEXTERN    float32_t  betaVinv_Buffer[110];
CCEXTERN    float32_t  betaVbypass_Buffer[110];
CCEXTERN    volatile Uint16 betaPtr INIT0;
CCEXTERN     float32_t betaI INIT0;
CCEXTERN     float32_t betaV INIT0;
CCEXTERN     float32_t betaVout INIT0;
CCEXTERN     float32_t betaVbyp INIT0;
#endif

CCEXTERN    volatile float32_t waveBuffer[100] INITARRAY0; //50
CCEXTERN    volatile float32_t phasShift INIT0;

CCEXTERN    volatile float32_t FrequencyF INIT0;

CCEXTERN    volatile int posCounter INIT0;
CCEXTERN    volatile int negCounter INIT0;

CCEXTERN    volatile int VrMin INIT0;
CCEXTERN    volatile int VrMax INIT0;
CCEXTERN    volatile int midPoint INIT0;
CCEXTERN    volatile int biasNew INIT0;




CCEXTERN    N16 _RTCSecond INIT0;
CCEXTERN    N16 _RTCMinute INIT0;
CCEXTERN    N16 _RTCHour INIT0;
CCEXTERN    N16 _RTCDate INIT0;
CCEXTERN    N16 _RTCMonth INIT0;
CCEXTERN    N16 _RTCYear INIT0;

CCEXTERN    N16 LCDMsg3 INIT0;
CCEXTERN    N16 msg3ptr INIT0;

CCEXTERN    volatile Uint16 frame_complete INIT0;
CCEXTERN    volatile Uint16 live_frame INIT0;
CCEXTERN    volatile Uint16 old_ms INIT0;
CCEXTERN    volatile Uint16 delayCount INIT0;
CCEXTERN    volatile Uint16 delayCount1 INIT0;
CCEXTERN     volatile Uint16 VrOK_timer INIT0;
CCEXTERN    volatile Uint16 VyOK_timer INIT0;
CCEXTERN    volatile Uint16 VbOK_timer INIT0;
CCEXTERN    volatile Uint16 preChargeOK_timer INIT0;
CCEXTERN    volatile Uint16 InverterOK_timer INIT0;
CCEXTERN    volatile Uint16 con1DiffOK_timer INIT0;

CCEXTERN    volatile Uint16 mainsVoltageLL_timer INIT0;
CCEXTERN    volatile Uint16 mainsVoltageHL_timer INIT0;
CCEXTERN    volatile Uint16 VdcHighLimitTimer INIT0;
CCEXTERN    volatile Uint16 VdcLowLimitTimer INIT0;
CCEXTERN    volatile Uint16 PrechargeTimer INIT0;
CCEXTERN    volatile Uint16 VGridRLowLimitTimer INIT0;
CCEXTERN    volatile Uint16 VGridRHighLimitTimer INIT0;
CCEXTERN    volatile Uint16 VGridYLowLimitTimer INIT0;
CCEXTERN    volatile Uint16 VGridYHighLimitTimer INIT0;
CCEXTERN    volatile Uint16 VGridBLowLimitTimer INIT0;
CCEXTERN    volatile Uint16 VGridBHighLimitTimer INIT0;
CCEXTERN    volatile Uint16 DcLinkOKtimer INIT0;

CCEXTERN    volatile Uint16 status_flag INIT0;
CCEXTERN    volatile Uint16 interchar_silent_timer INIT0;
CCEXTERN    volatile Uint16 char_waiting INIT0;
CCEXTERN    volatile Uint16 chars_rxd INIT0;
CCEXTERN    volatile Uint16 address INIT0;
CCEXTERN    volatile Uint16 msDelayCounter INIT0;
CCEXTERN    volatile Uint16 transmitNow INIT0;
CCEXTERN    volatile Uint16 st_addr INIT0;
CCEXTERN    volatile Uint16 nregs INIT0;
CCEXTERN    volatile Uint16 nBytes INIT0;
CCEXTERN    volatile Uint16 mainLoopCount INIT0;

CCEXTERN    volatile Uint16 seconds_old INIT0;
CCEXTERN    volatile Uint16 rxChar INIT0;
CCEXTERN    volatile Uint16 txNow INIT0;
CCEXTERN    volatile Uint16 clearFlag INIT0;
CCEXTERN    volatile Uint16 saveCounter INIT0;
CCEXTERN    volatile Uint16 lastCount INIT0;
CCEXTERN    volatile Uint16 countInterval INIT0;

CCEXTERN    volatile Uint16 CANOKTimer INIT0;
CCEXTERN    volatile Uint16 DI0timer INIT0;
CCEXTERN    volatile Uint16 DI1timer INIT0;
CCEXTERN    volatile Uint16 DI2timer INIT0;
CCEXTERN    volatile Uint16 DI3timer INIT0;
CCEXTERN    volatile Uint16 DI4timer INIT0;
CCEXTERN    volatile Uint16 DI5timer INIT0;
CCEXTERN    volatile Uint16 DI6timer INIT0;
CCEXTERN    volatile Uint16 DI7timer INIT0;
CCEXTERN    volatile Uint16 DI8timer INIT0;
CCEXTERN    volatile Uint16 DI9timer INIT0;
CCEXTERN    volatile Uint16 DI10timer INIT0;

CCEXTERN    volatile Uint16 overLoadTimer1;
CCEXTERN    volatile Uint16 overLoadTimer2;
CCEXTERN    volatile Uint16 overLoadTimer3;

CCEXTERN    volatile Uint16 rtcReadTimer INIT0;
CCEXTERN    volatile Uint16 msDelayCount INIT0;
CCEXTERN    volatile Uint16 magicID INIT0;
CCEXTERN    volatile Uint16 msgRefreshTimer INIT0;
CCEXTERN    volatile Uint16 DisplayRefreshTimer INIT0;

CCEXTERN    volatile Uint16 softCounter INIT0;
CCEXTERN    volatile Uint16 maxUnbalanceCurrent INIT0;
CCEXTERN    float32_t ref_inc;
CCEXTERN    float32_t ref_inc1;

CCEXTERN    volatile float32_t    voutR_dc INIT0;
CCEXTERN    volatile float32_t    voutY_dc INIT0;
CCEXTERN    volatile float32_t    voutB_dc INIT0;
CCEXTERN    volatile float32_t    Vout_Ref INIT0;
CCEXTERN    volatile float32_t    newDT INIT0;
CCEXTERN    volatile float32_t    soft_VoutRef INIT0;
CCEXTERN    volatile float32_t    pll_omegaN INIT0;
CCEXTERN    volatile float32_t    pll_omegaR INIT0;
CCEXTERN    volatile float32_t    pll_omega INIT0;
CCEXTERN    volatile float32_t    omega_inv INIT0;
CCEXTERN    volatile float32_t    rms_sum_voltage_R INIT0;
CCEXTERN    volatile float32_t    rms_sum_voltage_Y INIT0;
CCEXTERN    volatile float32_t    rms_sum_voltage_B INIT0;
CCEXTERN    volatile float32_t    rms_sum_gridcur_R INIT0;
CCEXTERN    volatile float32_t    rms_sum_gridcur_Y INIT0;
CCEXTERN    volatile float32_t    rms_sum_gridcur_B INIT0;
CCEXTERN    volatile float32_t    rms_sum_invcur_R INIT0;
CCEXTERN    volatile float32_t    rms_sum_invcur_Y INIT0;
CCEXTERN    volatile float32_t    rms_sum_invcur_B INIT0;

CCEXTERN    volatile float32_t    omega INIT0;

CCEXTERN    volatile float32_t    kw_sum_R INIT0;
CCEXTERN    volatile float32_t    kw_sum_Y INIT0;
CCEXTERN    volatile float32_t    kw_sum_B INIT0;
CCEXTERN    volatile float32_t pll_d INIT0;
CCEXTERN    volatile float32_t pll_q INIT0;
CCEXTERN    volatile float32_t pll_zero INIT0;
CCEXTERN    volatile float32_t pll_de INIT0;
CCEXTERN    volatile float32_t pll_qe INIT0;
CCEXTERN    volatile N16 toggle INIT0;

CCEXTERN    float32_t *waveptr;

CCEXTERN    volatile float32_t    prev_vdcin INIT0;
CCEXTERN    char    dispString[21];
CCEXTERN    char UnitSerial[20] INITARRAY0;
CCEXTERN    N16 ReferenceChangedFlag INIT0;
CCEXTERN    N16 specialCommnad INIT0;
CCEXTERN    N16 eventCount INIT0;
CCEXTERN    N16 errorRx INIT0;

CCEXTERN    N16 AlarmFlag1_old INIT0;
CCEXTERN    N16 AlarmFlag2_old INIT0;
CCEXTERN    N16 AlarmFlag3_old INIT0;
CCEXTERN    N16 AlarmFlag4_old INIT0;
CCEXTERN    N16 AlarmFlag5_old INIT0;
CCEXTERN    N16 samplesPerCycle INIT0;


CCEXTERN    N16 rtcUpdate  INIT0;

CCEXTERN  volatile N16    nPoints INIT0;
CCEXTERN    volatile N16    snap_start INIT0;
CCEXTERN    volatile N16    snap_end INIT0;
CCEXTERN    volatile N16 snap_ptr INIT0;
CCEXTERN    volatile N16 tmp_canRxCounter INIT0;


CCEXTERN    Uint16 Sector INIT0;  // Sector is treated as Q0 - independently with global Q
CCEXTERN    Uint16 zcdptr INIT0;
CCEXTERN    Uint16 FirstTime INIT0;

CCEXTERN    Uint16 mainLoopCount_prev INIT0;
CCEXTERN    Uint16 period INIT0;
CCEXTERN    Uint16 mainsFailTimer INIT0;
CCEXTERN    Uint16 CANOKtimer INIT0;
CCEXTERN    Uint16 CANReset INIT0;

CCEXTERN    volatile float32_t DMinVal INIT0;
CCEXTERN    volatile float32_t QMaxVal INIT0;
CCEXTERN    volatile float32_t qRef INIT0;

CCEXTERN    Uint16 Error INIT0;
CCEXTERN    SYSMODE SystemFlag;
CCEXTERN    LMTMODE LimitFlag;
CCEXTERN    OKMODE  SysOkFlag;
CCEXTERN    LMTMODE1 LimitFlagInv;
CCEXTERN    PARSTAT  UPSFlag;
CCEXTERN    PARSTAT  PairStatusFlag;
CCEXTERN    COMMMODE CommStatus;

CCEXTERN    sincos theta_inv1s;


CCEXTERN    float32_t bypassFailLevel   INIT0;
CCEXTERN    volatile float32_t    sin_val_old INIT0;

CCEXTERN    float32_t *waveptr;
CCEXTERN    volatile float32_t    bypSin INIT0;
CCEXTERN    volatile float32_t    bypCos INIT0;

CCEXTERN    volatile float32_t stat_current_zero INIT0;
CCEXTERN    volatile float32_t stat_current_d INIT0;
CCEXTERN    volatile float32_t stat_current_q INIT0;
CCEXTERN    volatile float32_t soft_VqRef INIT0;
CCEXTERN    volatile float32_t deSoftRef INIT0;

CCEXTERN    volatile float32_t    volRef_Iq   INIT0;
CCEXTERN    volatile float32_t    volRef_Id   INIT0;
CCEXTERN    volatile float32_t    defaultOmega   INIT0;

CCEXTERN    volatile float32_t    vbeta_bypass   INIT0;

CCEXTERN    pllBlock  pllBypass;

CCEXTERN PIreg pi_vbat;
CCEXTERN PIreg pi_ibat;

CCEXTERN    MIN_MAX chkFrequency;
CCEXTERN    MIN_MAX chkVr;
CCEXTERN    MIN_MAX chkVy;
CCEXTERN    MIN_MAX chkVb;
CCEXTERN    MIN_MAX chkIr1;
CCEXTERN    MIN_MAX chkIy1;
CCEXTERN    MIN_MAX chkIb1;

CCEXTERN    MIN_MAX chkIr2;
CCEXTERN    MIN_MAX chkIy2;
CCEXTERN    MIN_MAX chkIb2;

CCEXTERN    MIN_MAX chkIr3;
CCEXTERN    MIN_MAX chkIy3;
CCEXTERN    MIN_MAX chkIb3;

CCEXTERN    MIN_MAX chkVbypr;
CCEXTERN    MIN_MAX chkVbypy;
CCEXTERN    MIN_MAX chkVbypb;

CCEXTERN    MIN_MAX chkIbat;
CCEXTERN    MIN_MAX chkVbat;
CCEXTERN    MIN_MAX chkVbatpre;
CCEXTERN    MIN_MAX chkdcLink;


CCEXTERN    MCAN_RxBufElement rxMsg[1];
CCEXTERN    MCAN_RxBufElement rxMsg1;
CCEXTERN    MCAN_TxBufElement txMsg[1];
CCEXTERN    MCAN_RxFIFOStatus RxFS;

#endif //__CCVARIABLES__
