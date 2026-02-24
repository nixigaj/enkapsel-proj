/**
 * USBHIDMOUSE HID Mouse Header File
 * @file usb_hid_mouse.h
 * @ingroup usb_hid
 * @defgroup usb_hid_mouse USB Human Interface Device (HID) Mouse
 * @brief Contains the prototypes and data types for the USB Mouse application drivers.
 * @version USB Device Stack Driver Version 1.0.0
 * @{
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

#ifndef USB_HID_MOUSE_H
#define USB_HID_MOUSE_H

#include <usb_common_elements.h>
#include <usb_protocol_hid.h>
#include <stdbool.h>

/**
 * @ingroup usb_hid_mouse
 * @def USB_HID_MOUSE_REPORT_SIZE
 * @brief Size of the report for a standard HID mouse.
 */
#define USB_HID_MOUSE_REPORT_SIZE 12

/**
 * @ingroup usb_hid_mouse
 * @name HID Mouse button state
 * @brief Macros to signal the button state.
 */
///@{
#define HID_MOUSE_BUTTON_DOWN true
#define HID_MOUSE_BUTTON_UP false
///@}

/**
 * @ingroup usb_hid_mouse
 * @name HID Mouse buttons
 * @brief Macros for the mouse buttons.
 */
///@{
#define HID_MOUSE_LEFT_BUTTON 0x01u
#define HID_MOUSE_RIGHT_BUTTON 0x02u
#define HID_MOUSE_MIDDLE_BUTTON 0x04u
///@}

/**
 * @ingroup usb_hid_mouse
 * @brief Sets up the mouse application for use with the HID class.
 * @param reportPtr - Pointer to report descriptor
 * @return None.
 */
void USB_HIDMouseInitialize(USB_HID_REPORT_DESCRIPTOR_t *reportPtr);

// Custom scroll function
RETURN_CODE_t USB_HIDTouchpadScroll(int16_t dx, int16_t dy);

/**
 * @}
 */

#endif /* USB_HID_MOUSE_H */