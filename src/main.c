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

#define SYSCFG_VUSBCTRL_ADDR 0x0F0A

int main(void)
{
    CLOCK_init();
    USART0_init();
    stdout = &USART_stream;

    _delay_ms(500);
    printf("\n--- AVR32DU28 BASIC MOUSE ---\n");

    printf("Enabling VUSB Regulator...\n");
    *(volatile uint8_t *)(SYSCFG_VUSBCTRL_ADDR) = 0x01;

    printf("Initializing USB...\n");
    USBDevice_Initialize();
    sei();

    printf("Waiting for PC to see us...\n");

    uint8_t lastState = 255;
    uint32_t counter = 0;

    while(1)
    {
        USBDevice_Handle();

        RETURN_CODE_t status = USBDevice_StatusGet();

        if (status != lastState) {
            printf("Status Changed: %d\n", status);
            lastState = status;
        }

        if (status == SUCCESS)
        {
            counter++;
            if (counter > 400) {
                printf("Jiggle!\n");
                USB_HIDMouseMove(5, 0);
                _delay_ms(1);
                USB_HIDMouseMove(-5, 0);
                counter = 0;
            }
        }
    }
}
