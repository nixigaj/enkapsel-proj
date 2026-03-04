#define F_CPU 24000000UL
#define BAUD_RATE 115200
#define USART0_BAUD_VAL ((float)(4.0 * F_CPU / (BAUD_RATE)) + 0.5)

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h> // For abs()

// Include headers from your setup
#include "ccp.h"
#include "system/system.h"
#include "usb_device.h"
#include "usb_common_elements.h"

// --- AS5600 I2C Defines ---
#define AS5600_I2C_ADDR_W  0x6C  // 0x36 << 1 (Write Address)
#define AS5600_I2C_ADDR_R  0x6D  // (0x36 << 1) | 1 (Read Address)
#define AS5600_RAW_ANGLE_H 0x0C  // High byte register of raw angle

// --- Scroll Tuning Parameters ---
// Increase this to require more physical turning per scroll "tick"
// Decrease it to make the scroll wheel more sensitive
#define SCROLL_SENSITIVITY 2

// NEW: Increase this to make the page scroll faster per tick
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

// Led initialization
void LED_init(void) {
    PORTA.DIR |= PIN7_bm;
}

// --- TWI (I2C) Initialization ---
void TWI0_init(void) {
    PORTA.PIN2CTRL = PORT_PULLUPEN_bm;
    PORTA.PIN3CTRL = PORT_PULLUPEN_bm;
    // 100kHz standard mode at 24MHz
    TWI0.MBAUD = 115;
    TWI0.MCTRLA = TWI_ENABLE_bm;
    TWI0.MSTATUS = TWI_BUSSTATE_IDLE_gc;
}

// --- AS5600 Read Function ---
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

    // Variables for tracking rotation
    int16_t previous_angle = (int16_t)AS5600_readAngle();
    int16_t angle_accumulator = 0;
    uint16_t print_divider = 0;

    while(1) {
        USBDevice_Handle();
        RETURN_CODE_t status = USBDevice_StatusGet();

        // 1000Hz Hardware Timer Tick (Executes once every 1ms)
        if (TCB0.INTFLAGS & TCB_CAPT_bm) {
            TCB0.INTFLAGS = TCB_CAPT_bm; // Clear flag

            if (status == SUCCESS) {
                // 1. Read current angle
                int16_t current_angle = (int16_t)AS5600_readAngle();

                // 2. Calculate the difference
                int16_t diff = current_angle - previous_angle;

                // 3. Handle wrap-around (crossing the 0 / 4095 boundary)
                if (diff > 2048) {
                    diff -= 4096; // Turned backwards past 0
                } else if (diff < -2048) {
                    diff += 4096; // Turned forwards past 4095
                }

                // 4. Add difference to our accumulator (filters noise)
                angle_accumulator += diff;

                // 5. Check if we have turned enough to trigger a scroll
                if (abs(angle_accumulator) >= SCROLL_SENSITIVITY) {
                    // Determine how many scroll 'clicks' to calculate
                    int8_t scroll_steps = angle_accumulator / SCROLL_SENSITIVITY;

                    // Keep the remainder in the accumulator so we don't lose precision
                    angle_accumulator %= SCROLL_SENSITIVITY;

                    // Apply the multiplier right before sending to the PC
                    int8_t final_scroll = scroll_steps * SCROLL_MULTIPLIER;

                    PORTA.OUT |= PIN7_bm;

                    // Send the scroll command. (0, y) corresponds to vertical scrolling.
                    USB_HIDTouchpadScroll(0, final_scroll);
                } else {
                    PORTA.OUT &= ~PIN7_bm;
                }

                previous_angle = current_angle;

                // Optional: Print status every 100ms for debugging via UART
                if (++print_divider >= 100) {
                    print_divider = 0;
                    printf("Raw: %4d | Accumulator: %4d\n", current_angle, angle_accumulator);
                }
            }
        }
    }
}
