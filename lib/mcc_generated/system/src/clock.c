/**
* CLKCTRL Generated Driver File - PATCHED for PlatformIO/AVR128DA28 target
 */

#include "../clock.h"
// Include your shim to pick up the patch definitions
#include "../../xc.h"

void CLOCK_Initialize(void)
{
    // 1. Set Main Clock to Internal High Frequency (OSCHF)
    ccp_write_io((void*)&(CLKCTRL.MCLKCTRLA),
        (0 << CLKCTRL_CLKOUT_bp) | CLKCTRL_CLKSEL_OSCHF_gc);

    // 2. Set Prescaler (Div by 2 to be safe, or disabled for full speed)
    // Note: MCC generated DIV2, but we usually want 24MHz direct.
    // If you want 24MHz CPU, disable the prescaler (PEN=0).
    ccp_write_io((void*)&(CLKCTRL.MCLKCTRLB),
        CLKCTRL_PDIV_DIV2_gc | (0 << CLKCTRL_PEN_bp));

    // 3. Configure the Internal High Freq Oscillator (24 MHz)
    ccp_write_io((void*)&(CLKCTRL.OSCHFCTRLA),
        CLKCTRL_AUTOTUNE_OFF_gc |
        CLKCTRL_FRQSEL_24M_gc   |
        (0 << CLKCTRL_RUNSTDBY_bp));

    // 4. Tune (Default 0)
    ccp_write_io((void*)&(CLKCTRL.OSCHFTUNE), 0x0);

    // --- DELETED: XOSC32KCTRLA (Not using external 32k crystal) ---
    // --- DELETED: MCLKCTRLC (CFD not supported on DA28 headers) ---
    // --- DELETED: MCLKINTCTRL (CFD Interrupts) ---
    // --- DELETED: XOSCHFCTRLA (Not using external HF crystal) ---
    // --- DELETED: MCLKTIMEBASE (Not in DA28 struct) ---

    // 5. Wait for clock to stabilize
    while(!(CLKCTRL.MCLKSTATUS & CLKCTRL_OSCHFS_bm))
    {
        ; // Wait
    }
}

// --- DELETED: CFD_Enable / CFD_Disable functions (Not supported) ---
