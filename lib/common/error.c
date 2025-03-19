//  ***************************************************************************
/// @file    error.c
/// @author  Techart Micro Systems (ES)
//  ***************************************************************************
#include "common/error.h"




static volatile uintptr_t    error_fatal_caller_address = 0;
static volatile uint32_t     error_fatal_source_code_line = 0;
static error_fatal_handler_f error_fatal_once_handler = NULL;
static error_fatal_handler_f error_fatal_loop_handler = NULL;




#ifdef ERROR_ALLOW_SHORT_STRINGS
static const struct {
    error_t    error_code;
    const char *string;
} short_strings[] = {
    { E_OK,                                 "OK" },
    { E_FAILED,                             "FAILED" },
    { E_TIMEOUT,                            "TIMEOUT" },
    { E_CANCELLED,                          "CANCELLED" },
    { E_TERMINATED,                         "TERMINATED" },
    { E_ASYNC_WAIT,                         "ASYNC_WAIT" },
    { E_NOT_COMPLETED,                      "NOT_COMPLETED" },
    { E_OUT_OF_MEMORY,                      "OUT_OF_MEMORY" },
    { E_NO_DEVICE,                          "NO_DEVICE" },
    { E_NOT_SUPPORTED,                      "NOT_SUPPORTED" },
    { E_BUSY,                               "BUSY" },
    { E_OCCUPIED,                           "OCCUPIED" },
    { E_NO_DATA,                            "NO_DATA" },
    { E_INVALID_CONFIG,                     "INVALID_CONFIG" },
    { E_INVALID_ID,                         "INVALID_ID" },
    { E_INVALID_POINTER,                    "INVALID_POINTER" },
    { E_INVALID_ARG,                        "INVALID_ARG" },
    { E_UNDERRUN,                           "UNDERRUN" },
    { E_OVERRUN,                            "OVERRUN" },
    { E_OUT_OF_RANGE,                       "OUT_OF_RANGE" },
    { E_MEM_READ,                           "MEM_READ" },
    { E_MEM_WRITE,                          "MEM_WRITE" },
    { E_MEM_ACCESS,                         "MEM_ACCESS" },
    { E_MEM_ADDRESS,                        "MEM_ADDRESS" },
    { E_CRC_MISMATCH,                       "CRC_MISMATCH" },
    { E_COMM_ERROR,                         "COMM_ERROR" },
    { E_MODBUS_ILLEGAL_FUNCTION,            "MODBUS_ILLEGAL_FUNCTION" },
    { E_MODBUS_ILLEGAL_DATA_ADDRESS,        "MODBUS_ILLEGAL_DATA_ADDRESS" },
    { E_MODBUS_ILLEGAL_DATA_VALUE,          "MODBUS_ILLEGAL_DATA_VALUE" },
    { E_MODBUS_SLAVE_DEVICE_FAILURE,        "MODBUS_SLAVE_DEVICE_FAILURE" },
    { E_MODBUS_SLAVE_DEVICE_BUSY,           "MODBUS_SLAVE_DEVICE_BUSY" },
    { E_MODBUS_MEMORY_PARITY_ERROR,         "MODBUS_MEMORY_PARITY_ERROR" },
    { E_I2C_ADDRESS_NACK,                   "I2C_ADDRESS_NACK" },
    { E_I2C_DATA_NACK,                      "I2C_DATA_NACK" },
    { E_I2C_BUS_ERROR,                      "I2C_BUS_ERROR" },
    { E_I2C_ARBITRATION_LOST,               "I2C_ARBITRATION_LOST" },
    { E_I2C_UNEXPECTED_STOP,                "I2C_UNEXPECTED_STOP" },
    { E_I2C_UNEXPECTED_START,               "I2C_UNEXPECTED_START" },
    { E_I2C_GENERAL_ERROR,                  "I2C_GENERAL_ERROR" },
    { E_I2C_OVERRUN,                        "I2C_OVERRUN" },
    { E_I2C_SDA_CONSTANT_LOW,               "I2C_SDA_CONSTANT_LOW" },
    { E_SPI_OVERRUN,                        "SPI_OVERRUN" },
    { E_SPI_CRC,                            "SPI_CRC" },
    { E_UART_PARITY_ERROR,                  "UART_PARITY_ERROR" },
    { E_UART_FRAMING_ERROR,                 "UART_FRAMING_ERROR" },
    { E_UART_NOISE,                         "UART_NOISE" },
    { E_UART_OVERRUN,                       "UART_OVERRUN" },
    { E_UART_RX_BUFF_OVERFLOW,              "UART_RX_BUFF_OVERFLOW" },
    { E_UART_GENERAL_ERROR,                 "UART_GENERAL_ERROR" },
};
#endif // ERROR_ALLOW_SHORT_STRINGS


#ifdef ERROR_ALLOW_LONG_STRINGS
static const struct {
    error_t    error_code;
    const char *string;
} long_strings[] = {
    { E_OK,                                 "No error" },
    { E_FAILED,                             "Generic failure" },
    { E_TIMEOUT,                            "Time-out occurred" },
    { E_CANCELLED,                          "Operation cancelled" },
    { E_TERMINATED,                         "Operation terminated" },
    { E_ASYNC_WAIT,                         "Async operation is not completed yet" },
    { E_NOT_COMPLETED,                      "Current operation in progress" },
    { E_OUT_OF_MEMORY,                      "Out of memory" },
    { E_NO_DEVICE,                          "Device not found" },
    { E_NOT_SUPPORTED,                      "Operation/mode is not supported" },
    { E_BUSY,                               "Requested resource is busy now" },
    { E_OCCUPIED,                           "Device is occupied" },
    { E_NO_DATA,                            "Requested data is absent/empty" },
    { E_INVALID_CONFIG,                     "Invalid configuration" },
    { E_INVALID_ID,                         "Invalid identifier" },
    { E_INVALID_POINTER,                    "Invalid pointer" },
    { E_INVALID_ARG,                        "Invalid argument" },
    { E_UNDERRUN,                           "Data underrun occurred" },
    { E_OVERRUN,                            "Data overrun occurred" },
    { E_OUT_OF_RANGE,                       "Data out of range" },
    { E_MEM_READ,                           "Memory read error" },
    { E_MEM_WRITE,                          "Memory write error" },
    { E_MEM_ACCESS,                         "Memory access (read/write) error" },
    { E_MEM_ADDRESS,                        "Invalid memory address error" },
    { E_CRC_MISMATCH,                       "CRC mismatch" },
    { E_COMM_ERROR,                         "Generic communication error" },
    { E_MODBUS_ILLEGAL_FUNCTION,            "Modbus exception: illegal function" },
    { E_MODBUS_ILLEGAL_DATA_ADDRESS,        "Modbus exception: illegal data address" },
    { E_MODBUS_ILLEGAL_DATA_VALUE,          "Modbus exception: illegal data value" },
    { E_MODBUS_SLAVE_DEVICE_FAILURE,        "Modbus exception: slave defice failure" },
    { E_MODBUS_SLAVE_DEVICE_BUSY,           "Modbus exception: slave defice busy" },
    { E_MODBUS_MEMORY_PARITY_ERROR,         "Modbus exception: memory parity error" },
    { E_I2C_ADDRESS_NACK,                   "I2C: slave address has been NACKed" },
    { E_I2C_DATA_NACK,                      "I2C: data byte has been NACKed" },
    { E_I2C_BUS_ERROR,                      "I2C: bus error ocurred during transaction" },
    { E_I2C_ARBITRATION_LOST,               "I2C: arbitration lost during transaction" },
    { E_I2C_UNEXPECTED_STOP,                "I2C: stop ocurred during transaction" },
    { E_I2C_UNEXPECTED_START,               "I2C: unexpected start condition detected" },
    { E_I2C_GENERAL_ERROR,                  "I2C: general error ocurred during transaction" },
    { E_I2C_OVERRUN,                        "I2C: overrun I2C_DR in slave mode" },
    { E_I2C_SDA_CONSTANT_LOW,               "I2C: SDA line occupied" },
    { E_SPI_OVERRUN,                        "SPI: data overrun" },
    { E_SPI_CRC,                            "SPI: CRC error" },
    { E_UART_PARITY_ERROR,                  "UART: parity error" },
    { E_UART_FRAMING_ERROR,                 "UART: desynchronization, excessive noise or a break character is detected" },
    { E_UART_NOISE,                         "UART: noise is detected on a received frame" },
    { E_UART_OVERRUN,                       "UART: data overrun" },
    { E_UART_RX_BUFF_OVERFLOW,              "UART: receive buffer overflow" },
    { E_UART_GENERAL_ERROR,                 "UART: general error ocurred during transaction" },
};
#endif // ERROR_ALLOW_LONG_STRINGS







//  ***************************************************************************
/// @brief   Fatal error handler
/// @param   caller - pointer to function in which fatal error was occured
/// @param   source_code_line - source code line at which error was occured (use __LINE__ macro)
/// @note    this function will never return control to caller function (by design)
/// @return  none
//  ***************************************************************************
void error_fatal(uintptr_t caller, uint32_t source_code_line) {
    error_fatal_caller_address = caller;
    error_fatal_source_code_line = source_code_line;

    if (error_fatal_once_handler != NULL) error_fatal_once_handler();
    while (1) {
        if (error_fatal_loop_handler != NULL) error_fatal_loop_handler();
    }
    // Do not return from here
}


//  ***************************************************************************
/// @brief   Sets once handler for fatal error condition (launched once on fatal error condition detected)
/// @param   handler - handler function, can be NULL
/// @return  none
//  ***************************************************************************
void error_fatal_set_loop_handler(error_fatal_handler_f handler) {
    error_fatal_once_handler = handler;
}


//  ***************************************************************************
/// @brief   Sets loop handler for fatal error condition (launched in infinite loop if fatal error condition detected)
/// @param   handler - handler function, can be NULL
/// @return  none
//  ***************************************************************************
void error_fatal_set_once_handler(error_fatal_handler_f handler) {
    error_fatal_loop_handler = handler;
}




//  ***************************************************************************
/// @brief   Gets short description (actually error name) by error code
/// @param   error - @ref error_t, error code to be described
/// @return  short description string, "N/A" for non-defined error codes
//  ***************************************************************************
#ifdef ERROR_ALLOW_SHORT_STRINGS
const char *error_get_short_string(error_t error) {
    uint32_t i;


    for (i = 0; i < sizeof(short_strings) / sizeof(short_strings[0]); i++) {
        if (ERROR_GET_CODE(error) == short_strings[i].error_code) return short_strings[i].string;
    }

    return "N/A";
}
#endif // ERROR_ALLOW_SHORT_STRINGS


//  ***************************************************************************
/// @brief   Gets long description (actually user-friendly description) by error code
/// @param   error - @ref error_t, error code to be described
/// @return  long description string, "N/A" for non-defined error codes
//  ***************************************************************************
#ifdef ERROR_ALLOW_LONG_STRINGS
const char *error_get_long_string(error_t error) {
    uint32_t i;


    for (i = 0; i < sizeof(long_strings) / sizeof(long_strings[0]); i++) {
        if (ERROR_GET_CODE(error) == long_strings[i].error_code) return long_strings[i].string;
    }

    return "N/A";
}
#endif // ERROR_ALLOW_LONG_STRINGS
