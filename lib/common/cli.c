#include "common/cli.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "common/error.h"
#include "hal/systimer.h"
#include "ring_buff.h"
#include "common/parsers.h"


#define CLI_CMD_INACTIVE_INDEX (-1)


static cli_send_data_callback send_data_callback = NULL;
static cli_receive_data_callback receive_data_callback = NULL;
static uint8_t tx_ring_buff_data[CLI_TX_BUFF_SIZE];
static ring_buff_t tx_ring_buff;
static uint8_t rx_buff[CLI_RX_BUFF_SIZE];
static uint32_t rx_buff_index;
static bool is_rx_overflow;

static const cli_cmd_t *cli_ext_cmds;
static uint32_t cli_ext_cmds_qty;
static int32_t cli_current_cmd_index;
static uint32_t cli_cmd_argc;
static const uint8_t *cli_cmd_argv[CLI_CMD_MAX_ARG_QTY];


static void cli_send_process(void);
static void cli_rx_process(void);

static void cli_cmd_start(void);
static void cli_cmd_process(void);
static void cli_cmd_break(void);

static void cli_int_cmd_help(void);




void cli_init(cli_send_data_callback send_cb, cli_receive_data_callback recv_cb, const cli_cmd_t *cli_cmds, uint32_t cli_cmds_qty) {
    send_data_callback = send_cb;
    receive_data_callback = recv_cb;
    rx_buff_index = 0;
    is_rx_overflow = false;
    ring_buff_init(&tx_ring_buff, tx_ring_buff_data, sizeof(tx_ring_buff_data));

    cli_ext_cmds = cli_cmds;
    cli_ext_cmds_qty = cli_cmds_qty;
    cli_current_cmd_index = CLI_CMD_INACTIVE_INDEX;
}


void cli_process(void) {
    cli_rx_process();
    cli_cmd_process();
    cli_send_process();
}


error_t cli_print(const uint8_t *str) {
    uint32_t size;
    

    size = strlen((char*)str);
    return ring_buff_write_block(&tx_ring_buff, str, size);
}


error_t cli_safe_print(const uint8_t *str) {
    static timer_t dead_time_timer = 0;
    timer_t timeout_timer;
    uint32_t size, block_size, str_index;


    if (!timer_triggered(dead_time_timer)) return E_TIMEOUT;

    size = strlen((char*)str);
    if (size > sizeof(tx_ring_buff_data)) block_size = sizeof(tx_ring_buff_data);
    else block_size = size;
    str_index = 0;
    timeout_timer = timer_start_ms(100);
    while(size != 0) {
        cli_send_process();
        if (ring_buff_write_block(&tx_ring_buff, &str[str_index], block_size) == E_OK) {
            str_index += block_size;
            size -= block_size;
            if (size > sizeof(tx_ring_buff_data)) block_size = sizeof(tx_ring_buff_data);
            else block_size = size;
        }
        if (timer_triggered(timeout_timer)) {
            dead_time_timer = timer_start_ms(100);
            return E_TIMEOUT;
        }
    }
    
    return E_OK;
}


//  ***************************************************************************
/// @brief  Print string with formatter
/// @param  format - format specifiers string
/// @return @ref error_t
/// @note   Variadic function
//  ***************************************************************************
error_t cli_printf(const uint8_t *format, ...) {
    va_list va;
    uint8_t printf_buff[CLI_PRINTF_BUFF_SIZE];


    va_start(va, format);
    vsnprintf((char*)printf_buff, CLI_PRINTF_BUFF_SIZE, (char*)format, va);
    va_end(va);

    return cli_print(printf_buff);
}


//  ***************************************************************************
/// @brief  Safe print string with formatter
/// @param  format - format specifiers string
/// @return @ref error_t
/// @note   Variadic function
//  ***************************************************************************
error_t cli_safe_printf(const uint8_t *format, ...) {
    va_list va;
    uint8_t printf_buff[CLI_PRINTF_BUFF_SIZE];


    va_start(va, format);
    vsnprintf((char*)printf_buff, CLI_PRINTF_BUFF_SIZE, (char*)format, va);
    va_end(va);

    return cli_safe_print(printf_buff);
}




static void cli_send_process(void) {
    const uint8_t *tx_data_ptr;
    uint32_t tx_data_size, max_tx_data_size;
    
    
    ring_buff_get_read_pos(&tx_ring_buff, &tx_data_ptr, &tx_data_size);
    if (tx_data_size != 0) {
        if (send_data_callback(tx_data_ptr, tx_data_size, &max_tx_data_size) == E_OK) {
            if (tx_data_size > max_tx_data_size) tx_data_size = max_tx_data_size;
            ring_buff_clear(&tx_ring_buff, tx_data_size);
        }
    }
}


static void cli_rx_process(void) {
    uint32_t rx_raw_size, i;
    uint8_t rx_raw_buff[CLI_RX_RAW_BUFF_SIZE];


    if (receive_data_callback(rx_raw_buff, &rx_raw_size, sizeof(rx_raw_buff)) == E_OK) {
        for (i = 0; i < rx_raw_size; i++) {
            if (cli_current_cmd_index == CLI_CMD_INACTIVE_INDEX) {
                if ((rx_raw_buff[i] >= '\x20') && (rx_raw_buff[i] <= '\x7E')) {     // 20..7E - Printable symbol code
                    ring_buff_write(&tx_ring_buff, rx_raw_buff[i]);    // echo
                    if (rx_buff_index < (sizeof(rx_buff) - 1)) {   // last rx_buff byte used for EOL
                        rx_buff[rx_buff_index] = rx_raw_buff[i];
                        rx_buff_index++;
                    }
                    else {
                        is_rx_overflow = true;
                    }
                }
                else if (rx_raw_buff[i] == '\x7F') {    // 7F - Backspace code
                    if ((rx_buff_index > 0) && !is_rx_overflow) {
                        ring_buff_write(&tx_ring_buff, rx_raw_buff[i]);    // echo
                        rx_buff_index--;  // delete last symbol
                    }
                }
                else if (rx_raw_buff[i] == '\x0D') {    // 0D - Enter code
                    cli_cmd_start();
                }
            }
            else if (rx_raw_buff[i] == '\x03') {    // 03 - Control-C code
                cli_cmd_break();
            }
        }
    }
}



static void cli_cmd_start(void) {
    const uint8_t *cli_incorrect_cmd_msg = "CMD not found!";
    const uint8_t *cli_incorrect_args_msg = "Incorrect arg!";
    const uint8_t *cli_incorrect_failed_msg = "Failed!";
    error_t result;
  

    if (is_rx_overflow) {
        cli_safe_print("\r\n");
        cli_safe_print(cli_incorrect_cmd_msg);
        cli_cmd_argc = 0;
    }
    else {
        rx_buff[rx_buff_index] = '\0';
        pars_get_tokens_from_string(rx_buff, " ", cli_cmd_argv, CLI_CMD_MAX_ARG_QTY, &cli_cmd_argc);
    }


    if (cli_cmd_argc > 0) {
        for (cli_current_cmd_index = (cli_ext_cmds_qty - 1); cli_current_cmd_index > CLI_CMD_INACTIVE_INDEX; cli_current_cmd_index--) {
            if (strcmp((char*)cli_cmd_argv[0], (char*)cli_ext_cmds[cli_current_cmd_index].name) == 0) break;
        }

        cli_safe_print("\r\n");
        if (cli_current_cmd_index != CLI_CMD_INACTIVE_INDEX) {
            result = cli_ext_cmds[cli_current_cmd_index].func(cli_cmd_argc, cli_cmd_argv, CLI_CALL_FIRST);
            if (result == E_ASYNC_WAIT) {
                return;
            }
            else {
                cli_current_cmd_index = CLI_CMD_INACTIVE_INDEX;
                if (result == E_INVALID_ARG) cli_safe_print(cli_incorrect_args_msg);
                else if (result == E_FAILED) cli_safe_print(cli_incorrect_failed_msg);
            }
        }
        else if (strcmp((char*)cli_cmd_argv[0], "help") == 0) {
            cli_int_cmd_help();
        }
        else {
            cli_safe_print(cli_incorrect_cmd_msg);
        }
    }

    cli_safe_print("\r\n\r\n");
    cli_print(CLI_PROMPT);
    rx_buff_index = 0;
    is_rx_overflow = false;
}


static void cli_cmd_process(void) {
    if (cli_current_cmd_index == CLI_CMD_INACTIVE_INDEX) return;

    if (cli_ext_cmds[cli_current_cmd_index].func(cli_cmd_argc, cli_cmd_argv, CLI_CALL_REPEATED) != E_ASYNC_WAIT) {
        cli_current_cmd_index = CLI_CMD_INACTIVE_INDEX;
        cli_safe_print("\r\n\r\n");
        cli_print(CLI_PROMPT);
        rx_buff_index = 0;
        is_rx_overflow = false;
    }
}


static void cli_cmd_break(void) {
    if (cli_current_cmd_index == CLI_CMD_INACTIVE_INDEX) return;

    cli_ext_cmds[cli_current_cmd_index].func(cli_cmd_argc, cli_cmd_argv, CLI_CALL_TERMINATE);
    cli_current_cmd_index = CLI_CMD_INACTIVE_INDEX;
    cli_safe_print("\r\n\r\n");
    cli_print(CLI_PROMPT);
    rx_buff_index = 0;
    is_rx_overflow = false;
}




static void cli_int_cmd_help(void) {
    uint32_t i;


    for (i = 0; i < cli_ext_cmds_qty; i++) {
        if (i > 0) cli_safe_print("\r\n");
        if (cli_ext_cmds[i].usage != NULL) {
            cli_safe_printf("%s %s", cli_ext_cmds[i].name, cli_ext_cmds[i].usage);
        }
        else {
            cli_safe_print(cli_ext_cmds[i].name);
        }
    }
}
