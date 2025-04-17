//  ***************************************************************************
/// @file    error.c
//  ***************************************************************************
#include "common/error.h"


#ifdef ERRORS_STRINGS
static const struct {
    error_t    error_code;
    const char *string;
} errors_strings[] = {
    { E_OK,                                 "OK" },
    { E_NOT_RELEVANT,                       "NOT_RELEVANT" },
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
#endif // ERRORS_STRINGS


#ifdef LIB_DEBUG_EH
//  ***************************************************************************
/// @brief   Fatal error handler
/// @param   caller - pointer to function in which fatal error was occured
/// @param   source_code_line - source code line at which error was occured (use __LINE__ macro)
/// @return  none
/// @note    This function will never return control to caller function (by design).
/// @note    This function used for debug only.
//  ***************************************************************************
void error_fatal(uintptr_t caller, uint32_t source_code_line) {
    while (1) {

    }
    // Do not return from here
}
#endif   // LIB_DEBUG_EH


//  ***************************************************************************
/// @brief   Gets error code description
/// @param   error
/// @return  description string, "N/A" for non-defined error codes
//  ***************************************************************************
const char *error_get_string(error_t error) {
#ifdef ERRORS_STRINGS
    uint32_t i;
    for (i = 0; i < sizeof(errors_strings) / sizeof(errors_strings[0]); i++) {
        if (ERROR_GET_CODE(error) == errors_strings[i].error_code) return errors_strings[i].string;
    }
#endif // ERRORS_STRINGS

    return "N/A";
}
