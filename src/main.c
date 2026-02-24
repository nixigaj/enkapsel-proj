#define F_CPU 24000000UL
#define BAUD_RATE 115200
#define USART0_BAUD_VAL ((float)(4.0 * F_CPU / (BAUD_RATE)) + 0.5)

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>

#include "system/system.h"
#include "usb_device.h"
#include "usb_common_elements.h"

void CLOCK_init(void) {
    ccp_write_io((void*)&CLKCTRL.OSCHFCTRLA, CLKCTRL_FRQSEL_24M_gc);
    ccp_write_io((void*)&CLKCTRL.MCLKCTRLB, 0x00);
}

void USART0_init(void) {
    PORTA.DIR |= PIN0_bm;
    PORTA.DIR &= ~PIN1_bm;
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

// --- Hardware Timer Initialization ---
void TIMER_init(void) {
    // TCB0 at 24MHz. 24,000 ticks = 1ms (1000Hz)
    TCB0.CCMP = 24000;
    TCB0.CTRLA = TCB_CLKSEL_DIV1_gc | TCB_ENABLE_bm;
}

#define SYSCFG_VUSBCTRL_ADDR 0x0F0A

int main(void) {
    CLOCK_init();
    USART0_init();
    TIMER_init(); // Initialize our 1ms timer
    stdout = &USART_stream;

    _delay_ms(800);
    printf("\n--- AVR32DU28 1000Hz SCROLLER (SQUARE PATTERN) ---\n");

    printf("Enabling VUSB Regulator...\n");
    *(volatile uint8_t *)(SYSCFG_VUSBCTRL_ADDR) = 0x01;

    printf("Initializing USB...\n");
    USBDevice_Initialize();
    sei();

    printf("Waiting for PC to see us...\n");

    uint16_t tick_counter = 0;
    while(1)
    {
        USBDevice_Handle();
        RETURN_CODE_t status = USBDevice_StatusGet();

        // 1000Hz Hardware Timer Tick (Executes once every 1ms)
        if (TCB0.INTFLAGS & TCB_CAPT_bm)
        {
            TCB0.INTFLAGS = TCB_CAPT_bm; // Clear flag

            if (status == SUCCESS)
            {
                // Send scroll update every 8ms (125Hz) - Perfect for 120Hz screens
                if (tick_counter % 16 == 0)
                {
                    // Trace a square: 1 second (1000 ticks) per side
                    if (tick_counter < 1000)
                    {
                        USB_HIDTouchpadScroll(40, 0);   // Right
                    }
                    else if (tick_counter < 2000)
                    {
                        USB_HIDTouchpadScroll(0, 40);   // Down
                    }
                    else if (tick_counter < 3000)
                    {
                        USB_HIDTouchpadScroll(-40, 0);  // Left
                    }
                    else if (tick_counter < 4000)
                    {
                        USB_HIDTouchpadScroll(0, -40);  // Up
                    }
                }

                // Increment counter and reset after 4 seconds (4000 ticks)
                tick_counter++;
                if (tick_counter >= 4000)
                {
                    tick_counter = 0;
                }
            }
        }
    }
}