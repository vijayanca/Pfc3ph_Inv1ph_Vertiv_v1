

#define CCEXTERN    extern
#define INIT0
#define INITARRAY0

#include "f28x_project.h"
#include "../ccVariables.h"
#include "inc/hw_memmap.h"
#include "driverlib.h"
#include "device.h"
#include "inc/stw_types.h"
#include "inc/stw_dataTypes.h"
#include <string.h>
#include "mcan.h"

#define DATAADDR_INV    0x200
#define EVENTADDR_INV   0x280
#define SETADDR_INV_F   0x100
#define SETADDR_INV_I   0x101

#define DATAADDR_PFC    0x300
#define EVENTADDR_PFC   0x380
#define SETADDR_PFC_F   0x102
#define SETADDR_PF_I    0x103

#define ADDR_PFC_ALARMS 0x366
//
// Defines.
//
#define NUM_OF_MSG                      (1U)
#define MCAN_STD_ID_FILTER_NUM          (1U)
#define MCAN_FIFO_0_NUM                 (0U)
#define MCAN_FIFO_0_ELEM_SIZE           (MCAN_ELEM_SIZE_64BYTES)
#define MCAN_FIFO_1_NUM                 (NUM_OF_MSG)
#define MCAN_FIFO_1_WATERMARK           (NUM_OF_MSG)
#define MCAN_FIFO_1_ELEM_SIZE           (MCAN_ELEM_SIZE_64BYTES)
#define MCAN_RX_BUFF_NUM                (0U)
#define MCAN_RX_BUFF_ELEM_SIZE          (MCAN_ELEM_SIZE_64BYTES)
#define MCAN_TX_BUFF_SIZE               (2U)    // 0 = paralleling, 1 = display
#define MCAN_TX_FQ_SIZE                 (0U)
#define MCAN_TX_BUFF_ELEM_SIZE          (MCAN_ELEM_SIZE_64BYTES)
#define MCAN_TX_EVENT_SIZE              (0U)

#define MCAN_STD_ID_FILT_START_ADDR     (0x0U)
#define MCAN_FIFO_0_START_ADDR          (MCAN_STD_ID_FILT_START_ADDR + ((MCAN_STD_ID_FILTER_NUM * MCANSS_STD_ID_FILTER_SIZE_WORDS * 4U)))
#define MCAN_FIFO_1_START_ADDR          (MCAN_FIFO_0_START_ADDR + (MCAN_getMsgObjSize(MCAN_FIFO_0_ELEM_SIZE) * 4U * MCAN_FIFO_0_NUM))
#define MCAN_RX_BUFF_START_ADDR         (MCAN_FIFO_1_START_ADDR + (MCAN_getMsgObjSize(MCAN_FIFO_1_ELEM_SIZE) * 4U * MCAN_FIFO_1_NUM))
#define MCAN_TX_BUFF_START_ADDR         (MCAN_RX_BUFF_START_ADDR + (MCAN_getMsgObjSize(MCAN_RX_BUFF_ELEM_SIZE) * 4U * MCAN_RX_BUFF_NUM))
#define MCAN_TX_EVENT_START_ADDR        (MCAN_TX_BUFF_START_ADDR + (MCAN_getMsgObjSize(MCAN_TX_BUFF_ELEM_SIZE) * 4U * (MCAN_TX_BUFF_SIZE + MCAN_TX_FQ_SIZE)))

__interrupt void MCANIntr1ISR(void);
__interrupt void MCANARxISR(void);
void DisplayBroadcast(void);
void DisplayWriteProcessing(void);
N16  DisplayUnit(void);
N16  DisplaySetId(void);
static void InitMCANModule(uint32_t base, const MCAN_InitParams *initParams,
                           const MCAN_BitTimingParams *bitTimes,
                           const MCAN_MsgRAMConfigParams *ramCfg,
                           const MCAN_StdMsgIDFilterElement *filt);

// Paralleling with the other unit runs on CAN-B
#define PAR_CAN_BASE    MCANB_DRIVER_BASE


unsigned int num = 0;

// Global Variables.
//
int32_t     error = 0;
int32_t loopCnt = 0U;
N16 startAddr1 = 0;
N16 pktNumber = 0;

void    MCAN_Transmit(unsigned int Id, unsigned int bufID)
{
        txMsg[0].id       = (uint32_t)Id << 18;
        MCAN_writeMsgRam(PAR_CAN_BASE, MCAN_MEM_TYPE_BUF, bufID,&txMsg[bufID]);
        MCAN_txBufAddReq(PAR_CAN_BASE, 0U);
}
N16 canID = 0;
void    MCANBusProcessing(void)
{
    long val1 = 0;
    long val2 = 0;
    long *nptr = 0;

   if(poweronTimer < 1000) return;

/********************************************************************************/

       if( (milliseconds - oldmsCount1)> CAN_TxInterval)
        {
           if(canTxPakcetCount1)
           {
              val1 = (long) SystemFlag.all;
              nptr = (long *) &idc_fback;
              val2 = *nptr;
           }
           else
           {
              nptr = (long *) &ibat_fback;
              val1 = *nptr;
              nptr = (long *) &vdc_ref;
              val2 = *nptr;
           }

                txMsg[0].data[0] = (val1 >> 0)  & 0xff;
                txMsg[0].data[1] = (val1 >> 8)  & 0xff;
                txMsg[0].data[2] = (val1 >> 16) & 0xff;
                txMsg[0].data[3] = (val1 >> 24) & 0xff;
                txMsg[0].data[4] = (val2 >> 0) & 0xff;
                txMsg[0].data[5] = (val2 >> 8) & 0xff;
                txMsg[0].data[6] = (val2 >> 16) & 0xff;
                txMsg[0].data[7] = (val2 >> 24) & 0xff;

                canID = DataAddr + canTxPakcetCount1;
                MCAN_Transmit(canID,0);

                canTxPakcetCount1++;
                if(canTxPakcetCount1==2)    canTxPakcetCount1 = 0;

                oldmsCount1 = milliseconds;
          }

       DisplayBroadcast();
       DisplayWriteProcessing();
      }

/*------------------------------------------------------------------------------
 Display unit on CAN-A
 Classic CAN, 11-bit ID, DLC 8, int16 words little-endian (low byte first).

 Broadcast (rectifier -> display), ID = DISP_BASE_ID + (unit - 1) * DISP_UNIT_STEP + n
   unit 1: 0x200..0x2DB, unit 2: 0x300..0x3DB
   n = 0x00..0x77 : floats, bytes 0-3 = ArrayParamsF[2n], bytes 4-7 = ArrayParamsF[2n+1]
                    (float32), ArrayParamsF[0..239]
   n = 0x78..0xDB : ints, m = n - 0x78, words 0..3 = ArrayParamsI[4m .. 4m+3]
                    (int16), ArrayParamsI[0..399]
   One frame every DISP_FRAME_MS on TX buffer 1 -> 220 frames, all values
   every 440 ms.

 Write (display -> rectifier), same format as the inverter/PFC display protocol
   float: ID = DISP_SET_F_ID + 2*(unit-1)  (unit 1: 0x100, unit 2: 0x102)
          bytes 0-1 = ArrayParamsF index, bytes 4-7 = float32 -> UpdateOne(idx)
   int  : ID = DISP_SET_I_ID + 2*(unit-1)  (unit 1: 0x101, unit 2: 0x103)
          bytes 0-1 = ArrayParamsI index, bytes 4-5 = int16   -> UpdateOneInt(idx)
   UpdateOne/UpdateOneInt range-check the index, as for Modbus. The frame is
   captured in the RX ISR and applied in the main loop, since a commit
   (int index 228 = 1) writes the EEPROM.
------------------------------------------------------------------------------*/
#define DISP_BASE_ID        0x200U
#define DISP_UNIT_STEP      0x100U
#define DISP_NFLOAT         240U                    // ArrayParamsF[0..239]
#define DISP_NINT           400U                    // ArrayParamsI[0..399]
#define DISP_FLT_FRAMES     (DISP_NFLOAT / 2U)      // 120 frames, n = 0x00..0x77
#define DISP_INT_FRAMES     (DISP_NINT / 4U)        // 100 frames, n = 0x78..0xDB
#define DISP_NFRAMES        (DISP_FLT_FRAMES + DISP_INT_FRAMES)
#define DISP_TXBUF          1U
#define DISP_FRAME_MS       2U
#define DISP_SET_F_ID       0x100U
#define DISP_SET_I_ID       0x101U

static MCAN_TxBufElement dispMsg;
static N16 dispIdx = 0;
static N16 dispLastMs = 0;

static volatile N16 dispWrPending = 0;
static N16 dispWrId = 0;
static N16 dispWrData[8];

N16 DisplayUnit(void)
{
    return (address == 2) ? 2 : 1;
}

// Float-write ID for this unit; the int-write ID is this + 1
N16 DisplaySetId(void)
{
    return DISP_SET_F_ID + ((DisplayUnit() - 1) << 1);
}

void DisplayBroadcast(void)
{
    N16 k;
    int w;
    uint32_t u;

    if((N16)(milliseconds - dispLastMs) < DISP_FRAME_MS) return;
    if(MCAN_getTxBufReqPend(MCANA_DRIVER_BASE) & (1UL << DISP_TXBUF)) return;   // previous frame still queued
    dispLastMs = milliseconds;

    if(dispIdx < DISP_FLT_FRAMES)
    {
        for(k = 0; k < 2; k++)          // two float32
        {
            u = *(uint32_t *) &ArrayParamsF[(dispIdx << 1) + k];
            dispMsg.data[(k << 2) + 0] = (u >> 0)  & 0xff;
            dispMsg.data[(k << 2) + 1] = (u >> 8)  & 0xff;
            dispMsg.data[(k << 2) + 2] = (u >> 16) & 0xff;
            dispMsg.data[(k << 2) + 3] = (u >> 24) & 0xff;
        }
    }
    else
    {
        for(k = 0; k < 4; k++)          // four int16
        {
            w = ArrayParamsI[((dispIdx - DISP_FLT_FRAMES) << 2) + k];
            dispMsg.data[(k << 1) + 0] = w & 0xff;
            dispMsg.data[(k << 1) + 1] = (w >> 8) & 0xff;
        }
    }

    dispMsg.id  = (uint32_t)(DISP_BASE_ID + (DisplayUnit() - 1) * DISP_UNIT_STEP + dispIdx) << 18;
    dispMsg.rtr = 0U; dispMsg.xtd = 0U; dispMsg.esi = 0U;
    dispMsg.dlc = 8U; dispMsg.brs = 0U; dispMsg.fdf = 0U;
    dispMsg.efc = 0U; dispMsg.mm  = 0U;

    MCAN_writeMsgRam(MCANA_DRIVER_BASE, MCAN_MEM_TYPE_BUF, DISP_TXBUF, &dispMsg);
    MCAN_txBufAddReq(MCANA_DRIVER_BASE, DISP_TXBUF);

    if(++dispIdx >= DISP_NFRAMES) dispIdx = 0;
}

// Display write: CAN-A RX, interrupt line 1 (PIE 9.8 MCANA_1)
__interrupt void MCANARxISR(void)
{
    uint32_t intrStatus;
    MCAN_RxBufElement rx;
    MCAN_RxFIFOStatus fs;
    N16 k;

    intrStatus = MCAN_getIntrStatus(MCANA_DRIVER_BASE);
    HW_WR_FIELD32(MCANA_DRIVER_BASE + MCAN_MCANSS_EOI, MCAN_MCANSS_EOI, 0x2U);
    MCAN_clearIntrStatus(MCANA_DRIVER_BASE, intrStatus);

    if(MCAN_INTR_SRC_RX_FIFO1_NEW_MSG & intrStatus)
    {
        fs.num = MCAN_RX_FIFO_NUM_1;
        MCAN_getRxFIFOStatus(MCANA_DRIVER_BASE, &fs);
        MCAN_readMsgRam(MCANA_DRIVER_BASE, MCAN_MEM_TYPE_FIFO, 0U, MCAN_RX_FIFO_NUM_1, &rx);
        MCAN_writeRxFIFOAck(MCANA_DRIVER_BASE, MCAN_RX_FIFO_NUM_1, fs.getIdx);

        // Only this unit's two write IDs pass the CAN-A filter; one write
        // is held at a time until the main loop applies it
        if(!dispWrPending)
        {
            dispWrId = (N16)(rx.id >> 18);
            for(k = 0; k < 8; k++) dispWrData[k] = rx.data[k] & 0xff;
            dispWrPending = 1;
        }
    }

    PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;
}

// Apply a display write in the main loop, same as the Modbus write path
void DisplayWriteProcessing(void)
{
    N16 idx;
    int ival;
    uint32_t u;
    float fval;

    if(!dispWrPending) return;

    idx = dispWrData[0] | (dispWrData[1] << 8);

    if(dispWrId == DisplaySetId())                  // float write
    {
        u  = (uint32_t) dispWrData[4];
        u |= (uint32_t) dispWrData[5] << 8;
        u |= (uint32_t) dispWrData[6] << 16;
        u |= (uint32_t) dispWrData[7] << 24;
        fval = *(float *) &u;
        UpdateOne(idx, fval);
    }
    else if(dispWrId == (DisplaySetId() + 1))       // int write
    {
        ival = (int) (dispWrData[4] | (dispWrData[5] << 8));
        UpdateOneInt(idx, ival);
    }

    dispWrPending = 0;
}
long val1 = 0, val2 = 0;
__interrupt void MCANIntr1ISR(void)
{
    uint32_t intrStatus;

    float *fptr;
 //   N16 n1 = 0;

    // Paralleling RX: CAN-B, interrupt line 1 (PIE 9.10 MCANB_1)
    intrStatus = MCAN_getIntrStatus(PAR_CAN_BASE);
    HW_WR_FIELD32(PAR_CAN_BASE + MCAN_MCANSS_EOI, MCAN_MCANSS_EOI, 0x2U);
    MCAN_clearIntrStatus(PAR_CAN_BASE, intrStatus);

    if((MCAN_INTR_SRC_RX_FIFO1_NEW_MSG & intrStatus) == MCAN_INTR_SRC_RX_FIFO1_NEW_MSG)
    {
        MCAN_readMsgRam(PAR_CAN_BASE, MCAN_MEM_TYPE_FIFO, 0U,
                            MCAN_RX_FIFO_NUM_1, &rxMsg1);

        rxMsg[loopCnt] = rxMsg1;
        RxFS.num = MCAN_RX_FIFO_NUM_1;
        CanID = rxMsg1.id >> 18;

        MCAN_writeRxFIFOAck(PAR_CAN_BASE, MCAN_RX_FIFO_NUM_1,
                            RxFS.getIdx);

        if(CanID == PidAddr1) //from Other Unit
        {
            val1 = rxMsg1.data[1];val1 <<= 8; val1 |= rxMsg1.data[0];
            val2 = rxMsg1.data[3];val2 <<= 8; val2 |= rxMsg1.data[2];
            val1 = val1 | (val2 << 16);
            fptr = (float*) &val1;
            ibat_fback_remote   = *fptr;

            val1 = rxMsg1.data[5];val1 <<= 8; val1 |= rxMsg1.data[4];
            val2 = rxMsg1.data[7];val2 <<= 8; val2 |= rxMsg1.data[6];
            val1 = val1 | (val2 << 16);
            fptr = (float*) &val1;
            vol_ref_remote = *fptr;
        }

        if(CanID == PidAddr2) //from Other Unit
        {
            val1 = rxMsg1.data[1];val1 <<= 8; val1 |= rxMsg1.data[0];
            val2 = rxMsg1.data[3];val2 <<= 8; val2 |= rxMsg1.data[2];
            SystemFlag_Rem.all = val1;

            val1 = rxMsg1.data[5];val1 <<= 8; val1 |= rxMsg1.data[4];
            val2 = rxMsg1.data[7];val2 <<= 8; val2 |= rxMsg1.data[6];
            val1 = val1 | (val2 << 16);
            fptr = (float*) &val1;
            cur_fback_remote = *fptr;
            canFailureCount = 0;
        }

  }

    PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;
}


void    InitMCAN(void)
 {
    int i = 0;
    volatile uint32_t mode = 0U;
    uint32_t dataBytes = 64;
    MCAN_InitParams initParams;
    MCAN_MsgRAMConfigParams    msgRAMConfigParams;
    MCAN_StdMsgIDFilterElement stdFiltelem;
    MCAN_BitTimingParams       bitTimes;

//--------------------------------------------------------
    rxMsg[loopCnt].id = 0U;
    rxMsg[loopCnt].rtr = 0U;
    rxMsg[loopCnt].xtd = 0U;
    rxMsg[loopCnt].esi = 0U;
    rxMsg[loopCnt].rxts = 0U;
    rxMsg[loopCnt].dlc = 0U;
    rxMsg[loopCnt].brs = 0U;
    rxMsg[loopCnt].fdf = 0U;
    rxMsg[loopCnt].fidx = 0U;
    rxMsg[loopCnt].anmf = 0U;
    for(i = 0; i < dataBytes; i++)
    {
        rxMsg[loopCnt].data[i]  = 0;
    }

    txMsg[loopCnt].id       = (uint32_t)0x1 << 18;//((uint32_t)4) << 18;//((uint32_t)(0x4)) << 18U; // STDID[28:18]
    txMsg[loopCnt].rtr      = 0U;   // RTR = 0 (Data frame)
    txMsg[loopCnt].xtd      = 0U;   // XTD = 0 (11-bit standard identifier)
    txMsg[loopCnt].esi      = 0U;
    txMsg[loopCnt].dlc      = 8U;   // 8 bytes
    txMsg[loopCnt].brs      = 0U;   // classic CAN: no bit-rate switching
    txMsg[loopCnt].fdf      = 0U;   // classic CAN frame (FD mode is off)
    txMsg[loopCnt].efc      = 1U;   // Store TX events
    txMsg[loopCnt].mm       = 0xAAU;

    for(i = 0; i < dataBytes; i++)
    {
        txMsg[loopCnt].data[i]  = i + 1;
    }

    memset(&initParams, 0, sizeof(initParams));
    memset(&msgRAMConfigParams, 0, sizeof(msgRAMConfigParams));
    memset(&stdFiltelem, 0, sizeof(stdFiltelem));
    memset(&bitTimes, 0, sizeof(bitTimes));

    initParams.fdMode            = 0x0U; // FD operation enabled.
    initParams.brsEnable         = 0x0U; // Bit rate switching enabled.
    initParams.emulationEnable   = 0x1U; // MCANSS_CTRL.EMUEN: keep running on debug halt
    //
    msgRAMConfigParams.flssa                = MCAN_STD_ID_FILT_START_ADDR;
    msgRAMConfigParams.lss                  = MCAN_STD_ID_FILTER_NUM;
    msgRAMConfigParams.rxFIFO1startAddr     = MCAN_FIFO_1_START_ADDR;
    msgRAMConfigParams.rxFIFO1size          = MCAN_FIFO_1_NUM;
    msgRAMConfigParams.rxFIFO1waterMark     = MCAN_FIFO_1_WATERMARK;
    msgRAMConfigParams.rxFIFO1OpMode        = 0U;
    msgRAMConfigParams.rxFIFO1ElemSize      = MCAN_FIFO_1_ELEM_SIZE;

    msgRAMConfigParams.txStartAddr          = MCAN_TX_BUFF_START_ADDR;
    msgRAMConfigParams.txBufNum             = MCAN_TX_BUFF_SIZE;
    msgRAMConfigParams.txBufMode            = 0U;
    msgRAMConfigParams.txFIFOSize           = MCAN_TX_FQ_SIZE;
    msgRAMConfigParams.txBufElemSize        = MCAN_TX_BUFF_ELEM_SIZE;

    stdFiltelem.sfid2              = 0x400U; // Standard ID Filter Mask.    // A 0 in any bit-position indicates "don't care"
    stdFiltelem.sfid1              = 0x3FFU; // Standard ID Filter
    stdFiltelem.sfec               = 0x6U; // Store into FIFO 1.
    stdFiltelem.sft                = 0x2U; // SFID1 = filter; SFID2 = mask
    // Bit rate 250 kbit/s, classic CAN.
    // MCAN clock = SYSCLK 150 MHz / 3 (SysCtl_setMCANClk DIV_3) = 50 MHz
    // 50 MHz / 10 = 5 MHz tq; bit = 1 + 15 + 4 = 20 tq -> 250 kbit/s, SP 80 %
    // (register fields are value - 1)
    bitTimes.nomRatePrescalar   = 9U;   // /10
    bitTimes.nomTimeSeg1        = 14U;  // 15 tq (prop + phase1)
    bitTimes.nomTimeSeg2        = 3U;   // 4 tq
    bitTimes.nomSynchJumpWidth  = 3U;   // 4 tq
    bitTimes.dataRatePrescalar  = 9U;   // data phase unused (FD off)
    bitTimes.dataTimeSeg1       = 14U;
    bitTimes.dataTimeSeg2       = 3U;
    bitTimes.dataSynchJumpWidth = 3U;
    //
    // Same set-up on both modules (RX interrupts on line 1, enabled in main.c)
    // CAN-B = paralleling: accept IDs 0x000..0x3FF
    InitMCANModule(MCANB_DRIVER_BASE, &initParams, &bitTimes, &msgRAMConfigParams, &stdFiltelem);

    // CAN-A = display: accept only this unit's two write IDs (float, int)
    stdFiltelem.sfid1 = DisplaySetId();
    stdFiltelem.sfid2 = 0x7FEU;     // bit 0 don't care: ID and ID+1
    InitMCANModule(MCANA_DRIVER_BASE, &initParams, &bitTimes, &msgRAMConfigParams, &stdFiltelem);
}

static void InitMCANModule(uint32_t base, const MCAN_InitParams *initParams,
                           const MCAN_BitTimingParams *bitTimes,
                           const MCAN_MsgRAMConfigParams *ramCfg,
                           const MCAN_StdMsgIDFilterElement *filt)
{
    int i;

    while(FALSE == MCAN_isMemInitDone(base))
    {
    }
    MCAN_setOpMode(base, MCAN_OPERATION_MODE_SW_INIT);
    while (MCAN_OPERATION_MODE_SW_INIT != MCAN_getOpMode(base))
    {}

    MCAN_init(base, initParams);
    MCAN_setBitTime(base, bitTimes);
    MCAN_msgRAMConfig(base, ramCfg);
    MCAN_addStdMsgIDFilter(base, 0U, filt);

    // A pending clock-stop request (CCCR.CSR) keeps the node in INIT;
    // clear it before going to normal mode
    MCAN_addClockStopRequest(base, FALSE);
    MCAN_setOpMode(base, MCAN_OPERATION_MODE_NORMAL);

    // Wait for the node to leave INIT (bounded: 11 recessive bits at 250k
    // is ~44 us, allow ~1 ms) so a dead bus cannot hang the main loop
    for(i = 0; i < 1000; i++)
    {
        if(MCAN_OPERATION_MODE_NORMAL == MCAN_getOpMode(base)) break;
        DELAY_US(1);
    }
}

//---------------END OF FILE-------------------------------
