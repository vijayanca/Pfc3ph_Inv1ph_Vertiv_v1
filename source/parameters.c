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

Uint16 crc_rxd=0,crc_act=0;

const PARAM *paramptr;

const PARAM defaultValues[] = {
                               {50,900,720},  /*  DcLinkReference 100 */
                               {1,10,3},  /*  PowerLimit 101 */
                               {350,500,480},  /*  GridVoltageHL 102 */
                               {50,440,300},  /*  GridVoltageLL 103 */
                               {100,900,800},  /*  DCLinkHL 104 */
                               {10,800,350},  /*  DCLinkLL 105 */
                               {1,500,100},  /*  IinpHL 106 */
                               {2,1000,200},  /*  OC_Level 107 */
                               {100,1000,850},  /*  DCOV_Inst_Level 108 */
                               {0.01,120,0.5},  /*  GridVoltageHL_delay 109 */
                               {0.01,120,0.5},  /*  GridVoltageLL_delay 110 */
                               {0.01,120,0.5},  /*  DCLinkHL_delay 111 */
                               {0.01,120,0.5},  /*  DCLinkLL_delay 112 */
                               {0.01,120,0.5},  /*  IinpHL_delay 113 */
                               {40,60,47},  /*  FrequencyLowLimit 114 */

                               {45,65,53},  /*  FrequencyHighLimit 115 */
                               {0.01,120,1},  /*  FrequencyLowDelay 116 */
                               {0.01,120,1},  /*  FrequencyHighDelay 117 */
                               {10,800,250},  /*  PreChargeLevel 118 */
                               {1,20,5},  /*  vdc_hysteresis 119 */
                               {10,5000,1200},  /*  VinpRSF 120 */
                               {10,5000,1200},  /*  VinpYSF 121 */
                               {10,5000,1200},  /*  VinpBSF 122 */
                               {10,5000,1200},  /*  IinpRSF 123 */
                               {10,5000,1200},  /*  IinpYSF 124 */
                               {10,5000,1200},  /*  IinpBSF 125 */
                               {10,5000,1200},  /*  DcLinkVoltageSF 126 */
                               {10,5000,1200},  /*  VbatSF 127 */
                               {10,5000,1200},  /*  IbatSF 128 */
                               {10,5000,1200},  /*  Temp1SF 129 */

                               {10,5000,1200},  /*  Temp2SF 130 */
                               {0.0,0.5,0.2},  /*  CrossXL 131 */
                               {0.01,2,0.5},  /*  dclink_kp 132 */
                               {0.001,0.5,0.01},  /*  dclink_ki 133 */
                               {0.01,5,1},  /*  current_kp 134 */
                               {0.001,0.1,0.01},  /*  current_ki 135 */
                               {0.01,1,0.3},  /*  current_lpfT 136 */
                               {0.1,1,0.65},  /*  InputLPF 137 */
                               {0.1,0.5,0.4},  /*  pid_de_Umax 138 */
                               {0.05,0.5,0.25},  /*  pfc_pid_qmax 139 */
                               {-0.4,0,-0.02},  /*  pid_DCLink_Umin 140 */
                               {0,0.05,0.001},  /*  QRef 141 */
                               {6,24,10},  /*  FswPFC 142 (kHz) */
                               {0.6,10,1.0},  /*  PR_Kp 143 */
                               {10,4000,400},  /*  PR_Kr 144 */

                               {0,4000,0},  /*  PR_Kh 145 (< 1 = off) */
                               {-1.5,1.5,0},  /*  DtComp 146 (0 = off) */
                               {0.5,50,2},  /*  DtBand 147 (A) */
                               {1,1000,100},  /*  RatedKW 148 */
                               {50,440,250},  /*  GridFailLevel 149 */
                               {0,1,0},  /*  spare 150 */
                               {0,1,0},  /*  spare 151 */
                               {0,1,0},  /*  spare 152 */
                               {0,1,0},  /*  spare 153 */
                               {0,1,0},  /*  spare 154 */
                               {0,1,0},  /*  spare 155 */
                               {0,1,0},  /*  spare 156 */
                               {0,1,0},  /*  spare 157 */
                               {0,1,0},  /*  spare 158 */
                               {0,1,0},  /*  spare 159 */
    };


#define READLEN 0x20
#define NBYTES  0x40

#define BLOCKSIZE   15

N16 EPStartAddress[] = { 0, 64, 128, 192, 256, 320, 384 };

float val = 0;
long l_number;

N16 RestoreParams(N16 start)
{
 N16 ipar = 0;
 N16 updt = 0;
 float *fptr;

    eeprom_read(EP_PARAM_START + EPStartAddress[start/BLOCKSIZE], READLEN);
    crc_act = CRC16(spiReadBuffer,62);
    crc_rxd = spiReadBuffer[READLEN-1];

    if(crc_act != crc_rxd)
    {
        eeprom_read(EP_PARAM_START + EPStartAddress[start/BLOCKSIZE], READLEN);
        crc_act = CRC16(spiReadBuffer,62);
        crc_rxd = spiReadBuffer[READLEN-1];
    }

    if(crc_act != crc_rxd)
    {
        eeprom_read(EP_PARAM_START + EPStartAddress[start/BLOCKSIZE], READLEN);
        crc_act = CRC16(spiReadBuffer,62);
        crc_rxd = spiReadBuffer[READLEN-1];
    }

    if ((crc_rxd==crc_act) && crc_act)
    {
        for (ipar = 0; ipar < BLOCKSIZE; ipar++)
        {
            l_number = spiReadBuffer[ipar+ipar]; l_number <<= 16;
            l_number |= spiReadBuffer[ipar+ipar+1];
            fptr = (float *) &l_number;
            val = *fptr;

            if ((val > defaultValues[ipar + start].MaxValue) || (val < defaultValues[ipar + start].MinValue))
            {
                val = defaultValues[ipar + start].DefaultValue;
                updt = TRUE;
            }
            ArrayParamsF[ipar + WR_START + start] = val;
         }
    DELAY_US(1000);
    return(updt);
    }

    else
    {
        eeprom_read(EP_PARAM_START2 + EPStartAddress[start/BLOCKSIZE], READLEN);
        crc_act = CRC16(spiReadBuffer,62);
        crc_rxd = spiReadBuffer[READLEN-1];


        if(crc_act != crc_rxd)
        {
            eeprom_read(EP_PARAM_START2 + EPStartAddress[start/BLOCKSIZE], READLEN);
            crc_act = CRC16(spiReadBuffer,62);
            crc_rxd = spiReadBuffer[READLEN-1];
        }

        if(crc_act != crc_rxd)
        {
            eeprom_read(EP_PARAM_START2 + EPStartAddress[start/BLOCKSIZE], READLEN);
            crc_act = CRC16(spiReadBuffer,62);
            crc_rxd = spiReadBuffer[READLEN-1];
        }

        if ((crc_rxd==crc_act) && crc_act)
        {

            for (ipar = 0; ipar < BLOCKSIZE; ipar++)
            {
                l_number = spiReadBuffer[ipar+ipar]; l_number <<= 16;
                l_number |= spiReadBuffer[ipar+ipar+1];
                fptr = (float *) &l_number;
                val = *fptr;

                if ((val > defaultValues[ipar + start].MaxValue) || (val < defaultValues[ipar + start].MinValue))
                {
                    val = defaultValues[ipar + start].DefaultValue;
                    updt = TRUE;
                }
                ArrayParamsF[ipar + WR_START + start] = val;
            }
        DELAY_US(1000);
        return(updt);
        }

        else
        {
            eeprom_read(EP_PARAM_START3 + EPStartAddress[start/BLOCKSIZE], READLEN);
                crc_act = CRC16(spiReadBuffer,62);
                crc_rxd = spiReadBuffer[READLEN-1];

                if(crc_act != crc_rxd)
                {
                    eeprom_read(EP_PARAM_START3 + EPStartAddress[start/BLOCKSIZE], READLEN);
                    crc_act = CRC16(spiReadBuffer,62);
                    crc_rxd = spiReadBuffer[READLEN-1];
                }

                if(crc_act != crc_rxd)
                {
                    eeprom_read(EP_PARAM_START3 + EPStartAddress[start/BLOCKSIZE], READLEN);
                    crc_act = CRC16(spiReadBuffer,62);
                    crc_rxd = spiReadBuffer[READLEN-1];
                }

                if ((crc_rxd==crc_act) && crc_act)
                {
                    for (ipar = 0; ipar < BLOCKSIZE; ipar++)
                    {
                        l_number = spiReadBuffer[ipar+ipar]; l_number <<= 16;
                        l_number |= spiReadBuffer[ipar+ipar+1];
                        fptr = (float *) &l_number;
                        val = *fptr;

                        if ((val > defaultValues[ipar + start].MaxValue) || (val < defaultValues[ipar + start].MinValue))
                        {
                            val = defaultValues[ipar + start].DefaultValue;
                            updt = TRUE;
                        }
                        ArrayParamsF[ipar + WR_START + start] = val;
                    }
                DELAY_US(1000);
                return(updt);
                }
                else
                {
                    for (ipar = 0; ipar < BLOCKSIZE; ipar++)
                    {
                        val = defaultValues[ipar + start].DefaultValue;
                        ArrayParamsF[ipar + WR_START + start] = val;
                    }
                    return(1);
                }
        }//trying 3rd Block

    }//Trying second block
}


void    StoreParams(N16 start)
{
    Uint16 ipar = 0;
    long *intptr;
    long val = 0;

    for (ipar = 0; ipar < BLOCKSIZE; ipar++)
    {
        intptr = (long *) &ArrayParamsF[WR_START + start + ipar];
        val = *intptr;

        spiWriteBuffer[2*ipar] = val >> 16;
        spiWriteBuffer[2*ipar+1] = val & 0xffff;
    }
    spiWriteBuffer[BLOCKSIZE+BLOCKSIZE] = 0;
    crc_act = CRC16(spiWriteBuffer,62);
    spiWriteBuffer[BLOCKSIZE+BLOCKSIZE+1] = crc_act;

    eeprom_write(EP_PARAM_START + EPStartAddress[start/BLOCKSIZE], READLEN);
    DELAY_US(10000);
    eeprom_write(EP_PARAM_START2 + EPStartAddress[start/BLOCKSIZE], READLEN);
    DELAY_US(10000);
    eeprom_write(EP_PARAM_START3 + EPStartAddress[start/BLOCKSIZE], READLEN);
    DELAY_US(10000);


}


void RestoreSettings(void)
{
    int val = 0;
    int spiU = 0;

    rbuf[0] = 0xff;
    rbuf[1] = 0xff;
    val = eeprom_read_word(EP_IDLOC_F);

    if (val == MAGIC)
    {
        spiU  = RestoreParams( 0);
        spiU |= RestoreParams(15);
        spiU |= RestoreParams(30);
        spiU |= RestoreParams(45);

        if (spiU)
        {
            storeArrayToEEPROM();
            spiU = FALSE;
        }
    }
    else
    {
        DELAY_US(10000);
        SetFactoryDefaults();
    }
}

void storeArrayToEEPROM(void) {

  //  Uint16 ipar = 0;

    StoreParams(0);
    StoreParams(15);
    StoreParams(30);
    StoreParams(45);
}


void SetFactoryDefaults(void) {
    Uint16 parAddr;
   // Uint16 val, x;

    eeprom_write_word(EP_IDLOC_F, MAGIC);
    DELAY_US(10000);

    for (parAddr = 0; parAddr < NUMWPARAMS; parAddr++)
        ArrayParamsF[parAddr + WR_START] = defaultValues[parAddr].DefaultValue;

        Nop();
        Nop();
        storeArrayToEEPROM();
        updateRAMvalues(0);


}


/*
 * Update parameters and store them in to NVRAM
 */
N16 UpdateOne(N16 adr, float32_t val) {


    if ((adr >= WR_START) && (adr < (WR_START + NUMWPARAMS))) {
        paramptr = &defaultValues[adr - WR_START];

        if ((val <= paramptr->MaxValue) && (val >= paramptr->MinValue))
        {
            ArrayParamsF[adr] = val;

            WritePending = TRUE; AutoSaveTimer = 10;

            updateRAMvalues(adr);       // gains, limits, references to the controllers
              return (0);
        }
    }

    return (1);

} //function END



