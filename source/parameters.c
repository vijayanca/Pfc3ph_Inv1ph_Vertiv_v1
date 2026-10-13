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
                               {100,300,230},  /*  RatedVoltage 100 */
                               {1,500,100},  /*  RatedCurrent 101 */
                               {-0.3,0.0,-0.2},  /*  Cur_deMin 102 */
                               {0.01,0.5,0.2},  /*  Cur_deMax 103 */
                               {0.0,0.5,0.2},  /*  Cur_qeMax 104 */
                               {-1.0f,1.0f,0.0f},  /*  NotUsedW003 105 */
                               {100,300,280},  /*  InvVoltageHL 106 */
                               {50,250,200},  /*  InvVoltageLL 107 */
                               {100,280,280},  /*  BypVoltageHL 108 */
                               {100,300,200},  /*  BypVoltageLL 109 */
                               {45,60,47},  /*  BypFrequencyLL 110 */
                               {45,65,52},  /*  BypFrequencyHL 111 */
                               {0.01,120,0.5},  /*  InvVoltHL_delay 112 */
                               {0.01,30.0,2.0},  /*  InvVoltLL_delay 113 */
                               {0.01,10,0.5},  /*  BypVoltHL_delay 114 */
                               {0.01,10,0.5},  /*  BypVoltLL_delay 115 */
                               {0.01,10,0.5},  /*  BypFreq_delay 116 */
                               {80,200,105},  /*  InvCurHL_Lvl1 117 */
                               {80,200,150},  /*  InvCurHL_Lvl2 118 */
                               {80,300,200},  /*  InvCurHL_Lvl3 119 */
                               {0.1,7200,1800},  /*  InCurHL_delay_Lvl1 120 */
                               {0.1,600,30},  /*  InCurHL_delay_Lvl2 121 */
                               {0.1,300,1},  /*  InCurHL_delay_Lvl3 122 */
                               {100,800,720},  /*  DCLinkHighLmit 123 */
                               {20,800,500},  /*  DCLinkLowLmit 124 */
                               {0.1,300,1},  /*  DCLinkLowLmit_delay 125 */
                               {0.1,300,1},  /*  DCLinkHighLmit_delay 126 */
                               {100,500,450},  /*  VbatHighLmit 127 */
                               {50,450,240},  /*  VbatLowLmit 128 */
                               {0.1,300,1},  /*  VbatLowLmit_delay 129 */
                               {0.1,300,1},  /*  VbatHighLmit_delay 130 */
                               {1,500,300},  /*  IbatHighLmit 131 */
                               {0.1,300,1},  /*  IbatHighLmit_delay 132 */
                               {0.1,20,5},  /*  vdc_hysteresis 133 */
                               {50,450,240},  /*  VbatLowLmit 134 */

                               {0.5,2,1.4},  /*  TrxFactor 135 */
                               {0.9,1.1,1},  /*  Yfactor 136 */
                               {0.9,1.1,1},  /*  Bfactor 137 */
                               {5,1000,100},  /*  RatedKVA 138 */
                               {5,1000,100},  /*  RatedKW 139 */
                               {1,120,32},  /*  BatteryBlocks 140 */
                               {1,20,12},  /*  BlockVoltage 141 */
                               {0.001,0.1,0.005},  /*  LockingLevel 142 */
                               {6,40,12.8},  /*  Switching Freq 143 */
                               {1,500,200},  /*  maxCurrentTransfer 144 */
                               {0.01,.5,0.1},  /*  cur_neg_kp 145 */
                               {0.001,0.5,0.01},  /*  cur_neg_ki 146 */
                               {-0.5,1.047197551, 0.523598776},  /*  Angle30 147 */
                               {0.0001,0.1,0.001}, /* cur_neg_lpf  148  */

                               {0,2,0}, /* VoltCtrlMode  149  0:PI 1:PR 2:open loop (test) */
                               {0,15,3}, /* pr_hMask  150  */
                               {48,62,50},  /*  SetFrequency 151 */
                               { 100, 1100, 850},/* DCOV_Inst_Level 152 */
                               { 0.01, 4.0, 2.0},/* current_kp 153 */
                               { 0.001, 0.5, 0.01},/* current_ki 154 */
                               {0.01,2,0.1},  /*  inv_kp 155 */
                               {0.001,0.5,0.01},  /*  inv_ki 156 */
                               {0.001,0.1,0.005},  /*  LockLevel 157 */
                               {0.0,1.0,0.1},  /*  PhaseShift 158 */
                               {100,300,250},  /*  VacPeakLimit 159 */
                               {10,5000,1200},  /*  VinvRSF 160 */
                               {10,5000,1200},  /*  VinvYSF 161 */
                               {10,5000,1200},  /*  VinvBSF 162 */
                               {10,5000,1200},  /*  IinvRSF 163 */
                               {10,5000,1200},  /*  IinvYSF 164 */
                               {10,5000,1200},  /*  IinvBSF 165 */
                               {10,5000,1200},  /*  VoutRSF 166 */
                               {10,5000,1200},  /*  VoutYSF 167 */
                               {10,5000,1200},  /*  VoutBSF 168 */
                               {10,5000,1200},  /*  IoutRSF 169 */
                               {10,5000,1200},  /*  IoutYSF 170 */
                               {10,5000,1200},  /*  IoutBSF 171 */
                               {10,5000,1200},  /*  VbypRSF 172 */
                               {10,5000,1200},  /*  VbypYSF 173 */
                               {10,5000,1200},  /*  VbypBSF 174 */
                               {10,5000,1200},  /*  DcLinkVoltageSF 175 */
                               {10,5000,1200},  /*  VbatSF 176 */
                               {10,5000,1200},  /*  IbatSF 177 */
                               {10,5000,1200},  /*  Temp1SF 178 */
                               {10,5000,1200},  /*  Temp2SF 179 */

                               {0.00001,0.001,0.0001},  /*  omegaShift 180 */

                               {-1.5,1.5,0},  /*  DtComp 181 (0 = off) */
                               {0.5,50,2},  /*  DtBand 182 (A) */
                               {-180,180,0},  /*  pr_ph5 183 (deg) */
                               {-180,180,0},  /*  pr_ph7 184 (deg) */
                               {-180,180,0},  /*  pr_ph11 185 (deg, 11th and 13th) */
                               {0,0.05,0.004},  /*  pr_kh 186 */
                               {0.5,50,5},  /*  pr_wc 187 */
                               {0,5,1.5}, /* pr_delay  188  */
                               {0.01,0.3,0.1}, /* pr_hLimit  189  */

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
        spiU = RestoreParams( 0);
        spiU = RestoreParams(15);
        spiU = RestoreParams(30);
        spiU = RestoreParams(45);
        spiU = RestoreParams(60);
        spiU = RestoreParams(75);

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
	StoreParams(60);
	StoreParams(75);
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

            if ( (adr >= PID_START) && (adr <= PID_END)) updateRAMvalues(adr);
            if ( (adr >= REF_START) && (adr <= REF_END)) updateRAMvalues(adr);
            if ( (adr >= 117) && (adr <= 119)) updateRAMvalues(adr);

            if ( (adr == 107) || (adr == 109))
            {
                inverterFailLevel = InvVoltageLL/VinvRSF*0.9;
                bypassFailLevel = BypVoltageLL/VbypRSF*0.9;
            }

            if (adr == 152) dcOV_Inst_Level = DCOV_Inst_Level/DcLinkVoltageSF;

            if (adr == 143) //sw Freq
            {
                if(pwmEnabled==0) UPDATE_INV_FSW();
            }
              return (0);
        }
    }

    return (1);

} //function END



