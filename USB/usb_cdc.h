//  ***************************************************************************
/// @file    usb_cdc.h
/// @brief   USB CDC interface
//  ***************************************************************************
#ifndef _USB_CDC_H_
#define _USB_CDC_H_


#include <stdint.h>
#include <stdbool.h>


extern void usb_cdc_init(void);
////extern void usb_cdc_handler(void);

extern bool usb_cdc_is_usb_connected(void);

extern bool usb_cdc_is_data_sent(void);
extern bool usb_cdc_send_data(const uint8_t *data, uint32_t size);
extern bool usb_cdc_receive_data(uint8_t *data, uint32_t *size);


#endif   // _USB_CDC_H_
