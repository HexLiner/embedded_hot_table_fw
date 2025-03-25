//  ***************************************************************************
/// @file    usb_cdc.h
/// @brief   USB CDC interface
//  ***************************************************************************
#ifndef _USB_CDC_H_
#define _USB_CDC_H_


#include <stdint.h>
#include <stdbool.h>
#include "common/error.h"


extern error_t usb_cdc_init(void);
extern bool usb_cdc_is_usb_connected(void);
extern error_t usb_cdc_send_data(const uint8_t *data, uint32_t size, uint32_t *max_size);
extern error_t usb_cdc_receive_data(uint8_t *data, uint32_t *size, uint32_t max_size);


#endif   // _USB_CDC_H_
