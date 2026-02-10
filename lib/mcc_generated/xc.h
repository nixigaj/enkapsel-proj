#ifndef XC_H
#define XC_H

// --- 1. Global Configuration ---
#ifndef USB_EP_NUM
#define USB_EP_NUM 8
#endif

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

/* Fix for XC8 'nop' macro */
#ifndef _nop
#define _nop() __asm__ __volatile__ ("nop")
#endif

// --- 2. CLOCK Control Mappings ---
#ifndef CLKCTRL_PDIV_DIV2_gc
#define CLKCTRL_PDIV_DIV2_gc CLKCTRL_PDIV_2X_gc
#endif

#define CLKCTRL_AUTOTUNE_OFF_gc (0x00)
#define CLKCTRL_FRQSEL_24M_gc   CLKCTRL_FREQSEL_24M_gc
#define CLKCTRL_ALGSEL_BIN_gc   (0x00)
#define CLKCTRL_SEL_XTAL_gc     CLKCTRL_CLKSEL_EXTCLK_gc
#define CLKCTRL_SELHF_XTAL_gc   CLKCTRL_CLKSEL_EXTCLK_gc
#define CLKCTRL_CSUTHF_256_gc   (0x00)
#define CLKCTRL_FRQRANGE_8M_gc  CLKCTRL_FREQSEL_8M_gc
#define CLKCTRL_RUNSTBY_bp      CLKCTRL_RUNSTDBY_bp
#define CLKCTRL_CFDEN_bp        0
#define CLKCTRL_CFD_bp          0
#define CLKCTRL_CFDEN_bm        0
#define CLKCTRL_INTTYPE_INT_gc  0
#define CLKCTRL_CFDSRC_CLKMAIN_gc 0
#define CLKCTRL_CFDTST_bp       0
#define CLKCTRL_CFDSRC_t        uint8_t

// --- 3. System Configuration ---
#define SYSCFG_USBVREG_bp 0
#define SYSCFG_USBVREG_bm 0x01

// --- 4. USB Data Structures ---

typedef struct {
    register8_t CTRL;
    register8_t STATUS;
    union {
        struct {
            register8_t DATAL;
            register8_t DATAH;
        };
        register16_t DATAPTR;
    };
    union {
        struct {
            register8_t CNTL;
            register8_t CNTH;
        };
        register16_t CNT;
        register16_t MCNT;
    };
} USB_EP_t;

typedef struct {
    USB_EP_t OUT;
    USB_EP_t IN;
} USB_EP_PAIR_t;

// Renamed to USB_ENDPOINT_TABLE_t to match driver code
typedef struct {
    register8_t FIFO[USB_EP_NUM * 2];
    USB_EP_PAIR_t EP[USB_EP_NUM];
    register16_t FRAMENUM;
} USB_ENDPOINT_TABLE_t;

// Declare the external variable (Note: You must have commented out the
// conflicting 'extern' in usb_peripheral_avr_du.h for this to work)
extern USB_ENDPOINT_TABLE_t endpointTable;

// --- 5. USB Peripheral Registers ---

typedef struct {
    register8_t OUTCLR;
    register8_t OUTSET;
    register8_t INCLR;
    register8_t INSET;
} USB_EP_STATUS_t;

typedef struct {
    register8_t CTRLA;
    register8_t CTRLB;
    register8_t INTCTRLA;
    register8_t INTCTRLB;
    register8_t INTFLAGSA;
    register8_t INTFLAGSB;
    USB_EP_STATUS_t STATUS[8];
    register8_t ADDR;
    register8_t FADDR;
    register8_t FIFOWP;
    register8_t FIFORP;
    register8_t EPSTATUS;
    register8_t EPINTFLAG;
    register8_t EPINTCTRL;
    register8_t FRAMENUML;
    register8_t FRAMENUMH;
    register16_t EPPTR;
    register8_t BUSSTATE;
    register8_t reserved[0x1000];
} USB0_t;

#ifndef USB0
#define USB0 (*(volatile USB0_t *)(0x1000))
#endif

// --- 6. USB Bit Masks ---
#define USB_SOF_bm       0x01
#define USB_SUSPEND_bm   0x02
#define USB_RESUME_bm    0x04
#define USB_RESET_bm     0x08
#define USB_STALLED_bm   0x10
#define USB_UNF_bm       0x20
#define USB_OVF_bm       0x40

#define USB_TRNCOMPL_bm  0x01
#define USB_GNDONE_bm    0x02
#define USB_SETUP_bm     0x04
#define USB_RMWBUSY_bm   0x20
#define USB_BUSNAK_bm    0x80
#define USB_EPSETUP_bm   0x04
#define USB_GNAK_bm      0x80
#define USB_GNAUTO_bm    0x40
#define USB_ATTACH_bm    0x01
#define USB_URESUME_bm   0x02
#define USB_FIFOEN_bm    0x80
#define USB_ENABLE_bm    0x01
#define USB_SPEED_gm     0x04
#define USB_STFRNUM_bm   0x10

#define USB_FRAMENUM_gm  0x07FF

#define USB_UNFOVF_bm    (USB_UNF_bm | USB_OVF_bm)
#define USB_CRC_bm       0x04

#define USB_TCDSBL_bm    0x01
#define USB_DOSTALL_bm   0x10

#define USB_FIFORP_gm    0x0F
#define USB_FIFOWP_gm    0x0F
#define USB_EPNUM_gm     0x0F
#define USB_EPNUM_gp     0
#define USB_DIR_bm       0x80
#define USB_DIR_bp       7

#define USB_ADDR_gm      0x7F
#define USB_ADDR_gp      0
#define USB_MAXEP_gm     0xF0
#define USB_MAXEP_gp     4

#define USB_TYPE_gm      0x03
#define USB_TYPE_CONTROL_gc 0x00
#define USB_TYPE_ISOCHRONOUS_gc 0x01
#define USB_TYPE_BULK_gc 0x02
#define USB_TYPE_INTERRUPT_gc 0x03

#define USB_MULTIPKT_bm  0x04
#define USB_AZLP_bm      0x08
#define USB_STALL_bm     0x10
#define USB_BUSNACK_bm   0x20
#define USB_OVF_bm       0x40
#define USB_UNF_bm       0x20
#define USB_TOGGLE_bm    0x80

#define USB_BUFSIZE_DEFAULT_gm 0x70
#define USB_BUFSIZE_DEFAULT_gp 4
#define USB_BUFSIZE_ISO_gm     0x70
#define USB_BUFSIZE_ISO_BUF1023_gc 0x00

typedef uint8_t USB_TYPE_t;
#define USB_TYPE_ISO_gc     USB_TYPE_ISOCHRONOUS_gc
#define USB_TYPE_BULKINT_gc USB_TYPE_BULK_gc
#define USB_TYPE_DISABLE_gc 0x00

#define KEY_NOTACT_gc    (0x00)
#define LEVEL_BASIC_gc   (0x00)
#define UPDIPINCFG_UPDI_gc RSTPINCFG_UPDI_gc
#define USBSINK_ENABLE_gc (0x00)

#endif // XC_H
