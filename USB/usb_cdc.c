//  ***************************************************************************
/// @file    usb_cdc.c
//  ***************************************************************************

#include "usb_cdc.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "usbd_core.h"
#include "stm32f0xx_hal_pcd.h"
#include "usbd_desc.h"
#include "usbd_cdc.h"
#include "hal/gpio/gpio.h"
#include "hal/sysclk/sysclk.h"
#include "common/error.h"


extern PCD_HandleTypeDef hpcd;
static USBD_HandleTypeDef usbd_device;

static int8_t usbd_cdc_fops_init(void);
static int8_t usbd_cdc_fops_deinit(void);
static int8_t usbd_cdc_fops_control(uint8_t cmd, uint8_t *pbuf, uint16_t length);
static int8_t usbd_cdc_fops_receive(uint8_t *pbuf, uint32_t *Len);

static USBD_CDC_ItfTypeDef usbd_cdc_fops = {
    usbd_cdc_fops_init,
    usbd_cdc_fops_deinit,
    usbd_cdc_fops_control,
    usbd_cdc_fops_receive
};

USBD_CDC_LineCodingTypeDef linecoding = {
    115200, // baud rate
    0x00,   // stop bits-1
    0x00,   // parity - none
    0x08    // nb. of bits 8
};


static uint8_t usb_cdc_tx_buff[128];
static uint8_t usb_cdc_rx_buff[128];
static uint32_t usb_cdc_rx_data_size;
static bool usb_cdc_is_data_received = false;



error_t usb_cdc_init(void) {
    sysclk_enable_peripheral(GPIOA);
    sysclk_enable_peripheral(SYSCFG);
    SYSCFG->CFGR1 |= 1 << SYSCFG_CFGR1_PA11_PA12_RMP_Pos;  // Pins remap

    if (USBD_Init(&usbd_device, &VCP_Desc, 0) != USBD_OK) return E_FAILED;
    if (USBD_RegisterClass(&usbd_device, &USBD_CDC) != USBD_OK) return E_FAILED;
    if (USBD_CDC_RegisterInterface(&usbd_device, &usbd_cdc_fops) != USBD_OK) return E_FAILED;
    if (USBD_Start(&usbd_device) != USBD_OK) return E_FAILED;
    return E_OK;
}


bool usb_cdc_is_usb_connected(void) {
    return true;
}


error_t usb_cdc_send_data(const uint8_t *data, uint32_t size) {
    USBD_CDC_HandleTypeDef *hcdc = (USBD_CDC_HandleTypeDef*)usbd_device.pClassData;
    if (hcdc->TxState != 0) return E_BUSY;
    if (size > sizeof(usb_cdc_tx_buff)) return E_OUT_OF_MEMORY;

    memcpy(usb_cdc_tx_buff, data, size);
    USBD_CDC_SetTxBuffer(&usbd_device, usb_cdc_tx_buff, size);
    if (USBD_CDC_TransmitPacket(&usbd_device) != USBD_OK) return E_FAILED;
    return E_OK;
}


error_t usb_cdc_receive_data(uint8_t *data, uint32_t *size, uint32_t max_size) {
    if (!usb_cdc_is_data_received) return E_NO_DATA;
    if (usb_cdc_rx_data_size > max_size) return E_OUT_OF_MEMORY;

    memcpy(data, usb_cdc_rx_buff, usb_cdc_rx_data_size);
    *size = usb_cdc_rx_data_size;

    // reload buff ptr
    USBD_CDC_SetRxBuffer(&usbd_device, usb_cdc_rx_buff);
    USBD_CDC_ReceivePacket(&usbd_device);
    usb_cdc_rx_data_size = 0;
    usb_cdc_is_data_received = false;
    
    return E_OK;
}




static int8_t usbd_cdc_fops_init(void) {
    USBD_CDC_SetRxBuffer(&usbd_device, usb_cdc_rx_buff);
    return USBD_OK;
}


static int8_t usbd_cdc_fops_deinit(void) {
    return USBD_OK;
}


static int8_t usbd_cdc_fops_control(uint8_t cmd, uint8_t *pbuf, uint16_t length) {
    switch (cmd)
    {
        case CDC_SEND_ENCAPSULATED_COMMAND:
            // Add your code here
            break;

        case CDC_GET_ENCAPSULATED_RESPONSE:
            // Add your code here
            break;

        case CDC_SET_COMM_FEATURE:
            // Add your code here
            break;

        case CDC_GET_COMM_FEATURE:
            // Add your code here
            break;

        case CDC_CLEAR_COMM_FEATURE:
            // Add your code here
            break;

        case CDC_SET_LINE_CODING:
            linecoding.bitrate    = (uint32_t)(pbuf[0] | (pbuf[1] << 8) | \
                                               (pbuf[2] << 16) | (pbuf[3] << 24));
            linecoding.format     = pbuf[4];
            linecoding.paritytype = pbuf[5];
            linecoding.datatype   = pbuf[6];

            // Add your code here
            break;

        case CDC_GET_LINE_CODING:
            pbuf[0] = (uint8_t)(linecoding.bitrate);
            pbuf[1] = (uint8_t)(linecoding.bitrate >> 8);
            pbuf[2] = (uint8_t)(linecoding.bitrate >> 16);
            pbuf[3] = (uint8_t)(linecoding.bitrate >> 24);
            pbuf[4] = linecoding.format;
            pbuf[5] = linecoding.paritytype;
            pbuf[6] = linecoding.datatype;

            // Add your code here
            break;

        case CDC_SET_CONTROL_LINE_STATE:
            // Add your code here
            break;

        case CDC_SEND_BREAK:
            // Add your code here
            break;

        default:
            break;
  }

  return USBD_OK;
}


static int8_t usbd_cdc_fops_receive(uint8_t *Buf, uint32_t *Len) {
    ////if (usb_cdc_rx_data_size > sizeof(usb_cdc_rx_buff)) return USBD_FAIL;
    memcpy(usb_cdc_rx_buff, Buf, *Len);
    usb_cdc_rx_data_size += *Len;
    usb_cdc_is_data_received = true;

    return USBD_OK;
}





void USB_IRQHandler(void);
void USB_IRQHandler(void) {
    HAL_PCD_IRQHandler(&hpcd);
}
