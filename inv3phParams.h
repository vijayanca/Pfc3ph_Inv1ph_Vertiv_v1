/*
 * evChargerParams.h
 *
 *  Created on: 13-Aug-2023
 *      Author: vijay
 */

//COMPANYNAME:Techser Power Solutions Pvt.Ltd.
//UNITNAME:3PH UPS
//LOCATION:BANGALORE
//INPUTV:415V AC, 3PH, 50Hz
//OUTPUTV:415V AC, 3PH, 50Hz
//OUTPUTCUR:140A
//RATEDKVA:100KVA

#ifndef INV3PHPARAMS_H_
#define INV3PHPARAMS_H_

#define    VoutR                       ArrayParamsF[0]  //,V,1
#define    VoutY                       ArrayParamsF[1]  //,V,1
#define    VoutB                       ArrayParamsF[2]  //,V,1
#define    IoutR                       ArrayParamsF[3]  //,A,1
#define    IoutY                       ArrayParamsF[4]  //,A,1
#define    IoutB                       ArrayParamsF[5]  //,A,1
#define    VbypR                       ArrayParamsF[6]  //,V,1
#define    VbypY                       ArrayParamsF[7]  //,V,1
#define    VbypB                       ArrayParamsF[8]  //,V,1
#define    VinvR                       ArrayParamsF[9]  //,V,1
#define    VinvY                       ArrayParamsF[10]  //,V,1
#define    VinvB                       ArrayParamsF[11]  //,V,1
#define    IinvR                       ArrayParamsF[12]  //,A,1
#define    IinvY                       ArrayParamsF[13]  //,A,1
#define    IinvB                       ArrayParamsF[14]  //,A,1
#define    DcLinkVoltage               ArrayParamsF[15]  //,V,1
#define    Vbat                        ArrayParamsF[16]  //,V,1
#define    Ibat                        ArrayParamsF[17]  //,A,1
#define    Temp1                       ArrayParamsF[18]  //,V,1
#define    Temp2                       ArrayParamsF[19]  //,A,1

#define    kVA_Rph                     ArrayParamsF[20]  //,kVA,1
#define    kVA_Yph                     ArrayParamsF[21]  //,kVA,1
#define    kVA_Bph                     ArrayParamsF[22]  //,kVA,1
#define    kW_Rph                      ArrayParamsF[23]  //,kW,1
#define    kW_Yph                      ArrayParamsF[24]  //,kW,1
#define    kW_Bph                      ArrayParamsF[25]  //,kW,1
#define    powerfactorR                ArrayParamsF[26]  //,,1
#define    powerfactorY                ArrayParamsF[27]  //,,1
#define    powerfactorB                ArrayParamsF[28]  //,,1
#define    kVA_Total                   ArrayParamsF[29]  //,kVA,1
#define    kW_Total                    ArrayParamsF[30]  //,kW,1
#define    DC_kW                       ArrayParamsF[31]  //,kVA,1
#define    Efficiency                  ArrayParamsF[32]  //,%,1
#define    pfMeasured                  ArrayParamsF[33]  //,,1
#define    GridFrequency               ArrayParamsF[34]  //,Hz,1
#define    InvFrequency                ArrayParamsF[35]  //,Hz,1
#define    IoutMax                     ArrayParamsF[36]  //, ,1
#define    IbypR                       ArrayParamsF[37]  //,A,1
#define    IbypY                       ArrayParamsF[38]  //,A,1
#define    IbypB                       ArrayParamsF[39]  //,A,1
#define    Inv_vol_de_ref              ArrayParamsF[40]  //, ,1
#define    Inv_vol_de_fback            ArrayParamsF[41]  //, ,1
#define    Inv_vol_de_pidout           ArrayParamsF[42]  //, ,1
#define    Inv_vol_qe_ref              ArrayParamsF[43]  //, ,1
#define    Inv_vol_qe_fback            ArrayParamsF[44]  //, ,1
#define    Inv_vol_qe_pidout           ArrayParamsF[45]  //, ,1

#define    vol_de                      ArrayParamsF[46]  //, ,1
#define    vol_qe                      ArrayParamsF[47]  //, ,1
#define    refd_actual                 ArrayParamsF[48]  //, ,1
#define    refq_actual                 ArrayParamsF[49]  //, ,1

#define    InvCurHL_Lvl1a              ArrayParamsF[54]  //,A,1
#define    InvCurHL_Lvl2a              ArrayParamsF[55]  //,A,1
#define    InvCurHL_Lvl3a              ArrayParamsF[56]  //,A,1
#define    r_Angle_old                 ArrayParamsF[57]  //, ,1
#define    vacPeakLimit                ArrayParamsF[58]  //, ,1

#define    VoutRY                      ArrayParamsF[60]  //,V,1
#define    VoutYB                      ArrayParamsF[61]  //,V,1
#define    VoutBR                      ArrayParamsF[62]  //,V,1

#define    current_de_ref              ArrayParamsF[63]  //,,1
#define    current_qe_ref              ArrayParamsF[64]  //,,1
#define    current_de_fback            ArrayParamsF[65]  //,,1
#define    current_qe_fback            ArrayParamsF[66]  //,,1
#define    current_de_pidout           ArrayParamsF[67]  //,,1
#define    current_qe_pidout           ArrayParamsF[68]  //,,1

#define    remoteAngle                 ArrayParamsF[70]  //, ,1
#define    remoteOmega                 ArrayParamsF[71]  //, ,1
#define    remoteVref                  ArrayParamsF[72]  //, ,1
#define    remoteIref                  ArrayParamsF[73]  //, ,1
#define    newCurReference             ArrayParamsF[74]  //, ,1

#define    Cur_De                      ArrayParamsF[75]  //, ,1
#define    Cur_Qe                      ArrayParamsF[76]  //, ,1
#define    dcOV_Inst_Level             ArrayParamsF[77]  //, ,1

#define    VbypRY                      ArrayParamsF[80]  //,V,1
#define    VbypYB                      ArrayParamsF[81]  //,V,1
#define    VbypBR                      ArrayParamsF[82]  //,V,1
#define    IdRef                       ArrayParamsF[83]  //, ,1
#define    slave_Id                    ArrayParamsF[84]  //, ,1
#define    curN_de_fback               ArrayParamsF[65]  //,,1
#define    curN_qe_fback               ArrayParamsF[66]  //,,1
#define    curN_de_pidout              ArrayParamsF[67]  //,,1
#define    curN_qe_pidout              ArrayParamsF[68]  //,,1

//-------------------------------------------------------------------

#define    RatedVoltage                ArrayParamsF[100]  //,V,1
#define    RatedCurrent                ArrayParamsF[101]  //,A,1
#define    Cur_deMin                   ArrayParamsF[102]  //, ,1
#define    Cur_deMax                   ArrayParamsF[103]  //, ,1
#define    Cur_qeMax                   ArrayParamsF[104]  //, ,1
#define    AngleOffset                 ArrayParamsF[105]  //, ,1

#define    InvVoltageHL                ArrayParamsF[106]  //,V,1
#define    InvVoltageLL                ArrayParamsF[107]  //,V,1
#define    BypVoltageHL                ArrayParamsF[108]  //,V,1
#define    BypVoltageLL                ArrayParamsF[109]  //,V,1
#define    BypFrequencyLL              ArrayParamsF[110]  //,Hz,1
#define    BypFrequencyHL              ArrayParamsF[111]  //,Hz,1
#define    InvVoltHL_delay             ArrayParamsF[112]  //,S,1
#define    InvVoltLL_delay             ArrayParamsF[113]  //,S,1
#define    BypVoltHL_delay             ArrayParamsF[114]  //,S,1
#define    BypVoltLL_delay             ArrayParamsF[115]  //,S,1
#define    BypFreq_delay               ArrayParamsF[116]  //,S,1
#define    InvCurHL_Lvl1               ArrayParamsF[117]  //,%,1
#define    InvCurHL_Lvl2               ArrayParamsF[118]  //,%,1
#define    InvCurHL_Lvl3               ArrayParamsF[119]  //,%,1
#define    InCurHL_delay_Lvl1          ArrayParamsF[120]  //,S,1
#define    InCurHL_delay_Lvl2          ArrayParamsF[121]  //,S,1
#define    InCurHL_delay_Lvl3          ArrayParamsF[122]  //,S,1
#define    DCLinkHL                    ArrayParamsF[123]  //,V,1
#define    DCLinkLL                    ArrayParamsF[124]  //,V,1
#define    DCLinkHL_delay              ArrayParamsF[125]  //,S,1
#define    DCLinkLL_delay              ArrayParamsF[126]  //,S,1

#define    VbatHL                      ArrayParamsF[127]  //,V,1
#define    VbatLL                      ArrayParamsF[128]  //,V,1

#define    VbatHL_delay                ArrayParamsF[129]  //,S,1
#define    VbatLL_delay                ArrayParamsF[130]  //,S,1

#define    IbatHL                      ArrayParamsF[131]  //,A,1
#define    IbatHL_delay                ArrayParamsF[132]  //,S,1
#define    vdc_hysteresis              ArrayParamsF[133]  //,V,1
#define    Vbat_PreAlarm               ArrayParamsF[134]  //,V,1

#define    TrxFactor                   ArrayParamsF[135]  //, ,1
#define    Yfactor                     ArrayParamsF[136]  //, ,1
#define    Bfactor                     ArrayParamsF[137]  //, ,1
#define    RatedKVA                    ArrayParamsF[138]  //, ,1
#define    RatedKW                     ArrayParamsF[139]  //, ,1
#define    BatteryBlocks               ArrayParamsF[140]  //, ,1
#define    BlockVoltage                ArrayParamsF[141]  //, ,1
#define    LockingLevel                ArrayParamsF[142]  //, ,1
#define    FSW_Inverter                ArrayParamsF[143]  //, ,1
#define    MaxCurrentTransfer          ArrayParamsF[144]  //, ,1
#define    cur_neg_kp                  ArrayParamsF[145]  //, ,1
#define    cur_neg_ki                  ArrayParamsF[146]  //, ,1
#define    Angle30                     ArrayParamsF[147]  //, ,1
#define    cur_neg_lpf                 ArrayParamsF[148]  //, ,1
#define    VoltCtrlMode                ArrayParamsF[149]  //, ,1  
//0=dq PID, 1=PR, 2=open loop (test, fixed max modulation)
#define    pr_hMask                    ArrayParamsF[150]  //, ,1  
//b0=5th b1=7th b2=11th b3=13th

#define    SetFrequency                ArrayParamsF[151]  //, ,1
#define    DCOV_Inst_Level             ArrayParamsF[152] //,V,1
#define    cur_kp                      ArrayParamsF[153]  //, ,1
#define    cur_ki                      ArrayParamsF[154]  //, ,1
#define    inv_kp                      ArrayParamsF[155]  //, ,1
#define    inv_ki                      ArrayParamsF[156]  //, ,1
#define    LockLevel                   ArrayParamsF[157]  //, ,1
#define    PhaseShift                  ArrayParamsF[158]  //, ,1
#define    VacPeakLimit                ArrayParamsF[159]  //, ,1
#define    VoutRSF                     ArrayParamsF[160]  //, ,1
#define    VoutYSF                     ArrayParamsF[161]  //, ,1
#define    VoutBSF                     ArrayParamsF[162]  //, ,1
#define    IoutRSF                     ArrayParamsF[163]  //, ,1
#define    IoutYSF                     ArrayParamsF[164]  //, ,1
#define    IoutBSF                     ArrayParamsF[165]  //, ,1
#define    VbypRSF                     ArrayParamsF[166]  //, ,1
#define    VbypYSF                     ArrayParamsF[167]  //, ,1
#define    VbypBSF                     ArrayParamsF[168]  //, ,1
#define    VinvRSF                     ArrayParamsF[169]  //, ,1
#define    VinvYSF                     ArrayParamsF[170]  //, ,1
#define    VinvBSF                     ArrayParamsF[171]  //, ,1
#define    IinvRSF                     ArrayParamsF[172]  //, ,1
#define    IinvYSF                     ArrayParamsF[173]  //, ,1
#define    IinvBSF                     ArrayParamsF[174]  //, ,1
#define    DcLinkVoltageSF             ArrayParamsF[175]  //, ,1
#define    VbatSF                      ArrayParamsF[176]  //, ,1
#define    IbatSF                      ArrayParamsF[177]  //, ,1
#define    Temp1SF                     ArrayParamsF[178]  //, ,1
#define    Temp2SF                     ArrayParamsF[179]  //, ,1

#define    omegaShift                  ArrayParamsF[180]  //, ,1
#define    DtComp                      ArrayParamsF[181]  //, ,1
//  dead-time compensation, fraction of DBRED/TBPRD, sign = current polarity, 0 = off
#define    DtBand                      ArrayParamsF[182]  //,A,1
//  current band where the compensation fades in linearly
#define    pr_ph5                      ArrayParamsF[183]  //,deg,1
//  extra phase lead for the 5th harmonic resonator (plant phase at 250 Hz)
#define    pr_ph7                      ArrayParamsF[184]  //,deg,1
//  extra phase lead for the 7th harmonic resonator
#define    pr_ph11                     ArrayParamsF[185]  //,deg,1
//  extra phase lead for the 11th and 13th harmonic resonators
#define    pr_kh                       ArrayParamsF[186]  //, ,1
//  harmonic resonant gain
#define    pr_wc                       ArrayParamsF[187]  //,rad/s,1
//  resonator bandwidth
#define    pr_delay                    ArrayParamsF[188]  //,Ts,1
//phase lead compensation
#define    pr_hLimit                   ArrayParamsF[189]  //, ,1 
//harmonic output clamp


#define    LastMem              ArrayParamsF[199]

#define     NUMWPARAMS      90
#define     SFP_START       0
#define     PAR_START       0
#define     KVA_START       20
#define     WR_START        100
#define     SF_START        160
#define     LMT_START       106

#define     PID_START       145
#define     PID_END         156
#define     REF_START       100
#define     REF_END         105

/*
STAT1-BIT00: System Self Check
STAT1-BIT01: Inverter Disabled
STAT1-BIT02: Inverter Ramping
STAT1-BIT03: UPS on Battery
STAT1-BIT04: Load on Inverter
STAT1-BIT05: Load on Bypass
STAT1-BIT06: Manual Bypass
STAT1-BIT07: OverLoad Trip
STAT1-BIT08: VCESAT Protection
STAT1-BIT09: Inverter Failure
STAT1-BIT10: Not Synced
STAT1-BIT11: OC LVL1 ACTIVE
STAT1-BIT12: OC LVL2 ACTIVE
STAT1-BIT13: TRIP FAULT
STAT1-BIT14: LATCHED Fault
STAT1-BIT15: BITS VALID

STAT2-BIT00: Bypass Vrph Low
STAT2-BIT01: Bypass Vyph Low
STAT2-BIT02: Bypass Vbph Low
STAT2-BIT03: Bypass Vrph High
STAT2-BIT04: Bypass Vyph High
STAT2-BIT05: Bypass Vbph High
STAT2-BIT06: VBat Low
STAT2-BIT07: VBat High
STAT2-BIT08: Batt. Pre-Alarm
STAT2-BIT09: DC Link Low
STAT2-BIT10: DC Link High
STAT2-BIT11: DC Link OK
STAT2-BIT12: Bypass Inhibit
STAT2-BIT13: Current Mode
STAT2-BIT14: BYPASS OK
STAT2-BIT15: BITS VALID

STAT3-BIT00: Inv Vol_R Low
STAT3-BIT01: Inv Vol_Y Low
STAT3-BIT02: Inv Vol_B Low
STAT3-BIT03: Inv Vol_R High
STAT3-BIT04: Inv Vol_Y High
STAT3-BIT05: Inv Vol_B High
STAT3-BIT06: OverCurr.R Level1
STAT3-BIT07: OverCurr.Y Level1
STAT3-BIT08: OverCurr.B Level1
STAT3-BIT09: OverCurr.R Level2
STAT3-BIT10: OverCurr.Y Level2
STAT3-BIT11: OverCurr.B Level2
STAT3-BIT12: OverCurr.R Level3
STAT3-BIT13: OverCurr.Y Level3
STAT3-BIT14: OverCurr.B Level3
STAT3-BIT15: BITS VALID


STAT4-BIT00: INV ON/OFF SW
STAT4-BIT01: Accept/ Reset
STAT4-BIT02: MAINS FAILURE
STAT4-BIT03: Maint.Bypass
STAT4-BIT04: Output MCB Off
STAT4-BIT05: Bypass MCB Off
STAT4-BIT06: Over Temp HS
STAT4-BIT07: Emergency PB
STAT4-BIT08: Not Used
STAT4-BIT09: Not Used

STAT4-BIT10: INVV_R OK
STAT4-BIT11: INVV_Y OK
STAT4-BIT12: INVV_B OK
STAT4-BIT13: Bypass Rph OK
STAT4-BIT14: Bypass Yph OK
STAT4-BIT15: Bypass Bph OK

STAT5-BIT00: Pair Powering Up
STAT5-BIT01: Pair not Enabled
STAT5-BIT02: Pair Ramping
STAT5-BIT03: Pair On Battery
STAT5-BIT04: Pair Inv Static On
STAT5-BIT05: Pair Byp Static On
STAT5-BIT06: Pair Manual Bypass
STAT5-BIT07: Pair Waiting..
STAT5-BIT08: Pair Not Synced
STAT5-BIT09: Pair Tripped
STAT5-BIT10: Pair Inv.MCB Off
STAT5-BIT11: Pair Byp.MCB Off
STAT5-BIT12: Pair Inverter OK
STAT5-BIT13: Pair Bypass OK
STAT5-BIT14: Pair Master Mode
STAT5-BIT15: BITSVALID

STAT6-BIT00: INPUT AC FAILURE
STAT6-BIT01: DC GROUND FAULT
STAT6-BIT02: BATTERY CB OPEN
STAT6-BIT03: BYPASS FAILURE
STAT6-BIT04: OVERLOAD
STAT6-BIT05: COMMON FAULT
STAT6-BIT06: UPS ON BATTERY
STAT6-BIT07: INVERTER HEALTHY
STAT6-BIT08: CHARGER FIALURE
STAT6-BIT09: LOAD ON BYPASS

STAT7-BIT00: PFC CAN-Com OK
STAT7-BIT01: Master Comm OK
STAT7-BIT02: Slave Comm OK
STAT7-BIT03: RS485 Comm OK


STAT4-BIT15: BITSVALID

PLOTS-WAV00: Inv.Vol_R
PLOTS-WAV01: Inv.Vol_Y
PLOTS-WAV02: Inv.Vol_B
PLOTS-WAV03: Iout_R
PLOTS-WAV04: Iout_Y
PLOTS-WAV05: Iout_B
PLOTS-WAV06: Byp.Vol_R
PLOTS-WAV07: Byp.Vol_Y
PLOTS-WAV08: Byp.Vol_B
PLOTS-WAV09: Ref.Vol_R
PLOTS-WAV10: Ref.Vol_Y
PLOTS-WAV11: Ref.Vol_B
PLOTS-WAV12: Corr.OutputR
PLOTS-WAV13: Corr.OutputY
PLOTS-WAV14: Corr.OutputB
PLOTS-WAV15: vol_de_fbk
PLOTS-WAV16: vol_qe_fbk
PLOTS-WAV17: PLL_Sin
PLOTS-WAV18: inv_cos
PLOTS-WAV19: Inv_Sin

*/

#define     CardID              ArrayParamsI[0]  //, ,1
#define     swVersion           ArrayParamsI[1]   //, ,100
#define     swMinSec            ArrayParamsI[2]  //, ,1
#define     swDateHour          ArrayParamsI[3]  //, ,1
#define     swYearMonth         ArrayParamsI[4]  //, ,1
#define     DigitalInputFlag1   ArrayParamsI[5]   //, ,2
#define     CommStatusFlag      ArrayParamsI[6]   //, ,2
#define     RelayFlag           ArrayParamsI[7]  //, ,1
#define     AlarmFlag1          ArrayParamsI[8]   //, ,2
#define     AlarmFlag2          ArrayParamsI[9]   //, ,2
#define     AlarmFlag3          ArrayParamsI[10]  //, ,2
#define     AlarmFlag4          ArrayParamsI[11]  //, ,2
#define     AlarmFlag5          ArrayParamsI[12]  //, ,2
#define     milliseconds        ArrayParamsI[13]  //, ,1
#define     seconds             ArrayParamsI[14]  //, ,1
#define     minutes             ArrayParamsI[15]  //, ,1

#define     AnalogInput1        ArrayParamsI[16]  //, ,1
#define     AnalogInput2        ArrayParamsI[17]  //, ,1
#define     AnalogInput3        ArrayParamsI[18]  //, ,1
#define     AnalogInput4        ArrayParamsI[19]  //, ,1
#define     AnalogInput5        ArrayParamsI[20]  //, ,1
#define     AnalogInput6        ArrayParamsI[21]  //, ,1
#define     AnalogInput7        ArrayParamsI[22]  //, ,1
#define     AnalogInput8        ArrayParamsI[23]  //, ,1
#define     AnalogInput9        ArrayParamsI[24]  //, ,1
#define     AnalogInput10       ArrayParamsI[25]  //, ,1
#define     AnalogInput11       ArrayParamsI[26]  //, ,1
#define     AnalogInput12       ArrayParamsI[27]  //, ,1
#define     AnalogInput13       ArrayParamsI[28]  //, ,1
#define     AnalogInput14       ArrayParamsI[29]  //, ,1
#define     AnalogInput15       ArrayParamsI[30]  //, ,1
#define     AnalogInput16       ArrayParamsI[31]  //, ,1
#define     AnalogInput17       ArrayParamsI[32]  //, ,1
#define     AnalogInput18       ArrayParamsI[33]  //, ,1
#define     AnalogInput19       ArrayParamsI[34]  //, ,1
#define     AnalogInput20       ArrayParamsI[35]  //, ,1
#define     AnalogInput21       ArrayParamsI[36]  //, ,1
#define     restartTimer        ArrayParamsI[37]  //, ,1

#define     TripCount           ArrayParamsI[38]  //, ,1
#define     xfr2InvTimer        ArrayParamsI[39]  //, ,1
#define     xfr2BypTimer        ArrayParamsI[40]  //, ,1
#define     xfrCommand          ArrayParamsI[41]  //, ,1
#define     WritePending        ArrayParamsI[42]  //, ,1
#define     WritePendingInt     ArrayParamsI[43]  //, ,1
#define     pwm1Count           ArrayParamsI[44]  //, ,1
#define     MasterVoltagePresent  ArrayParamsI[45]  //, ,1
#define     MuteFlag            ArrayParamsI[46]  //, ,1
#define     FaultBits           ArrayParamsI[47]  //, ,1
#define     BuzzerOn            ArrayParamsI[48]  //, ,1
#define     currentMode         ArrayParamsI[49]  //, ,1
#define     MasterPresentTimer  ArrayParamsI[50]  //, ,1

#define     reason              ArrayParamsI[51]  //, ,1
#define     systemRunningTimer  ArrayParamsI[52]  //, ,1
#define     vceSatFault1        ArrayParamsI[53]  //, ,1
#define     vceSatFault2        ArrayParamsI[54]  //, ,1
#define     pwmEnabled          ArrayParamsI[55]  //, ,1
#define     Master1             ArrayParamsI[56]  //, ,1
#define     FSW_Inv             ArrayParamsI[57]  //, ,1
#define     AdcIsrCount         ArrayParamsI[58]  //, ,1
#define     T0IsrCount          ArrayParamsI[59]  //, ,1
#define     nextMaster          ArrayParamsI[60]  //, ,1
#define     MasterOnline        ArrayParamsI[62]  //, ,1
#define     RecordNumber        ArrayParamsI[63]  //, ,1
#define     MainsFailureCount   ArrayParamsI[64]  //, ,1
#define     poweronTimer        ArrayParamsI[65]  //,ms,1
#define     unitOnCommand       ArrayParamsI[66]  //, ,1
#define     pulseON             ArrayParamsI[69]  //, ,1

#define     tmp_epwm7Count      ArrayParamsI[70]  //, ,1
#define     CAN_InitCount       ArrayParamsI[71]  //, ,1
#define     holdTimer           ArrayParamsI[72]  //, ,1
#define     countInterval1      ArrayParamsI[73]  //, ,1
#define     nUnits              ArrayParamsI[74]  //, ,1
#define     Synced              ArrayParamsI[75]  //, ,1
#define     bypassFailed        ArrayParamsI[76]  //, ,1
#define     omegaActive         ArrayParamsI[77]  //, ,1
#define     frequencyMatch      ArrayParamsI[78]  //, ,1
#define     inverterFailed      ArrayParamsI[79]  //, ,1

#define     Lvl1_TimerR         ArrayParamsI[80]  //, ,1
#define     Lvl1_TimerY         ArrayParamsI[81]  //, ,1
#define     Lvl1_TimerB         ArrayParamsI[82]  //, ,1
#define     Lvl2_TimerR         ArrayParamsI[83]  //, ,1
#define     Lvl2_TimerY         ArrayParamsI[84]  //, ,1
#define     Lvl2_TimerB         ArrayParamsI[85]  //, ,1
#define     Lvl3_TimerR         ArrayParamsI[86]  //, ,1
#define     Lvl3_TimerY         ArrayParamsI[87]  //, ,1
#define     Lvl3_TimerB         ArrayParamsI[88]  //, ,1

#define     AutoSaveTimer       ArrayParamsI[89]  //, ,1
#define     dcOV                ArrayParamsI[90]  //, ,1

#define     Pfc_AlarmFlag1      ArrayParamsI[91]   //, ,2
#define     Pfc_AlarmFlag2      ArrayParamsI[92]   //, ,2
#define     Pfc_AlarmFlag3      ArrayParamsI[93]   //, ,2
#define     Pfc_AlarmFlag4      ArrayParamsI[94]   //, ,2
#define     EmergencyStop       ArrayParamsI[95]  //, ,1
#define     triggerCount        ArrayParamsI[96]  //, ,1

#define     eventSerialNumber   ArrayParamsI[97]  //, ,1
#define     canRxCounter        ArrayParamsI[98]  //, ,1
#define     txMsgCount          ArrayParamsI[99]  //, ,1



//--------------------------WRITTABLE------------------------------
#define     UnitOnCommand       ArrayParamsI[100]  //, ,1
#define     AcceptReset         ArrayParamsI[101]  //, ,1
#define     DI_AlarmBits        ArrayParamsI[102]  //, ,1
#define     DI_AlarmBits_PFC    ArrayParamsI[103]  //, ,1

#define     TestMode            ArrayParamsI[104]  //, ,1
#define     Master              ArrayParamsI[105]  //, ,1
#define     MaxRestartCount     ArrayParamsI[106]  //, ,1
#define     MainsOKDelay        ArrayParamsI[107]  //,S,1

#define     CaptureNow          ArrayParamsI[108]  //, ,1
#define     BiasOffsetN         ArrayParamsI[109]  //, ,1
#define     BiasOffset          ArrayParamsI[110]  //, ,1
#define     TripBits            ArrayParamsI[111]  //, ,1

#define     EcoMode             ArrayParamsI[112]  //, ,1
#define     IbatOffset          ArrayParamsI[113]  //, ,1
#define     UnitAddress         ArrayParamsI[114]  //, ,1
#define     NoFilter            ArrayParamsI[115]  //, ,1
#define     FaultResetDelay     ArrayParamsI[116]  //,S,1
#define     autoBypassFlag      ArrayParamsI[117]  //, ,1
#define     RestartDelay        ArrayParamsI[118]  //, ,1
#define     BypassAbscent       ArrayParamsI[119]  //, ,1
#define     xfrToInverterDelay  ArrayParamsI[120]  //, ,1
#define     xfrToBypassDelay    ArrayParamsI[121]  //, ,1
#define     DI_NO_Flag          ArrayParamsI[122]  //, ,1
#define     UpdateEEPROM        ArrayParamsI[123]  //, ,1
#define     BeepDuration        ArrayParamsI[124]  //, ,1
#define     nCycles             ArrayParamsI[125]  //, ,1
#define     PFSign              ArrayParamsI[126]  //, ,1
#define     RevPolarity         ArrayParamsI[127]  //, ,1
#define     DCOV_Inst_Delay     ArrayParamsI[128]  //,50uS,1
#define     UnbalanceCorrection ArrayParamsI[129]

#define     adcIsrTime          ArrayParamsI[130]  //, ,1
#define     adrIsrDuration      ArrayParamsI[131]  //, ,1
#define     slaveOnInverter     ArrayParamsI[132]  //, ,1
#define     slaveOnBypass       ArrayParamsI[133]  //, ,1
#define     slaveDisabled       ArrayParamsI[134]  //, ,1
#define     slaveInvOK          ArrayParamsI[135]  //, ,1
#define     slave0_AlarmFlag1   ArrayParamsI[136]  //, ,2
#define     slave0_AlarmFlag2   ArrayParamsI[137]  //, ,2
#define     masterAlarmFlag1    ArrayParamsI[138]  //, ,2
#define     masterAlarmFlag2    ArrayParamsI[139]  //, ,2
#define     Pair_StatusFlag     ArrayParamsI[140]  //, ,2

#define     an1Offset           ArrayParamsI[160]  //, ,1
#define     an2Offset           ArrayParamsI[161]  //, ,1
#define     an3Offset           ArrayParamsI[162]  //, ,1
#define     an4Offset           ArrayParamsI[163]  //, ,1
#define     an5Offset           ArrayParamsI[164]  //, ,1
#define     an6Offset           ArrayParamsI[165]  //, ,1
#define     an7Offset           ArrayParamsI[166]  //, ,1
#define     an8Offset           ArrayParamsI[167]  //, ,1
#define     an9Offset           ArrayParamsI[168]  //, ,1
#define     an10Offset          ArrayParamsI[169]  //, ,1
#define     an11Offset          ArrayParamsI[170]  //, ,1
#define     an12Offset          ArrayParamsI[171]  //, ,1
#define     an13Offset          ArrayParamsI[172]  //, ,1
#define     an14Offset          ArrayParamsI[173]  //, ,1
#define     an15Offset          ArrayParamsI[174]  //, ,1
#define     an16Offset          ArrayParamsI[175]  //, ,1
#define     an17Offset          ArrayParamsI[176]  //, ,1
#define     an18Offset          ArrayParamsI[177]  //, ,1
#define     an19Offset          ArrayParamsI[178]  //, ,1

#define     RTCSecond           ArrayParamsI[180]  //, ,1
#define     RTCMinute           ArrayParamsI[181]  //, ,1
#define     RTCHour             ArrayParamsI[182]  //, ,1
#define     RTCDay              ArrayParamsI[183]  //, ,1
#define     RTCDate             ArrayParamsI[184]  //, ,1
#define     RTCMonth            ArrayParamsI[185]  //, ,1
#define     RTCYear             ArrayParamsI[186]  //, ,1

#define LastMemInt              ArrayParamsI[199]    //,ms,1
//------------------Defines-------------------------------------
#define     CMD_SETDFLT         555
#define     WR_START1           100
#define     ADR_RESET           101
#define     NUMWPARAMS1         60
#define     ADR_COMMIT          123
#define     SET_DEFAULT         0x0002
#define     UPDATE_SERIAL       0x0004
#define     BASE_RTC            180
#define     ADR_UID             114
#define     ADR_MASTER          105

#define     UnitOnCommand_Bit       DOParams[100]
#define     AcceptReset_Bit         DOParams[101]
#define     TestMode_Bit            DOParams[102]
#define     Master_Bit              DOParams[103]
#define     emShutdown_Bit          DOParams[104]
#define     autoBypassFlag_Bit      DOParams[105]
#define     BypassAbscent_Bit       DOParams[106]
#define     UpdateEEPROM_Bit        DOParams[107]
#define     PFSign_Bit              DOParams[108]
#define     RevPolarity_Bit         DOParams[109]


#endif


