#define F_CPU 24000000UL
#define BAUD_RATE 115200
#define USART0_BAUD_VAL ((float)(4.0 * F_CPU / (BAUD_RATE)) + 0.5)

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>


#include "ccp.h"
#include "usb_device.h"
#include "usb_common_elements.h"

#define AS5600_I2C_ADDR_W  0x6C  // 0x36 << 1 (Write Address)
#define AS5600_I2C_ADDR_R  0x6D  // (0x36 << 1) | 1 (Read Address)
#define AS5600_RAW_ANGLE_H 0x0C  // High byte register of raw angle

// --- Scroll Tuning Parameters ---
#define SCROLL_SENSITIVITY 2
#define SCROLL_MULTIPLIER 5

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

void LED_init(void) {
    PORTA.DIR |= PIN7_bm;
}

// I2C init
void TWI0_init(void) {
    PORTA.PIN2CTRL = PORT_PULLUPEN_bm;
    PORTA.PIN3CTRL = PORT_PULLUPEN_bm;
    // 100kHz standard mode at 24MHz
    TWI0.MBAUD = 115;
    TWI0.MCTRLA = TWI_ENABLE_bm;
    TWI0.MSTATUS = TWI_BUSSTATE_IDLE_gc;
}

uint16_t AS5600_readAngle(void) {
    uint8_t high_byte, low_byte;

    TWI0.MADDR = AS5600_I2C_ADDR_W;
    while (!(TWI0.MSTATUS & TWI_WIF_bm));

    TWI0.MDATA = AS5600_RAW_ANGLE_H;
    while (!(TWI0.MSTATUS & TWI_WIF_bm));

    TWI0.MADDR = AS5600_I2C_ADDR_R;
    while (!(TWI0.MSTATUS & TWI_RIF_bm));

    high_byte = TWI0.MDATA;
    TWI0.MCTRLB = TWI_ACKACT_ACK_gc | TWI_MCMD_RECVTRANS_gc;
    while (!(TWI0.MSTATUS & TWI_RIF_bm));

    low_byte = TWI0.MDATA;
    TWI0.MCTRLB = TWI_ACKACT_NACK_gc | TWI_MCMD_STOP_gc;

    return ((uint16_t)(high_byte & 0x0F) << 8) | low_byte;
}

// Hardware timer init
void TIMER_init(void) {
    // TCB0 at 24MHz. 24,000 ticks = 1ms (1000Hz)
    TCB0.CCMP = 24000;
    TCB0.CTRLA = TCB_CLKSEL_DIV1_gc | TCB_ENABLE_bm;
}

#define SYSCFG_VUSBCTRL_ADDR 0x0F0A

int main(void) {
    CLOCK_init();
    USART0_init();
    LED_init();
    TWI0_init();
    TIMER_init();
    stdout = &USART_stream;

    _delay_ms(800);
    printf("\n--- AVR32DU28 AS5600 USB SCROLL WHEEL ---\n");

    printf("Enabling VUSB Regulator...\n");
    *(volatile uint8_t *)(SYSCFG_VUSBCTRL_ADDR) = 0x01;

    printf("Initializing USB...\n");
    USBDevice_Initialize();
    sei();

    printf("Waiting for PC to recognize device...\n");

    // Rotation tracking
    int16_t previous_angle = (int16_t)AS5600_readAngle();
    int16_t angle_accumulator = 0;
    uint16_t print_divider = 0;

    while(1) {
        USBDevice_Handle();
        RETURN_CODE_t status = USBDevice_StatusGet();

        // 1000Hz hardware tick
        if (TCB0.INTFLAGS & TCB_CAPT_bm) {
            TCB0.INTFLAGS = TCB_CAPT_bm;

            if (status == SUCCESS) {
                int16_t current_angle = (int16_t)AS5600_readAngle();

                int16_t diff = current_angle - previous_angle;

                // Wrap around
                if (diff > 2048) {
                    diff -= 4096;
                } else if (diff < -2048) {
                    diff += 4096;
                }

                // Add to accumulator for hysteresis and noise filtering
                angle_accumulator += diff;

                if (abs(angle_accumulator) >= SCROLL_SENSITIVITY) {
                    int8_t scroll_steps = angle_accumulator / SCROLL_SENSITIVITY;

                    // Keep remainder in accumulator
                    angle_accumulator %= SCROLL_SENSITIVITY;

                    int8_t final_scroll = scroll_steps * SCROLL_MULTIPLIER;

                    PORTA.OUT |= PIN7_bm; // Indicator led on

                    USB_HIDTouchpadScroll(0, final_scroll);
                } else {
                    PORTA.OUT &= ~PIN7_bm;
                }

                previous_angle = current_angle;

                // UART Debug
                if (++print_divider >= 100) {
                    print_divider = 0;
                    printf("Raw: %4d | Accumulator: %4d\n", current_angle, angle_accumulator);
                }
            }
        }
    }
}
