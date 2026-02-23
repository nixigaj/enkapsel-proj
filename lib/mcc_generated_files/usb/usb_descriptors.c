/**
 * USB_DEVICE_STACK Generated Driver File
 * 
 * @file usb_descriptors.c
 * 
 * @ingroup usb_device_stack
 * 
 * @brief Driver implementation file for example application descriptors.
 *
 * @version USB_DEVICE_STACK Driver Version 1.0.0
*/
/*
© [2026] Microchip Technology Inc. and its subsidiaries.

    Subject to your compliance with these terms, you may use Microchip 
    software and any derivatives exclusively with Microchip products. 
    You are responsible for complying with 3rd party license terms  
    applicable to your use of 3rd party software (including open source  
    software) that may accompany Microchip software. SOFTWARE IS ?AS IS.? 
    NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS 
    SOFTWARE, INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT,  
    MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT 
    WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY 
    KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF 
    MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE 
    FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP?S 
    TOTAL LIABILITY ON ALL CLAIMS RELATED TO THE SOFTWARE WILL NOT 
    EXCEED AMOUNT OF FEES, IF ANY, YOU PAID DIRECTLY TO MICROCHIP FOR 
    THIS SOFTWARE.
*/

#include <string.h>
#include "usb_descriptors.h"
#include <usb_protocol_headers.h>

//Report descriptor for a standard mouse with three buttons and no scroll wheel
USB_HID_REPORT_DESCRIPTOR_t USB_HIDMouseReportDescriptor = {
    {
        0x05, 0x01, /* Usage Page (Generic Desktop),       */
        0x09, 0x02, /* Usage (Mouse),                      */
        0xA1, 0x01, /*  Collection (Application),          */
        0x09, 0x01, /*   Usage (Pointer),                  */
        0xA1, 0x00, /*  Collection (Physical),             */
        0x05, 0x09, /*     Usage Page (Buttons),           */
        0x19, 0x01, /*     Usage Minimum (01),             */
        0x29, 0x03, /*     Usage Maximum (03),             */
        0x15, 0x00, /*     Logical Minimum (0),            */
        0x25, 0x01, /*     Logical Maximum (1),            */
        0x75, 0x01, /*     Report Size (1),                */
        0x95, 0x03, /*     Report Count (3),               */
        0x81, 0x02, /*     Input (Data, Variable, Absolute) */
        0x75, 0x05, /*     Report Size (5),                */
        0x95, 0x01, /*     Report Count (1),               */
        0x81, 0x01, /*     Input (Constant),               */
        0x05, 0x01, /*     Usage Page (Generic Desktop),   */
        0x09, 0x30, /*     Usage (X),                      */
        0x09, 0x31, /*     Usage (Y),                      */
        0x15, 0x81, /*     Logical Minimum (-127),         */
        0x25, 0x7F, /*     Logical Maximum (127),          */
        0x75, 0x08, /*     Report Size (8),                */
        0x95, 0x02, /*     Report Count (2),               */
        0x81, 0x06, /*     Input (Data, Variable, Relative) */
        0xC0, /*  End Collection,                    */
        0xC0, /* End Collection                      */
    }
};

static USB_DEVICE_DESCRIPTOR_t deviceDescriptor = {
    .header = {
        .bLength = sizeof(USB_DEVICE_DESCRIPTOR_t),
        .bDescriptorType = USB_DESCRIPTOR_TYPE_DEVICE,
    },
    .bcdUSB = 0x200,            // USB 2.0
    .bDeviceClass = USB_NO_DEVICE_CLASS,        // Not defined on device level
    .bDeviceSubClass = 0x00,            // Not defined in Device Descriptor level
    .bDeviceProtocol = 0x00,            // Not defined in Device Descriptor level
    .bMaxPacketSize0 = USB_EP0_SIZE,    // EP0 size
    .idVendor = 0x04D8,            // MCHP VID
    .idProduct = 0x0B13,          // PID 0x0010-0x002F reserved for testing/non-public demos
    .bcdDevice = 0x0110,   // 01.1.0,
    .iManufacturer = 0x01,              // String index 1
    .iProduct = 0x02,                   // String index 2
    .iSerialNumber = 0x03,              // String index 3
    .bNumConfigurations = 0x01          // Number of configurations 
};

static USB_APPLICATION_CONFIGURATION_t configurationDescriptor = {
    .Config1 =
    {
        .Configuration =
        {
            .header = 
            {
                .bLength = sizeof (USB_CONFIGURATION_DESCRIPTOR_t),
                .bDescriptorType = (uint8_t)USB_DESCRIPTOR_TYPE_CONFIGURATION,
            },
            .wTotalLength = sizeof (USB_APPLICATION_CONFIGURATION1_t),
            .bNumInterfaces = USB_INTERFACE_NUM,
            .bConfigurationValue = 1u,
            .iConfiguration = 0u,
            .bmAttributes = USB_CONFIG_ATTR_MUST_SET | USB_CONFIG_ATTR_BUS_POWERED,
            .bMaxPower = USB_CONFIG_MAX_POWER(2),
        },
        .Interface0Alternate0 =
        {
            .header =
            {
                .bLength = sizeof (USB_INTERFACE_DESCRIPTOR_t),
                .bDescriptorType = USB_DESCRIPTOR_TYPE_INTERFACE,
            },
            .bInterfaceNumber = 0U,
            .bAlternateSetting = 0U,
            .bNumEndpoints = 1U,
            .bInterfaceClass = USB_HID_DEVICE_CLASS, // HID
            .bInterfaceSubClass = HID_SUB_CLASS_NOBOOT,
            .bInterfaceProtocol = HID_PROTOCOL_MOUSE,
            .iInterface = 0U,
        },        
        .HID_Descriptor0 =
        {
            .header =
            {
                .bLength = sizeof (USB_HID_DESCRIPTOR_t),
                .bDescriptorType = USB_DT_HID,
            },
            .bcdHID = 0x111, // 1.11
            .bCountryCode = USB_HID_NO_COUNTRY_CODE,
            .bNumDescriptors = USB_HID_NUM_DESC,
            .bRDescriptorType = USB_DT_HID_REPORT,
            .wDescriptorLength = USB_HID_REPORT_DESCRIPTOR_SIZE,
        },
        .Interface0Alternate0_Endpoint1IN =
        {
            .header =
            {
                .bLength = sizeof (USB_ENDPOINT_DESCRIPTOR_t),
                .bDescriptorType = USB_DESCRIPTOR_TYPE_ENDPOINT,
            },
            .bEndpointAddress =
            {
                .direction = USB_EP_DIR_IN,
                .address = INTERFACE0ALTERNATE0_INTERRUPT_EP1_IN,
            },
            .bmAttributes =
            {
                .type = INTERRUPT,
                .synchronisation = 0U, // None
                .usage = 0U, // None
            },
            .wMaxPacketSize = INTERFACE0ALTERNATE0_INTERRUPT_EP1_IN_SIZE,
            .bInterval = 1U,
        },
    },
};

static USB_STRING_LANG_ID_DESCRIPTOR_t langIDDescriptor  = {
    .header =
    {
        .bLength = sizeof (USB_STRING_LANG_ID_DESCRIPTOR_t),
        .bDescriptorType = USB_DESCRIPTOR_TYPE_STRING,
    },
    .id_array =
    {LANG_EN_US},
};

static USB_APPLICATION_STRING_DESCRIPTORS_t stringDescriptors = {
    .manufacturer_header =
    {
        .bLength = sizeof (stringDescriptors.manufacturer) + sizeof (USB_DESCRIPTOR_HEADER_t),
        .bDescriptorType = USB_DESCRIPTOR_TYPE_STRING,
    },
    .manufacturer = STRING_MANUFACTURER,
    .product_header =
    {
        .bLength = sizeof (stringDescriptors.product) + sizeof (USB_DESCRIPTOR_HEADER_t),
        .bDescriptorType = USB_DESCRIPTOR_TYPE_STRING,
    },
    .product = STRING_PRODUCT,
    .serial_header =
    {
        .bLength = sizeof (stringDescriptors.serial) + sizeof (USB_DESCRIPTOR_HEADER_t),
        .bDescriptorType = USB_DESCRIPTOR_TYPE_STRING,
    },
    .serial = STRING_SERIAL,
};

USB_DESCRIPTOR_POINTERS_t descriptorPointers = {
    .devicePtr = (USB_DEVICE_DESCRIPTOR_t *) & deviceDescriptor,
    .configurationsPtr = (USB_CONFIGURATION_DESCRIPTOR_t *) & configurationDescriptor,
    .deviceBOSptr = NULL,
    .langIDptr = &langIDDescriptor,
    .stringPtrs =
    {
        &stringDescriptors.manufacturer_header,
    },
};

/**
 End of File
*/
