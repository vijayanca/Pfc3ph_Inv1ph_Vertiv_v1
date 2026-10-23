/*
 * pfc3phParams.h
 *
 * Parameter map of the 3-phase PFC (active front end) on the Common
 * Controller v2. Control as in the RBC (RBC2_KDSENG_v4): DC-link PI -> Id
 * reference, dq PI or alpha-beta PR current control.
 *
 *  Created on: 23-Oct-2026 (from inv3phParams.h)
 *      Author: vijay
 */

//COMPANYNAME:Vertiv
//UNITNAME:3PH PFC
//LOCATION:
//INPUTV:415V AC, 3PH, 50Hz
//OUTPUTV:DC LINK
//OUTPUTCUR:
//RATEDKVA:

#ifndef PFC3PHPARAMS_H_
#define PFC3PHPARAMS_H_

#define    VinpRY                      ArrayParamsF[0]  //,V,1
#define    VinpYB                      ArrayParamsF[1]  //,V,1
#define    VinpBR                      ArrayParamsF[2]  //,V,1
#define    VinpRph                     ArrayParamsF[3]  //,V,1
#define    VinpYph                     ArrayParamsF[4]  //,V,1
#define    VinpBph                     ArrayParamsF[5]  //,V,1
#define    IinpR                       ArrayParamsF[6]  //,A,1
#define    IinpY                       ArrayParamsF[7]  //,A,1
#define    IinpB                       ArrayParamsF[8]  //,A,1
#define    DcLinkVoltage               ArrayParamsF[9]  //,V,1
#define    Vbat                        ArrayParamsF[10]  //,V,1
#define    Ibat                        ArrayParamsF[11]  //,A,1
#define    Temp1                       ArrayParamsF[12]  //,C,1
#define    Temp2                       ArrayParamsF[13]  //,C,1
#define    DcLinkRefActive             ArrayParamsF[14]  //,V,1

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
#define    GridFrequency               ArrayParamsF[34]  //,Hz,1

#define    dclink_ref                  ArrayParamsF[40]  //, ,1
#define    dclink_fback                ArrayParamsF[41]  //, ,1
#define    dclink_pidout               ArrayParamsF[42]  //, ,1
#define    current_de_ref              ArrayParamsF[43]  //, ,1
#define    current_qe_ref              ArrayParamsF[44]  //, ,1
#define    current_de_fback            ArrayParamsF[45]  //, ,1
#define    current_qe_fback            ArrayParamsF[46]  //, ,1
#define    current_de_pidout           ArrayParamsF[47]  //, ,1
#define    current_qe_pidout           ArrayParamsF[48]  //, ,1
#define    vol_de                      ArrayParamsF[49]  //, ,1
#define    vol_qe                      ArrayParamsF[50]  //, ,1
#define    ffvd_mon                    ArrayParamsF[51]  //, ,1
#define    ccvd_mon                    ArrayParamsF[52]  //, ,1
#define    ccvq_mon                    ArrayParamsF[53]  //, ,1
#define    vdref_mon                   ArrayParamsF[54]  //, ,1
#define    vqref_mon                   ArrayParamsF[55]  //, ,1
#define    DMinVal_mon                 ArrayParamsF[56]  //, ,1

//-------------------------------------------------------------------

#define    DcLinkReference             ArrayParamsF[100]  //,V,1
#define    PowerLimit                  ArrayParamsF[101]  //, ,1
//  DC-link PI output limit = PowerLimit x 0.1 (Id reference, p.u.)
#define    GridVoltageHL               ArrayParamsF[102]  //,V,1
#define    GridVoltageLL               ArrayParamsF[103]  //,V,1
#define    DCLinkHL                    ArrayParamsF[104]  //,V,1
#define    DCLinkLL                    ArrayParamsF[105]  //,V,1
#define    IinpHL                      ArrayParamsF[106]  //,A,1
#define    OC_Level                    ArrayParamsF[107]  //,A,1
//  instantaneous overcurrent, peak
#define    DCOV_Inst_Level             ArrayParamsF[108]  //,V,1
#define    GridVoltageHL_delay         ArrayParamsF[109]  //,S,1
#define    GridVoltageLL_delay         ArrayParamsF[110]  //,S,1
#define    DCLinkHL_delay              ArrayParamsF[111]  //,S,1
#define    DCLinkLL_delay              ArrayParamsF[112]  //,S,1
#define    IinpHL_delay                ArrayParamsF[113]  //,S,1
#define    FrequencyLowLimit           ArrayParamsF[114]  //,Hz,1
#define    FrequencyHighLimit          ArrayParamsF[115]  //,Hz,1
#define    FrequencyLowDelay           ArrayParamsF[116]  //,S,1
#define    FrequencyHighDelay          ArrayParamsF[117]  //,S,1
#define    PreChargeLevel              ArrayParamsF[118]  //,V,1
#define    vdc_hysteresis              ArrayParamsF[119]  //,V,1
#define    VinpRSF                     ArrayParamsF[120]  //, ,1
#define    VinpYSF                     ArrayParamsF[121]  //, ,1
#define    VinpBSF                     ArrayParamsF[122]  //, ,1
#define    IinpRSF                     ArrayParamsF[123]  //, ,1
#define    IinpYSF                     ArrayParamsF[124]  //, ,1
#define    IinpBSF                     ArrayParamsF[125]  //, ,1
#define    DcLinkVoltageSF             ArrayParamsF[126]  //, ,1
#define    VbatSF                      ArrayParamsF[127]  //, ,1
#define    IbatSF                      ArrayParamsF[128]  //, ,1
#define    Temp1SF                     ArrayParamsF[129]  //, ,1
#define    Temp2SF                     ArrayParamsF[130]  //, ,1
#define    CrossXL                     ArrayParamsF[131]  //, ,1
#define    dclink_kp                   ArrayParamsF[132]  //, ,1
#define    dclink_ki                   ArrayParamsF[133]  //, ,1
#define    current_kp                  ArrayParamsF[134]  //, ,1
#define    current_ki                  ArrayParamsF[135]  //, ,1
#define    current_lpfT                ArrayParamsF[136]  //, ,1
#define    InputLPF                    ArrayParamsF[137]  //, ,1
#define    pid_de_Umax                 ArrayParamsF[138]  //, ,1
#define    pfc_pid_qmax                ArrayParamsF[139]  //, ,1
#define    pid_DCLink_Umin             ArrayParamsF[140]  //, ,1
//  reverse (regenerative) current demand limit of the DC-link PI
#define    QRef                        ArrayParamsF[141]  //, ,1
#define    FswPFC                      ArrayParamsF[142]  //,kHz,1
#define    PR_Kp                       ArrayParamsF[143]  //, ,1
#define    PR_Kr                       ArrayParamsF[144]  //,1/s,1
#define    PR_Kh                       ArrayParamsF[145]  //,1/s,1
//  5th / 7th harmonic resonator gain (PR mode), < 1 = off
#define    DtComp                      ArrayParamsF[146]  //, ,1
//  dead-time compensation, fraction of DBRED/TBPRD, sign = current polarity, 0 = off
#define    DtBand                      ArrayParamsF[147]  //,A,1
//  current band where the compensation fades in linearly
#define    RatedKW                     ArrayParamsF[148]  //,kW,1
#define    GridFailLevel               ArrayParamsF[149]  //,V,1
//  fast mains failure (positive sequence, line-line rms): PFC pulses off

#define    LastMem              ArrayParamsF[199]

#define     NUMWPARAMS      60
#define     WR_START        100

/*
STAT1-BIT00: System Self Check
STAT1-BIT01: PFC Disabled
STAT1-BIT02: DC Link Ramping
STAT1-BIT03: Precharge On
STAT1-BIT04: Main Contactor On
STAT1-BIT05: Wrong Phase Sequence
STAT1-BIT06: Grid Failure
STAT1-BIT07: Input OverLoad
STAT1-BIT08: VCESAT Protection
STAT1-BIT09: Inst. OverCurrent
STAT1-BIT10: DC Link Inst. OV
STAT1-BIT11: Emergency Stop
STAT1-BIT12: DI Trip
STAT1-BIT13: TRIP FAULT
STAT1-BIT14: LATCHED Fault
STAT1-BIT15: BITS VALID

STAT2-BIT00: Input Vrph Low
STAT2-BIT01: Input Vyph Low
STAT2-BIT02: Input Vbph Low
STAT2-BIT03: Input Vrph High
STAT2-BIT04: Input Vyph High
STAT2-BIT05: Input Vbph High
STAT2-BIT06: Input Ir High
STAT2-BIT07: Input Iy High
STAT2-BIT08: Input Ib High
STAT2-BIT09: Inst.OC R
STAT2-BIT10: Inst.OC Y
STAT2-BIT11: Inst.OC B
STAT2-BIT12: Frequency Low
STAT2-BIT13: Frequency High
STAT2-BIT14: Not Used
STAT2-BIT15: BITS VALID

STAT3-BIT00: DC Link Low
STAT3-BIT01: DC Link High
STAT3-BIT02: DC Link OK
STAT3-BIT03: DC Link Inst.OV
STAT3-BIT04: Not Used
STAT3-BIT05: Not Used
STAT3-BIT06: Not Used
STAT3-BIT07: Not Used
STAT3-BIT08: Not Used
STAT3-BIT09: Not Used
STAT3-BIT10: Not Used
STAT3-BIT11: Not Used
STAT3-BIT12: Not Used
STAT3-BIT13: Not Used
STAT3-BIT14: Not Used
STAT3-BIT15: BITS VALID

STAT4-BIT00: Unit ON (DI1)
STAT4-BIT01: Reset (DI2)
STAT4-BIT02: Main Con.Aux (DI3)
STAT4-BIT03: DI4
STAT4-BIT04: DI5
STAT4-BIT05: DI6
STAT4-BIT06: Heatsink Temp (DI7)
STAT4-BIT07: Emergency PB (DI8)
STAT4-BIT08: Input Vr OK
STAT4-BIT09: Input Vy OK
STAT4-BIT10: Input Vb OK
STAT4-BIT11: Input OK
STAT4-BIT12: DC Link OK
STAT4-BIT13: Precharge OK
STAT4-BIT14: Grid Sync OK
STAT4-BIT15: Main Con. Closed

STAT5-BIT00: PFC Enabled
STAT5-BIT01: Test Mode
STAT5-BIT02: Relay Precharge
STAT5-BIT03: Relay Main Con.
STAT5-BIT04: Relay PFC OK
STAT5-BIT05: Relay Common Fault
STAT5-BIT06: Dmax Ramping
STAT5-BIT07: PR Current Control
STAT5-BIT15: BITSVALID

PLOTS-WAV00: Vgrid_R
PLOTS-WAV01: Vgrid_Y
PLOTS-WAV02: Vgrid_B
PLOTS-WAV03: Iinp_R
PLOTS-WAV04: Iinp_Y
PLOTS-WAV05: Iinp_B
PLOTS-WAV06: DC Link
PLOTS-WAV07: Mod_Alpha
PLOTS-WAV08: Mod_Beta
PLOTS-WAV09: Id_fbk
PLOTS-WAV10: Iq_fbk
PLOTS-WAV11: Id_out
PLOTS-WAV12: Iq_out
PLOTS-WAV13: DCLink_PI_out
PLOTS-WAV14: DCLink_SoftRef
PLOTS-WAV15: PLL_Sin
PLOTS-WAV16: PLL_Sin0
PLOTS-WAV17: Mod_Ta
PLOTS-WAV18: Mod_Tb
PLOTS-WAV19: Mod_Tc

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
#define     WritePending        ArrayParamsI[42]  //, ,1
#define     WritePendingInt     ArrayParamsI[43]  //, ,1
#define     MuteFlag            ArrayParamsI[46]  //, ,1
#define     FaultBits           ArrayParamsI[47]  //, ,1
#define     BuzzerOn            ArrayParamsI[48]  //, ,1

#define     reason              ArrayParamsI[51]  //, ,1
#define     systemRunningTimer  ArrayParamsI[52]  //, ,1
#define     pwmEnabled          ArrayParamsI[55]  //, ,1
#define     MainConClosed       ArrayParamsI[56]  //, ,1
#define     FSW_pfc             ArrayParamsI[57]  //, ,1
#define     AdcIsrCount         ArrayParamsI[58]  //, ,1
#define     T0IsrCount          ArrayParamsI[59]  //, ,1
#define     Dmax_Ramping        ArrayParamsI[60]  //, ,1
#define     wrongSeqCount       ArrayParamsI[61]  //, ,1
#define     gridFailed          ArrayParamsI[62]  //, ,1
#define     RecordNumber        ArrayParamsI[63]  //, ,1
#define     PrechargeTimer      ArrayParamsI[64]  //,S,1
#define     poweronTimer        ArrayParamsI[65]  //,ms,1
#define     unitOnCommand       ArrayParamsI[66]  //, ,1
#define     Syncfailtimer       ArrayParamsI[67]  //,ms,1
#define     mainConTimer        ArrayParamsI[68]  //,ms,1
#define     pulseON             ArrayParamsI[69]  //, ,1

#define     AutoSaveTimer       ArrayParamsI[89]  //, ,1
#define     adcIsrTime          ArrayParamsI[90]  //, ,1
#define     adrIsrDuration      ArrayParamsI[91]  //, ,1
#define     claLayoutOK         ArrayParamsI[92]  //, ,1

#define     EmergencyStop       ArrayParamsI[95]  //, ,1
#define     eventSerialNumber   ArrayParamsI[97]  //, ,1
#define     canRxCounter        ArrayParamsI[98]  //, ,1
#define     txMsgCount          ArrayParamsI[99]  //, ,1



//--------------------------WRITTABLE------------------------------
#define     UnitOnCommand       ArrayParamsI[100]  //, ,1
#define     AcceptReset         ArrayParamsI[101]  //, ,1
#define     TestMode            ArrayParamsI[102]  //, ,1
#define     MaxRestartCount     ArrayParamsI[103]  //, ,1
#define     PreCharge_delay     ArrayParamsI[104]  //,S,1
#define     DCOV_Inst_Delay     ArrayParamsI[105]  //,50uS,1
#define     OC_Delay            ArrayParamsI[106]  //,50uS,1
#define     TripBits            ArrayParamsI[107]  //, ,1
//  DI bits (STAT4 low byte) that trip the PFC
#define     CaptureNow          ArrayParamsI[108]  //, ,1
#define     RestartDelay        ArrayParamsI[109]  //,S,1
#define     FaultResetDelay     ArrayParamsI[110]  //,S,1
#define     RevPolarity         ArrayParamsI[111]  //, ,1
#define     PFSign              ArrayParamsI[112]  //, ,1
#define     PfcCtrlMode         ArrayParamsI[113]  //, ,1
//  PFC current control: 0 = dq PI, 1 = PR (alpha-beta)
#define     UnitAddress         ArrayParamsI[114]  //, ,1
#define     MainConAuxUsed      ArrayParamsI[115]  //, ,1
//  1 = main contactor closed from the aux contact (DI3), 0 = 0.5 s after the command
#define     AutoLevel           ArrayParamsI[116]  //, ,1
//  1 = precharge level 1.2 x input line voltage
#define     nCycles             ArrayParamsI[117]  //, ,1
#define     IbatOffset          ArrayParamsI[118]  //, ,1
//  battery current sensor zero, 12-bit ADC count
#define     DI_NO_Flag          ArrayParamsI[122]  //, ,1
#define     UpdateEEPROM        ArrayParamsI[123]  //, ,1

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

// Modbus coils 100.. (DObitAddress[] in modbus.c gives the int parameter)
#define     UnitOnCommand_Bit       DOParams[100]
#define     AcceptReset_Bit         DOParams[101]
#define     TestMode_Bit            DOParams[102]
#define     RevPolarity_Bit         DOParams[103]
#define     PFSign_Bit              DOParams[104]
#define     MainConAuxUsed_Bit      DOParams[105]
#define     UpdateEEPROM_Bit        DOParams[106]
#define     emShutdown_Bit          DOParams[107]


#endif
