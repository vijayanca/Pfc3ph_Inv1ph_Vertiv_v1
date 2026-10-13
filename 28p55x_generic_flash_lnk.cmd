MEMORY
{
   BEGIN            : origin = 0x080000, length = 0x000002  // Update the codestart location as needed

   BOOT_RSVD        : origin = 0x000002, length = 0x000126     /* Part of M0, BOOT rom will use this for stack */
   RAMM0            : origin = 0x000128, length = 0x0002D8
   RAMM1            : origin = 0x000400, length = 0x000400

   RAMLS03          : origin = 0x008000, length = 0x002000     // CPU: .bss
   RAMLS4           : origin = 0x00A000, length = 0x000800     // CPU: .data, .sysmem
   // LS5..LS7 are CLA data memory (claInit.c); the CPU keeps read/write access
   RAMLS5           : origin = 0x00A800, length = 0x000800     // CLA: scratchpad, .bss_cla, .const_cla
   RAMLS67          : origin = 0x00B000, length = 0x001000     // CLA + CPU: Cla1DataRam (claShared.h)

   // LS8 + LS9 are CLA program memory, seen by the CLA at 0x4000 (CPU 0x14000).
   // The inverter control loop runs there (cla_tasks.cla, controlCore.h).
   RAMLS8_9_CLA     : origin = 0x004000, length = 0x004000

   // GS0 + GS1 (contiguous) as one block for .TI.ramfunc (CPU ISRs run from RAM)
   RAMGS01          : origin = 0x00C000, length = 0x004000
   RAMGS2           : origin = 0x010000, length = 0x002000
   RAMGS3           : origin = 0x012000, length = 0x002000

   /* Flash Banks (128 sectors each) */
   FLASH_BANK0     : origin = 0x080002, length = 0x1FFFE
   FLASH_BANK12   : origin = 0x0A0000, length = 0x40000
   /*FLASH_BANK2     : origin = 0x0C0000, length = 0x20000 */
   FLASH_BANK3     : origin = 0x0E0000, length = 0x20000
   //FLASH_BANK4     : origin = 0x100000, length = 0x08000


   CLATOCPURAM      : origin = 0x001480,   length = 0x000080
   CPUTOCLARAM      : origin = 0x001500,   length = 0x000080
   CLATODMARAM      : origin = 0x001680,   length = 0x000080
   DMATOCLARAM      : origin = 0x001700,   length = 0x000080

   RESET            : origin = 0x3FFFC0, length = 0x000002
}


SECTIONS
{
   codestart        : > BEGIN
   .text            : > FLASH_BANK12, ALIGN(8)
   .cinit           : > FLASH_BANK0, ALIGN(8)
   .switch          : > FLASH_BANK0, ALIGN(8)
   .reset           : > RESET, TYPE = DSECT /* not used, */

   .stack           : > RAMM1
#if defined(__TI_EABI__)
   .bss             : > RAMLS03
   .bss:output      : > RAMLS03
   .init_array      : > FLASH_BANK0, ALIGN(8)
   .const           : > FLASH_BANK0, ALIGN(8)
   .data            : > RAMLS4
   .sysmem          : > RAMLS4
#else
   .pinit           : > FLASH_BANK0, ALIGN(8)
   .ebss            : >> RAMLS5 | RAMLS6
   .econst          : > FLASH_BANK0, ALIGN(8)
   .esysmem         : > RAMLS5
#endif

   ramgs2 : > RAMGS2

   #if defined(__TI_EABI__)
       .TI.ramfunc : {} LOAD = FLASH_BANK3,
                        RUN = RAMGS01,
                        LOAD_START(RamfuncsLoadStart),
                        LOAD_SIZE(RamfuncsLoadSize),
                        LOAD_END(RamfuncsLoadEnd),
                        RUN_START(RamfuncsRunStart),
                        RUN_SIZE(RamfuncsRunSize),
                        RUN_END(RamfuncsRunEnd),
                        ALIGN(8)
   #else
       .TI.ramfunc : {} LOAD = FLASH_BANK3,
                        RUN = RAMLS0,
                        LOAD_START(_RamfuncsLoadStart),
                        LOAD_SIZE(_RamfuncsLoadSize),
                        LOAD_END(_RamfuncsLoadEnd),
                        RUN_START(_RamfuncsRunStart),
                        RUN_SIZE(_RamfuncsRunSize),
                        RUN_END(_RamfuncsRunEnd),
                        ALIGN(8)
   #endif


BIG_ARRAYS          : > RAMGS3

/* CLA sections (C2000Ware 28p55x_cla_ls8_9_flash_lnk.cmd). Cla1Prog and
   .const_cla are copied from flash by claInit(); Cla1Prog to the CPU address
   of LS8/LS9 (Cla1ProgRunStart + 0x10000). */
#if defined(__TI_EABI__)
    Cla1Prog        : LOAD = FLASH_BANK3,
                      RUN = RAMLS8_9_CLA,
                      LOAD_START(Cla1ProgLoadStart),
                      RUN_START(Cla1ProgRunStart),
                      LOAD_SIZE(Cla1ProgLoadSize),
                      ALIGN(4)

   .const_cla       : LOAD = FLASH_BANK3,
                      RUN = RAMLS5,
                      RUN_START(Cla1ConstRunStart),
                      LOAD_START(Cla1ConstLoadStart),
                      LOAD_SIZE(Cla1ConstLoadSize),
                      ALIGN(4)
#endif

   Cla1ToCpuMsgRAM  : > CLATOCPURAM
   CpuToCla1MsgRAM  : > CPUTOCLARAM

   .scratchpad      : > RAMLS5
   .bss_cla         : > RAMLS5
   Cla1DataRam      : > RAMLS67
}

/*
//===========================================================================
// End of file.
//===========================================================================
*/
