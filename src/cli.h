//  ***************************************************************************
/// @file    cli.h
/// @brief   Simple CLI
//  ***************************************************************************
#ifndef _CLI_H_
#define _CLI_H_

#include <stdint.h>
#include <stdbool.h>
#include "common/error.h"


#define CLI_CMD_MAX_ARG_QTY   (10)
#define CLI_TX_BUFF_SIZE      (100)
#define CLI_RX_BUFF_SIZE      (100)
#define CLI_RX_RAW_BUFF_SIZE  (100)
#define CLI_PROMPT            ("\r\n> ")


typedef enum {
    CLI_CALL_FIRST,
    CLI_CALL_REPEATED,
    CLI_CALL_TERMINATE
} cli_call_state_t;

typedef error_t (*cli_send_data_callback)(const uint8_t *data, uint32_t size, uint32_t *max_size);
typedef error_t (*cli_receive_data_callback)(uint8_t *data, uint32_t *size, uint32_t max_size);

typedef error_t (*cli_cmd_callback)(uint32_t argc, const uint8_t **argv, cli_call_state_t state);

typedef struct {
    const uint8_t    *name;    // function name to CLI call
    const uint8_t    *usage;   // function usage for "help". Should be without function name! Can be NULL
    cli_cmd_callback func;     // function callback
} cli_cmd_t;




extern void cli_init(cli_send_data_callback send_cb, cli_receive_data_callback recv_cb, const cli_cmd_t *cli_cmds, uint32_t cli_cmds_qty);
extern void cli_process(void);

extern error_t cli_print(const uint8_t *str);
extern error_t cli_safe_print(const uint8_t *str);


#endif  // _CLI_H_
