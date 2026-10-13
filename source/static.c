/*
 * static.c
 *
 *  Created on: 24-Nov-2024
 *      Author: VCA
 */

#define CCEXTERN    extern
#define INIT0
#define INITARRAY0
#include "f28x_project.h"
#include "../ccVariables.h"

volatile N16 LoadOnBypassTimer = 0;

#ifdef _FLASH
    #pragma CODE_SECTION(StaticSwitchControl_Slave, ".TI.ramfunc");
    #pragma CODE_SECTION(StaticSwitchControl_Master, ".TI.ramfunc");
#endif


#define     INVFAIL (inverterFailed || !LimitFlag.bit.INVRTROK || SystemFlag.bit.INVRTRDISABL)

inline void    NormalOperation(void)
{
    if(SystemFlag.bit.LOADONINV)
            {
                if(inverterFailed || !LimitFlag.bit.INVRTROK || SystemFlag.bit.INVRTRDISABL)
                    {
                        xfrCommand = 0;
                        if(LimitFlag.bit.BYPASSOK)  xfrCommand = 2;
                    }
            }

    if(SystemFlag.bit.LOADONBYP)
            {
                if(!LimitFlag.bit.BYPASSOK)  xfrCommand = 0;
                if(LimitFlag.bit.INVRTROK && !SystemFlag.bit.INVRTRDISABL) xfrCommand = 1;
             }

    if(LimitFlag.bit.INVRTROK  && !SystemFlag.bit.INVRTRDISABL && (Synced || !LimitFlag.bit.BYPASSOK) ) xfrCommand = 1;
    else if (inverterFailed || SystemFlag.bit.INVRTRDISABL && LimitFlag.bit.BYPASSOK) xfrCommand = 2;
    if(!LimitFlag.bit.INVRTROK && !LimitFlag.bit.BYPASSOK) xfrCommand = 0;
}

void StaticSwitchControl_Slave(void)
{
    xfr2InvTimer = 0;
    xfr2BypTimer = 0;

            if(PairStatusFlag.bit.LOADONBYP) LoadOnBypassTimer = 20;
            if(LoadOnBypassTimer) LoadOnBypassTimer--;

           if(PairStatusFlag.bit.BITSVALID)
            {
                if(SystemFlag.bit.LOADONINV)
                {
                   if INVFAIL
                       {
                           xfrCommand = 0;
                           if(LimitFlag.bit.BYPASSOK && (PairStatusFlag.bit.LOADONBYP || !PairStatusFlag.bit.INVOK || PairStatusFlag.bit.INVRTRDISABL)) xfrCommand = 2;
                       }
                }
                else  if(SystemFlag.bit.LOADONBYP)
                {
                    if(PairStatusFlag.bit.INVOK && !PairStatusFlag.bit.INVRTRDISABL) UPSFlag.bit.WAITMASTER = TRUE;//waiting to Master to transfer
                    if(!LimitFlag.bit.BYPASSOK || !PairStatusFlag.bit.LOADONBYP)
                        {
                            xfrCommand = 0;
                            if(LimitFlag.bit.INVRTROK && !SystemFlag.bit.INVRTRDISABL && (PairStatusFlag.bit.LOADONINV ||
                                    (!PairStatusFlag.bit.INVOK && !PairStatusFlag.bit.LOADONBYP) )) xfrCommand = 1;
                        }

                }

//---------------------------------------------------------------------------------------------------
                    if(LimitFlag.bit.BYPASSOK && (PairStatusFlag.bit.LOADONBYP || !CommStatus.bit.CAN_MASTER ||
                            PairStatusFlag.bit.OUTMCBOFF || PairStatusFlag.bit.INVRTRDISABL ) && !UPSFlag.bit.WAITMASTER) xfrCommand = 2;

                    if(LimitFlag.bit.INVRTROK && !SystemFlag.bit.INVRTRDISABL && (PairStatusFlag.bit.LOADONINV || PairStatusFlag.bit.OUTMCBOFF ||
                            ( (!PairStatusFlag.bit.INVOK || PairStatusFlag.bit.INVRTRDISABL) && (LoadOnBypassTimer==0)) )) xfrCommand = 1;
//---------------------------------------------------------------------------------------------------
                if(!PairStatusFlag.bit.INVOK || PairStatusFlag.bit.INVRTRDISABL || !LimitFlag.bit.INVRTROK || !PairStatusFlag.bit.LOADONBYP) UPSFlag.bit.WAITMASTER = FALSE;
            } //COMM OK

            else
            { //No communication

                NormalOperation();
            }

    //****************************************************************************************
        //Actual Transfer:
        if((xfrCommand==0) || SystemFlag.bit.POWERINGUP || EmergencyStop || UPSFlag.bit.WAITMASTER || UPSFlag.bit.OUTMCBOFF || SystemFlag.bit.MANUALBYP)
            {
                DisableInvSS();
                DisableBypSS();
                SystemFlag.bit.LOADONBYP =  FALSE;
                SystemFlag.bit.LOADONINV =  FALSE;

            }

        else if((xfrCommand ==1) && (xfr2InvTimer==0) && !SystemFlag.bit.INVRTRDISABL)
            {
                DisableBypSS();
                EnableInvSS();
                SystemFlag.bit.LOADONINV =  TRUE;
                SystemFlag.bit.LOADONBYP =  FALSE;

            }

        else if((xfrCommand ==2) && (xfr2BypTimer==0))
            {
                DisableInvSS();
                EnableBypSS();
                SystemFlag.bit.LOADONBYP =  TRUE;
                SystemFlag.bit.LOADONINV =  FALSE;
            }
    }

void StaticSwitchControl_Master(void)
{
    CommStatus.bit.CAN_MASTER =  FALSE;

     if(SystemFlag.bit.LOADONINV)
            {
                if(inverterFailed || !LimitFlag.bit.INVRTROK || SystemFlag.bit.INVRTRDISABL)
                    {
                        xfrCommand = 0;
                        if(!PairStatusFlag.bit.LOADONINV && LimitFlag.bit.BYPASSOK)  xfrCommand = 2;
                    }
            }

     else if(SystemFlag.bit.LOADONBYP)
            {
                if(!LimitFlag.bit.BYPASSOK) xfrCommand = 0;
                if(LimitFlag.bit.INVRTROK)  xfrCommand = 1;
                if(PairStatusFlag.bit.INVOK && SystemFlag.bit.INVRTRDISABL && PairStatusFlag.bit.LOADONBYP) UPSFlag.bit.WAITMASTER = TRUE; //Wait Slave
             }

     if(LimitFlag.bit.INVRTROK  && !SystemFlag.bit.INVRTRDISABL && (Synced || !LimitFlag.bit.BYPASSOK)) xfrCommand = 1;
     else if (!PairStatusFlag.bit.LOADONINV && (inverterFailed || SystemFlag.bit.INVRTRDISABL || !LimitFlag.bit.INVRTROK) &&
             LimitFlag.bit.BYPASSOK && !UPSFlag.bit.WAITMASTER) xfrCommand = 2;

    if((!LimitFlag.bit.INVRTROK && !LimitFlag.bit.BYPASSOK) || UPSFlag.bit.WAITMASTER) xfrCommand = 0;

    if(!PairStatusFlag.bit.INVOK || PairStatusFlag.bit.INVRTRDISABL) UPSFlag.bit.WAITMASTER = FALSE;

  //  xfrCommand = newTransfer;
//****************************************************************************************
    xfr2InvTimer = 0;
    xfr2BypTimer = 0;
    //Actual Transfer:
    if((xfrCommand==0) || SystemFlag.bit.POWERINGUP || EmergencyStop || UPSFlag.bit.OUTMCBOFF || SystemFlag.bit.MANUALBYP)
        {
            DisableInvSS();
            DisableBypSS();
            SystemFlag.bit.LOADONBYP =  FALSE;
            SystemFlag.bit.LOADONINV =  FALSE;
        }

    else if((xfrCommand ==1) && (xfr2InvTimer==0))
        {
            DisableBypSS();
            EnableInvSS();
            SystemFlag.bit.LOADONINV =  TRUE;
            SystemFlag.bit.LOADONBYP =  FALSE;

        }

    else if((xfrCommand == 2) && (xfr2BypTimer==0))
        {
            DisableInvSS();
            EnableBypSS();
            SystemFlag.bit.LOADONBYP =  TRUE;
            SystemFlag.bit.LOADONINV =  FALSE;
        }
}


