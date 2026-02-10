#define F_CPU 24000000UL
#define BAUD_RATE 115200

// Calculation for AVR-DU/DA
#define USART0_BAUD_VAL ((float)(4.0 * F_CPU / (BAUD_RATE)) + 0.5)

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
//#include <avr/cpufunc.h> // Required for ccp_write_io

// Include generated USB headers
#include "../lib/mcc_generated/system/system.h"
#include "../lib/mcc_generated/usb/usb_device.h"
#include "../lib/mcc_generated/usb/usb_common/usb_common_elements.h"

// --- CLOCK INIT (From your working reference) ---
void CLOCK_init(void) {
    ccp_write_io((void*)&CLKCTRL.OSCHFCTRLA, CLKCTRL_FRQSEL_24M_gc);
    ccp_write_io((void*)&CLKCTRL.MCLKCTRLB, 0x00);
}

void USART0_init(void) {
    PORTA.DIR |= PIN0_bm;       // PA0 -> TX
    PORTA.DIR &= ~PIN1_bm;      // PA1 -> RX
    USART0.BAUD = (uint16_t)USART0_BAUD_VAL;
    USART0.CTRLB |= USART_TXEN_bm | USART_RXEN_bm;
}

void USART0_sendChar(const char c) {
    while (!(USART0.STATUS & USART_DREIF_bm)) {}
    USART0.TXDATAL = c;
}

int USART0_printChar(const char c, FILE *stream) {
    USART0_sendChar(c);
    return 0;
}

FILE USART_stream = FDEV_SETUP_STREAM(USART0_printChar, NULL, _FDEV_SETUP_WRITE);

// Manual definition for VUSB Regulator Control (Address 0x0F0A on AVR-DU)
#define SYSCFG_VUSBCTRL_ADDR 0x0F0A

int main(void) {
    // 1. Initialize Clock & Serial FIRST
    CLOCK_init();
    USART0_init();
    stdout = &USART_stream;

    // Small delay to let the terminal catch up
    _delay_ms(800);
    printf("\n--- AVR32DU28 init USB ---\n");

    // 3. Force VUSB Regulator ON (Mandatory for USB)
    printf("Enabling VUSB Regulator...\n");
    *(volatile uint8_t *)(SYSCFG_VUSBCTRL_ADDR) = 0x01;

    // 4. Initialize USB Stack
    printf("Initializing USB Stack...\n");
    USBDevice_Initialize();

    // 5. Enable Interrupts
    printf("Enabling Interrupts...\n");
    sei();

    printf("Waiting for Enumeration...\n");

    // State tracking variables
    uint8_t lastState = 255;
    uint32_t counter = 0;

    while(1)
    {
        // 4. USB Tasks
        USBDevice_Handle();

        // Check Status
        RETURN_CODE_t status = USBDevice_StatusGet();

        // Print status change only when it changes
        if (status != lastState) {
            printf("USB Status: ");
            switch(status) {
            case USB_DETACHED: printf("DETACHED (0)\n"); break;
            case USB_ATTACHED: printf("ATTACHED (1)\n"); break;
            case USB_POWERED:  printf("POWERED (2)\n"); break;
            case USB_DEFAULT:  printf("DEFAULT (3)\n"); break;
            case USB_ADDRESS:  printf("ADDRESS (4)\n"); break;
            case USB_CONFIGURED: printf("CONFIGURED (5) - READY!\n"); break;
            default: printf("UNKNOWN (%d)\n", status); break;
            }
            lastState = status;
        }

        // 5. Application Logic
        if (status == USB_CONFIGURED)
        {
            // Move mouse occasionally to prove it works
            counter++;
            if (counter > 200000) {
                printf("Sending Mouse Move...\n");
                USB_HIDMouseMove(2, 0);
                _delay_ms(20);
                USB_HIDMouseMove(-2, 0);
                counter = 0;
            }
        }
    }
}