/*
 * parameters.c
 *
 *  Created on: 18-Jul-2023
 *      Author: VCA
 */

#define CCEXTERN    extern
#define INIT0
#define INITARRAY0

#include "F28x_Project.h"
#include "../ccVariables.h"


#define rbuf    spiReadBuffer
#define wbuf    spiWriteBuffer
//                     12345678901234567890
char *defaultSerial = "VERTIV/PFC3PH/0001";

Uint16 crc_rxd1=0,crc_act1=0;

const PARAM_INT *paramptr_int;
const PARAM_INT defaultValuesInt[] = {

                                     {0,1,0},  /*  UnitOnCommand 100 */
                                     {0,1,0},  /*  AcceptReset 101 */
                                     {0,1,0},  /*  TestMode 102 */
                                     {1,10,3},  /*  MaxRestartCount 103 */
                                     {1,10,2},  /*  PreCharge_delay 104 (s) */
                                     {1,1000,100},  /*  DCOV_Inst_Delay 105 (samples) */
                                     {1,1000,100},  /*  OC_Delay 106 (samples) */
                                     {0,1023,0},  /*  TripBits 107 (64 = DI7 heatsink) */
                                     {0,1,0},  /*  CaptureNow 108 */
                                     {5,120,10},  /*  RestartDelay 109 */
                                     {1,300,60},  /*  FaultResetDelay 110 */
                                     {0,1,1},  /*  RevPolarity 111 */
                                     {0,1,1},  /*  PFSign 112 */
                                     {0,1,1},  /*  PfcCtrlMode 113 (0 PI, 1 PR) */
                                     {1,32,1},  /*  UnitAddress 114 */

                                      {0,1,1},  /*  MainConAuxUsed 115 */
                                      {0,1,0},  /*  AutoLevel 116 */
                                      {1,200,8},  /*  nCycles 117 */
                                      {0,4095,1500},  /*  IbatOffset 118 (12-bit ADC count) */
                                      {0,1,0},  /*  spare 119 */
                                      {0,1,0},  /*  spare 120 */
                                      {0,1,0},  /*  spare 121 */
                                      {0,1023,0},  /*  DI_NO_Flag 122 */
                                      {0,1,0},  /*  UpdateEEPROM 123 */
                                      {0,1,0},  /*  spare 124 */
                                      {0,1,0},  /*  spare 125 */
                                      {0,1,0},  /*  spare 126 */
                                      {0,1,0},  /*  spare 127 */
                                      {0,1,0},  /*  spare 128 */
                                      {0,1,0},  /*  spare 129 */

                                      {0,1000,0},  /*   130 */
                                      {0,1000,0},  /*   131 */
                                      {0,1000,0},  /*   132 */
                                      {0,1000,0},  /*   133 */
                                      {0,1000,0},  /*   134 */
                                      {0,1000,0},  /*   135 */
                                      {0,1000,0},  /*   136 */
                                      {0,1000,0},  /*   137 */
                                      {0,1000,0},  /*   138 */
                                      {0,1000,0},  /*   139 */
                                      {0,1000,0},  /*   140 */
                                      {0,1000,0},  /*   141 */
                                      {0,1000,0},  /*   142 */
                                      {0,1000,0},  /*   143 */
                                      {0,1000,0},  /*   144 */
                                      {0,1000,0},  /*   145 */
                                      {0,1000,0},  /*   146 */
                                      {0,1000,0},  /*   147 */
                                      {0,1000,0},  /*   148 */
                                      {0,1000,0},  /*   149 */
                                      {0,1000,0},  /*   150 */
                                      {0,1000,0},  /*   151 */
                                      {0,1000,0},  /*   152 */
                                      {0,1000,0},  /*   153 */
                                      {0,1000,0},  /*   154 */
                                      {0,1000,0},  /*   155 */
                                      {0,1000,0},  /*   156 */
                                      {0,1000,0},  /*   157 */
                                      {0,1000,0},  /*   158 */
                                      {0,1000,0},  /*   159 */

            };





#define READLEN 0x20
#define NBYTES  (0x20<<1)


#define BLOCKSIZE   30

N16 EPStartAddress1[] = { 0, 64, 128, 192, 256 };

N16 RestoreParamsInt(N16 start)
{
 N16 ipar = 0;
 N16 updt = 0;
 N16 val = 0;



    eeprom_read(EP_INTPARAM_START + EPStartAddress1[start/30], READLEN);
    crc_act1 = CRC16(spiReadBuffer,62);
    crc_rxd1 = spiReadBuffer[READLEN-1];

    if(crc_act1 != crc_rxd1)
    {
        eeprom_read(EP_INTPARAM_START + EPStartAddress1[start/30], READLEN);
        crc_act1 = CRC16(spiReadBuffer,62);
        crc_rxd1 = spiReadBuffer[READLEN-1];
    }

    if(crc_act1 != crc_rxd1)
    {
        eeprom_read(EP_INTPARAM_START + EPStartAddress1[start/30], READLEN);
        crc_act1 = CRC16(spiReadBuffer,62);
        crc_rxd1 = spiReadBuffer[READLEN-1];
    }

    if ((crc_rxd1==crc_act1) && crc_act1)
    {
        for (ipar = 0; ipar < BLOCKSIZE; ipar++)
        {
            val = spiReadBuffer[ipar];
            if ((val > defaultValuesInt[ipar + start].MaxValue) || (val < defaultValuesInt[ipar + start].MinValue))
            {
                val = defaultValuesInt[ipar + start].DefaultValue;
                updt = TRUE;
            }
            ArrayParamsI[ipar + WR_START + start] = val;
        }
    DELAY_US(1000);
    return(updt);
    }

    else
    {
        eeprom_read(EP_INTPARAM_START2 + EPStartAddress1[start/30], READLEN);
        crc_act1 = CRC16(spiReadBuffer,62);
        crc_rxd1 = spiReadBuffer[READLEN-1];


        if(crc_act1 != crc_rxd1)
        {
            eeprom_read(EP_INTPARAM_START2 + EPStartAddress1[start/30], READLEN);
            crc_act1 = CRC16(spiReadBuffer,62);
            crc_rxd1 = spiReadBuffer[READLEN-1];
        }

        if(crc_act1 != crc_rxd1)
        {
            eeprom_read(EP_INTPARAM_START2 + EPStartAddress1[start/30], READLEN);
            crc_act1 = CRC16(spiReadBuffer,62);
            crc_rxd1 = spiReadBuffer[READLEN-1];
        }

        if ((crc_rxd1==crc_act1) && crc_act1)
        {

            for (ipar = 0; ipar < BLOCKSIZE; ipar++)
            {
                val = spiReadBuffer[ipar];
                if ((val > defaultValuesInt[ipar + start].MaxValue) || (val < defaultValuesInt[ipar + start].MinValue))
                {
                    val = defaultValuesInt[ipar + start].DefaultValue;
                    updt = TRUE;
                }
                ArrayParamsI[ipar + WR_START + start] = val;
            }
        DELAY_US(1000);
        return(updt);
        }

        else
        {
            eeprom_read(EP_INTPARAM_START3 + EPStartAddress1[start/30], READLEN);
                crc_act1 = CRC16(spiReadBuffer,62);
                crc_rxd1 = spiReadBuffer[READLEN-1];

                if(crc_act1 != crc_rxd1)
                {
                    eeprom_read(EP_INTPARAM_START3 + EPStartAddress1[start/30], READLEN);
                    crc_act1 = CRC16(spiReadBuffer,62);
                    crc_rxd1 = spiReadBuffer[READLEN-1];
                }

                if(crc_act1 != crc_rxd1)
                {
                    eeprom_read(EP_INTPARAM_START3 + EPStartAddress1[start/30], READLEN);
                    crc_act1 = CRC16(spiReadBuffer,62);
                    crc_rxd1 = spiReadBuffer[READLEN-1];
                }

                if ((crc_rxd1==crc_act1) && crc_act1)
                {
                    for (ipar = 0; ipar < BLOCKSIZE; ipar++)
                    {
                        val = spiReadBuffer[ipar];
                        if ((val > defaultValuesInt[ipar + start].MaxValue) || (val < defaultValuesInt[ipar + start].MinValue))
                        {
                            val = defaultValuesInt[ipar + start].DefaultValue;
                            updt = TRUE;
                        }
                        ArrayParamsI[ipar + WR_START + start] = val;
                    }
                DELAY_US(1000);
                return(updt);
                }
                else
                {
                    for (ipar = 0; ipar < BLOCKSIZE; ipar++)
                    {
                        val = defaultValuesInt[ipar + start].DefaultValue;
                        ArrayParamsI[ipar + WR_START + start] = val;
                    }
                    return(1);
                }
        }//trying 3rd Block

    }//Trying second block
}


void    StoreParamsInt(N16 start)
{
    Uint16 ipar = 0;

    for (ipar = 0; ipar < BLOCKSIZE; ipar++)
    {
        spiWriteBuffer[ipar] = ArrayParamsI[WR_START + ipar + start];
    }
    spiWriteBuffer[BLOCKSIZE] = 0;
    crc_act1 = CRC16(spiWriteBuffer,62);
    spiWriteBuffer[BLOCKSIZE+1] = crc_act1;

    eeprom_write(EP_INTPARAM_START + EPStartAddress1[start/30], READLEN);
    DELAY_US(10000);
    eeprom_write(EP_INTPARAM_START2 + EPStartAddress1[start/30], READLEN);
    DELAY_US(10000);
    eeprom_write(EP_INTPARAM_START3 + EPStartAddress1[start/30], READLEN);
    DELAY_US(10000);


}


void RestoreSettingsInt(void) {

    int val = 0;

    rbuf[0] = 0xff;
    rbuf[1] = 0xff;

    val = eeprom_read_word(EP_IDLOC_I);

    if (val == MAGIC1) {

        spiUpdate = RestoreParamsInt(0);
        spiUpdate = RestoreParamsInt(30);

        if (spiUpdate)
        {
            storeArrayToEEPROMInt();
            spiUpdate = FALSE;
        }

    }   //MAGIC VALID
    else {

        DELAY_US(10000);
        SetFactoryDefaultsInt();
    }
}


void storeArrayToEEPROMInt(void)
{

  //  Uint16 ipar = 0;

    StoreParamsInt(0);
    StoreParamsInt(30);
}


void SetFactoryDefaultsInt(void) {
    Uint16 parAddr;
 //   Uint16 val, x;

    eeprom_write_word(EP_IDLOC_I, MAGIC1);
    DELAY_US(10000);

    for (parAddr = 0; parAddr < NUMWPARAMS1; parAddr++)
        ArrayParamsI[parAddr + WR_START] = defaultValuesInt[parAddr].DefaultValue;

        Nop();
        Nop();
        storeArrayToEEPROMInt();
}


N16 UpdateOneInt(N16 adr, int val) {


    if((adr==ADR_RESET) && val)
    {
        if(BuzzerOn && (MuteFlag==0)) MuteFlag = TRUE;
        else
        {
            RESET_FAULTS();
        }
        return 0;
    }


    if((adr==ADR_COMMIT) && val)
    {
        if(WritePendingInt)
            {
                storeArrayToEEPROMInt();
                WritePendingInt = 0;

            }

            if(WritePending)
                {
                    storeArrayToEEPROM();
                    WritePending = 0;

                }
            return 0;
    }


    if ((adr >= WR_START) && (adr < (WR_START + NUMWPARAMS1))) {
        paramptr_int = &defaultValuesInt[adr - WR_START];

        if ((val <= paramptr_int->MaxValue) && (val >= paramptr_int->MinValue))
        {
            ArrayParamsI[adr] = val;

            WritePendingInt = TRUE;
            AutoSaveTimer = 10;

            if(adr== ADR_UID) address = UnitAddress;

              return (0);
        }
    }

    return (1);

} //function END
