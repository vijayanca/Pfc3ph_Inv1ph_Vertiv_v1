/*
 * board_pin_map.h
 *
 * Common Controller board - pin map
 * Schematic : Common_Board_3ph_Inv_pfc_rect12p_v2 (02-Sep-2026)
 * MCU       : TMS320F28P550SG9PZR (100-pin LQFP, PZ)
 *
 * Named board_pin_map.h so it does not clash with TI's own driverlib
 * "pin_map.h" (which holds the GPIO_x_<FUNC> mux macros used below).
 *
 * Conventions
 *   xxx_GPIO      : GPIO number (for GPIO_writePin / GPIO_readPin etc.)
 *   xxx_PIN_CFG   : TI driverlib mux macro for GPIO_setPinConfig()
 *   xxx_ADC_BASE  : ADC module base for that signal (default choice)
 *   xxx_ADC_CH    : ADC input channel number -> cast to (ADC_Channel)
 *   Pin numbers in comments are 100-PZ package pins.
 *
 * NOTE: If a xxx_PIN_CFG macro name does not compile, grep your
 * C2000Ware f28p55x driverlib/pin_map.h for the GPIO number - newer
 * releases use PICO/POCI/PTE instead of SIMO/SOMI/STE.
 */

#ifndef BOARD_PIN_MAP_H
#define BOARD_PIN_MAP_H

#include "driverlib.h"      /* pulls in TI pin_map.h, gpio.h, adc.h */

/* ======================================================================
 * PWM outputs (via SN74AC573 latches U30 / U33 to gate-drive headers)
 * ====================================================================== */
/* Bridge 1 -> U30 -> CON15 */
#define PWM_RTOP_GPIO           0U      /* pin 79  PWM1  EPWM1A */
#define PWM_RTOP_PIN_CFG        GPIO_0_EPWM1_A
#define PWM_RBOT_GPIO           1U      /* pin 78  PWM2  EPWM1B */
#define PWM_RBOT_PIN_CFG        GPIO_1_EPWM1_B
#define PWM_YTOP_GPIO           2U      /* pin 77  PWM3  EPWM2A */
#define PWM_YTOP_PIN_CFG        GPIO_2_EPWM2_A
#define PWM_YBOT_GPIO           3U      /* pin 76  PWM4  EPWM2B */
#define PWM_YBOT_PIN_CFG        GPIO_3_EPWM2_B
#define PWM_BTOP_GPIO           4U      /* pin 75  PWM5  EPWM3A */
#define PWM_BTOP_PIN_CFG        GPIO_4_EPWM3_A
#define PWM_BBOT_GPIO           5U      /* pin 89  PWM6  EPWM3B */
#define PWM_BBOT_PIN_CFG        GPIO_5_EPWM3_B

#define PWM_BR1_R_BASE          EPWM1_BASE
#define PWM_BR1_Y_BASE          EPWM2_BASE
#define PWM_BR1_B_BASE          EPWM3_BASE

/* Bridge 2 (primed signals) -> U33 -> CON19 */
#define PWM2_RTOP_GPIO          6U      /* pin 97  PWM7  EPWM4A */
#define PWM2_RTOP_PIN_CFG       GPIO_6_EPWM4_A
#define PWM2_RBOT_GPIO          7U      /* pin 84  PWM8  EPWM4B */
#define PWM2_RBOT_PIN_CFG       GPIO_7_EPWM4_B
#define PWM2_YTOP_GPIO          8U      /* pin 74  PWM9  EPWM5A */
#define PWM2_YTOP_PIN_CFG       GPIO_8_EPWM5_A
#define PWM2_YBOT_GPIO          9U      /* pin 90  PWM10 EPWM5B */
#define PWM2_YBOT_PIN_CFG       GPIO_9_EPWM5_B
#define PWM2_BTOP_GPIO          10U     /* pin 93  PWM11 EPWM6A */
#define PWM2_BTOP_PIN_CFG       GPIO_10_EPWM6_A
#define PWM2_BBOT_GPIO          11U     /* pin 52  PWM12 EPWM6B */
#define PWM2_BBOT_PIN_CFG       GPIO_11_EPWM6_B

#define PWM_BR2_R_BASE          EPWM4_BASE
#define PWM_BR2_Y_BASE          EPWM5_BASE
#define PWM_BR2_B_BASE          EPWM6_BASE

/* Static switch status / drive -> U33 Q6/Q7 -> CON24 */
#define STAT_INV_GPIO           12U     /* pin 51  PWM13 EPWM7A */
#define STAT_INV_PIN_CFG        GPIO_12_EPWM7_A
#define STAT_BYP_GPIO           13U     /* pin 50  PWM14 EPWM7B */
#define STAT_BYP_PIN_CFG        GPIO_13_EPWM7_B
#define PWM_STAT_BASE           EPWM7_BASE

/* Gate-driver fault inputs */
#define VCESAT1_GPIO            54U     /* pin 13  bridge 1 driver fault (CON15) */
#define VCESAT2_GPIO            55U     /* pin 43  bridge 2 driver fault (CON19) */

/* ======================================================================
 * Relay outputs (GPIO -> ULN2003 U26/U27 -> RLY_01..RLY_12)
 * Active high: GPIO = 1 -> relay ON
 * RLY3 (GPIO24) and RLY7 (GPIO32) are boot-mode pins with 3K3 pull-ups
 * (R214/R217) - see note at end of file.
 * ====================================================================== */
#define RLY1_GPIO               60U     /* pin 44 */
#define RLY2_GPIO               33U     /* pin 53 */
#define RLY3_GPIO               24U     /* pin 56  boot pin, pulled up */
#define RLY4_GPIO               25U     /* pin 57 */
#define RLY5_GPIO               26U     /* pin 58 */
#define RLY6_GPIO               27U     /* pin 59 */
#define RLY7_GPIO               32U     /* pin 64  boot pin, pulled up */
#define RLY8_GPIO               58U     /* pin 67 */
#define RLY9_GPIO               22U     /* pin 83 */
#define RLY10_GPIO              44U     /* pin 85 */
#define RLY11_GPIO              61U     /* pin 91  also drives K1/K2 analog mux (RLY_11) */
#define RLY12_GPIO              59U     /* pin 92  also drives K1/K2 analog mux (RLY_12) */

#define RELAY_COUNT             12U

/* ======================================================================
 * Digital inputs (PC817 opto, DI01..DI10)
 * Check polarity on the bench: opto ON pulls the GPIO side low/high
 * depending on the transistor stage.
 * ====================================================================== */
#define DI1_GPIO                247U    /* pin 42  AIO (C14) - digital input only */
#define DI2_GPIO                20U     /* pin 48  AGPIO (A17/B17/C17) */
#define DI3_GPIO                21U     /* pin 49  AGPIO (A18/B18/C18) */
#define DI4_GPIO                63U     /* pin 47 */
#define DI5_GPIO                48U     /* pin 7  */
#define DI6_GPIO                49U     /* pin 8  */
#define DI7_GPIO                50U     /* pin 9  */
#define DI8_GPIO                51U     /* pin 10 */
#define DI9_GPIO                52U     /* pin 11 */
#define DI10_GPIO               53U     /* pin 12 */

#define DI_COUNT                10U

/* Mains zero-cross detect */
#define ZCD_R_GPIO              47U     /* pin 6  - route via INPUTXBAR for eCAP/XINT */

/* ======================================================================
 * Misc
 * ====================================================================== */
#define SYSLED_GPIO             62U     /* pin 46  LED5 yellow, active low (LED to +3.3V) */

/* ======================================================================
 * RS485 #1 - SCIB -> U28 ISO3082
 * ====================================================================== */
#define RS485_1_SCI_BASE        SCIB_BASE
#define RS485_1_TX_GPIO         14U     /* pin 96  TXD1 */
#define RS485_1_TX_PIN_CFG      GPIO_14_SCIB_TX
#define RS485_1_RX_GPIO         15U     /* pin 95  RXD1 */
#define RS485_1_RX_PIN_CFG      GPIO_15_SCIB_RX
#define RS485_1_TXEN_GPIO       40U     /* pin 80  TXEN1 (DE + /RE), 1 = transmit */

/* ======================================================================
 * RS485 #2 - SCIA -> U31 ISO3082
 * ====================================================================== */
#define RS485_2_SCI_BASE        SCIA_BASE
#define RS485_2_TX_GPIO         29U     /* pin 100 TXD */
#define RS485_2_TX_PIN_CFG      GPIO_29_SCIA_TX
#define RS485_2_RX_GPIO         28U     /* pin 1   RXD */
#define RS485_2_RX_PIN_CFG      GPIO_28_SCIA_RX
#define RS485_2_TXEN_GPIO       34U     /* pin 94  TXEN (DE + /RE), 1 = transmit */

/* ======================================================================
 * CAN (MCAN, CAN-FD capable) -> ISO1042 U32 / U34
 * ====================================================================== */
#define CAN_A_BASE              MCANA_DRIVER_BASE
#define CAN_A_RX_GPIO           30U     /* pin 98  MCANA_RX (mux 10) */
#define CAN_A_RX_PIN_CFG        GPIO_30_MCANA_RX
#define CAN_A_TX_GPIO           31U     /* pin 99  MCANA_TX (mux 10) */
#define CAN_A_TX_PIN_CFG        GPIO_31_MCANA_TX

#define CAN_B_BASE              MCANB_DRIVER_BASE
#define CAN_B_RX_GPIO           18U     /* pin 68  MCANB_RX (mux 3) - shares X2 */
#define CAN_B_RX_PIN_CFG        GPIO_18_MCANB_RX
#define CAN_B_TX_GPIO           19U     /* pin 69  MCANB_TX (mux 3) - shares X1 */
#define CAN_B_TX_PIN_CFG        GPIO_19_MCANB_TX

/* ======================================================================
 * SPI-A -> 25LC512 EEPROM (U38)
 * Chip select driven as plain GPIO so it stays low for a full
 * multi-byte EEPROM command.
 * ====================================================================== */
#define EEPROM_SPI_BASE         SPIA_BASE
#define EEPROM_SIMO_GPIO        16U     /* pin 54 */
#define EEPROM_SIMO_PIN_CFG     GPIO_16_SPIA_PICO
#define EEPROM_SOMI_GPIO        17U     /* pin 55 */
#define EEPROM_SOMI_PIN_CFG     GPIO_17_SPIA_POCI
#define EEPROM_CLK_GPIO         56U     /* pin 65 */
#define EEPROM_CLK_PIN_CFG      GPIO_56_SPIA_CLK
#define EEPROM_CS_GPIO          57U     /* pin 66  EPSEL, active low */

/* ======================================================================
 * USB (not used yet - needs external clock on X1/X2)
 * ====================================================================== */
#define USB_DM_GPIO             23U     /* pin 81  USB0DM (analog ALT function) */
#define USB_DP_GPIO             41U     /* pin 82  USB0DP (analog ALT function) */

/* ======================================================================
 * ADC inputs
 * All channels referenced to external 3.0 V (TL431) on VREFHI.
 * Default ADC choices spread each 3-phase group over different ADCs so
 * the phases can be sampled simultaneously. Alternatives listed in
 * comments - change BASE/CH together.
 * ====================================================================== */

/* --- Inverter voltages ------------------------------------------------ */
#define VR_INV_ADC_BASE         ADCD_BASE   /* ADC_01 pin 14: A6 / D14 / E14 */
#define VR_INV_ADC_CH           14U
#define VY_INV_ADC_BASE         ADCE_BASE   /* ADC_02 pin 15: B2 / C6 / E12 */
#define VY_INV_ADC_CH           12U
#define VB_INV_ADC_BASE         ADCB_BASE   /* ADC_03 pin 16: B3 */
#define VB_INV_ADC_CH           3U

/* --- Bypass / mains voltages ----------------------------------------- */
#define VR_BYP_ADC_BASE         ADCC_BASE   /* ADC_04 pin 17: A2 / B6 / C9 */
#define VR_BYP_ADC_CH           9U
#define VY_BYP_ADC_BASE         ADCB_BASE   /* ADC_05 pin 18: A3 / B9 / C7 */
#define VY_BYP_ADC_CH           9U
#define VB_BYP_VLOAD_ADC_BASE   ADCA_BASE   /* ADC_06 pin 19: A14 / B14 / C4 */
#define VB_BYP_VLOAD_ADC_CH     14U         /* Vb_byp or Vload, selected by RLY11/RLY12 */

/* --- DC link / battery ------------------------------------------------ */
#define VDCLINK_ADC_BASE        ADCC_BASE   /* ADC_07 pin 20: A11 / B10 / C0 */
#define VDCLINK_ADC_CH          0U
#define VBAT_ADC_BASE           ADCC_BASE   /* ADC_08 pin 21: B12 / C2 */
#define VBAT_ADC_CH             2U
#define IBAT_ADC_BASE           ADCD_BASE   /* ADC_09 pin 22: A1 / B7 / D11 */
#define IBAT_ADC_CH             11U

/* --- Inverter inductor currents -------------------------------------- */
#define IR_IND_ADC_BASE         ADCA_BASE   /* ADC_10 pin 23: A0 / B15 / C15 (also DACA_OUT) */
#define IR_IND_ADC_CH           0U
#define IY_IND_ADC_BASE         ADCC_BASE   /* ADC_11 pin 28: A12 / C5 */
#define IY_IND_ADC_CH           5U
#define IB_IND_ADC_BASE         ADCE_BASE   /* ADC_12 pin 29: C1 / E11  (schematic label wrong, PCB OK) */
#define IB_IND_ADC_CH           11U

/* --- Output voltages -------------------------------------------------- */
#define VR_OUT_ADC_BASE         ADCE_BASE   /* ADC_13 pin 30: B11 / D16 / E16 */
#define VR_OUT_ADC_CH           16U
#define VY_OUT_ADC_BASE         ADCD_BASE   /* ADC_14 pin 31: A7 / B30 / C3 / D12 / E30 */
#define VY_OUT_ADC_CH           12U
#define VB_OUT_ADC_BASE         ADCD_BASE   /* ADC_15 pin 32: B5 / D15 / E15 */
#define VB_OUT_ADC_CH           15U

/* --- Output currents -------------------------------------------------- */
#define IR_OUT_ADC_BASE         ADCA_BASE   /* ADC_16 pin 35: A5 only */
#define IR_OUT_ADC_CH           5U
#define IY_OUT_ADC_BASE         ADCB_BASE   /* ADC_17 pin 36: A4 / B8 */
#define IY_OUT_ADC_CH           8U
#define IB_OUT_ADC_BASE         ADCA_BASE   /* ADC_18 pin 37: A8 only (same ADC as Ir_out) */
#define IB_OUT_ADC_CH           8U

/* --- Temperatures ----------------------------------------------------- */
#define TEMP1_ADC_BASE          ADCA_BASE   /* ADC_19 pin 38: A9 */
#define TEMP1_ADC_CH            9U
#define TEMP2_ADC_BASE          ADCC_BASE   /* ADC_20 pin 39: B4 / C8 */
#define TEMP2_ADC_CH            8U
#define TEMP3_ADC_BASE          ADCB_BASE   /* ADC_21 pin 40: A10 / B1 / C10 */
#define TEMP3_ADC_CH            1U

/* --- Offset reference (1.5 V mid-rail for bipolar channels) ----------- */
#define VREF1P5_ADC_BASE        ADCB_BASE   /* pin 41: B0 / C11 */
#define VREF1P5_ADC_CH          0U

/* ======================================================================
 * Usage example
 *
 *   GPIO_setPinConfig(PWM_RTOP_PIN_CFG);
 *
 *   GPIO_setPinConfig(GPIO_60_GPIO60);          // RLY1
 *   GPIO_setDirectionMode(RLY1_GPIO, GPIO_DIR_MODE_OUT);
 *   GPIO_writePin(RLY1_GPIO, 0U);
 *
 *   // AGPIO / AIO digital inputs (DI1..DI3) need analog mode off:
 *   GPIO_setAnalogMode(DI2_GPIO, GPIO_ANALOG_DISABLED);
 *
 *   ADC_setupSOC(VR_INV_ADC_BASE, ADC_SOC_NUMBER0, ADC_TRIGGER_EPWM1_SOCA,
 *                (ADC_Channel)VR_INV_ADC_CH, 10U);
 *
 * ======================================================================
 * Schematic notes (v2)
 *  1. ADC_12 (pin 29) = Ib_ind. PCB is correct; schematic v2 label
 *     reads "Iy_ind" and D16/D17 clamp nets are not linked - fix in
 *     schematic only.
 *  2. GPIO24 (RLY3) and GPIO32 (RLY7) have 3K3 pull-ups for boot mode.
 *     The pull-up drives the ULN2003 input high during reset / while
 *     halted in the debugger, so those two relays may energise until
 *     firmware takes the pins over.
 *  3. GPIO18/19 are used for MCANB, so X1/X2 are unavailable for a
 *     crystal. If an external clock is needed, MCANB can move to
 *     GPIO33 (RX) / GPIO32 (TX), both mux 10 - currently RLY2 / RLY7.
 *  4. U30/U33 changed to SN74ACT573 (3.3 V PWM into 5 V latch).
 *     ACT inputs are TTL level (VIH 2.0 V), OK from 3.3 V.
 *     Outputs: 24 mA rated per pin, 200 mA total per package.
 * ====================================================================== */

#endif /* BOARD_PIN_MAP_H */
