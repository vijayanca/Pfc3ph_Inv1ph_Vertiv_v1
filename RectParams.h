#ifndef __RECTPARAMS_H__
#define __RECTPARAMS_H__
//Edit details after the : mark
//Dont change anything up to the :


//COMPANYNAME:GETEK Electronics Pvt.Ltd.
//UNITNAME:RECT 12PULSE
//LOCATION:CHENNAI
//INPUTV:415V AC, 3PH, 60Hz
//OUTPUTV:220V DC
//OUTPUTCUR:300A
//RATEDKVA:70KVA

#define    VinpRY           ArrayParamsF[0]  //,V,1
#define    VinpYB           ArrayParamsF[1]  //,V,1
#define    VinpBR           ArrayParamsF[2]  //,V,1
#define    IinpR            ArrayParamsF[3]  //,A,1
#define    IinpY            ArrayParamsF[4]  //,A,1
#define    IinpB            ArrayParamsF[5]  //,A,1
#define    Vdc              ArrayParamsF[6]  //,V,1
#define    Idc1             ArrayParamsF[7]  //,V,1
#define    Idc2             ArrayParamsF[8]  //,V,1
#define    Vbat             ArrayParamsF[9]  //,V,1
#define    Ibat             ArrayParamsF[10]  //,A,1
#define    LoadVoltage      ArrayParamsF[11]  //,V,1
#define    BattTemp         ArrayParamsF[12]  //,degC,1
#define    Temp2            ArrayParamsF[13]  //,degC,1
#define    IdcTotal         ArrayParamsF[14] //,A,1
#define    LoadCurrent      ArrayParamsF[15] //,A,1
#define    VdcRipple        ArrayParamsF[16] //,V,1
#define    Temp3            ArrayParamsF[17]  //,degC,1

#define    kVA_Rph          ArrayParamsF[20] //,kVA,1
#define    kVA_Yph          ArrayParamsF[21] //,kVA,1
#define    kVA_Bph          ArrayParamsF[22] //,kVA,1
#define    kW_Rph           ArrayParamsF[23] //,kW,1
#define    kW_Yph           ArrayParamsF[24] //,kW,1
#define    kW_Bph           ArrayParamsF[25] //,kW,1
#define    powerfactorR     ArrayParamsF[26] //,,1
#define    powerfactorY     ArrayParamsF[27] //,,1
#define    powerfactorB     ArrayParamsF[28] //,,1
#define    kVA_Total        ArrayParamsF[29] //,kVA,1

#define    kW_Total         ArrayParamsF[30] //,kW,1
#define    DC_kW            ArrayParamsF[31] //,kW,1
#define    Efficiency       ArrayParamsF[32] //,%,1
#define    pfMeasured       ArrayParamsF[33] //,,1
#define    MainsFrequency   ArrayParamsF[34] //,Hz,1
#define    VinpR            ArrayParamsF[35]  //,V,1
#define    VinpY            ArrayParamsF[36]  //,V,1
#define    VinpB            ArrayParamsF[37]  //,V,1
#define    vol_de           ArrayParamsF[38]  //, ,1
#define    vol_qe           ArrayParamsF[39]  //, ,1

#define    vdc_ref          ArrayParamsF[40] //,,1
#define    vdc_fback        ArrayParamsF[41] //,,1
#define    vdc_pidout       ArrayParamsF[42] //,,1
#define    idc_ref          ArrayParamsF[43] //,,1
#define    idc_fback        ArrayParamsF[44] //,,1
#define    idc_pidout       ArrayParamsF[45] //,,1
#define    ibat_ref         ArrayParamsF[46] //,,1
#define    ibat_fback       ArrayParamsF[47] //,,1
#define    ibat_pidout      ArrayParamsF[48] //,,1

#define    v_pidout_remote      ArrayParamsF[49]  //,,1
#define    ibat_fback_remote    ArrayParamsF[50]  //,,1
#define    cur_fback_remote     ArrayParamsF[51]  //,,1
#define    vol_ref_remote       ArrayParamsF[52]  //,,1

#define    res_pid              ArrayParamsF[54]  //,,1
#define    soft_VdcRef          ArrayParamsF[55]  //,,1
#define    soft_IdcRef          ArrayParamsF[56]  //,,1
#define    soft_IbatRef         ArrayParamsF[57]  //,,1
#define    ExtVdcRef            ArrayParamsF[58]  //,V,1
#define    ExtIbatRef           ArrayParamsF[59]  //,A,1
#define    Idc1_2_Ref           ArrayParamsF[60]  //,,1
#define    Vdc_Ref              ArrayParamsF[61]  //,,1
#define    Idc_Ref              ArrayParamsF[62]  //,,1
#define    Ibat_Ref             ArrayParamsF[63]  //,,1
#define    eqrelTimer           ArrayParamsF[64]  //,,1
#define    OneDegree            ArrayParamsF[65]  //,,1
#define    SlaveIdcRef          ArrayParamsF[71]  //,,1

#define    AutoBoostCurLevel    ArrayParamsF[78]    //, ,1
#define    sfFactor             ArrayParamsF[79]    //, ,1
#define    TempCompV            ArrayParamsF[80]    //,V,1
#define    idc1Sum              ArrayParamsF[81]    //, ,1
#define    idc2Sum              ArrayParamsF[82]    //, ,1
#define    idc1_fbk             ArrayParamsF[83]    //,,1
#define    idc2_fbk             ArrayParamsF[84]    //,,1
#define     OmegaDefault        ArrayParamsF[85]    //, ,1
#define     PLL_Lock_Min        ArrayParamsF[86]    //, ,1
#define     PLL_Lock_Max        ArrayParamsF[87]    //, ,1
#define     AlphaAngle          ArrayParamsF[88]    //,deg,1

#define     DcLink_diff         ArrayParamsF[91]    //,V,1
#define     vdcSum              ArrayParamsF[92]    //, ,1
#define     vdcFbk              ArrayParamsF[93]    //,,1
#define     idc1Fbk             ArrayParamsF[94]    //,,1
#define     idc2Fbk             ArrayParamsF[95]    //,,1
#define     IdcFback            ArrayParamsF[96]    //,,1

#define     ibatFbk             ArrayParamsF[97]    //,,1
#define     Idc1_81             ArrayParamsF[98]    //,,1
#define     Idc2_81             ArrayParamsF[99]    //,,1
#define     vdc_inc             ArrayParamsF[100]    //,,1
#define     idc_inc             ArrayParamsF[101]    //,,1
#define     Vfloat              ArrayParamsF[102]    //,,1
#define     Vboost              ArrayParamsF[103]    //,,1
#define     Veq                 ArrayParamsF[104]    //,,1
#define     Vdg                 ArrayParamsF[105]    //,,1
#define     Vtest               ArrayParamsF[106]    //,,1
#define     Ibatref             ArrayParamsF[107]    //,,1
#define     RippleLimit         ArrayParamsF[108]    //,,1
#define     SlaveIbatRef        ArrayParamsF[109]    //,,1

//-------------------------------------------------------------------------
#define    VdcRef                  ArrayParamsF[120]  //,V,1
#define    IdcRef                  ArrayParamsF[121]  //,A,1
#define    IbatRef                 ArrayParamsF[122]  //,V,1
#define    FloatVRef               ArrayParamsF[123]  //,V,1
#define    BoostVRef               ArrayParamsF[124]  //,V,1
#define    EqualiseVRef            ArrayParamsF[125]  //,V,1
#define    DGModeVRef              ArrayParamsF[126]  //,V,1
#define    FloatIbatRef            ArrayParamsF[127]  //,A,1
#define    BoostIbatRef            ArrayParamsF[128]  //,A,1
#define    EqualiseIbatRef         ArrayParamsF[129]  //,A,1
#define    VacHL                   ArrayParamsF[130]  //,V,1
#define    VacLL                   ArrayParamsF[131]  //,V,1
#define    IacHL                   ArrayParamsF[132]  //,A,1
#define    VdcHL                   ArrayParamsF[133]  //,V,1
#define    VdcLL                   ArrayParamsF[134]  //,V,1
#define    VbatHL                  ArrayParamsF[135]  //,V,1
#define    VbatLL                  ArrayParamsF[136]  //,V,1
#define    VloadHL                 ArrayParamsF[137]  //,V,1
#define    VloadLL                 ArrayParamsF[138]  //,V,1
#define    IdcHL                   ArrayParamsF[139]  //,A,1
#define    IbatHL                  ArrayParamsF[140]  //,A,1
#define    VacHL_recover           ArrayParamsF[141]  //,V,1
#define    VacLL_recover           ArrayParamsF[142]  //,V,1
#define    IacHL_recover           ArrayParamsF[143]  //,A,1
#define    VdcHL_recover           ArrayParamsF[144]  //,V,1
#define    VdcLL_recover           ArrayParamsF[145]  //,V,1
#define    VbatHL_recover          ArrayParamsF[146]  //,V,1
#define    VbatLL_recover          ArrayParamsF[147]  //,V,1
#define    VloadHL_recover         ArrayParamsF[148]  //,V,1
#define    VloadLL_recover         ArrayParamsF[149]  //,V,1
#define    IdcHL_recover           ArrayParamsF[150]  //,A,1
#define    IbatHL_recover          ArrayParamsF[151]  //,A,1
#define    VacHL_delay             ArrayParamsF[152]  //,S,1
#define    VacLL_delay             ArrayParamsF[153]  //,S,1
#define    IacHL_delay             ArrayParamsF[154]  //,S,1
#define    VdcHL_delay             ArrayParamsF[155]  //,S,1
#define    VdcLL_delay             ArrayParamsF[156]  //,S,1
#define    VbatHL_delay            ArrayParamsF[157]  //,S,1
#define    VbatLL_delay            ArrayParamsF[158]  //,S,1
#define    VloadHL_delay           ArrayParamsF[159]  //,S,1
#define    VloadLL_delay           ArrayParamsF[160]  //,S,1
#define    IdcHL_delay             ArrayParamsF[161]  //,S,1
#define    IbatHL_delay            ArrayParamsF[162]  //,S,1

#define    VinpRSF                 ArrayParamsF[163]  //,,1
#define    VinpYSF                 ArrayParamsF[164]  //,,1
#define    VinpBSF                 ArrayParamsF[165]  //,,1
#define    IinpRSF                 ArrayParamsF[166]  //,,1
#define    IinpYSF                 ArrayParamsF[167]  //,,1
#define    IinpBSF                 ArrayParamsF[168]  //,,1
#define    VdcSF                   ArrayParamsF[169]  //,,1
#define    Idc1SF                  ArrayParamsF[170]  //,,1
#define    Idc2SF                  ArrayParamsF[171]  //,,1
#define    VbatSF                  ArrayParamsF[172]  //,,1
#define    IbatSF                  ArrayParamsF[173]  //,,1
#define    VloadSF                 ArrayParamsF[174]  //,,1
#define    Temp1SF                 ArrayParamsF[175]  //,,1
#define    Temp2SF                 ArrayParamsF[176]  //,,1
#define    Temp3SF                 ArrayParamsF[177]  //,,1

#define    SyncAdjustAngle         ArrayParamsF[179]  //,Deg,1
#define    Bridge2AngleAdjust      ArrayParamsF[180]  //,,1
#define    VrefAdjust              ArrayParamsF[181]  //,,1
#define    MinLoadCurrent          ArrayParamsF[182]  //,,1
#define    MinUnbalanceCurrent     ArrayParamsF[183]  //,,1
#define    LpfT_Idc                ArrayParamsF[184]  //,,1
#define    vdc_kp                  ArrayParamsF[185]  //,,1
#define    vdc_ki                  ArrayParamsF[186]  //,,1
#define    idc_kp                  ArrayParamsF[187]  //,,1
#define    idc_ki                  ArrayParamsF[188]  //,,1
#define    ibat_kp                 ArrayParamsF[189]  //,,1
#define    ibat_ki                 ArrayParamsF[190]  //,,1
#define    GridFrequency           ArrayParamsF[191]  //,Hz,1
#define    MinVdcRef               ArrayParamsF[192]  //,%,1
#define    MinIbatRef              ArrayParamsF[193]  //,%,1
#define    FrequencyLL             ArrayParamsF[194]  //,Hz,1
#define    FrequencyHL             ArrayParamsF[195]  //,Hz,1
#define    FrequencyLL_delay       ArrayParamsF[196]  //,S,1
#define    FrequencyHL_delay       ArrayParamsF[197]  //,S,1
#define    BatteryAH               ArrayParamsF[198]  //,,1
#define    ChargingLevel           ArrayParamsF[199]  //,%,1
#define    BoostChangeoverLevel    ArrayParamsF[200]  //,%,1
#define    IbatHysteresis          ArrayParamsF[201]  //,A,1
#define    BoostPeriod             ArrayParamsF[202]  //,Min,1
#define    EqualizerTime           ArrayParamsF[203]  //,Min,1
#define    NumberOfCells           ArrayParamsF[204]  //,,1
#define    CellFloatLevel          ArrayParamsF[205]  //,V,1
#define    CellBoostLevel          ArrayParamsF[206]  //,V,1
#define    CellEqualizerLevel      ArrayParamsF[207]  //,V,1
#define    CellNoChargeLevel       ArrayParamsF[208]  //,V,1
#define    Batt_TestMode           ArrayParamsF[209]  //,,1
#define    Batt_Duration           ArrayParamsF[210]  //,Min,1
#define    Batt_SetLevel           ArrayParamsF[211]  //,V,1
#define    Batt_FailLevel          ArrayParamsF[212]  //,V,1
#define    Batt_TempComp           ArrayParamsF[213]  //, ,1
#define    Batt_Rated_Temp         ArrayParamsF[214]  //,Deg,1
#define    Batt_Drop_perDegree     ArrayParamsF[215]  //,mV,1
#define    TotalRunningTime        ArrayParamsF[216]  //,,1
#define    BattRunningTime         ArrayParamsF[217]  //,,1
#define    MinVoltage              ArrayParamsF[218]  //,V,1
#define    MinCurrent              ArrayParamsF[219]  //,A,1
#define    Rly1_On_Level           ArrayParamsF[220]  //,V,1
#define    Rly2_On_Level           ArrayParamsF[221]  //,V,1
#define    Rly3_On_Level           ArrayParamsF[222]  //,V,1
#define    Rly4_On_Level           ArrayParamsF[223]  //,V,1
#define    Rly1_Off_Level          ArrayParamsF[224]  //,V,1
#define    Rly2_Off_Level          ArrayParamsF[225]  //,V,1
#define    Rly3_Off_Level          ArrayParamsF[226]  //,V,1
#define    Rly4_Off_Level          ArrayParamsF[227]  //,V,1

#define    Lpf_T_Iref              ArrayParamsF[228]  //,,1
#define    Lpf_T_Vdc               ArrayParamsF[229]  //,,1
#define    Lpf_T_Idc               ArrayParamsF[230]  //,,1
#define    PulseFrequency          ArrayParamsF[231]  //,,1
#define    kp_ibalance             ArrayParamsF[232]  //,,1
#define    ki_ibalance             ArrayParamsF[233]  //,,1
#define    VrippleMax              ArrayParamsF[234]  //,%,1
#define    not_used_w008           ArrayParamsF[235]  //,,1
#define    not_used_w009           ArrayParamsF[236]  //,,1
#define    not_used_w010           ArrayParamsF[237]  //,,1
#define    not_used_w011           ArrayParamsF[238]  //,,1
#define    not_used_w012           ArrayParamsF[239]  //,,1


#define     NUMWPARAMS          120
#define     SFP_START           0
#define     PAR_START           0
#define     KVA_START           20
#define     WR_START            120
#define     SF_START            163
#define     LMT_START           130
#define     PID_START           185
#define     PID_END             190
#define     W_VREF              120
#define     W_IREF              121
#define     ADR_GFREQ           191

#define     REF_START           120
#define     REF_END             129


#define     CardID                  ArrayParamsI[0]    //, ,1
#define     swVersion               ArrayParamsI[1]    //, ,100

#define     swMinSec                ArrayParamsI[2]
#define     swDateHour              ArrayParamsI[3]
#define     swYearMonth             ArrayParamsI[4]
#define     Minutes                 ArrayParamsI[5]
#define     DigitalInputFlag1       ArrayParamsI[6]   //, ,2
#define     faultFlag               ArrayParamsI[7]   //, ,2
#define     AlarmFlag1              ArrayParamsI[8]   //, ,2
#define     AlarmFlag2              ArrayParamsI[9]   //, ,2
#define     AlarmFlag3              ArrayParamsI[10]  //, ,2
#define     AlarmFlag4              ArrayParamsI[11]  //, ,2
#define     AlarmFlag5              ArrayParamsI[12]  //, ,2
#define     AlarmFlag6              ArrayParamsI[13]  //, ,2
#define     AlarmFlag7              ArrayParamsI[14]  //, ,2
#define     AlarmFlag8              ArrayParamsI[15]  //, ,2

#define     AnalogInput1            ArrayParamsI[16] //,VRY,1
#define     AnalogInput2            ArrayParamsI[17] //,VYB,1
#define     AnalogInput3            ArrayParamsI[18] //,VBR,1
#define     AnalogInput4            ArrayParamsI[19] //,IR,1
#define     AnalogInput5            ArrayParamsI[20] //,IY,1
#define     AnalogInput6            ArrayParamsI[21] //,IB,1
#define     AnalogInput7            ArrayParamsI[22] //,VDC,1
#define     AnalogInput8            ArrayParamsI[23] //,IDC,1
#define     AnalogInput9            ArrayParamsI[24] //,IDC2,1
#define     AnalogInput10           ArrayParamsI[25] //,VBAT,1
#define     AnalogInput11           ArrayParamsI[26] //,IBAT,1
#define     AnalogInput12           ArrayParamsI[27] //,VLOAD,1
#define     AnalogInput13           ArrayParamsI[28] //,VREF,1
#define     AnalogInput14           ArrayParamsI[29] //,IREF,1
#define     AnalogInput15           ArrayParamsI[30] //,Vref,1
#define     AnalogInput16           ArrayParamsI[31] //,Temp1,1
#define     AnalogInput17           ArrayParamsI[32] //,TEMP2,1
#define     AnalogInput18           ArrayParamsI[33] //,Temp3,1
#define     AnalogInput19           ArrayParamsI[34] //,DI2,1
#define     AnalogInput20           ArrayParamsI[35] //,DI_3,1
#define     AnalogInput21           ArrayParamsI[36] //,DI4,1
#define     LmtMode                 ArrayParamsI[37]
#define     RelayFlag               ArrayParamsI[38]  //, ,2
#define     RecordNumber            ArrayParamsI[39]

#define     AlphaCount              ArrayParamsI[40]
#define     periodR                 ArrayParamsI[41]
#define     periodY                 ArrayParamsI[42]
#define     periodB                 ArrayParamsI[43]
#define     Bridge2LagCount         ArrayParamsI[44]
#define     Bridge2LeadCount        ArrayParamsI[45]
#define     Bridge2LagCount_1       ArrayParamsI[46]
#define     Bridge2LeadCount_1      ArrayParamsI[47]
#define     drive_enable            ArrayParamsI[48]
#define     opMode                  ArrayParamsI[49]

#define     recordNumber            ArrayParamsI[50]
#define     mcuSeconds              ArrayParamsI[51]
#define     pulseWidth              ArrayParamsI[52]
#define     reason                  ArrayParamsI[53]
#define     zcdRcount               ArrayParamsI[54]
#define     zcdYcount               ArrayParamsI[55]
#define     zcdBcount               ArrayParamsI[56]
#define     zcdRCount               ArrayParamsI[57]
#define     SystemFlag_Remote       ArrayParamsI[58] //, ,2

#define     IOM1_DI_Flag1           ArrayParamsI[60]
#define     IOM1_DI_Flag2           ArrayParamsI[61]
#define     IOM1_DO_Flag1           ArrayParamsI[62]
#define     IOM1_DO_Flag2           ArrayParamsI[63]
#define     IOM2_DI_Flag1           ArrayParamsI[64]
#define     IOM2_DI_Flag2           ArrayParamsI[65]
#define     IOM2_DO_Flag1           ArrayParamsI[66]
#define     IOM2_DO_Flag2           ArrayParamsI[67]
#define     setValue                ArrayParamsI[68]
#define     TripCount               ArrayParamsI[69]

#define     restartTimer            ArrayParamsI[70]
#define     poweronTimer            ArrayParamsI[71]
#define     systemRunningTimer      ArrayParamsI[72]
#define     count_30                ArrayParamsI[73]
#define     count_60                ArrayParamsI[74]
#define     WritePending            ArrayParamsI[75]
#define     WritePendingInt         ArrayParamsI[76]
#define     autoSaveFlag            ArrayParamsI[77]
#define     milliseconds            ArrayParamsI[78]
#define     seconds                 ArrayParamsI[79]
#define     GoodPhases              ArrayParamsI[80]
#define     eventSerialNumber       ArrayParamsI[81]
#define     SamplesPerCycle         ArrayParamsI[82]  //,,1
#define     EqualizeTimer           ArrayParamsI[83]
#define     pwmEnabled              ArrayParamsI[84]
#define     BoostTimer              ArrayParamsI[85]
#define     doBalancing             ArrayParamsI[86]
#define     count30_SP1             ArrayParamsI[87]
#define     count30_SP2             ArrayParamsI[88]
#define     Batt_TestTimer          ArrayParamsI[89]

#define     errorCount              ArrayParamsI[90]
#define     AdcIsrCount             ArrayParamsI[91]
#define     T0IsrCount              ArrayParamsI[92]
#define     T1IsrCount              ArrayParamsI[93]
#define     PidCount                ArrayParamsI[94]
#define     iom1_relayflag1         ArrayParamsI[95]  //, ,2
#define     iom1_relayflag2         ArrayParamsI[96]  //, ,2
#define     iom2_relayflag1         ArrayParamsI[97]  //, ,2
#define     iom2_relayflag2         ArrayParamsI[98]  //, ,2
#define     cyclePoints             ArrayParamsI[99]

#define     DataAddr                ArrayParamsI[100]
#define     EventAddr               ArrayParamsI[101]
#define     SetAddr                 ArrayParamsI[102]
#define     PidAddr1                ArrayParamsI[103]
#define     PidAddr2                ArrayParamsI[104]
#define     adcSamples              ArrayParamsI[105]
#define     RlyLevelFlag            ArrayParamsI[106]

#define     IOM1_DI_Flag1_1         ArrayParamsI[107]  //, ,2
#define     IOM1_DI_Flag2_1         ArrayParamsI[108]  //, ,2
#define     IOM2_DI_Flag1_1         ArrayParamsI[109]  //, ,2
#define     IOM2_DI_Flag2_1         ArrayParamsI[110]  //, ,2
#define     TripBitsFlag            ArrayParamsI[111]
#define     canFailureCount         ArrayParamsI[112]  //,,1
#define     canRxCount              ArrayParamsI[113]  //,,1
#define     runModeSelect           ArrayParamsI[114]  //,,1
#define     cycle3count             ArrayParamsI[115]  //,,1
#define     zcdCount1               ArrayParamsI[116]  //,,1
#define     zcdCount2               ArrayParamsI[117]  //,,1
#define     CAN_TxInterval          ArrayParamsI[118]  //,,1
#define     MasterRunning           ArrayParamsI[119]  //,,1

#define    vrOffset                ArrayParamsI[120]  //,,1
#define    vyOffset                ArrayParamsI[121]  //,,1
#define    vbOffset                ArrayParamsI[122]  //,,1
#define    irOffset                ArrayParamsI[123]  //,,1
#define    iyOffset                ArrayParamsI[124]  //,,1
#define    ibOffset                ArrayParamsI[125]  //,,1

#define     FullCycle              ArrayParamsI[126]    //, ,1
#define     MinPoints              ArrayParamsI[127]    //, ,1
#define     MinPeriod              ArrayParamsI[128]    //, ,1
#define     samplingCount          ArrayParamsI[129]    //, ,1
#define    alphaCountR             ArrayParamsI[130]  //,,1
#define    rph30_1                 ArrayParamsI[131]  //,,1
#define    rph30_2                 ArrayParamsI[132]  //,,1
#define    rph30_3                 ArrayParamsI[133]  //,,1
#define    rph30_4                 ArrayParamsI[134]  //,,1
#define    rph30_5                 ArrayParamsI[135]  //,,1
#define    rph30_6                 ArrayParamsI[136]  //,,1

#define    alphaCountY             ArrayParamsI[137]  //,,1
#define    yph30_1                 ArrayParamsI[138]  //,,1
#define    yph30_2                 ArrayParamsI[139]  //,,1
#define    yph30_3                 ArrayParamsI[140]  //,,1
#define    yph30_4                 ArrayParamsI[141]  //,,1
#define    yph30_5                 ArrayParamsI[142]  //,,1
#define    yph30_6                 ArrayParamsI[143]  //,,1
#define    Transition              ArrayParamsI[144]  //,,1
#define    TransTimer              ArrayParamsI[145]  //,,1
#define    PulseFillPeriod         ArrayParamsI[146]  //,,1
#define    DI_Equalize             ArrayParamsI[147]  //,,1
#define    count30SP              ArrayParamsI[148]  //,,1


//-----------------WRITTABLE PARAMS---------------------

#define    UnitOnCommand           ArrayParamsI[200]
#define    AcceptReset             ArrayParamsI[201]
#define    TestMode                ArrayParamsI[202]
#define    CurrentSensingMode      ArrayParamsI[203]  //,,1
#define    ExtRefEnable            ArrayParamsI[204]  //,,1
#define    ManualBalancing         ArrayParamsI[205]  //,,1
#define    Auto_BoostEnable        ArrayParamsI[206]  //,,1
#define    BiasOffset              ArrayParamsI[207]  //,,1
#define    unitOncmd               ArrayParamsI[208]  //,,1

#define    CANFailDelay            ArrayParamsI[209]  //,,1
#define    BalancingRate           ArrayParamsI[210]  //,,1
#define    AverageCount            ArrayParamsI[211]  //,,1
#define    IdcHalfLimiting         ArrayParamsI[212]  //,,1
#define    __notUsedW5             ArrayParamsI[213]  //,,1
#define    __notUsedW6             ArrayParamsI[214]  //,,1

#define    MsgMask1                ArrayParamsI[215]  //,,1
#define    MsgMask2                ArrayParamsI[216]  //,,1
#define    MsgMask3                ArrayParamsI[217]  //,,1
#define    EventMask1              ArrayParamsI[218]  //,,1
#define    EventMask2              ArrayParamsI[219]  //,,1

#define    EventMask3              ArrayParamsI[220]  //,,1
#define    BalancingEnable         ArrayParamsI[221]  //,,1
#define    BalancingDirection      ArrayParamsI[222]  //,,1
#define    MaxBalanceCount         ArrayParamsI[223]  //,,1
#define    IbatOffset              ArrayParamsI[224]  //,,1
#define    SoftstartInterval       ArrayParamsI[225]  //,,1
#define    RestartDelay            ArrayParamsI[226]  //,,1
#define    FaultResetDelay         ArrayParamsI[227]  //,,1
#define    UpdateEEPROM            ArrayParamsI[228]  //,,1
#define    notusedx                ArrayParamsI[229]  //,,1

#define    OpenLoop                ArrayParamsI[230]  //, ,1
#define    FiringAngle             ArrayParamsI[231]  //, ,10
#define    SharingEnable           ArrayParamsI[232]  //, ,1
#define    IacLimitEnable          ArrayParamsI[233]  //,,1
#define    DeviceID                ArrayParamsI[234]  //,,1
#define    BaudRate                ArrayParamsI[235]  //,KB,10
#define    nCycles                 ArrayParamsI[236]  //,,1
#define    SyncCorrection          ArrayParamsI[237]  //,,1
#define    Auto_SetRefLevels       ArrayParamsI[238]  //,,1
#define    PI_Interval             ArrayParamsI[239]  //,,1
#define    MaxRestartCount         ArrayParamsI[240]  //,,1
#define    PacketDelay             ArrayParamsI[241]  //,,1
#define    DO_TestFlag             ArrayParamsI[242]  //,,1
#define    ManualModeSelect        ArrayParamsI[243]  //,,1
#define    ForcedFloat             ArrayParamsI[244]  //,,1
#define    ForcedBoost             ArrayParamsI[245]  //,,1
#define    ForcedBatTest           ArrayParamsI[246]  //,,1
#define    ForcedEqualize          ArrayParamsI[247]  //,,1
#define    DI_ModeSelect           ArrayParamsI[248]  //,,1
#define    CorrectionSpeed         ArrayParamsI[249]  //,,1
#define    FilledPulse             ArrayParamsI[250]  //,,1

#define    __notUzd005             ArrayParamsI[251]  //,,1
#define    bridge2                 ArrayParamsI[252]  //,,1
#define    cfgAlarmFlagI           ArrayParamsI[253]  //,,1
#define    cfgTripFlagI            ArrayParamsI[254]  //,,1
#define    cfgAutoResetFlagI       ArrayParamsI[255]  //,,1

#define    cfgAlarmFlag            ArrayParamsI[256]  //,,1
#define    cfgTripFlag             ArrayParamsI[257]  //,,1
#define    cfgNOFlag               ArrayParamsI[258]  //,,1
#define    cfgAutoResetFlag        ArrayParamsI[259]  //,,1

#define    CommonAlarmBits1       ArrayParamsI[364]  //,,1
#define    CommonAlarmBits2       ArrayParamsI[365]  //,,1
#define    CommonAlarmBits3       ArrayParamsI[366]  //,,1
#define    CommonAlarmBits4       ArrayParamsI[367]  //,,1
#define    CommonAlarmBits5       ArrayParamsI[368]  //,,1
#define    CommonAlarmBits6       ArrayParamsI[369]  //,,1


#define    RTCSecond               ArrayParamsI[380]  //,,1
#define    RTCMinute               ArrayParamsI[381]  //,,1
#define    RTCHour                 ArrayParamsI[382]  //,,1
#define    RTCDay                  ArrayParamsI[383]  //,,1
#define    RTCDate                 ArrayParamsI[384]  //,,1
#define    RTCMonth                ArrayParamsI[385]  //,,1
#define    RTCYear                 ArrayParamsI[386]  //,,1

#define     UnitOnCommand_Bit       DOParams[100]
#define     AcceptReset_Bit         DOParams[101]
#define     TestMode_Bit            DOParams[102]
#define     RevPolarity_Bit         DOParams[103]
#define     AutoLevel_Bit           DOParams[104]
#define     PFSign_Bit              DOParams[105]
#define     UpdateEEPROM_Bit        DOParams[106]
#define     emShutdown_Bit          DOParams[107]


#define     MAIN_RLY_SIG_START      276
#define     IOM1_RLY_SIG_START      288
#define     IOM2_RLY_SIG_START      304

#define     MAIN_RLY_DLY_START      320
#define     IOM1_RLY_DLY_START      332
#define     IOM2_RLY_DLY_START      348
#define     TRIP_SIG_START          370


//Flag Definitions:
/*
STAT1-BIT00: 01.System Self Check
STAT1-BIT01: 02.Rectifier ON
STAT1-BIT02: 03.Output Ramping
STAT1-BIT03: 04.Wrong Sequence
STAT1-BIT04: 05.Iac Unbalance
STAT1-BIT05: 06.Single Phasing
STAT1-BIT06: 07.Batt.Failure
STAT1-BIT07: 08.INPUT OVERLOAD
STAT1-BIT08: 09.CV Mode
STAT1-BIT09: 10.CC Mode
STAT1-BIT10: 11.Unit On
STAT1-BIT11: 12.High Ripple
STAT1-BIT12: 13.
STAT1-BIT13: 14.Trip Fault
STAT1-BIT14: 15.Latch Fault
STAT1-BIT15: 16.BITS VALID

STAT2-BIT00: 17.Mains Vol.Low
STAT2-BIT01: 18.Mains Vol.High
STAT2-BIT02: 19.Common Alarm
STAT2-BIT03: 20.Vdc High
STAT2-BIT04: 21.Idc High
STAT2-BIT05: 22.Ibat High
STAT2-BIT06: 23.VLoad High
STAT2-BIT07: 24.VBat high
STAT2-BIT08: 25.Vdc Low
STAT2-BIT09: 26.Vbat Low
STAT2-BIT10: 27.Vload Low
STAT2-BIT11: 28.CAN Failure
STAT2-BIT12: 29.Current Mode
STAT2-BIT13: 30.Voltage Mode
STAT2-BIT14: 31.IBatt. Mode
STAT2-BIT15: 32.Bits VALID

STAT3-BIT00: 33.UNIT ON
STAT3-BIT01: 34.ACCEPT/RESET
STAT3-BIT02: 35.CP/CC Mode
STAT3-BIT03: 36.DG_Mode
STAT3-BIT04: 37.FloatMode
STAT3-BIT05: 38.BoostMode
STAT3-BIT06: 38.Batt.TestMode
STAT3-BIT07: 39.Equalize Mode
STAT3-BIT08: 40.AutoMode
STAT3-BIT09: 41.Not Assigned
STAT3-BIT10: 42.Not Assigned
STAT3-BIT11: 43.Not Assigned
STAT3-BIT12: 44.Mode Selection


STAT4-BIT00: 49. Float Mode
STAT4-BIT01: 50. Boost Mode
STAT4-BIT02: 51. Equalize
STAT4-BIT03: 52. Bat.Test Mode
STAT4-BIT04: 53. DG Mode
STAT4-BIT05: 54. Discharge
STAT4-BIT06: 55. Charge
STAT4-BIT07: 56. Bat.Test FAIL
STAT4-BIT08: 57. VRY OK
STAT4-BIT09: 58. VYB OK
STAT4-BIT10: 59. VBR OK
STAT4-BIT11: 60. INPUT OK
STAT4-BIT12: 61. SYNC OK
STAT4-BIT13: 61. INP.AC FAIL
STAT4-BIT14: 61. OUTPUT FAIL
STAT4-BIT15: 64. OpenLoop

STAT5-BIT00: 65.IOM1_DI_01
STAT5-BIT01: 66.IOM1_DI_02
STAT5-BIT02: 67.IOM1_DI_03
STAT5-BIT03: 68.IOM1_DI_04
STAT5-BIT04: 69.IOM1_DI_05
STAT5-BIT05: 70.IOM1_DI_06
STAT5-BIT06: 71.IOM1_DI_07
STAT5-BIT07: 72.IOM1_DI_08
STAT5-BIT08: 73.IOM1_DI_09
STAT5-BIT09: 74.IOM1_DI_10
STAT5-BIT10: 75.IOM1_DI_11
STAT5-BIT11: 76.IOM1_DI_12
STAT5-BIT12: 77.IOM1_DI_13
STAT5-BIT13: 78.IOM1_DI_14
STAT5-BIT14: 79.IOM1_DI_15
STAT5-BIT15: 80.IOM1_DI_16

STAT6-BIT00: 81.IOM2_DI_01
STAT6-BIT01: 82.IOM2_DI_02
STAT6-BIT02: 83.IOM2_DI_03
STAT6-BIT03: 84.IOM2_DI_04
STAT6-BIT04: 85.IOM2_DI_05
STAT6-BIT05: 86.IOM2_DI_06
STAT6-BIT06: 87.IOM2_DI_07
STAT6-BIT07: 88.IOM2_DI_08
STAT6-BIT08: 89.IOM2_DI_09
STAT6-BIT09: 90.IOM2_DI_10
STAT6-BIT10: 91.IOM2_DI_11
STAT6-BIT11: 92.IOM2_DI_12
STAT6-BIT12: 93.IOM2_DI_13
STAT6-BIT13: 94.IOM2_DI_14
STAT6-BIT14: 95.IOM2_DI_15
STAT6-BIT15: 96.IOM2_DI_16


STAT7-BIT00: 97.RLY1 On
STAT7-BIT01: 98.RLY2 On
STAT7-BIT02: 99.RLY3 On
STAT7-BIT03:100.RLY4 On
STAT7-BIT04:101.RLY5 On
STAT7-BIT05:102.RLY6 On
STAT7-BIT06:103.RLY7 On
STAT7-BIT07:104.RLY8 On
STAT7-BIT08:105.RLY9 On
STAT7-BIT09:106.RLY10 On
STAT7-BIT10:107.RLY11 On
STAT7-BIT11:108.RLY12 On

PLOTS-WAV00: Inp.Vol_R
PLOTS-WAV01: Inp.Vol_Y
PLOTS-WAV02: Inp.Vol_B
PLOTS-WAV03: Iinp_R
PLOTS-WAV04: Iinp_Y
PLOTS-WAV05: Iinp_B
PLOTS-WAV06: Vdc
PLOTS-WAV07: VLoad
PLOTS-WAV08: Idc
PLOTS-WAV09: Ibat
PLOTS-WAV10: VdcPID
PLOTS-WAV11: R top_1
PLOTS-WAV12: R bot_1
PLOTS-WAV13: Y top_1
PLOTS-WAV14: Y bot_1
PLOTS-WAV15: B top_1
PLOTS-WAV16: B bot_1
PLOTS-WAV17: R top_2
PLOTS-WAV18: Y top_2
PLOTS-WAV19: PLL.Sine
*/

#define   INT_RANGE_WMIN 700
#define   INT_RANGE_WMAX 899
#define   FLOAT_RANGE_WMIN 240
#define   FLOAT_RANGE_WMAX 480

#define   INT_RANGE_RMIN 500
#define   INT_RANGE_RMAX 899
#define   FLOAT_RANGE_RMIN 0
#define   FLOAT_RANGE_RMAX 450

#define     SERIAL_START    390
#define     SERIAL_END      399
#define     CMD_SETDFLT     999
#define     ADR_RESET       201
#define     ADR_COMMIT      228
#define     DELAY_START     240
#define     BASE_RTC        380
#define     ALCFG_START     256
#define     COMALRM_START   364
#define     ADDR_UID        234
#define     ADDR_BAUD       235

#define     SET_DEFAULT      0x0002
#define     UPDATE_SERIAL    0x0004
#define     WR_START1        200

#define     NUMWPARAMS1      180

#endif //__RECTPARAMS_H__

