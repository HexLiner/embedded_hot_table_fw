//  ***************************************************************************
/// @file    cli.h
/// @brief   Simple CLI
//  ***************************************************************************
#ifndef _CLI_H_
#define _CLI_H_

#include <stdint.h>
#include <stdbool.h>
#include "common/error.h"


typedef error_t (*cli_send_data_callback)(const uint8_t *data, uint32_t size);
typedef error_t (*cli_receive_data_callback)(uint8_t *data, uint32_t *size, uint32_t max_size);


extern void cli_init(cli_send_data_callback send_cb, cli_receive_data_callback recv_cb);
extern void cli_process(void);
extern error_t cli_print(const uint8_t *str);


#endif  // _CLI_H_
