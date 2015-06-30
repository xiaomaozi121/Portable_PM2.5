/**
 ****************************************************************************************
 *
 * @file periph_setup.c
 *
 * @brief Peripherals setup and initialization. 
 *
 * Copyright (C) 2012. Dialog Semiconductor Ltd, unpublished work. This computer 
 * program includes Confidential, Proprietary Information and is a Trade Secret of 
 * Dialog Semiconductor Ltd.  All use, disclosure, and/or reproduction is prohibited 
 * unless authorized in writing. All Rights Reserved.
 *
 * <bluetooth.support@diasemi.com> and contributors.
 *
 ****************************************************************************************
 */
/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "rwip_config.h"             // SW configuration
#include "periph_setup.h"            // peripheral configuration
#include "global_io.h"
#include "gpio.h"
#include "app_xAPP_proj.h"
#ifdef CFG_PRINTF_UART2
#include "uart.h" 
#include "reg_uart.h"
#endif
#ifdef CFG_USER_UART
#include "user_uart.h"  
#endif
/**
 ****************************************************************************************
 * @brief Each application reserves its own GPIOs here.
 *
 * @return void
 ****************************************************************************************
 */

#if DEVELOPMENT_DEBUG

void GPIO_reservations(void)
{
/*
* Globally reserved GPIOs reservation
*/

/*
* Application specific GPIOs reservation. Used only in Development mode (#if DEVELOPMENT_DEBUG)
    
i.e.  
    RESERVE_GPIO(DESCRIPTIVE_NAME, GPIO_PORT_0, GPIO_PIN_1, PID_GPIO);    //Reserve P_01 as Generic Purpose I/O
*/
#ifdef CFG_PRINTF_UART2
	RESERVE_GPIO( UART2_TX, UART2_PORT,  UART2_TX_PIN, PID_UART2_TX);
	RESERVE_GPIO( UART2_RX, UART2_PORT,  UART2_RX_PIN, PID_UART2_RX);
#endif
#ifdef CFG_LED_PORT
	RESERVE_GPIO( GPIO_LED, GPIO_PORT_1,  GPIO_PIN_0, PID_GPIO);
#endif 
#ifdef CFG_PRINTF_UART
	RESERVE_GPIO( UART1_TX, GPIO_PORT_0,  GPIO_PIN_5, PID_UART1_TX);
	RESERVE_GPIO( UART1_RX, GPIO_PORT_0,  GPIO_PIN_4, PID_UART1_RX);
#endif 
#ifdef CFG_USER_UART
	RESERVE_GPIO( UART1_TX, GPIO_PORT_0,  GPIO_PIN_5, PID_UART1_TX);
	RESERVE_GPIO( UART1_RX, GPIO_PORT_0,  GPIO_PIN_4, PID_UART1_RX);
#endif
	RESERVE_GPIO( Wake_UP, GPIO_Trriger_Port,  GPIO_Trriger_Pin, PID_GPIO);
}
#endif //DEVELOPMENT_DEBUG

/**
 ****************************************************************************************
 * @brief Map port pins
 *
 * The Uart and SPI port pins and GPIO ports are mapped
 ****************************************************************************************
 */
void set_pad_functions(void)        // set gpio port function mode
{

#ifdef CFG_PRINTF_UART2
	GPIO_ConfigurePin( UART2_PORT, UART2_TX_PIN, OUTPUT, PID_UART2_TX, false );
	GPIO_ConfigurePin( UART2_PORT, UART2_RX_PIN, INPUT, PID_UART2_RX, false );
#endif
#ifdef CFG_LED_PORT
	GPIO_ConfigurePin( GPIO_PORT_1, GPIO_PIN_0, OUTPUT, PID_GPIO, false );
#endif
#ifdef CFG_PRINTF_UART
	GPIO_ConfigurePin( GPIO_PORT_0, GPIO_PIN_5, OUTPUT, PID_UART1_TX, false );
	GPIO_ConfigurePin( GPIO_PORT_0, GPIO_PIN_4, INPUT, PID_UART1_RX, false );
#endif
#ifdef CFG_USER_UART
	GPIO_ConfigurePin( GPIO_PORT_0, GPIO_PIN_5, OUTPUT, PID_UART1_TX, false );
	GPIO_ConfigurePin( GPIO_PORT_0, GPIO_PIN_4, INPUT, PID_UART1_RX, false );
#endif
	GPIO_ConfigurePin( GPIO_Trriger_Port, GPIO_Trriger_Pin, INPUT, PID_GPIO, false );

/*
* Configure application ports.
i.e.    
    GPIO_ConfigurePin( GPIO_PORT_0, GPIO_PIN_1, OUTPUT, PID_GPIO, false ); // Set P_01 as Generic purpose Output
*/
}


/**
 ****************************************************************************************
 * @brief Enable pad's and peripheral clocks assuming that peripherals' power domain is down. The Uart and SPi clocks are set.
 *
 * @return void
 ****************************************************************************************
 */
void periph_init(void) 
{
		// Power up peripherals' power domain
    SetBits16(PMU_CTRL_REG, PERIPH_SLEEP, 0);
    while (!(GetWord16(SYS_STAT_REG) & PER_IS_UP)) ; 
    
#if ES4_CODE
    SetBits16(CLK_16M_REG,XTAL16_BIAS_SH_DISABLE, 1);
#endif    
	
	//rom patch
	patch_func();
	
	//Init pads
	set_pad_functions();

#if (BLE_APP_PRESENT)
/*
* (Re)Initialize peripherals
i.e.    
    uart_init(UART_BAUDRATE_115K2, 3);
*/        
#endif

#ifdef CFG_PRINTF_UART2
		SetBits16(CLK_PER_REG, UART2_ENABLE, 1);
		uart2_init(UART_BAUDRATE_115K2, 3);
		uart2_rec_data_avail_setf(1); 
#endif
    
#ifdef CFG_PRINTF_UART
		SetBits16(CLK_PER_REG, UART1_ENABLE, 1);
		uart_init(UART_BAUDRATE_115K2, 3);
#endif
#ifdef CFG_USER_UART
		user_uart_init(UART_BAUDRATE_115K2, 3);
#endif
		GPIO_RegisterCallback(GPIO_Trriger_IRQn, gpio0_callback);
		GPIO_EnableIRQ(GPIO_Trriger_Port, GPIO_Trriger_Pin, GPIO_Trriger_IRQn, 0, 0, 20);
		// Enable the pads
		SetBits16(SYS_CTRL_REG, PAD_LATCH_EN, 1);
}

