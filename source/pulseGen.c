/*
 * pulseGen.c
 *
 *  Created on: 17-Sep-2026
 *      Author: vijay
 */

#define CCEXTERN    extern
#define INIT0
#define INITARRAY0
#include "f28x_project.h"
#include "../ccVariables.h"
#include "inc/hw_memmap.h"
#include "sci.h"

// eCAP1 timebase = SYSCLK; SYSTEM_FREQUENCY (MHz) from rectDefines.h
#define ECAP_TICKS_PER_US   ((N32)SYSTEM_FREQUENCY)
volatile N32 prevR_Count = 0;
volatile N32 prevY_Count = 0;
volatile N32 prevB_Count = 0;

#pragma CODE_SECTION(cpu_timer0_isr, ".TI.ramfunc");
#pragma CODE_SECTION(cpu_timer1_isr, ".TI.ramfunc");

#define RP1_ON_NOFILL  EPwm1Regs.AQCSFRC.bit.CSFA = AQ_SET
#define RP1_OFF_NOFILL EPwm1Regs.AQCSFRC.bit.CSFA = AQ_CLEAR
#define RN1_ON_NOFILL  EPwm1Regs.AQCSFRC.bit.CSFB = AQ_SET
#define RN1_OFF_NOFILL EPwm1Regs.AQCSFRC.bit.CSFB = AQ_CLEAR
#define YP1_ON_NOFILL  EPwm2Regs.AQCSFRC.bit.CSFA = AQ_SET
#define YP1_OFF_NOFILL EPwm2Regs.AQCSFRC.bit.CSFA = AQ_CLEAR
#define YN1_ON_NOFILL  EPwm2Regs.AQCSFRC.bit.CSFB = AQ_SET
#define YN1_OFF_NOFILL EPwm2Regs.AQCSFRC.bit.CSFB = AQ_CLEAR
#define BP1_ON_NOFILL  EPwm3Regs.AQCSFRC.bit.CSFA = AQ_SET
#define BP1_OFF_NOFILL EPwm3Regs.AQCSFRC.bit.CSFA = AQ_CLEAR
#define BN1_ON_NOFILL  EPwm3Regs.AQCSFRC.bit.CSFB = AQ_SET
#define BN1_OFF_NOFILL EPwm3Regs.AQCSFRC.bit.CSFB = AQ_CLEAR

#define RP2_ON_NOFILL  EPwm4Regs.AQCSFRC.bit.CSFA = AQ_SET
#define RP2_OFF_NOFILL EPwm4Regs.AQCSFRC.bit.CSFA = AQ_CLEAR
#define RN2_ON_NOFILL  EPwm4Regs.AQCSFRC.bit.CSFB = AQ_SET
#define RN2_OFF_NOFILL EPwm4Regs.AQCSFRC.bit.CSFB = AQ_CLEAR
#define YP2_ON_NOFILL  EPwm5Regs.AQCSFRC.bit.CSFA = AQ_SET
#define YP2_OFF_NOFILL EPwm5Regs.AQCSFRC.bit.CSFA = AQ_CLEAR
#define YN2_ON_NOFILL  EPwm5Regs.AQCSFRC.bit.CSFB = AQ_SET
#define YN2_OFF_NOFILL EPwm5Regs.AQCSFRC.bit.CSFB = AQ_CLEAR
#define BP2_ON_NOFILL  EPwm6Regs.AQCSFRC.bit.CSFA = AQ_SET
#define BP2_OFF_NOFILL EPwm6Regs.AQCSFRC.bit.CSFA = AQ_CLEAR
#define BN2_ON_NOFILL  EPwm6Regs.AQCSFRC.bit.CSFB = AQ_SET
#define BN2_OFF_NOFILL EPwm6Regs.AQCSFRC.bit.CSFB = AQ_CLEAR



//-----------------------------------------------------------

#define RP1_ON  EPwm1Regs.AQCTLA.bit.CAD = AQ_SET
#define RP1_OFF EPwm1Regs.AQCTLA.bit.CAD = AQ_CLEAR
#define RN1_ON  EPwm1Regs.AQCTLB.bit.CBD = AQ_SET
#define RN1_OFF EPwm1Regs.AQCTLB.bit.CBD = AQ_CLEAR
#define YP1_ON  EPwm2Regs.AQCTLA.bit.CAD = AQ_SET
#define YP1_OFF EPwm2Regs.AQCTLA.bit.CAD = AQ_CLEAR
#define YN1_ON  EPwm2Regs.AQCTLB.bit.CBD = AQ_SET
#define YN1_OFF EPwm2Regs.AQCTLB.bit.CBD = AQ_CLEAR
#define BP1_ON  EPwm3Regs.AQCTLA.bit.CAD = AQ_SET
#define BP1_OFF EPwm3Regs.AQCTLA.bit.CAD = AQ_CLEAR
#define BN1_ON  EPwm3Regs.AQCTLB.bit.CBD = AQ_SET
#define BN1_OFF EPwm3Regs.AQCTLB.bit.CBD = AQ_CLEAR

#define RP2_ON  EPwm4Regs.AQCTLA.bit.CAD = AQ_SET
#define RP2_OFF EPwm4Regs.AQCTLA.bit.CAD = AQ_CLEAR
#define RN2_ON  EPwm4Regs.AQCTLB.bit.CBD = AQ_SET
#define RN2_OFF EPwm4Regs.AQCTLB.bit.CBD = AQ_CLEAR
#define YP2_ON  EPwm5Regs.AQCTLA.bit.CAD = AQ_SET
#define YP2_OFF EPwm5Regs.AQCTLA.bit.CAD = AQ_CLEAR
#define YN2_ON  EPwm5Regs.AQCTLB.bit.CBD = AQ_SET
#define YN2_OFF EPwm5Regs.AQCTLB.bit.CBD = AQ_CLEAR
#define BP2_ON  EPwm6Regs.AQCTLA.bit.CAD = AQ_SET
#define BP2_OFF EPwm6Regs.AQCTLA.bit.CAD = AQ_CLEAR
#define BN2_ON  EPwm6Regs.AQCTLB.bit.CBD = AQ_SET
#define BN2_OFF EPwm6Regs.AQCTLB.bit.CBD = AQ_CLEAR


#define PON 0.2

inline void    Pulse_01(int k);
inline void    Pulse_02(int k);
inline void    Pulse_03(int k);
inline void    Pulse_04(int k);
inline void    Pulse_05(int k);
inline void    Pulse_06(int k);
inline void    Pulse_07(int k);
inline void    Pulse_08(int k);
inline void    Pulse_09(int k);
inline void    Pulse_10(int k);
inline void    Pulse_11(int k);
inline void    Pulse_12(int k);


__interrupt void cpu_timer0_isr(void) {

    volatile N32 capcount_now = 0;

    temp_T0IsrCount++;
    capcount_now = ECap1Regs.TSCTR;

    if(pwmEnabled)
    {
     switch(rp_count)
        {
        case 0:
            Pulse_01(FilledPulse);
            STOP_TIMER0;
            LOAD_TIMER0(count30_SP1);
            START_TIMER0;
            rp_count++;
            alphaCountR = (capcount_now - alpha_R_start)/ECAP_TICKS_PER_US;
            break;
        case 1:
            Pulse_02(FilledPulse);
            STOP_TIMER0;
            LOAD_TIMER0(count30_SP2);
            START_TIMER0;
            rp_count++;
            rph30_1 = (capcount_now - prevR_Count)/ECAP_TICKS_PER_US;
            break;
        case 2:
            Pulse_03(FilledPulse);
            STOP_TIMER0;
            LOAD_TIMER0(count30_SP1);
            START_TIMER0;
            rp_count++;
            rph30_2 = (capcount_now - prevR_Count)/ECAP_TICKS_PER_US;
            break;
        case 3:
            Pulse_04(FilledPulse);
            STOP_TIMER0;
            LOAD_TIMER0(count30_SP2);
            START_TIMER0;
            rp_count++;
            rph30_3 = (capcount_now - prevR_Count)/ECAP_TICKS_PER_US;
            break;
        case 4:
               Pulse_05(FilledPulse);
               STOP_TIMER0;
               LOAD_TIMER0(count30_SP1);
               START_TIMER0;
               rp_count++;
               rph30_4 = (capcount_now - prevR_Count)/ECAP_TICKS_PER_US;
               break;
           case 5:
               Pulse_06(FilledPulse);
               STOP_TIMER0;
               rp_count++;
               rph30_5 = (capcount_now - prevR_Count)/ECAP_TICKS_PER_US;
               break;
        }
    }
    else
    {
        DISABLE_PWM();
    }
    prevR_Count = capcount_now;
 CpuTimer0Regs.TCR.bit.TIF = 1;
 PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;   // (Timer2 is also group1 on C28)
}

__interrupt void cpu_timer1_isr(void)
{
    volatile N32 capcount_now = 0;

    temp_T1IsrCount++;
    capcount_now = ECap1Regs.TSCTR;

    if(pwmEnabled)
    {
    switch(bp_count)
    {
        case 0:
            Pulse_07(FilledPulse);
            STOP_TIMER1;
            LOAD_TIMER1(count30_SP1);
            START_TIMER1;
            bp_count++;
            alphaCountY = (capcount_now - alpha_Y_start)/ECAP_TICKS_PER_US;
            break;

        case 1:
            Pulse_08(FilledPulse);
            STOP_TIMER1;
            LOAD_TIMER1(count30_SP2);
            START_TIMER1;
            bp_count++;
            yph30_1 = (capcount_now - prevY_Count)/ECAP_TICKS_PER_US;
            break;

        case 2:
            Pulse_09(FilledPulse);
            STOP_TIMER1;
            LOAD_TIMER1(count30_SP1);
            START_TIMER1;
            bp_count++;
            yph30_2 = (capcount_now - prevY_Count)/ECAP_TICKS_PER_US;
            break;

        case 3:
            Pulse_10(FilledPulse);
            STOP_TIMER1;
            LOAD_TIMER1(count30_SP2);
            START_TIMER1;
            bp_count++;
            yph30_3 = (capcount_now - prevY_Count)/ECAP_TICKS_PER_US;
            break;

        case 4:
            Pulse_11(FilledPulse);
            STOP_TIMER1;
            LOAD_TIMER1(count30_SP1);
            START_TIMER1;
            bp_count++;
            yph30_4 = (capcount_now - prevY_Count)/ECAP_TICKS_PER_US;
            break;

        case 5:
            Pulse_12(FilledPulse);
            STOP_TIMER1;
            bp_count++;
            yph30_5 = (capcount_now - prevY_Count)/ECAP_TICKS_PER_US;
            break;
    }
}
else
{
    DISABLE_PWM();
}
    prevY_Count = capcount_now;
    CpuTimer1Regs.TCR.bit.TIF = 1;
    // Timer1 is a CPU-level interrupt (INT13), not routed through the PIE,
}


inline void    Pulse_01(int x)
        {

            if(x)
            {
                BP1_OFF;
                RP1_ON;
                RP1 = PON; BP1 = 0;
            }
            else
            {
                BP2_OFF_NOFILL;
                RP1_ON_NOFILL;
                RP1 = PON; BP2 = 0;
            }
         }

inline void    Pulse_02(int x)
        {
            if(x)
            {
                BP2_OFF;
                RP2_ON;
                RP2 = PON; BP2 = 0;
            }
            else
            {
                YN1_OFF_NOFILL;
                RP2_ON_NOFILL;
                RP2 = PON; YN1 = 0;
            }
         }

inline void    Pulse_03(int x)
        {

            if(x)
            {
               YN1_OFF;
               BN1_ON;
               BN1 = PON; YN1 = 0;
            }
            else
            {
                YN2_OFF_NOFILL;
                BN1_ON_NOFILL;
                BN1 = PON; YN2 = 0;
            }
         }

inline void    Pulse_04(int x)
        {
            if(x)
            {
               YN2_OFF;
               BN2_ON;
               BN2 = PON; YN2 = 0;
            }
            else
            {
                RP1_OFF_NOFILL;
                BN2_ON_NOFILL;
                BN2 = PON; RP1 = 0;
            }
         }
inline void    Pulse_05(int x)
        {
            if(x)
            {
                RP1_OFF;
                YP1_ON;
                YP1 = PON; RP1 = 0;
            }
            else
            {
                RP2_OFF_NOFILL;
                YP1_ON_NOFILL;
                YP1 = PON; RP2 = 0;
            }
         }


inline void    Pulse_06(int x)
        {
            if(x)
            {
                RP2_OFF;
                YP2_ON;
                YP2 = PON; RP2 = 0;
            }
            else
            {
                BN1_OFF_NOFILL;
                YP2_ON_NOFILL;
                YP2 = PON; BN1 = 0;
            }
         }

inline void    Pulse_07(int x)
        {
            if(x)
            {
                BN1_OFF;
                RN1_ON;
                RN1 = PON;BN1 = 0;
            }
            else
            {
                BN2_OFF_NOFILL;
                RN1_ON_NOFILL;
                RN1 = PON;BN2 = 0;
            }
         }
inline void    Pulse_08(int x)
         {

             if(x)
             {
                 BN2_OFF;
                 RN2_ON;
                 BN2 = 0;RN2 = PON;
             }
             else
             {
                 YP1_OFF_NOFILL;
                 RN2_ON_NOFILL;
                 RN2 = PON;YP1 = 0;
             }
          }

inline void    Pulse_09(int x)
         {
             if(x)
             {
                   YP1_OFF;
                   BP1_ON;
                   YP1 = 0;BP1 = PON;
             }
             else
             {
                 YP2_OFF_NOFILL;
                 BP1_ON_NOFILL;
                 BP1 = PON;YP2 = 0;
             }
          }

inline void    Pulse_10(int x)
         {

             if(x)
             {
                YP2_OFF;
                BP2_ON;
                YP2 = 0;BP2 = PON;
             }
             else
             {
                 RN1_OFF_NOFILL;
                 BP2_ON_NOFILL;
                 BP2 = PON;RN1 = 0;
             }
          }

inline void    Pulse_11(int x)
         {

             if(x)
             {
                   RN1_OFF;
                   YN1_ON;
                   RN1 = 0;YN1 = PON;
             }
             else
             {

                 RN2_OFF_NOFILL;
                 YN1_ON_NOFILL;
                 RN2 = 0;YN1 = PON;
             }
          }
inline void    Pulse_12(int x)
         {

             if(x)
             {
                   RN2_OFF;
                   YN2_ON;
                   RN2 = 0;YN2 = PON;
             }
             else
             {
                 //RN2_OFF_NOFILL;
                 BP1_OFF_NOFILL;
                 YN2_ON_NOFILL;
                 BP1 = 0;YN2 = PON;
             }
          }



