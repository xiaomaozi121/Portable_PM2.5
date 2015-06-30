#ifndef _USER_UART_H_
#define _USER_UART_H_

/**
 ****************************************************************************************
 * @defgroup UART UART
 * @ingroup DRIVERS
 *
 * @brief UART driver
 *
 * @{
 *
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include <stdint.h>       // integer definition
#include <stdbool.h>      // boolean definition

/*
 * DEFINES
 *****************************************************************************************
 */

/// Divider for 921600 bits/s
//#define UART_BAUDRATE_921K6         tbd
/// Divider for 460800 bits/s
//#define UART_BAUDRATE_460K8         tbd
/// Divider for 230400 bits/s
//#define UART_BAUDRATE_230K4         tbd2
/// Divider for 115200 bits/s
#define UART_BAUDRATE_115K2         9 // 9 is according 115200 in stead of 4
/// Divider for 57600 bits/s
//#define UART_BAUDRATE_57K6          tbd
/// Divider for 38400 bits/s
//#define UART_BAUDRATE_38K4          tbd
/// Divider for 28800 bits/s
//#define UART_BAUDRATE_28K8          tbd
/// Divider for 19200 bits/s
//#define UART_BAUDRATE_19K2          tbd
/// Divider for 9600 bits/s
//#define UART_BAUDRATE_9K6           tbd

/// Baudrate used on the UART
#ifndef CFG_ROM
#define UART_BAUDRATE  UART_BAUDRATE_115K2
#else //CFG_ROM
#define UART_BAUDRATE  UART_BAUDRATE_460K8
#endif //CFG_ROM

#if (UART_BAUDRATE == UART_BAUDRATE_921K6)
#define UART_CHAR_DURATION        11
#else
#define UART_CHAR_DURATION       (UART_BAUDRATE * 22)
#endif // (UART_BAUDRATE == UART_BAUDRATE_921K6)


/*
 * ENUMERATION DEFINITIONS
 *****************************************************************************************
 */

/// Generic enable/disable enum for UART driver
enum
{
    /// uart disable
    UART_DISABLE = 0,
    /// uart enable
    UART_ENABLE  = 1
};

/// Character format
enum
{
    /// char format 5
    UART_CHARFORMAT_5 = 0,
    /// char format 6
    UART_CHARFORMAT_6 = 1,
    /// char format 7
    UART_CHARFORMAT_7 = 2,
    /// char format 8
    UART_CHARFORMAT_8 = 3
};

/// Stop bit
enum
{
    /// stop bit 1
    UART_STOPBITS_1 = 0,
    /* Note: The number of stop bits is 1.5 if a
     * character format with 5 bit is chosen */
    /// stop bit 2
    UART_STOPBITS_2 = 1
};

/// Parity bit
enum
{
    /// even parity
    UART_PARITYBIT_EVEN  = 0,
    /// odd parity
    UART_PARITYBIT_ODD   = 1,
    /// space parity
    UART_PARITYBIT_SPACE = 2, // The parity bit is always 0.
    /// mark parity
    UART_PARITYBIT_MARK  = 3  // The parity bit is always 1.
};

/* Error detection */
enum
{
    /// error detection disabled
    UART_ERROR_DETECT_DISABLED = 0,
    /// error detection enabled
    UART_ERROR_DETECT_ENABLED  = 1
};

/// status values
enum
{
    /// status ok
    UART_STATUS_OK,
    /// status not ok
    UART_STATUS_ERROR
};

/// Character format
enum
{
    /// data length 5 bits
    UART_DATALENGTH_5 = 0,
    /// data length 6 bits
    UART_DATALENGTH_6 = 1,
    /// data length 7 bits
    UART_DATALENGTH_7 = 2,
    /// data length 8 bits
    UART_DATALENGTH_8 = 3
};

/// Flow control
enum
{
    /// disabled auto flow control
    UART_FLOWCONTROL_DISABLED,
    /// enabled auto flow control
    UART_FLOWCONTROL_ENABLED,
};
#define Peri_Init
// Select UART settings
#define UART_BAUDRATE     UART_BAUDRATE_115K2       // Baudrate in bits/s: {9K6, 14K4, 19K2, 28K8, 38K4, 57K6, 115K2}
#define UART_DATALENGTH   UART_DATALENGTH_8         // Datalength in bits: {5, 6, 7, 8}
#define UART_PARITY       UART_PARITYBIT_EVEN       // Parity: {UART_PARITY_NONE, UART_PARITY_EVEN, UART_PARITY_ODD}
#define UART_STOPBITS     UART_STOPBITS_1           // Stop bits: {UART_STOPBITS_1, UART_STOPBITS_2} 
#define UART_FLOWCONTROL  UART_FLOWCONTROL_DISABLED // Flow control: {UART_FLOWCONTROL_DISABLED, UART_FLOWCONTROL_ENABLED}

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initializes the UART to default values.
 *****************************************************************************************
 */
void uart_init( uint8_t, uint8_t);
void user_uart_init(uint8_t , uint8_t );
void user_uart_init_func(uint8_t baudr, uint8_t mode);

#ifndef CFG_ROM
/**
 ****************************************************************************************
 * @brief Enable UART flow.
 *****************************************************************************************
 */
void uart_flow_on(void);
void user_uart_flow_on(void);

/**
 ****************************************************************************************
 * @brief Disable UART flow.
 *****************************************************************************************
 */
bool uart_flow_off(void);
void user_uart_flow_off(void);

/**
 ****************************************************************************************
 * @brief Finish current UART transfers
 *****************************************************************************************
 */
void uart_finish_transfers(void);
void user_uart_finish_transfers(void);
#endif //CFG_ROM

/**
 ****************************************************************************************
 * @brief Starts a data reception.
 *
 * As soon as the end of the data transfer or a buffer overflow is detected,
 * the hci_uart_rx_done function is executed.
 *
 * @param[in,out]  bufptr Pointer to the RX buffer
 * @param[in]      size   Size of the expected reception
 *****************************************************************************************
 */
//void uart_read(uint8_t *bufptr, uint32_t size);
void uart_read(uint8_t *bufptr, uint32_t size, void (*callback) (uint8_t));
void user_uart_read(uint8_t *bufptr, uint32_t size, void (*callback) (uint8_t));

/**
 ****************************************************************************************
 * @brief Starts a data transmission.
 *
 * As soon as the end of the data transfer is detected, the hci_uart_tx_done function is
 * executed.
 *
 * @param[in]  bufptr Pointer to the TX buffer
 * @param[in]  size   Size of the transmission
 *****************************************************************************************
 */
//void uart_write(uint8_t *bufptr, uint32_t size);
void uart_write(uint8_t *bufptr, uint32_t size, void (*callback) (uint8_t));
void user_uart_write(uint8_t *bufptr, uint32_t size, void (*callback) (uint8_t));
void user_uart_send_byte(char ch);
void printf2_string(uint8_t * str);
void user_uart_write_func(uint8_t *bufptr, uint32_t size, void (*callback) (uint8_t));
void user_uart_flow_on_func(void);
bool user_uart_flow_off_func(void);
void user_uart_finish_transfers_func(void);
void user_uart_read_func(uint8_t *bufptr, uint32_t size,void (*callback) (uint8_t));
void user_uart_write_func(uint8_t *bufptr, uint32_t size,void (*callback) (uint8_t));\
/**
 ****************************************************************************************
 * @brief Serves the data transfer interrupt requests.
 *
 * It clears the requests and executes the appropriate callback function.
 *****************************************************************************************
 */
void user_uart_isr(void);

/// @} UART
#endif /* _UART_H_ */
