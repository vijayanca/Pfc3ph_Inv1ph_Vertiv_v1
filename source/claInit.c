/*
 * claInit.c
 *
 * CLA1 set up for the inverter control loop (CONTROL_ON_CLA = 1), after
 * C2000Ware driverlib/f28p55x/examples/cla/cla_asin_ls8_9.c:
 *   LS8 + LS9  CLA program (Cla1Prog, copied from flash)
 *   LS5        CLA data: scratchpad, .bss_cla, .const_cla
 *   LS6 + LS7  CLA data: Cla1DataRam, the CPU/CLA shared variables (claShared.h)
 *   Task 1 <- ADCA INT1 (EPWM8 SOCA, 20 kHz), end of task -> CPU ClaTask1EndISR
 *   Task 2 <- EPWM1 INT (inverter PWM, counter zero)
 *   Task 8 <- software, run once here (ctl_init)
 * Called from main() after setupADC() and the PIE set up, before EINT.
 */

#define CCEXTERN    extern
#define INIT0
#define INITARRAY0
#include "f28x_project.h"
#include "../ccVariables.h"
#include "driverlib.h"
#include <string.h>

#if CONTROL_ON_CLA

extern uint32_t Cla1ProgRunStart, Cla1ProgLoadStart, Cla1ProgLoadSize;
extern uint32_t Cla1ConstRunStart, Cla1ConstLoadStart, Cla1ConstLoadSize;

extern __attribute__((interrupt)) void Cla1Task1(void);
extern __attribute__((interrupt)) void Cla1Task2(void);
extern __attribute__((interrupt)) void Cla1Task3(void);
extern __attribute__((interrupt)) void Cla1Task4(void);
extern __attribute__((interrupt)) void Cla1Task5(void);
extern __attribute__((interrupt)) void Cla1Task6(void);
extern __attribute__((interrupt)) void Cla1Task7(void);
extern __attribute__((interrupt)) void Cla1Task8(void);

// 1 when the CLA sees the same CTL_SHARED size as the CPU (claShared.h layout)
volatile uint16_t claLayoutOK = 0;

static void claConfigMemory(void)
{
    EALLOW;

#ifdef _FLASH
    // CLA program to the CPU address of LS8/LS9 (CLA 0x4000 = CPU 0x14000)
    memcpy((uint32_t *)((uint32_t)&Cla1ProgRunStart + (0x10000U)), (uint32_t *)&Cla1ProgLoadStart,
           (uint32_t)&Cla1ProgLoadSize);
    memcpy((uint32_t *)&Cla1ConstRunStart, (uint32_t *)&Cla1ConstLoadStart,
           (uint32_t)&Cla1ConstLoadSize);
#endif

    MemCfg_initSections(MEMCFG_SECT_MSGCLA1TOCPU);
    while (!MemCfg_getInitStatus(MEMCFG_SECT_MSGCLA1TOCPU)){};
    MemCfg_initSections(MEMCFG_SECT_MSGCPUTOCLA1);
    while (!MemCfg_getInitStatus(MEMCFG_SECT_MSGCPUTOCLA1)){};

    MemCfg_setLSRAMControllerSel(MEMCFG_SECT_LS8, MEMCFG_LSRAMCONTROLLER_CPU_CLA1);
    MemCfg_setCLAMemType(MEMCFG_SECT_LS8, MEMCFG_CLA_MEM_PROGRAM);
    MemCfg_setLSRAMControllerSel(MEMCFG_SECT_LS9, MEMCFG_LSRAMCONTROLLER_CPU_CLA1);
    MemCfg_setCLAMemType(MEMCFG_SECT_LS9, MEMCFG_CLA_MEM_PROGRAM);

    // data blocks: contents are kept (cinit has already set Cla1DataRam)
    MemCfg_setLSRAMControllerSel(MEMCFG_SECT_LS5, MEMCFG_LSRAMCONTROLLER_CPU_CLA1);
    MemCfg_setCLAMemType(MEMCFG_SECT_LS5, MEMCFG_CLA_MEM_DATA);
    MemCfg_setLSRAMControllerSel(MEMCFG_SECT_LS6, MEMCFG_LSRAMCONTROLLER_CPU_CLA1);
    MemCfg_setCLAMemType(MEMCFG_SECT_LS6, MEMCFG_CLA_MEM_DATA);
    MemCfg_setLSRAMControllerSel(MEMCFG_SECT_LS7, MEMCFG_LSRAMCONTROLLER_CPU_CLA1);
    MemCfg_setCLAMemType(MEMCFG_SECT_LS7, MEMCFG_CLA_MEM_DATA);

    EDIS;
}

void claInit(void)
{
    claConfigMemory();

    EALLOW;
#pragma diag_suppress=770
    CLA_mapTaskVector(CLA1_BASE, CLA_MVECT_1, (uint16_t)&Cla1Task1);
    CLA_mapTaskVector(CLA1_BASE, CLA_MVECT_2, (uint16_t)&Cla1Task2);
    CLA_mapTaskVector(CLA1_BASE, CLA_MVECT_3, (uint16_t)&Cla1Task3);
    CLA_mapTaskVector(CLA1_BASE, CLA_MVECT_4, (uint16_t)&Cla1Task4);
    CLA_mapTaskVector(CLA1_BASE, CLA_MVECT_5, (uint16_t)&Cla1Task5);
    CLA_mapTaskVector(CLA1_BASE, CLA_MVECT_6, (uint16_t)&Cla1Task6);
    CLA_mapTaskVector(CLA1_BASE, CLA_MVECT_7, (uint16_t)&Cla1Task7);
    CLA_mapTaskVector(CLA1_BASE, CLA_MVECT_8, (uint16_t)&Cla1Task8);
#pragma diag_warning=770

    CLA_enableIACK(CLA1_BASE);
    CLA_enableTasks(CLA1_BASE, CLA_TASKFLAG_1 | CLA_TASKFLAG_2 | CLA_TASKFLAG_8);

    // one-time state set up on the CLA, before the hardware triggers are connected
    ctl.claLayout = 0;
    CLA_setTriggerSource(CLA_TASK_8, CLA_TRIGGER_SOFTWARE);
    CLA_forceTasks(CLA1_BASE, CLA_TASKFLAG_8);
    while (CLA_getTaskRunStatus(CLA1_BASE, CLA_TASK_8) ||
           (CLA_getPendingTaskFlag(CLA1_BASE, CLA_TASK_8))) {};
    claLayoutOK = (ctl.claLayout == (uint16_t)sizeof(CTL_SHARED));

    CLA_setTriggerSource(CLA_TASK_1, CLA_TRIGGER_ADCA1);
    CLA_setTriggerSource(CLA_TASK_2, CLA_TRIGGER_EPWM1INT);

    // (the driverlib calls above end with EDIS)
    EALLOW;
    // ADCA INT1 continuous: the CLA task does not clear the ADC flag
    AdcaRegs.ADCINTSEL1N2.bit.INT1CONT = 1;
    AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;
    AdcaRegs.ADCINTOVFCLR.bit.ADCINT1 = 1;
    EDIS;

    // a flag left set before the trigger was connected would block every
    // further EPWM1 INT (task 2 clears it from now on)
    EPwm1Regs.ETCLR.bit.INT = 1;
}

#endif
