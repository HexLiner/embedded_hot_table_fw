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


extern PCD_HandleTypeDef hpcd;
static USBD_HandleTypeDef usbd_device;

static int8_t TEMPLATE_Init(void);
static int8_t TEMPLATE_DeInit(void);
static int8_t TEMPLATE_Control(uint8_t cmd, uint8_t *pbuf, uint16_t length);
static int8_t TEMPLATE_Receive(uint8_t *pbuf, uint32_t *Len);

static USBD_CDC_ItfTypeDef usbd_cdc_fops = {
    TEMPLATE_Init,
    TEMPLATE_DeInit,
    TEMPLATE_Control,
    TEMPLATE_Receive
};

USBD_CDC_LineCodingTypeDef linecoding = {
    115200, /* baud rate*/
    0x00,   /* stop bits-1*/
    0x00,   /* parity - none*/
    0x08    /* nb. of bits 8*/
};


static uint8_t usb_cdc_rx_buff[128];
static uint32_t usb_cdc_rx_data_size;
static bool usb_cdc_is_data_received = false;



void usb_cdc_init(void) {
    sysclk_enable_peripheral(GPIOA);
    sysclk_enable_peripheral(SYSCFG);
    SYSCFG->CFGR1 |= 1 << SYSCFG_CFGR1_PA11_PA12_RMP_Pos;  // Pins remap

    USBD_Init(&usbd_device, &VCP_Desc, 0);
    USBD_RegisterClass(&usbd_device, &USBD_CDC);
    USBD_CDC_RegisterInterface(&usbd_device, &usbd_cdc_fops);
    USBD_Start(&usbd_device);
}


bool usb_cdc_is_usb_connected(void) {
    return true;
}


bool usb_cdc_is_data_sent(void) {
    USBD_CDC_HandleTypeDef *hcdc = (USBD_CDC_HandleTypeDef*)usbd_device.pClassData;
    if (hcdc->TxState != 0) return false;
    return true;
}


bool usb_cdc_send_data(const uint8_t *data, uint32_t size) {
    uint8_t result = USBD_OK;
    static uint8_t tx_buff[256];   ////


    if (!usb_cdc_is_data_sent()) return false;

    memcpy(tx_buff, data, size);
    USBD_CDC_SetTxBuffer(&usbd_device, tx_buff, size);
    result = USBD_CDC_TransmitPacket(&usbd_device);
    if (result != USBD_OK) return false;
    return true;
}


bool usb_cdc_receive_data(uint8_t *data, uint32_t *size) {
    if (!usb_cdc_is_data_received) return false;

    memcpy(data, usb_cdc_rx_buff, usb_cdc_rx_data_size);
    *size = usb_cdc_rx_data_size;
    usb_cdc_rx_data_size = 0;
    usb_cdc_is_data_received = false;
    return true;
}




static int8_t TEMPLATE_Init(void) {
    USBD_CDC_SetRxBuffer(&usbd_device, usb_cdc_rx_buff);
    return USBD_OK;
}


static int8_t TEMPLATE_DeInit(void) {
    return USBD_OK;
}


static int8_t TEMPLATE_Control(uint8_t cmd, uint8_t *pbuf, uint16_t length) {
  switch (cmd)
  {
    case CDC_SEND_ENCAPSULATED_COMMAND:
      /* Add your code here */
      break;

    case CDC_GET_ENCAPSULATED_RESPONSE:
      /* Add your code here */
      break;

    case CDC_SET_COMM_FEATURE:
      /* Add your code here */
      break;

    case CDC_GET_COMM_FEATURE:
      /* Add your code here */
      break;

    case CDC_CLEAR_COMM_FEATURE:
      /* Add your code here */
      break;

    case CDC_SET_LINE_CODING:
      linecoding.bitrate    = (uint32_t)(pbuf[0] | (pbuf[1] << 8) | \
                                         (pbuf[2] << 16) | (pbuf[3] << 24));
      linecoding.format     = pbuf[4];
      linecoding.paritytype = pbuf[5];
      linecoding.datatype   = pbuf[6];

      /* Add your code here */
      break;

    case CDC_GET_LINE_CODING:
      pbuf[0] = (uint8_t)(linecoding.bitrate);
      pbuf[1] = (uint8_t)(linecoding.bitrate >> 8);
      pbuf[2] = (uint8_t)(linecoding.bitrate >> 16);
      pbuf[3] = (uint8_t)(linecoding.bitrate >> 24);
      pbuf[4] = linecoding.format;
      pbuf[5] = linecoding.paritytype;
      pbuf[6] = linecoding.datatype;

      /* Add your code here */
      break;

    case CDC_SET_CONTROL_LINE_STATE:
      /* Add your code here */
      break;

    case CDC_SEND_BREAK:
      /* Add your code here */
      break;

    default:
      break;
  }

  return USBD_OK;
}

static int8_t TEMPLATE_Receive(uint8_t *Buf, uint32_t *Len) {
    // reload buff ptr
    USBD_CDC_SetRxBuffer(&usbd_device, Buf);
    USBD_CDC_ReceivePacket(&usbd_device);

    if (*Len > sizeof(usb_cdc_rx_buff)) return USBD_FAIL;
    memcpy(usb_cdc_rx_buff, Buf, *Len);    //// need ring buffer
    usb_cdc_rx_data_size = *Len;
    usb_cdc_is_data_received = true;

    return USBD_OK;
}





void USB_IRQHandler(void);
void USB_IRQHandler(void) {
    HAL_PCD_IRQHandler(&hpcd);
}
