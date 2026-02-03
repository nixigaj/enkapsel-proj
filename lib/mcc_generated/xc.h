#ifndef XC_H
#define XC_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <string.h>
#include <stdbool.h>

/* Fix for XC8 'nop' macro */
#ifndef _nop
#define _nop() __asm__ __volatile__ ("nop")
#endif

/* * ==========================================
 * AVR32DU28 Compatibility Patch for DA28
 * ==========================================
 */

// 1. Clock Control (CLKCTRL) Mappings
// The DU series adds "MCLKCTRLC" and new prescalers not found in DA.
// We map them to the closest valid registers or dummy values to satisfy compilation.

// PDIV_DIV2 exists in DA but might be named differently in the header.
// Try mapping it to the generic divide-by-2 mask if missing.
#ifndef CLKCTRL_PDIV_DIV2_gc
#define CLKCTRL_PDIV_DIV2_gc CLKCTRL_PDIV_2X_gc
#endif

// The DU has a Clock Failure Detection (CFD) feature in MCLKCTRLC.
// The DA28 struct *does not* have MCLKCTRLC.
// We must define a dummy register so the code compiles.
// (The writes will go nowhere, but that is fine for now as long as clock is stable).
typedef struct {
    uint8_t MCLKCTRLC; // Fake register
    uint8_t XOSCHFCTRLA;
    uint8_t MCLKINTCTRL;
    uint8_t MCLKINTFLAGS;
    uint8_t MCLKTIMEBASE;
} CLKCTRL_DUMMY_t;

// Macros to satisfy the "undeclared" errors
#define CLKCTRL_AUTOTUNE_OFF_gc (0x00) // Dummy
#define CLKCTRL_FRQSEL_24M_gc   CLKCTRL_FREQSEL_24M_gc
#define CLKCTRL_ALGSEL_BIN_gc   (0x00) // Dummy
#define CLKCTRL_SEL_XTAL_gc     CLKCTRL_CLKSEL_EXTCLK_gc
#define CLKCTRL_SELHF_XTAL_gc   CLKCTRL_CLKSEL_EXTCLK_gc
#define CLKCTRL_CSUTHF_256_gc   (0x00) // Dummy
#define CLKCTRL_FRQRANGE_8M_gc  CLKCTRL_FREQSEL_8M_gc
#define CLKCTRL_RUNSTBY_bp      CLKCTRL_RUNSTDBY_bp
#define CLKCTRL_CFDEN_bp        0
#define CLKCTRL_CFD_bp          0
#define CLKCTRL_CFDEN_bm        0
#define CLKCTRL_INTTYPE_INT_gc  0
#define CLKCTRL_CFDSRC_CLKMAIN_gc 0
#define CLKCTRL_CFDTST_bp       0
#define CLKCTRL_CFDSRC_t        uint8_t

// 2. Fuses & System Config
#define KEY_NOTACT_gc    (0x00)
#define LEVEL_BASIC_gc   (0x00)
#define UPDIPINCFG_UPDI_gc RSTPINCFG_UPDI_gc // DA uses RSTPINCFG
#define USBSINK_ENABLE_gc (0x00) // Feature not in DA header

#endif // XC_H
