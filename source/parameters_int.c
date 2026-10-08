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

Uint16 crc_rxd1=0,crc_act1=0;

const PARAM_INT *paramptr_int;
const PARAM_INT defaultValuesInt[] = {
                                      {0,1,0}, /* UnitOnCommand 200 */
                                      {0,1,0}, /* AcceptReset 201 */
                                      {0,1,0}, /* TestMode 202 */
                                      {0,1,0}, /* CurrentSensingMode 203 */
                                      {0,2,0}, /* ExtRefEnable 204 */
                                      {0,1,0}, /* BoostEnable 205 */
                                      {0,1,0}, /* AutoBoostEnable 206 */
                                      {0,1000,0}, /* BiasOffset 207 */
                                      {0,1,0}, /* unitOncmd 208 */
                                      {10,1000,20}, /* CANFailDelay 209 */
                                      {20,1000,20}, /* BalancingRate 210 */
                                      {1,12,3}, /* AverageCount 211: DC average = 1/3 cycle */
                                      {0,200,0}, /* irOffset 212 */
                                      {0,3600,0}, /* Idc1Offset 213: ADC counts at 0 A */
                                      {0,3600,0}, /* Idc2Offset 214: ADC counts at 0 A */
                                      {0,32767,0}, /* MsgMask1 215 */
                                      {0,32767,0}, /* MsgMask2 216 */
                                      {0,32767,0}, /* MsgMask3 217 */
                                      {0,32767,0}, /* EventMask1 218 */
                                      {0,32767,0}, /* EventMask2 219 */
                                      {0,32767,0}, /* EventMask3 220 */
                                      {0,1,0}, /* BalancingEnable 221 */
                                      {0,1,0}, /* BalancingDirection 222 */
                                      {10,500,100}, /* MaxBalanceCount 223 */
                                      {0,3600,0}, /* IbatOffset 224 */
                                      {1,120,5}, /* SoftstartInterval 225 */
                                      {1,60,5}, /* RestartDelay 226 */
                                      {10,300,120}, /* FaultResetDelay 227 */
                                      {0,1,0}, /* UpdateEEPROM 228 */
                                      {0,3000,0}, /* dummyVdc 229 */
                                      {0,1,0}, /* OpenLoop 230 */
                                      {0,1000,0}, /* FiringAngle 231 */
                                      {0,1,0}, /* SharingEnable 232 */
                                      {0,1,0}, /* IacLimitEnable 233 */
                                      {1,32,1}, /* DeviceID 234 */
                                      {96,1152,384}, /* BaudRate 235 */
                                      {1,100,8}, /* nCycles 236 */
                                      {100,5000,100}, /* SyncCorrection 237 */
                                      {0,1,0}, /* AutoSetRefLevels 238 */
                                      {1,100,10}, /* PI_Interval 239 */
                                      {1,1000,3}, /* MaxRestartCount 240 */
                                      {1,100,5}, /* PacketDelay 241 */
                                      {0,32767,0}, /* DO_TestFlag 242 */
                                      {0,1,0}, /* ManualModeSelect 243 */
                                      {0,1,0}, /* ForcedFloat 244 */
                                      {0,1,0}, /* ForcedBoost 245 */
                                      {0,1,0}, /* ForcedBatTest 246 */
                                      {0,1,0}, /* ForcedEqualize 247 */
                                      {0,1,0}, /* DI_ModeSelect 248 */
                                      {0,1,0}, /* __notUzd006 249 */
                                      {0,1,0}, /* __notUzd007 250 */
                                      {1,1,10}, /* canFailureCount 251 */
                                      {0,1,0}, /* canRxCount 252 */
                                      {0,32767,0}, /* cfgAlarmFlagI 253 */
                                      {0,32767,0}, /* cfgTripFlagI 254 */
                                      {0,32767,32767}, /* cfgAutoResetFlagI 255 */
                                      {0,32767,0}, /* cfgAlarmFlag 256 */
                                      {0,32767,0}, /* cfgTripFlag 257 */
                                      {0,32767,0}, /* cfgNOFlag 258 */
                                      {0,4095,4095}, /* cfgAutoResetFlag 259 */
                                      {0,255,0}, /* IOM1_cfgAlarmFlag1 260 */
                                      {0,255,0}, /* IOM1_cfgTripFlag1 261 */
                                      {0,255,0}, /* IOM1_cfgNOFlag1 262 */
                                      {0,255,255}, /* IOM1_cfgAutoResetFlag1 263 */
                                      {0,255,0}, /* IOM1_cfgAlarmFlag2 264 */
                                      {0,255,0}, /* IOM1_cfgTripFlag2 265 */
                                      {0,255,0}, /* IOM1_cfgNOFlag2 266 */
                                      {0,255,255}, /* IOM1_cfgAutoResetFlag2 267 */
                                      {0,255,0}, /* IOM2_cfgAlarmFlag1 268 */
                                      {0,255,0}, /* IOM2_cfgTripFlag1 269 */
                                      {0,255,0}, /* IOM2_cfgNOFlag1 270 */
                                      {0,255,255}, /* IOM2_cfgAutoResetFlag1 271 */
                                      {0,255,0}, /* IOM2_cfgAlarmFlag2 272 */
                                      {0,255,0}, /* IOM2_cfgTripFlag2 273 */
                                      {0,255,0}, /* IOM2_cfgNOFlag2 274 */
                                      {0,255,255}, /* IOM2_cfgAutoResetFlag2 275 */
                                      {0,96,1}, /* MAIN_Signal_Relay1 276 */
                                      {0,96,2}, /* MAIN_Signal_Relay2 277 */
                                      {0,96,3}, /* MAIN_Signal_Relay3 278 */
                                      {0,96,4}, /* MAIN_Signal_Relay4 279 */
                                      {0,96,5}, /* MAIN_Signal_Relay5 280 */
                                      {0,96,6}, /* MAIN_Signal_Relay6 281 */
                                      {0,96,7}, /* MAIN_Signal_Relay7 282 */
                                      {0,96,8}, /* MAIN_Signal_Relay8 283 */
                                      {0,96,9}, /* MAIN_Signal_Relay9 284 */
                                      {0,96,10}, /* MAIN_Signal_Relay10 285 */
                                      {0,96,11}, /* MAIN_Signal_Relay11 286 */
                                      {0,96,12}, /* MAIN_Signal_Relay12 287 */
                                      {0,96,13}, /* IOM1_Signal_Relay1 288 */
                                      {0,96,14}, /* IOM1_Signal_Relay2 289 */
                                      {0,96,15}, /* IOM1_Signal_Relay3 290 */
                                      {0,96,16}, /* IOM1_Signal_Relay4 291 */
                                      {0,96,17}, /* IOM1_Signal_Relay5 292 */
                                      {0,96,18}, /* IOM1_Signal_Relay6 293 */
                                      {0,96,19}, /* IOM1_Signal_Relay7 294 */
                                      {0,96,20}, /* IOM1_Signal_Relay8 295 */
                                      {0,96,21}, /* IOM1_Signal_Relay9 296 */
                                      {0,96,22}, /* IOM1_Signal_Relay10 297 */
                                      {0,96,23}, /* IOM1_Signal_Relay11 298 */
                                      {0,96,24}, /* IOM1_Signal_Relay12 299 */
                                      {0,96,25}, /* IOM1_Signal_Relay13 300 */
                                      {0,96,26}, /* IOM1_Signal_Relay14 301 */
                                      {0,96,27}, /* IOM1_Signal_Relay15 302 */
                                      {0,96,28}, /* IOM1_Signal_Relay16 303 */
                                      {0,96,29}, /* IOM2_Signal_Relay1 304 */
                                      {0,96,30}, /* IOM2_Signal_Relay2 305 */
                                      {0,96,31}, /* IOM2_Signal_Relay3 306 */
                                      {0,96,32}, /* IOM2_Signal_Relay4 307 */
                                      {0,96,33}, /* IOM2_Signal_Relay5 308 */
                                      {0,96,34}, /* IOM2_Signal_Relay6 309 */
                                      {0,96,35}, /* IOM2_Signal_Relay7 310 */
                                      {0,96,36}, /* IOM2_Signal_Relay8 311 */
                                      {0,96,37}, /* IOM2_Signal_Relay9 312 */
                                      {0,96,38}, /* IOM2_Signal_Relay10 313 */
                                      {0,96,39}, /* IOM2_Signal_Relay11 314 */
                                      {0,96,40}, /* IOM2_Signal_Relay12 315 */
                                      {0,96,41}, /* IOM2_Signal_Relay13 316 */
                                      {0,96,42}, /* IOM2_Signal_Relay14 317 */
                                      {0,96,43}, /* IOM2_Signal_Relay15 318 */
                                      {0,96,44}, /* IOM2_Signal_Relay16 319 */
                                      {0,900,1}, /* MAIN_Delay_Relay1 320 */
                                      {0,900,1}, /* MAIN_Delay_Relay2 321 */
                                      {0,900,1}, /* MAIN_Delay_Relay3 322 */
                                      {0,900,1}, /* MAIN_Delay_Relay4 323 */
                                      {0,900,1}, /* MAIN_Delay_Relay5 324 */
                                      {0,900,1}, /* MAIN_Delay_Relay6 325 */
                                      {0,900,1}, /* MAIN_Delay_Relay7 326 */
                                      {0,900,1}, /* MAIN_Delay_Relay8 327 */
                                      {0,900,1}, /* MAIN_Delay_Relay9 328 */
                                      {0,900,1}, /* MAIN_Delay_Relay10 329 */
                                      {0,900,1}, /* MAIN_Delay_Relay11 330 */
                                      {0,900,1}, /* MAIN_Delay_Relay12 331 */
                                      {0,900,1}, /* IOM1_Delay_Relay1 332 */
                                      {0,900,1}, /* IOM1_Delay_Relay2 333 */
                                      {0,900,1}, /* IOM1_Delay_Relay3 334 */
                                      {0,900,1}, /* IOM1_Delay_Relay4 335 */
                                      {0,900,1}, /* IOM1_Delay_Relay5 336 */
                                      {0,900,1}, /* IOM1_Delay_Relay6 337 */
                                      {0,900,1}, /* IOM1_Delay_Relay7 338 */
                                      {0,900,1}, /* IOM1_Delay_Relay8 339 */
                                      {0,900,1}, /* IOM1_Delay_Relay9 340 */
                                      {0,900,1}, /* IOM1_Delay_Relay10 341 */
                                      {0,900,1}, /* IOM1_Delay_Relay11 342 */
                                      {0,900,1}, /* IOM1_Delay_Relay12 343 */
                                      {0,900,1}, /* IOM1_Delay_Relay13 344 */
                                      {0,900,1}, /* IOM1_Delay_Relay14 345 */
                                      {0,900,1}, /* IOM1_Delay_Relay15 346 */
                                      {0,900,1}, /* IOM1_Delay_Relay16 347 */
                                      {0,900,1}, /* IOM2_Delay_Relay1 348 */
                                      {0,900,1}, /* IOM2_Delay_Relay2 349 */
                                      {0,900,1}, /* IOM2_Delay_Relay3 350 */
                                      {0,900,1}, /* IOM2_Delay_Relay4 351 */
                                      {0,900,1}, /* IOM2_Delay_Relay5 352 */
                                      {0,900,1}, /* IOM2_Delay_Relay6 353 */
                                      {0,900,1}, /* IOM2_Delay_Relay7 354 */
                                      {0,900,1}, /* IOM2_Delay_Relay8 355 */
                                      {0,900,1}, /* IOM2_Delay_Relay9 356 */
                                      {0,900,1}, /* IOM2_Delay_Relay10 357 */
                                      {0,900,1}, /* IOM2_Delay_Relay11 358 */
                                      {0,900,1}, /* IOM2_Delay_Relay12 359 */
                                      {0,900,1}, /* IOM2_Delay_Relay13 360 */
                                      {0,900,1}, /* IOM2_Delay_Relay14 361 */
                                      {0,900,1}, /* IOM2_Delay_Relay15 362 */
                                      {0,900,1}, /* IOM2_Delay_Relay16 363 */
                                      {0,32767,0}, /* CommonAlarmBits1 364 */
                                      {0,32767,0}, /* CommonAlarmBits1 365 */
                                      {0,32767,0}, /* CommonAlarmBits1 366 */
                                      {0,32767,0}, /* CommonAlarmBits1 367 */
                                      {0,32767,0}, /* CommonAlarmBits1 368 */
                                      {0,32767,0}, /* CommonAlarmBits1 369 */
                                      {0,32,4}, /* TRIP_Signal_1 370 */
                                      {0,32,5}, /* TRIP_Signal_2 371 */
                                      {0,32,6}, /* TRIP_Signal_3 372 */
                                      {0,32,8}, /* TRIP_Signal_4 373 */
                                      {0,32,18}, /* TRIP_Signal_5 374 */
                                      {0,32,20}, /* TRIP_Signal_6 375 */
                                      {0,32,21}, /* TRIP_Signal_7 376 */
                                      {0,32,22}, /* TRIP_Signal_8 377 */
                                      {0,32,23}, /* TRIP_Signal_9 378 */
                                      {0,32,24}, /* TRIP_Signal_10 379 */
                       };

#define READLEN 0x20
#define NBYTES  (0x20<<1)

#define BLOCKSIZE   30

N16 EPStartAddress1[] = { 0, 64, 128, 192, 256, 320, 384 };

N16 RestoreParamsInt(N16 start)
{
 N16 ipar = 0;
 N16 updt = 0;
 N16 val = 0;

    eeprom_read_words(EP_INTPARAM_START + EPStartAddress1[start/30], spiReadBuffer, READLEN);
    crc_act1 = CRC16(spiReadBuffer,62);
    crc_rxd1 = spiReadBuffer[READLEN-1];

    if(crc_act1 != crc_rxd1)
    {
        eeprom_read_words(EP_INTPARAM_START + EPStartAddress1[start/30], spiReadBuffer, READLEN);
        crc_act1 = CRC16(spiReadBuffer,62);
        crc_rxd1 = spiReadBuffer[READLEN-1];
    }

    if(crc_act1 != crc_rxd1)
    {
        eeprom_read_words(EP_INTPARAM_START + EPStartAddress1[start/30], spiReadBuffer, READLEN);
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
            ArrayParamsI[ipar + WR_START1 + start] = val;
        }
    DELAY_US(1000);
    return(updt);
    }

    else
    {
        eeprom_read_words(EP_INTPARAM_START2 + EPStartAddress1[start/30], spiReadBuffer, READLEN);
        crc_act1 = CRC16(spiReadBuffer,62);
        crc_rxd1 = spiReadBuffer[READLEN-1];


        if(crc_act1 != crc_rxd1)
        {
            eeprom_read_words(EP_INTPARAM_START2 + EPStartAddress1[start/30], spiReadBuffer, READLEN);
            crc_act1 = CRC16(spiReadBuffer,62);
            crc_rxd1 = spiReadBuffer[READLEN-1];
        }

        if(crc_act1 != crc_rxd1)
        {
            eeprom_read_words(EP_INTPARAM_START2 + EPStartAddress1[start/30], spiReadBuffer, READLEN);
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
                ArrayParamsI[ipar + WR_START1 + start] = val;
            }
        DELAY_US(1000);
        return(updt);
        }

        else
        {
            eeprom_read_words(EP_INTPARAM_START3 + EPStartAddress1[start/30], spiReadBuffer, READLEN);
                crc_act1 = CRC16(spiReadBuffer,62);
                crc_rxd1 = spiReadBuffer[READLEN-1];

                if(crc_act1 != crc_rxd1)
                {
                    eeprom_read_words(EP_INTPARAM_START3 + EPStartAddress1[start/30], spiReadBuffer, READLEN);
                    crc_act1 = CRC16(spiReadBuffer,62);
                    crc_rxd1 = spiReadBuffer[READLEN-1];
                }

                if(crc_act1 != crc_rxd1)
                {
                    eeprom_read_words(EP_INTPARAM_START3 + EPStartAddress1[start/30], spiReadBuffer, READLEN);
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
                        ArrayParamsI[ipar + WR_START1 + start] = val;
                    }
                DELAY_US(1000);
                return(updt);
                }
                else
                {
                    for (ipar = 0; ipar < BLOCKSIZE; ipar++)
                    {
                        val = defaultValuesInt[ipar + start].DefaultValue;
                        ArrayParamsI[ipar + WR_START1 + start] = val;
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
        spiWriteBuffer[ipar] = ArrayParamsI[WR_START1 + ipar + start];
    }
    spiWriteBuffer[BLOCKSIZE] = 0;
    crc_act1 = CRC16(spiWriteBuffer,62);
    spiWriteBuffer[BLOCKSIZE+1] = crc_act1;

    eeprom_write_words(EP_INTPARAM_START + EPStartAddress1[start/30], spiWriteBuffer, READLEN);
    DELAY_US(10000);
    eeprom_write_words(EP_INTPARAM_START2 + EPStartAddress1[start/30], spiWriteBuffer, READLEN);
    DELAY_US(10000);
    eeprom_write_words(EP_INTPARAM_START3 + EPStartAddress1[start/30], spiWriteBuffer, READLEN);
    DELAY_US(10000);


}


void RestoreSettingsInt(void) {

    int val = 0;

    rbuf[0] = 0xff;
    rbuf[1] = 0xff;

    val = eeprom_read_word(EP_IDLOC_I);

    if (val == MAGIC1) {

        spiUpdate = RestoreParamsInt(0);
        spiUpdate += RestoreParamsInt(30);
        spiUpdate += RestoreParamsInt(60);
        spiUpdate += RestoreParamsInt(90);
        spiUpdate += RestoreParamsInt(120);
        spiUpdate += RestoreParamsInt(150);

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
    StoreParamsInt(60);
    StoreParamsInt(90);
    StoreParamsInt(120);
    StoreParamsInt(150);
}


void SetFactoryDefaultsInt(void)
{
    Uint16 parAddr;
    eeprom_write_word(EP_IDLOC_I, MAGIC1);
    DELAY_US(10000);

    for (parAddr = 0; parAddr < NUMWPARAMS1; parAddr++)
        ArrayParamsI[parAddr + WR_START1] = defaultValuesInt[parAddr].DefaultValue;

        Nop();
        Nop();
        storeArrayToEEPROMInt();
}


N16 UpdateOneInt(N16 adr, int val) {


    if( ((adr==201) || (adr==11)) && val)
    {
        RESET_ALL();
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


    if ((adr >= WR_START1) && (adr < (WR_START1 + NUMWPARAMS1)))
    {
        paramptr_int = &defaultValuesInt[adr - WR_START1];

        if ((val <= paramptr_int->MaxValue) && (val >= paramptr_int->MinValue))
        {
            if(((adr>243) && (adr<248)) && val)
            {
                ArrayParamsI[244] = 0; ArrayParamsI[245] = 0;ArrayParamsI[246] = 0;ArrayParamsI[247] = 0;
                ArrayParamsI[adr] = val;
                WritePendingInt = TRUE;  AutoSaveTimer = 10;
            }
            else
            {
                if(ArrayParamsI[adr] == val) Nop();
                else { ArrayParamsI[adr] = val;           WritePendingInt = TRUE;  AutoSaveTimer = 10; }

            }
        }

        if(adr==ADDR_UID) {
            address = DeviceID;
            if(DeviceID == 1)
            {
                DataAddr = DATAADDR_1;
                EventAddr = EVENTADDR_1;
                SetAddr = SETADDR_1;
                PidAddr1 = DATAADDR_2 + 32;
                PidAddr2 = DATAADDR_2 + 34;
            }
            else
            {
                DataAddr = DATAADDR_2;
                EventAddr = EVENTADDR_2;
                SetAddr = SETADDR_2;
                PidAddr1 = DATAADDR_1 + 32;
                PidAddr2 = DATAADDR_1 + 34;
            }
        }
        if(adr==ADDR_BAUD)
        {
                InitSCIA(BaudRate);
        }
//#define    ForcedFloat             ArrayParamsI[244]  //,,1
//#define    ForcedBoost             ArrayParamsI[245]  //,,1
//#define    ForcedBatTest           ArrayParamsI[246]  //,,1
//#define    ForcedEqualize          ArrayParamsI[247]  //,,1

        return(0);
    }

    return (1);

} //function END
