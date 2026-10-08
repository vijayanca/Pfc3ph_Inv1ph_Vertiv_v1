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
                               {1,600,220}, /* VdcRef 120 */
                               {1,5000,400}, /* IdcRef 121 */
                               {5,5000,200}, /* IbatRef 122 */
                               {1,5000,220}, /* FloatVRef 123 */
                               {1,600,235}, /* BoostVRef 124 */
                               {1,500,260}, /* EqualiseVRef 125 */
                               {1,440,200}, /* DGModeVRef 126 */
                               {1,2000,200}, /* FloatIbatRef 127 */
                               {5,2000,240}, /* BoostIbatRef 128 */
                               {5,2000,250}, /* EqualiseIbatRef 129 */
                               {100,650,460}, /* VacHL 130 */
                               {100,600,350}, /* VacLL 131 */
                               {10,1000,300}, /* IacHL 132 */
                               {10,500,240}, /* VdcHL 133 */
                               {10,500,180}, /* VdcLL 134 */
                               {10,500,240}, /* VbatHL 135 */
                               {10,500,180}, /* VbatLL 136 */
                               {10,500,240}, /* VloadHL 137 */
                               {10,500,180}, /* VloadLL 138 */
                               {10,1000,440}, /* IdcHL 139 */
                               {10,1000,220}, /* IbatHL 140 */
                               {100,650,450}, /* VacHL_recover 141 */
                               {100,650,360}, /* VacLL_recover 142 */
                               {10,1000,280}, /* IacHL_recover 143 */
                               {10,500,230}, /* VdcHL_recover 144 */
                               {10,500,190}, /* VdcLL_recover 145 */
                               {10,500,230}, /* VbatHL_recover 146 */
                               {10,500,190}, /* VbatLL_recover 147 */
                               {10,500,230}, /* VloadHL_recover 148 */
                               {10,500,190}, /* VloadLL_recover 149 */
                               {10,1000,300}, /* IdcHL_recover 150 */
                               {10,1000,300}, /* IbatHL_recover 151 */
                               {0.01,120,0.5}, /* VacHL_delay 152 */
                               {0.01,120,0.5}, /* VacLL_delay 153 */
                               {0.01,120,0.5}, /* IacHL_delay 154 */
                               {0.01,120,0.5}, /* VdcHL_delay 155 */
                               {0.01,120,0.5}, /* VdcLL_delay 156 */
                               {0.01,120,0.5}, /* VbatHL_delay 157 */
                               {0.01,120,0.5}, /* VbatLL_delay 158 */
                               {0.01,120,0.5}, /* VloadHL_delay 159 */
                               {0.01,120,0.5}, /* VloadLL_delay 160 */
                               {0.01,120,0.5}, /* IdcHL_delay 161 */
                               {0.01,120,0.5}, /* IbatHL_delay 162 */
                               {0.1,5000,1235}, /* VinpRSF 163 */
                               {0.1,5000,1235}, /* VinpYSF 164 */
                               {0.1,5000,1235}, /* VinpBSF 165 */
                               {0.1,5000,110}, /* IinpRSF 166 */
                               {0.1,5000,110}, /* IinpYSF 167 */
                               {0.1,5000,110}, /* IinpBSF 168 */
                               {0.1,5000,726}, /* VdcSF 169 */
                               {0.1,5000,498}, /* Idc1SF 170 */
                               {0.1,5000,498}, /* Idc2SF 171 */
                               {0.1,5000,723}, /* VbatSF 172 */
                               {0.1,5000,1500}, /* IbatSF 173 */
                               {0.1,5000,721}, /* VloadSF 174 */
                               {0.1,5000,200}, /* Temp1SF 175 */
                               {0.1,5000,200}, /* Temp2SF 176 */
                               {0.1,5000,200}, /* Temp3SF 177 (was extTemp1SF, unused) */
                               {0.1,5000,1500}, /* extTemp2SF 178 */
                               {-120.0,120.0,-30.0}, /* SyncAdjustAngle 179 */
                               {-3.0,3.0,0.5}, /* Bridge2AngleAdjust 180 */
                               {-0.1,0.1,0.002}, /* VrefAdjust 181 */
                               {1,500,50}, /* MinLoadCurrent 182 */
                               {1,100,5}, /* MinUnbalanceCurrent 183 */
                               {0.01,0.8,0.2}, /* LpfT_Idc 184 */
                               {0.1,5,0.5}, /* vdc_kp 185 */
                               {0.001,0.8,0.01}, /* vdc_ki 186 */
                               {0.1,5,0.5}, /* idc_kp 187 */
                               {0.001,0.8,0.01}, /* idc_ki 188 */
                               {0.1,5,0.5}, /* ibat_kp 189 */
                               {0.001,0.8,0.01}, /* ibat_ki 190 */
                               {50,60,60}, /* GridFrequency 191 */
                               {0,99,80}, /* MinVdcRef 192 */
                               {0,99,80}, /* MinIbatRef 193 */
                               {45,65,57}, /* FrequencyLL 194 */
                               {45,65,62}, /* FrequencyHL 195 */
                               {0.1,10,1}, /* FrequencyLL_delay 196 */
                               {0.1,5,1}, /* FrequencyHL_delay 197 */
                               {10,5000,250}, /* BatteryAH 198 */
                               {10,200,20}, /* ChargingLevel 199 */
                               {10,100,25}, /* BoostChangeoverLevel 200 */
                               {1,100,2}, /* IbatHysteresis 201 */
                               {10,1440,60}, /* BoostPeriod 202 */
                               {10,2880,48}, /* EqualizerTime 203 */
                               {10,500,100}, /* NumberOfCells 204 */
                               {0.5,12,2.25}, /* CellFloatLevel 205 */
                               {0.5,12,2.35}, /* CellBoostLevel 206 */
                               {0.5,12,2.65}, /* CellEqualizerLevel 207 */
                               {0.5,12,2}, /* CellNoChargeLevel 208 */
                               {0,1,0}, /* Batt_TestMode 209 */
                               {5,120,30}, /* Batt_Duration 210 */
                               {10,500,230}, /* Batt_SetLevel 211 */
                               {10,500,250}, /* Batt_FailLevel 212 */
                               {0,1,0}, /* Batt_TempComp 213 */
                               {10,60,25}, /* Batt_Rated_Temp 214 */
                               {1,2000,24}, /* Batt_Drop_perDegree 215 */
                               {0,10000,0}, /* TotalRunningTime 216 */
                               {0,10000,0}, /* BattRunningTime 217 */
                               {0,100,20}, /* MinVoltage 218 */
                               {0,20,2}, /* MinCurrent 219 */
                               {10,600,200}, /* Rly1_On_Level 220 */
                               {10,600,220}, /* Rly2_On_Level 221 */
                               {10,600,240}, /* Rly3_On_Level 222 */
                               {10,600,260}, /* Rly4_On_Level 223 */
                               {10,600,190}, /* Rly1_Off_Level 224 */
                               {10,600,210}, /* Rly2_Off_Level 225 */
                               {10,600,230}, /* Rly3_Off_Level 226 */
                               {10,600,250}, /* Rly4_Off_Level 227 */
                               {0.01,0.99,0.5}, /* Lpf_T_Iref 228 */
                               {0.01,0.5,0.2}, /* Lpf_T_vdc 229 */
                               {0.01,0.5,0.2}, /* Lpf_T_Idc 230 */
                               {0,1,0}, /* not_used_w004 231 */
                               {0,1,0}, /* not_used_w005 232 */
                               {0,1,0}, /* not_used_w006 233 */
                               {0,1,0}, /* not_used_w007 234 */
                               {0,1,0}, /* not_used_w008 235 */
                               {0,1,0}, /* not_used_w009 236 */
                               {0,1,0}, /* not_used_w010 237 */
                               {0,1,0}, /* not_used_w011 238 */
                               {0,1,0}, /* not_used_w012 239 */
                };


#define READLEN 0x20
#define NBYTES  0x40

#define BLOCKSIZE   15

N16 EPStartAddress[] = { 0, 64, 128, 192, 256, 320, 384, 448 };

float val = 0;
long l_number;

N16 RestoreParams(N16 start)
{
 N16 ipar = 0;
 N16 updt = 0;
 float *fptr;
//eeprom_write_words
     eeprom_read_words(EP_PARAM_START + EPStartAddress[start/BLOCKSIZE], spiReadBuffer, READLEN);
    crc_act = CRC16(spiReadBuffer,62);
    crc_rxd = spiReadBuffer[READLEN-1];

    if(crc_act != crc_rxd)
    {
        eeprom_read_words(EP_PARAM_START + EPStartAddress[start/BLOCKSIZE], spiReadBuffer, READLEN);
        crc_act = CRC16(spiReadBuffer,62);
        crc_rxd = spiReadBuffer[READLEN-1];
    }

    if(crc_act != crc_rxd)
    {
        eeprom_read_words(EP_PARAM_START + EPStartAddress[start/BLOCKSIZE], spiReadBuffer, READLEN);
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
        eeprom_read_words(EP_PARAM_START2 + EPStartAddress[start/BLOCKSIZE], spiReadBuffer, READLEN);
        crc_act = CRC16(spiReadBuffer,62);
        crc_rxd = spiReadBuffer[READLEN-1];


        if(crc_act != crc_rxd)
        {
            eeprom_read_words(EP_PARAM_START2 + EPStartAddress[start/BLOCKSIZE], spiReadBuffer, READLEN);
            crc_act = CRC16(spiReadBuffer,62);
            crc_rxd = spiReadBuffer[READLEN-1];
        }

        if(crc_act != crc_rxd)
        {
            eeprom_read_words(EP_PARAM_START2 + EPStartAddress[start/BLOCKSIZE], spiReadBuffer, READLEN);
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
            eeprom_read_words(EP_PARAM_START3 + EPStartAddress[start/BLOCKSIZE], spiReadBuffer, READLEN);
                crc_act = CRC16(spiReadBuffer,62);
                crc_rxd = spiReadBuffer[READLEN-1];

                if(crc_act != crc_rxd)
                {
                    eeprom_read_words(EP_PARAM_START3 + EPStartAddress[start/BLOCKSIZE], spiReadBuffer, READLEN);
                    crc_act = CRC16(spiReadBuffer,62);
                    crc_rxd = spiReadBuffer[READLEN-1];
                }

                if(crc_act != crc_rxd)
                {
                    eeprom_read_words(EP_PARAM_START3 + EPStartAddress[start/BLOCKSIZE], spiReadBuffer, READLEN);
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

    eeprom_write_words(EP_PARAM_START + EPStartAddress[start/BLOCKSIZE], spiWriteBuffer, READLEN);
    DELAY_US(10000);
    eeprom_write_words(EP_PARAM_START2 + EPStartAddress[start/BLOCKSIZE], spiWriteBuffer, READLEN);
    DELAY_US(10000);
    eeprom_write_words(EP_PARAM_START3 + EPStartAddress[start/BLOCKSIZE], spiWriteBuffer, READLEN);
    DELAY_US(10000);


}

int spiU = 0;

void RestoreSettings(void)
{
    int val = 0;


    rbuf[0] = 0xff;
    rbuf[1] = 0xff;
    val = eeprom_read_word(EP_IDLOC_F);

    if (val == MAGIC)
    {
        spiU = RestoreParams( 0);
        spiU += RestoreParams(15);
        spiU += RestoreParams(30);
        spiU += RestoreParams(45);
        spiU += RestoreParams(60);
        spiU += RestoreParams(75);
        spiU += RestoreParams(90);
        spiU += RestoreParams(105);

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
	StoreParams(90);
	StoreParams(105);
}

void SetFactoryDefaults(void) {
    Uint16 parAddr;
   // Uint16 x;

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
N16 UpdateOne(N16 adr1, float32_t val)
{
    N16 adr = adr1;     // float index from the caller (SCIA, SCIB or CAN)

    if ((adr >= WR_START) && (adr < (WR_START + NUMWPARAMS))) {
        paramptr = &defaultValues[adr - WR_START];

        if ((val <= paramptr->MaxValue) && (val >= paramptr->MinValue))
        {
            if(ArrayParamsF[adr] == val) Nop();
            else
                {
                    ArrayParamsF[adr] = val;
                    WritePending = TRUE;
                    AutoSaveTimer = 10;
                }

            if ( (adr >= PID_START) && (adr <= PID_END)) updateRAMvalues(adr);
            if ( (adr >= REF_START) && (adr <= REF_END)) updateRAMvalues(adr);
            if (adr == 228) updateRAMvalues(adr);
            if (adr == ADR_GFREQ)
            {
                InitPLL(&pllGrid);
                freqNotSet = TRUE;
            }

              return (0);
        }
    }

    return (1);

} //function END



