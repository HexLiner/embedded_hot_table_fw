#include "cli.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "common/error.h"
#include "ring_buff.h"


#define SYM_NONE                            (0xFF)
#define SYM_CONTROL                         (0x80)
#define SYM_BACKSPACE                       (SYM_CONTROL|0x01)
#define SYM_TAB                             (SYM_CONTROL|0x02)
#define SYM_ENTER                           (SYM_CONTROL|0x03)
#define SYM_CTRLC                           (SYM_CONTROL|0x04)
#define SYM_UP                              (SYM_CONTROL|0x05)
#define SYM_DOWN                            (SYM_CONTROL|0x06)
#define SYM_LEFT                            (SYM_CONTROL|0x07)
#define SYM_RIGHT                           (SYM_CONTROL|0x08)
#define SYM_DEL                             (SYM_CONTROL|0x09)
#define SYM_HOME                            (SYM_CONTROL|0x0A)
#define SYM_END                             (SYM_CONTROL|0x0B)


static cli_send_data_callback send_data_callback = NULL;
static cli_receive_data_callback receive_data_callback = NULL;
static uint8_t tx_ring_buff_data[100];
static ring_buff_t tx_ring_buff;
static uint8_t rx_buff[30];
static uint32_t rx_buff_index;
static bool is_rx_overflow;
static const cli_cmd_t *cli_cmds_int;
static uint32_t cli_cmds_qty_int;

static bool cli_cmd_in_process;



static void cli_cmd_start(void);
static void cli_cmd_break(void);
static void cli_cmd_process(void);



void cli_init(cli_send_data_callback send_cb, cli_receive_data_callback recv_cb, const cli_cmd_t *cli_cmds, uint32_t cli_cmds_qty) {
    send_data_callback = send_cb;
    receive_data_callback = recv_cb;
    cli_cmds_int = cli_cmds;
    cli_cmds_qty_int = cli_cmds_qty;

    rx_buff_index = 0;
    is_rx_overflow = false;
    ring_buff_init(&tx_ring_buff, tx_ring_buff_data, sizeof(tx_ring_buff_data));

    cli_cmd_in_process = false;
}





void cli_process(void) {
    uint32_t rx_raw_size, i;
    uint8_t ctrl_code;
    uint8_t rx_raw_buff[30];
    const uint8_t *tx_data_ptr;
    uint32_t tx_data_size, max_tx_data_size;


    ctrl_code = SYM_NONE;
    if (receive_data_callback(rx_raw_buff, &rx_raw_size, sizeof(rx_raw_buff)) == E_OK) {
        for(i = 0; i < rx_raw_size; i++) {
            if (!cli_cmd_in_process) {
                if ((rx_raw_buff[i] >= '\x20') && (rx_raw_buff[i] <= '\x7E')) {     // 20..7E   Printable symbol code
                    ring_buff_write(&tx_ring_buff, rx_raw_buff[i]);
                    if (rx_buff_index < sizeof(rx_buff)) {
                        rx_buff[rx_buff_index] = rx_raw_buff[i];
                        rx_buff_index++;
                    }
                    else {
                        is_rx_overflow = true;
                    }
                }
                else if (rx_raw_buff[i] == '\x7F') {                      // 7F       Backspace code
                    if ((rx_buff_index > 0) && !is_rx_overflow) rx_buff_index--;  // delete last symbol
                    ring_buff_write(&tx_ring_buff, rx_raw_buff[i]);
                }
                else if (rx_raw_buff[i] == '\x0D') {                      // 0D       Enter code
                    cli_cmd_start();
                }
            }
            else if (rx_raw_buff[i] == '\x03') {                      // 03       Control-C code
                cli_cmd_break();
            }
        }
    }

    cli_cmd_process();

    ring_buff_get_read_pos(&tx_ring_buff, &tx_data_ptr, &tx_data_size);
    if (tx_data_size != 0) {
        if (send_data_callback(tx_data_ptr, tx_data_size, &max_tx_data_size) == E_OK) {
            if (tx_data_size > max_tx_data_size) tx_data_size = max_tx_data_size;
            ring_buff_clear(&tx_ring_buff, tx_data_size);
        }
    }
}


error_t cli_print(const uint8_t *str) {
    uint32_t size = strlen((char*)str);
    return ring_buff_write_block(&tx_ring_buff, str, size);
}



static void cli_cmd_start(void) {
    uint8_t *ptr;
  
  
    if (rx_buff_index == 0) {   //// and all space symb
        cli_print("\r\n> ");
    }
    else {
        cli_cmd_in_process = true;
        cli_print("CMD start\r\n");


        rx_buff[rx_buff_index] = '\0';
        ptr = rx_buff;
        cli_cmds_int[0].funk(0, &ptr, CLI_CALL_FIRST);
    }
}

static void cli_cmd_break(void) {
    cli_cmd_in_process = false;
    rx_buff_index = 0;
    is_rx_overflow = false;
    cli_print("CMD break\r\n> ");
}

static void cli_cmd_process(void) {
    
}



/*
static void cli_cmd_processing(void) {
    uint16_t addr;
    uint8_t value[4];
    bool is_error = true;
    bool is_epprom_cmd;
    uint16_t reg_qty;


    cli_uart_tx_size = 0;
    cli_uart_tx_buff[cli_uart_tx_size] = '\r';
    cli_uart_tx_size++;
    cli_uart_tx_buff[cli_uart_tx_size] = '\n';
    cli_uart_tx_size++;

    if (cli_uart_rx_cnt > 0) {

        // ewAAAA XX[XX][XXXX] | erAAAA | edAAAA | eqAAAA  (all in HEX)  - read/write EEPROM
        // rwAAAA XX[XX][XXXX] | rrAAAA | rdAAAA | rqAAAA  (all in HEX)  - read/write RAM

        if (cli_uart_rx_buff[0] == 'e') {
            reg_qty = DEVICE_EEPROM_REG_QTY;
            is_epprom_cmd = true;
        }
        else if (cli_uart_rx_buff[0] == 'r') {
            reg_qty = DEVICE_RAM_REG_QTY;
            is_epprom_cmd = false;
        }
        else {
            goto end;
        }

        if (cli_uart_rx_cnt >= 6) {
            addr = 0;
            if (!hex_text_to_u8(&cli_uart_rx_buff[2], ((uint8_t*)&addr + 1))) goto end;
            if (!hex_text_to_u8(&cli_uart_rx_buff[4], (uint8_t*)&addr)) goto end;
            

            if ((cli_uart_rx_buff[1] == 'r') && (cli_uart_rx_cnt == 6)) {
                if (addr > reg_qty) goto end;
                
                if (is_epprom_cmd) {
                    eeprom_driver_read(addr, 1, value);
                }
                else {
                    value[0] = *device_registers_ptr[addr];
                }

                u8_to_hex_text(value[0], &cli_uart_tx_buff[cli_uart_tx_size]);
                cli_uart_tx_size += 2;

                is_error = false;
            }
            else if ((cli_uart_rx_buff[1] == 'd') && (cli_uart_rx_cnt == 6)) {
                if ((addr + 2) > reg_qty) goto end;

                if (is_epprom_cmd) {
                    eeprom_driver_read(addr, 2, value);
                }
                else {
                    // BE - HHLL
                    value[0] = *device_registers_ptr[addr];
                    value[1] = *device_registers_ptr[addr + 1];
                }

                u8_to_hex_text(value[0], &cli_uart_tx_buff[cli_uart_tx_size]);
                cli_uart_tx_size += 2;
                u8_to_hex_text(value[1], &cli_uart_tx_buff[cli_uart_tx_size]);
                cli_uart_tx_size += 2;

                is_error = false;
            }
            else if ((cli_uart_rx_buff[1] == 'q') && (cli_uart_rx_cnt == 6)) {
                if ((addr + 4) > reg_qty) goto end;

                if (is_epprom_cmd) {
                    eeprom_driver_read(addr, 4, value);
                }
                else {
                    // BE - HHLL
                    value[0] = *device_registers_ptr[addr];
                    value[1] = *device_registers_ptr[addr + 1];
                    value[2] = *device_registers_ptr[addr + 2];
                    value[3] = *device_registers_ptr[addr + 3];
                }

                u8_to_hex_text(value[0], &cli_uart_tx_buff[cli_uart_tx_size]);
                cli_uart_tx_size += 2;
                u8_to_hex_text(value[1], &cli_uart_tx_buff[cli_uart_tx_size]);
                cli_uart_tx_size += 2;
                u8_to_hex_text(value[2], &cli_uart_tx_buff[cli_uart_tx_size]);
                cli_uart_tx_size += 2;
                u8_to_hex_text(value[3], &cli_uart_tx_buff[cli_uart_tx_size]);
                cli_uart_tx_size += 2;

                is_error = false;
            }
            else if (cli_uart_rx_buff[1] == 'w') {
                if (cli_uart_rx_cnt == 9) {
                    if (addr > reg_qty) goto end;
                    if (!hex_text_to_u8(&cli_uart_rx_buff[7], &value[0])) goto end;

                    if (is_epprom_cmd) {
                        eeprom_driver_write(addr, 1, value);
                    }
                    else {
                        *device_registers_ptr[addr] = value[0];
                    }
                }
                else if (cli_uart_rx_cnt == 11) {
                    if ((addr + 2) > reg_qty) goto end;
                    // BE - HHLL
                    if (!hex_text_to_u8(&cli_uart_rx_buff[7], &value[0])) goto end;
                    if (!hex_text_to_u8(&cli_uart_rx_buff[9], &value[1])) goto end;

                    if (is_epprom_cmd) {
                        eeprom_driver_write(addr, 2, value);
                    }
                    else {
                        *device_registers_ptr[addr] = value[0];
                        *device_registers_ptr[addr + 1] = value[1];
                    }
                }
                else if (cli_uart_rx_cnt == 15) {
                    if ((addr + 4) > reg_qty) goto end;
                    // BE - HHLL
                    if (!hex_text_to_u8(&cli_uart_rx_buff[7], &value[0])) goto end;
                    if (!hex_text_to_u8(&cli_uart_rx_buff[9], &value[1])) goto end;
                    if (!hex_text_to_u8(&cli_uart_rx_buff[11], &value[2])) goto end;
                    if (!hex_text_to_u8(&cli_uart_rx_buff[13], &value[3])) goto end;

                    if (is_epprom_cmd) {
                        eeprom_driver_write(addr, 4, value);
                    }
                    else {
                        *device_registers_ptr[addr] = value[0];
                        *device_registers_ptr[addr + 1] = value[1];
                        *device_registers_ptr[addr + 2] = value[2];
                        *device_registers_ptr[addr + 3] = value[3];
                    }
                }
                else {
                    goto end;
                }

                cli_uart_tx_buff[cli_uart_tx_size] = 'O';
                cli_uart_tx_size++;
                cli_uart_tx_buff[cli_uart_tx_size] = 'K';
                cli_uart_tx_size++;

                is_error = false;
            }
        }

    }
    else {
        is_error = false;
    }


end:
    if (is_error) {
        cli_uart_tx_buff[cli_uart_tx_size] = 'E';
        cli_uart_tx_size++;
        cli_uart_tx_buff[cli_uart_tx_size] = 'r';
        cli_uart_tx_size++;
        cli_uart_tx_buff[cli_uart_tx_size] = 'r';
        cli_uart_tx_size++;
        cli_uart_tx_buff[cli_uart_tx_size] = '!';
        cli_uart_tx_size++;
    }
    cli_uart_tx_buff[cli_uart_tx_size] = '\r';
    cli_uart_tx_size++;
    cli_uart_tx_buff[cli_uart_tx_size] = '\n';
    cli_uart_tx_size++;
    cli_uart_tx_buff[cli_uart_tx_size] = '@';
    cli_uart_tx_size++;
    cli_uart_tx_buff[cli_uart_tx_size] = ' ';
    cli_uart_tx_size++;

    cli_uart_rx_cnt = 0;
}




static bool hex_text_to_u8(uint8_t *simw_ptr, uint8_t *result) {
    uint8_t result_tmp;


    result_tmp = simw_ptr[0];
    if ((simw_ptr[0] < '0') || (simw_ptr[0] > '9')) {
        if ((simw_ptr[0] < 'A') || (simw_ptr[0] > 'F')) return false;
        result_tmp -= ('A' - 10 - '0');
    }
    result_tmp -= '0';
    *result = result_tmp << 4;
    
    result_tmp = simw_ptr[1];
    if ((simw_ptr[1] < '0') || (simw_ptr[1] > '9')) {
        if ((simw_ptr[1] < 'A') || (simw_ptr[1] > 'F')) return false;
        result_tmp -= ('A' - 10 - '0');
    }
    result_tmp -= '0';
    *result |= result_tmp;

    return true;
}


static void u8_to_hex_text(uint8_t value, uint8_t *result) {
    uint8_t value_temp;


    value_temp = value & 0x0F;
    if (value_temp > 9) result[1] = ('A' - 10);
    else result[1] = '0';
    result[1] += value_temp;

    value_temp = value >> 4;
    if (value_temp > 9) result[0] = ('A' - 10);
    else result[0] = '0';
    result[0] += value_temp;
}
*/