
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
#include "pfc3phDefines.h"
#include "pfc3phParams.h"
#include "pll3ph.h"
#include "controlMacros.h"
#include "claShared.h"
#include "BoardMacros.h"


CCEXTERN    union canPacket rxPacket;
CCEXTERN    union canPacket txPacket;

CCEXTERN   volatile N16 PFCPwmPeriod INIT0;
CCEXTERN   volatile N16 OffsetNotDone INIT0;


CCEXTERN    long offsets[20] INITARRAY0;
CCEXTERN    N16 capCount INIT0;
CCEXTERN    volatile N16 temp_AdcIsrCount INIT0;
CCEXTERN    volatile N16 temp_T0IsrCount INIT0;

CCEXTERN    int DOParams[NPARAMS] INITARRAY0;
CCEXTERN    int DIParams[NPARAMS] INITARRAY0;

CCEXTERN    volatile Uint16 last_stored_time INIT0;
CCEXTERN    volatile Uint16 recordWriteTimer INIT0;
CCEXTERN    volatile Uint16 old_RecordNumber INIT0;
CCEXTERN    Uint16 PowerOnDelay INIT0;
CCEXTERN    eventPacket Events[4];
CCEXTERN    eventPacket EventTX;

CCEXTERN    LPF32 lpf32_vry, lpf32_vyb, lpf32_vbr;
CCEXTERN    LPF32 lpf32_vr, lpf32_vy, lpf32_vb;
CCEXTERN    LPF32 lpf32_ir, lpf32_iy, lpf32_ib;
CCEXTERN    LPF32 lpf32_dclink;

CCEXTERN    volatile Uint16 DItimers[16] INITARRAY0;
CCEXTERN    N16 msgPending INIT0;
CCEXTERN    N16 autoSaveFlag  INIT0;
CCEXTERN    N16 txMsgCount_tmp  INIT0;

CCEXTERN    volatile float32_t    dcLinkReference INIT0;     // p.u.
CCEXTERN    volatile float32_t    ref_inc INIT0;             // DC-link soft-start step, p.u./ms
CCEXTERN    volatile float32_t    mainsFailLevel INIT0;      // p.u. peak, positive sequence
CCEXTERN    volatile float32_t    pll_omega INIT0;
CCEXTERN    LPF lpf_omega;
CCEXTERN    volatile N16 tripLatch INIT0;       // TRIP_xx from the control sample, read by updateStatus
CCEXTERN    volatile N16 pfcSoftStart INIT0;    // DC-link reference ramping (1 ms timer)

CCEXTERN    N16 spiUpdate INIT0;
CCEXTERN    volatile N16 spi_timeout INIT0;
CCEXTERN    volatile N16 spi_error INIT0;

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

CCEXTERN    Uint16 spiReadBuffer[32] INITARRAY0;
CCEXTERN    Uint16 spiWriteBuffer[32] INITARRAY0;

CCEXTERN    unsigned char *rxbuffer;
CCEXTERN    unsigned char *datablock;

CCEXTERN    volatile float32_t waveBuffer[100] INITARRAY0; //50

CCEXTERN    volatile Uint16 frame_complete INIT0;
CCEXTERN    volatile Uint16 live_frame INIT0;
CCEXTERN    volatile Uint16 interchar_silent_timer INIT0;
CCEXTERN    volatile Uint16 char_waiting INIT0;
CCEXTERN    volatile Uint16 chars_rxd INIT0;
CCEXTERN    volatile Uint16 address INIT0;
CCEXTERN    volatile Uint16 st_addr INIT0;
CCEXTERN    volatile Uint16 nregs INIT0;
CCEXTERN    volatile Uint16 nBytes INIT0;
CCEXTERN    volatile Uint16 mainLoopCount INIT0;

CCEXTERN    volatile Uint16 seconds_old INIT0;
CCEXTERN    volatile Uint16 rxChar INIT0;
CCEXTERN    volatile Uint16 saveCounter INIT0;

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

CCEXTERN    volatile Uint16 msDelayCount INIT0;

CCEXTERN    float32_t *waveptr;

CCEXTERN    N16 ReferenceChangedFlag INIT0;
CCEXTERN    N16 specialCommnad INIT0;
CCEXTERN    N16 eventCount INIT0;
CCEXTERN    N16 errorRx INIT0;

CCEXTERN    N16 AlarmFlag1_old INIT0;
CCEXTERN    N16 AlarmFlag2_old INIT0;
CCEXTERN    N16 AlarmFlag3_old INIT0;
CCEXTERN    N16 AlarmFlag4_old INIT0;
CCEXTERN    N16 AlarmFlag5_old INIT0;
CCEXTERN    N16 AlarmFlag6_old INIT0;
CCEXTERN    N16 AlarmFlag7_old INIT0;

CCEXTERN    volatile N16    snap_start INIT0;
CCEXTERN    volatile N16    snap_end INIT0;
CCEXTERN    volatile N16 snap_ptr INIT0;
CCEXTERN    volatile N16 tmp_canRxCounter INIT0;

CCEXTERN    Uint16 mainLoopCount_prev INIT0;

CCEXTERN    SYSMODE SystemFlag;
CCEXTERN    ACLMTMODE ACLimitFlag;
CCEXTERN    DCLMTMODE DCLimitFlag;
CCEXTERN    OKMODE  SysOkFlag;
CCEXTERN    STATMODE StatFlag;
CCEXTERN    COMMMODE CommStatus;

CCEXTERN    float32_t LowValue INIT0;
CCEXTERN    float32_t LowDelay INIT0;

CCEXTERN    pllBlock  pllGrid;

CCEXTERN    MIN_MAX chkFrequency;
CCEXTERN    MIN_MAX chkVr;
CCEXTERN    MIN_MAX chkVy;
CCEXTERN    MIN_MAX chkVb;
CCEXTERN    MIN_MAX chkIr;
CCEXTERN    MIN_MAX chkIy;
CCEXTERN    MIN_MAX chkIb;
CCEXTERN    MIN_MAX chkdcLink;


//------------------------ 1-phase inverter, static switch, battery
CCEXTERN    volatile N16 InvPwmPeriod INIT0;
CCEXTERN    volatile N16 BBPwmPeriod INIT0;
CCEXTERN    INVMODE  InvFlag;
CCEXTERN    BATTMODE BattFlag;
CCEXTERN    pllBlock pllByp;                    // 1-phase SOGI PLL on the bypass (AI18)
CCEXTERN    LPF lpf_omegaByp;
CCEXTERN    volatile float32_t angleInv INIT0;      // inverter angle, rad
CCEXTERN    volatile float32_t omegaInv INIT0;      // rad/sample
CCEXTERN    volatile float32_t bypassFailLevel INIT0;   // p.u. peak
CCEXTERN    volatile N16 invSoftStart INIT0;
CCEXTERN    volatile N16 syncCount INIT0;
CCEXTERN    volatile N16 bypSyncOK INIT0;       // bypass usable for synchronisation (1 ms)
CCEXTERN    float32_t nobuck_ui INIT0;          // no buck-boost: DC-link reduction integrator, V
CCEXTERN    LPF32 lpf32_vinv, lpf32_vout, lpf32_vbyp, lpf32_iinv, lpf32_iout;
CCEXTERN    MIN_MAX chkVinv;
CCEXTERN    MIN_MAX chkVbyp;
CCEXTERN    MIN_MAX chkIout;
CCEXTERN    MIN_MAX chkVbat;
CCEXTERN    float32_t InvOverloadA INIT0;       // RatedCurrent x InvOverload / 100

CCEXTERN    MCAN_RxBufElement rxMsg[1];
CCEXTERN    MCAN_RxBufElement rxMsg1;
CCEXTERN    MCAN_TxBufElement txMsg[1];
CCEXTERN    MCAN_RxFIFOStatus RxFS;

#endif //__CCVARIABLES__
