/*
 * pfc3phParams.h
 *
 * Parameter map of the 3-phase PFC + 1-phase inverter (optional battery
 * buck-boost) on the Common Controller v2. PFC control as in the RBC
 * (RBC2_KDSENG_v4): DC-link PI -> Id reference, dq PI or alpha-beta PR
 * current control.
 *
 *  Created on: 23-Oct-2026 (from inv3phParams.h)
 *      Author: vijay
 */

//COMPANYNAME:Vertiv
//UNITNAME:3PH PFC + 1PH INVERTER
//LOCATION:
//INPUTV:415V AC L-L (240V L-N sensing), 2:1 autotransformer to the PFC (~220V L-L), 3PH, 50Hz
//OUTPUTV:230V AC, 1PH
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

// 1-phase inverter / output / bypass / battery
#define    Vinv                        ArrayParamsF[60]  //,V,1
#define    Vout                        ArrayParamsF[61]  //,V,1
#define    Vbyp                        ArrayParamsF[62]  //,V,1
#define    Iinv                        ArrayParamsF[63]  //,A,1
#define    Iout                        ArrayParamsF[64]  //,A,1
#define    kW_Out                      ArrayParamsF[65]  //,kW,1
#define    kVA_Out                     ArrayParamsF[66]  //,kVA,1
#define    pf_Out                      ArrayParamsF[67]  //,,1
#define    LoadPercent                 ArrayParamsF[68]  //,%,1
#define    InvFrequency                ArrayParamsF[69]  //,Hz,1
#define    BypFrequency                ArrayParamsF[70]  //,Hz,1
#define    InvVrefActive               ArrayParamsF[71]  //,V,1
#define    invModulation               ArrayParamsF[72]  //, ,1
#define    bbDuty_mon                  ArrayParamsF[73]  //, ,1
#define    IbatCharge                  ArrayParamsF[74]  //,A,1
//  battery current, charging positive
#define    DcLinkRefAuto               ArrayParamsF[75]  //,V,1
//  DC-link reference from the battery current limit (no buck-boost)
#define    syncError                   ArrayParamsF[76]  //, ,1

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

//--------------------- 1-phase inverter, bypass ---------------------
#define    RatedVoltage                ArrayParamsF[150]  //,V,1
#define    RatedCurrent                ArrayParamsF[151]  //,A,1
#define    InvVoltageHL                ArrayParamsF[152]  //,V,1
#define    InvVoltageLL                ArrayParamsF[153]  //,V,1
#define    InvVoltHL_delay             ArrayParamsF[154]  //,S,1
#define    InvVoltLL_delay             ArrayParamsF[155]  //,S,1
#define    BypVoltageHL                ArrayParamsF[156]  //,V,1
#define    BypVoltageLL                ArrayParamsF[157]  //,V,1
#define    BypVoltHL_delay             ArrayParamsF[158]  //,S,1
#define    BypVoltLL_delay             ArrayParamsF[159]  //,S,1
#define    InvOverload                 ArrayParamsF[160]  //,%,1
#define    InvOverload_delay           ArrayParamsF[161]  //,S,1
#define    InvOC_Level                 ArrayParamsF[162]  //,A,1
//  instantaneous inverter overcurrent (Iinv), peak
#define    InvDCLinkMin                ArrayParamsF[163]  //,V,1
//  inverter stops below this DC-link voltage
#define    VinvSF                      ArrayParamsF[164]  //, ,1
#define    VoutSF                      ArrayParamsF[165]  //, ,1
#define    VbypSF                      ArrayParamsF[166]  //, ,1
#define    IinvSF                      ArrayParamsF[167]  //, ,1
#define    IoutSF                      ArrayParamsF[168]  //, ,1
#define    inv_kp                      ArrayParamsF[169]  //, ,1
//  voltage loop proportional gain (V/V)
#define    inv_kr                      ArrayParamsF[170]  //,1/s,1
//  voltage loop fundamental resonant gain
#define    inv_kh                      ArrayParamsF[171]  //,1/s,1
//  voltage loop 3rd/5th/7th resonant gain (pr_hMaskInv)
#define    inv_kci                     ArrayParamsF[172]  //,V/A,1
//  inner current loop (virtual resistance on Iinv), 0 = off
#define    FswInv                      ArrayParamsF[173]  //,kHz,1
#define    LockingLevel                ArrayParamsF[174]  //, ,1
//  synchronised when |sin(phase error)| below this
//--------------------- battery, buck-boost --------------------------
#define    VbatFloat                   ArrayParamsF[175]  //,V,1
#define    IbatRef                     ArrayParamsF[176]  //,A,1
//  battery charging current limit
#define    DcLinkMinNoBuck             ArrayParamsF[177]  //,V,1
#define    DcLinkMaxNoBuck             ArrayParamsF[178]  //,V,1
#define    VbatHL                      ArrayParamsF[179]  //,V,1
#define    VbatLL                      ArrayParamsF[180]  //,V,1
//  battery end of discharge (buck-boost: boost stops)
#define    VbatHL_delay                ArrayParamsF[181]  //,S,1
#define    VbatLL_delay                ArrayParamsF[182]  //,S,1
#define    DcLinkBoostRef              ArrayParamsF[183]  //,V,1
//  DC link held by the boost on mains failure
#define    BoostDmax                   ArrayParamsF[184]  //, ,1
#define    vbat_kp                     ArrayParamsF[185]  //, ,1
#define    vbat_ki                     ArrayParamsF[186]  //, ,1
#define    ibat_kp                     ArrayParamsF[187]  //, ,1
#define    ibat_ki                     ArrayParamsF[188]  //, ,1
#define    IbatOC_Level                ArrayParamsF[189]  //,A,1
//  instantaneous battery overcurrent (buck-boost), peak
#define    boost_kp                    ArrayParamsF[190]  //, ,1
#define    boost_ki                    ArrayParamsF[191]  //, ,1
#define    nobuck_kp                   ArrayParamsF[192]  //,V/A,1
//  no buck-boost: DC-link reduction per A of excess charging current
#define    nobuck_ki                   ArrayParamsF[193]  //, ,1
#define    FswBB                       ArrayParamsF[194]  //,kHz,1
#define    RatedKVA                    ArrayParamsF[195]  //,kVA,1

#define    LastMem              ArrayParamsF[199]

#define     NUMWPARAMS      100
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

STAT6-BIT00: Inverter Disabled
STAT6-BIT01: Inverter Ramping
STAT6-BIT02: Load on Inverter
STAT6-BIT03: Load on Bypass
STAT6-BIT04: Manual Bypass
STAT6-BIT05: Not Synced
STAT6-BIT06: Inverter Voltage Low
STAT6-BIT07: Inverter Voltage High
STAT6-BIT08: Inverter OverLoad
STAT6-BIT09: Inverter Inst.OC
STAT6-BIT10: Bypass OK
STAT6-BIT11: Inverter OK
STAT6-BIT12: DC Link Low (Inverter)
STAT6-BIT13: Inverter Trip
STAT6-BIT14: Inverter Latched
STAT6-BIT15: BITSVALID

STAT7-BIT00: Buck-Boost Fitted
STAT7-BIT01: Battery Charging (Buck)
STAT7-BIT02: Battery Discharging (Boost)
STAT7-BIT03: Charge Current Limit
STAT7-BIT04: Battery Low
STAT7-BIT05: Battery High
STAT7-BIT06: Battery Inst.OC
STAT7-BIT07: Buck-Boost VCESAT
STAT7-BIT08: Battery End of Discharge
STAT7-BIT15: BITSVALID

PLOTS-WAV00: Vgrid_R
PLOTS-WAV01: Vgrid_Y
PLOTS-WAV02: Vgrid_B
PLOTS-WAV03: Iinp_R
PLOTS-WAV04: Iinp_Y
PLOTS-WAV05: Iinp_B
PLOTS-WAV06: DC Link
PLOTS-WAV07: Id_fbk
PLOTS-WAV08: Iq_fbk
PLOTS-WAV09: DCLink_PI_out
PLOTS-WAV10: Vinv
PLOTS-WAV11: Vout
PLOTS-WAV12: Iinv
PLOTS-WAV13: Iout
PLOTS-WAV14: Vbypass
PLOTS-WAV15: Inv_Vref
PLOTS-WAV16: Inv_Mod
PLOTS-WAV17: Inv_Sin
PLOTS-WAV18: Byp_PLL_Sin
PLOTS-WAV19: Ibat

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
#define     AlarmFlag6          ArrayParamsI[70]  //, ,2
#define     AlarmFlag7          ArrayParamsI[71]  //, ,2
#define     invPwmEnabled       ArrayParamsI[72]  //, ,1
#define     bbMode              ArrayParamsI[73]  //, ,1
//  0 = off, 1 = buck (charging), 2 = boost (discharging)
#define     InvTripCount        ArrayParamsI[74]  //, ,1
#define     invRestartTimer     ArrayParamsI[75]  //,S,1
#define     xfrCommand          ArrayParamsI[76]  //, ,1
#define     Synced              ArrayParamsI[77]  //, ,1
#define     bypassFailed        ArrayParamsI[78]  //, ,1
#define     FSW_inv             ArrayParamsI[79]  //, ,1
#define     xfr2InvTimer        ArrayParamsI[80]  //,S,1
#define     invRunningTimer     ArrayParamsI[81]  //,S,1
#define     invPulseON          ArrayParamsI[82]  //, ,1
#define     bbPulseON           ArrayParamsI[83]  //, ,1

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
#define     BuckBoostEnable     ArrayParamsI[124]  //, ,1
//  0 = battery on the DC link (variable 320-400 V), 1 = buck-boost (680 V DC link)
#define     InvEnable           ArrayParamsI[125]  //, ,1
#define     IbatPolarity        ArrayParamsI[126]  //, ,1
//  1 = battery current sensor reversed (charging must read positive)
#define     InvRevPolarity      ArrayParamsI[127]  //, ,1
//  1 = inverter current sensor reversed
#define     pr_hMaskInv         ArrayParamsI[128]  //, ,1
//  inverter resonators: b0 = 3rd, b1 = 5th, b2 = 7th
#define     InvMaxRestart       ArrayParamsI[129]  //, ,1
#define     xfrToInverterDelay  ArrayParamsI[130]  //,S,1
#define     InvOC_Delay         ArrayParamsI[131]  //,50uS,1
#define     BBOC_Delay          ArrayParamsI[132]  //,50uS,1

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
#define     BuckBoostEnable_Bit     DOParams[107]
#define     InvEnable_Bit           DOParams[108]
#define     emShutdown_Bit          DOParams[109]


#endif
