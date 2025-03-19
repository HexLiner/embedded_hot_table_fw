//  ***************************************************************************
/// @file    error.h
/// @author  Techart Micro Systems (ES)
/// @brief   Error sybsystem
//  ***************************************************************************
#ifndef _ERROR_H_
#define _ERROR_H_

#include <stdint.h>
#include <stdlib.h>



typedef uint32_t error_t;


typedef void (*error_fatal_handler_f)(void);



#define E_SOFTWARE_FLAG                 (0x80000000ul)
#define E_NESTING_MASK                  (0x70000000ul)
#define E_NESTING_OFFSET                (28)
#define E_SOURCE_MASK                   (0x0FF00000ul)
#define E_SOURCE_OFFSET                 (20)
#define E_CODE_MASK                     (0x000FFFFFul)
#define E_CODE_OFFSET                   (0)

#define ERROR_GET_SOFTWARE_FLAG(e)          ((e) & (E_SOFTWARE_FLAG))
#define ERROR_GET_NESTING(e)                ((e) & (E_NESTING_MASK))
#define ERROR_GET_SOURCE(e)                 ((e) & (E_SOURCE_MASK))
#define ERROR_GET_CODE(e)                   ((e) & (E_CODE_MASK))


/// @defgroup error_t_source Error sources
/// @{
#define E_SOURCE_NONE                       ((0x00ul << E_SOURCE_OFFSET) & E_SOURCE_MASK) ///< Source not specified
#define E_SOURCE_I2C                        ((0x01ul << E_SOURCE_OFFSET) & E_SOURCE_MASK) ///< I2C driver
#define E_SOURCE_SPI                        ((0x02ul << E_SOURCE_OFFSET) & E_SOURCE_MASK) ///< SPI driver
#define E_SOURCE_UART                       ((0x03ul << E_SOURCE_OFFSET) & E_SOURCE_MASK) ///< UART driver
#define E_SOURCE_MODBUS                     ((0x04ul << E_SOURCE_OFFSET) & E_SOURCE_MASK) ///< Modbus driver
#define E_SOURCE_DEVICE_DRIVER              ((0x05ul << E_SOURCE_OFFSET) & E_SOURCE_MASK) ///< Device driver
/// @}


/// @defgroup error_t Error codes
/// @{
#define E_OK                                (0x00000ul) ///< No error - operation completed successfully
// End-of-operation faults
#define E_FAILED                            (0x00001ul) ///< Generic failure
#define E_TIMEOUT                           (0x00002ul) ///< Time-out occurred
#define E_CANCELLED                         (0x00003ul) ///< Operation cancelled
#define E_TERMINATED                        (0x00004ul) ///< Operation terminated
#define E_ASYNC_WAIT                        (0x00005ul) ///< Async operation is not completed yet
#define E_NOT_COMPLETED                     (0x00006ul) ///< Current operation in progress
#define E_OUT_OF_MEMORY                     (0x00007ul) ///< Out of memory
// Begin-of-operation faults
#define E_NO_DEVICE                         (0x00101ul) ///< Device not found
#define E_NOT_SUPPORTED                     (0x00102ul) ///< Operation/mode is not supported
#define E_BUSY                              (0x00103ul) ///< Requested resource is busy now
#define E_OCCUPIED                          (0x00104ul) ///< Device is occupied
#define E_NO_DATA                           (0x00105ul) ///< Requested data is absent/empty
// Data check faults
#define E_INVALID_CONFIG                    (0x00201ul) ///< Invalid configuration
#define E_INVALID_ID                        (0x00202ul) ///< Invalid identifier
#define E_INVALID_POINTER                   (0x00203ul) ///< Invalid pointer
#define E_INVALID_ARG                       (0x00204ul) ///< Invalid argument
#define E_UNDERRUN                          (0x00205ul) ///< Data underrun occurred
#define E_OVERRUN                           (0x00206ul) ///< Data overrun occurred
#define E_OUT_OF_RANGE                      (0x00207ul) ///< Data out of range
// Memory access faults
#define E_MEM_READ                          (0x00301ul) ///< Memory read error
#define E_MEM_WRITE                         (0x00302ul) ///< Memory write error
#define E_MEM_ACCESS                        (0x00303ul) ///< Memory access (read/write) error
#define E_MEM_ADDRESS                       (0x00304ul) ///< Invalid memory address error
// Misc faults
#define E_CRC_MISMATCH                      (0x00401ul) ///< CRC mismatch
#define E_COMM_ERROR                        (0x00402ul) ///< Generic communication error
// Modbus faults
#define E_MODBUS_ILLEGAL_FUNCTION           (0x00501ul) ///< Modbus exception: illegal function
#define E_MODBUS_ILLEGAL_DATA_ADDRESS       (0x00502ul) ///< Modbus exception: illegal data address
#define E_MODBUS_ILLEGAL_DATA_VALUE         (0x00503ul) ///< Modbus exception: illegal data value
#define E_MODBUS_SLAVE_DEVICE_FAILURE       (0x00504ul) ///< Modbus exception: slave defice failure
#define E_MODBUS_SLAVE_DEVICE_BUSY          (0x00506ul) ///< Modbus exception: slave defice busy
#define E_MODBUS_MEMORY_PARITY_ERROR        (0x00508ul) ///< Modbus exception: memory parity error
// I2C faults
#define E_I2C_ADDRESS_NACK                  (0x00601ul) ///< I2C: slave address has been NACKed
#define E_I2C_DATA_NACK                     (0x00602ul) ///< I2C: data byte has been NACKed
#define E_I2C_BUS_ERROR                     (0x00603ul) ///< I2C: bus error ocurred during transaction
#define E_I2C_ARBITRATION_LOST              (0x00604ul) ///< I2C: arbitration lost during transaction
#define E_I2C_UNEXPECTED_STOP               (0x00605ul) ///< I2C: stop ocurred during transaction
#define E_I2C_UNEXPECTED_START              (0x00606ul) ///< I2C: unexpected start condition detected
#define E_I2C_GENERAL_ERROR                 (0x00607ul) ///< I2C: general error ocurred during transaction
#define E_I2C_OVERRUN                       (0x00608ul) ///< I2C: overrun I2C_DR in slave mode
#define E_I2C_SDA_CONSTANT_LOW              (0x00609ul) ///< I2C: SDA line occupied
// SPI faults
#define E_SPI_OVERRUN                       (0x00701ul) ///< SPI: data overrun
#define E_SPI_CRC                           (0x00702ul) ///< SPI: CRC error
// UART faults
#define E_UART_PARITY_ERROR                 (0x00801ul) ///< UART: parity error
#define E_UART_FRAMING_ERROR                (0x00802ul) ///< UART: desynchronization, excessive noise or a break character is detected
#define E_UART_NOISE                        (0x00803ul) ///< UART: noise is detected on a received frame
#define E_UART_OVERRUN                      (0x00804ul) ///< UART: data overrun
#define E_UART_RX_BUFF_OVERFLOW             (0x00805ul) ///< UART: receive buffer overflow
#define E_UART_GENERAL_ERROR                (0x00806ul) ///< UART: general error ocurred during transaction
/// @}




#define ERROR_FATAL(caller, line)           (error_fatal((uintptr_t)caller, line))




extern void error_fatal(uintptr_t caller, uint32_t source_code_line);
extern void error_fatal_set_once_handler(error_fatal_handler_f handler);
extern void error_fatal_set_loop_handler(error_fatal_handler_f handler);

#ifdef ERROR_ALLOW_SHORT_STRINGS
extern const char *error_get_short_string(error_t error);
#else // ERROR_ALLOW_SHORT_STRINGS
#define error_get_short_string(a) ("N/A")
#endif // ERROR_ALLOW_SHORT_STRINGS

#ifdef ERROR_ALLOW_LONG_STRINGS
extern const char *error_get_long_string(error_t error);
#else // ERROR_ALLOW_LONG_STRINGS
#define error_get_long_string(a) ("N/A")
#endif // ERROR_ALLOW_LONG_STRINGS




#endif // _ERROR_H_
