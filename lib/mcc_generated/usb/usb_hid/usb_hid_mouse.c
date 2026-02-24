/**
 *  USBHIDMOUSE HID Mouse Source File
 *  @file usb_hid_mouse.c
 *  @ingroup usb_hid
 *  @brief Contains the implementation for the USB Mouse drivers.
 *  @version USB Device Stack Driver Version 1.0.0
 */

/*
    (c) 2023 Microchip Technology Inc. and its subsidiaries.

    Subject to your compliance with these terms, you may use Microchip software and any
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party
    license terms applicable to your use of third party software (including open source software) that
    may accompany Microchip software.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS
    FOR A PARTICULAR PURPOSE.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS
    SOFTWARE.
 */

#include <usb_hid_mouse.h>
#include <stddef.h>
#include <string.h>
#include <usb_common_elements.h>
#include <usb_hid.h>
#include <usb_hid_transfer.h>
#include <usb_core.h>
#include <usb_config.h>
#include <usb_protocol_hid.h>

STATIC uint8_t usbHIDMouseRate;
STATIC uint8_t usbHIDMouseProtocol;
STATIC USB_MOUSE_REPORT_DATA_t mouseInputReport;

void USB_HIDMouseInitialize(USB_HID_REPORT_DESCRIPTOR_t *reportPtr)
{
    usbHIDMouseRate = 0;
    usbHIDMouseProtocol = 0;

    // Wipe the entire 12-byte struct clean on startup
    memset(&mouseInputReport, 0, sizeof(USB_MOUSE_REPORT_DATA_t));

    USB_HIDInitialize(&usbHIDMouseRate, &usbHIDMouseProtocol, reportPtr);
};

// Custom scroll function
static uint16_t ptp_center_x = 2048;
static uint16_t ptp_center_y = 2048;

RETURN_CODE_t USB_HIDTouchpadScroll(int16_t dx, int16_t dy)
{
    // If our fake fingers approach the edge of the 4095x4095 surface, lift and reset
    if (ptp_center_x < 500 || ptp_center_x > 3500 ||
        ptp_center_y < 500 || ptp_center_y > 3500)
    {
        mouseInputReport.reportId = 1;
        mouseInputReport.tipSwitch1 = 0; // Lift finger 1
        mouseInputReport.tipSwitch2 = 0; // Lift finger 2
        mouseInputReport.contactCount = 0;

        // Reset to center
        ptp_center_x = 2048;
        ptp_center_y = 2048;

        return USB_HIDMouseReportInSend(&mouseInputReport);
    }

    // Move the fake fingers
    ptp_center_x += dx;
    ptp_center_y += dy;

    mouseInputReport.reportId = 1;
    mouseInputReport.contactCount = 2; // Two fingers = scroll

    // Finger 1
    mouseInputReport.tipSwitch1 = 1;
    mouseInputReport.contactId1 = 0;
    mouseInputReport.x1 = ptp_center_x;
    mouseInputReport.y1 = ptp_center_y;

    // Finger 2 (Placed 500 units horizontally away from Finger 1)
    mouseInputReport.tipSwitch2 = 1;
    mouseInputReport.contactId2 = 1;
    mouseInputReport.x2 = ptp_center_x + 500;
    mouseInputReport.y2 = ptp_center_y;

    return USB_HIDMouseReportInSend(&mouseInputReport);
}
