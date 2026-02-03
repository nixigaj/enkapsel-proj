#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

// Include the MCC generated system
#include "../lib/mcc_generated/system/system.h"
#include "../lib/mcc_generated/usb/usb_device.h"

int main(void)
{
    // 1. Initialize Clock (24MHz) and Pins
    SYSTEM_Initialize();

    // 2. Initialize USB Stack
    USB_Device_Initialize();

    // 3. Enable Global Interrupts (Mandatory for USB)
    sei();

    while(1)
    {
        // Simple status check
        if (USB_Device_GetState() == USB_STATE_CONFIGURED) {
            // Success! The host has enumerated us.
            // Your scroll wheel logic goes here.
        }
    }
    return 0;
}