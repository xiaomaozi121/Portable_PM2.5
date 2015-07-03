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
#include "periph_setup.h"            // periphera configuration
#include "global_io.h"
#include "gpio.h"
#include "uart.h"                    // UART initialization
#include "reg_uart.h"   // uart register
#include "app_proxr_proj.h"

#ifndef _PERIPH_SETUP_H_
#define _PERIPH_SETUP_H_

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
* Application specific GPIOs reservation
*/    
#if (BLE_APP_PRESENT)
#if BLE_PROX_REPORTER
   
    RESERVE_GPIO( GREEN_LED, GPIO_ALERT_LED_PORT, GPIO_ALERT_LED_PIN, PID_GPIO);
#endif
#if BLE_BATT_SERVER && USE_BAT_LEVEL_ALERT
	//Setup LED GPIO for battery alert
    RESERVE_GPIO( RED_LED, GPIO_BAT_LED_PORT, GPIO_BAT_LED_PIN, PID_GPIO);
#endif
    
#if (BLE_SPOTA_RECEIVER)
    RESERVE_GPIO( SPI_CLK, LASER_STATE_PORT, GPIO_PIN_0, PID_SPI_CLK);
    RESERVE_GPIO( SPI_DO, LASER_STATE_PORT, GPIO_PIN_6, PID_SPI_DO);
    RESERVE_GPIO( SPI_DI, LASER_STATE_PORT, GPIO_PIN_5, PID_SPI_DI);
    RESERVE_GPIO( SPI_EN, LASER_STATE_PORT, GPIO_PIN_3, PID_SPI_EN);
    
    //RESERVE_GPIO( I2C_SCL, LASER_STATE_PORT, GPIO_PIN_2, PID_I2C_SCL);
    //RESERVE_GPIO( I2C_SDA, LASER_STATE_PORT, GPIO_PIN_3, PID_I2C_SDA);
#endif
#ifdef CFG_PRINTF_UART2
	RESERVE_GPIO( UART2_TX, UART2_PORT,  UART2_TX_PIN, PID_UART2_TX);
	RESERVE_GPIO( UART2_RX, UART2_PORT,  UART2_RX_PIN, PID_UART2_RX);
#endif
		#if USE_PUSH_BUTTON
			RESERVE_GPIO( PUSH_BUTTON, GPIO_BUTTON_PORT, GPIO_BUTTON_PIN, PID_GPIO);   
    #endif // USE_PUSH_BUTTON
		RESERVE_GPIO( PM25, LASER_STATE_PORT, GPIO_PIN_7, PID_GPIO);
		//RESERVE_GPIO( PM25SWITCH, GPIO_PORT_1, GPIO_PIN_0, PID_GPIO);
#endif
    
}
#endif

/**
 ****************************************************************************************
 * @brief Map port pins
 *
 * The Uart and SPI port pins and GPIO ports(for debugging) are mapped
 ****************************************************************************************
 */
void set_pad_functions(void)        // set gpio port function mode
{
    
#if BLE_PROX_REPORTER
    
    GPIO_ConfigurePin( GPIO_ALERT_LED_PORT, GPIO_ALERT_LED_PIN, OUTPUT, PID_GPIO, false ); //Alert LED
#endif
#if BLE_BATT_SERVER  && USE_BAT_LEVEL_ALERT
    GPIO_ConfigurePin( GPIO_BAT_LED_PORT, GPIO_BAT_LED_PIN, OUTPUT, PID_GPIO, false ); //Battery alert LED
#endif
    
#if (BLE_SPOTA_RECEIVER)
    //GPIO_ConfigurePin(LASER_STATE_PORT, GPIO_PIN_2, INPUT, PID_I2C_SCL, false);
    //GPIO_ConfigurePin(LASER_STATE_PORT, GPIO_PIN_3, INPUT, PID_I2C_SDA, false);
    
    GPIO_ConfigurePin( LASER_STATE_PORT, GPIO_PIN_3, OUTPUT, PID_SPI_EN, true );
    GPIO_ConfigurePin( LASER_STATE_PORT, GPIO_PIN_0, OUTPUT, PID_SPI_CLK, false );
    GPIO_ConfigurePin( LASER_STATE_PORT, GPIO_PIN_6, OUTPUT, PID_SPI_DO, false );	
    GPIO_ConfigurePin( LASER_STATE_PORT, GPIO_PIN_5, INPUT, PID_SPI_DI, false );
#endif
#ifdef CFG_PRINTF_UART2
		GPIO_ConfigurePin( UART2_PORT, UART2_TX_PIN, OUTPUT, PID_UART2_TX, false );
		GPIO_ConfigurePin( UART2_PORT, UART2_RX_PIN, INPUT, PID_UART2_RX, false );
#endif
#if USE_PUSH_BUTTON
		GPIO_ConfigurePin( GPIO_BUTTON_PORT, GPIO_BUTTON_PIN, INPUT_PULLUP, PID_GPIO, false ); // Push Button 
#endif // USE_PUSH_BUTTON
	GPIO_ConfigurePin( LASER_STATE_PORT, LASER_STATE_PIN, OUTPUT, PID_GPIO, true );
	//GPIO_ConfigurePin( GPIO_PORT_1, GPIO_PIN_0, INPUT_PULLUP, PID_GPIO, true );
}


/**
 ****************************************************************************************
 * @brief Enable pad's and peripheral clocks assuming that peripherals' power domain is down. The Uart and SPi clocks are set.
 *
 * @return void
 ****************************************************************************************
 */
void periph_init(void)  // set i2c, spi, uart, uart2 serial clks
{
		// Power up peripherals' power domain
    SetBits16(PMU_CTRL_REG, PERIPH_SLEEP, 0);
    while (!(GetWord16(SYS_STAT_REG) & PER_IS_UP)) ; 
    
    SetBits16(CLK_16M_REG,XTAL16_BIAS_SH_DISABLE, 1);
	
		//rom patch
		patch_func();
		
		//Init pads
		set_pad_functions();


#if (BLE_APP_PRESENT)
    
#if BLE_PROX_REPORTER
    app_proxr_port_reinit(GPIO_ALERT_LED_PORT, GPIO_ALERT_LED_PIN);
    #if USE_PUSH_BUTTON
    app_button_enable();
    #endif // USE_PUSH_BUTTON
#elif BLE_FINDME_LOCATOR
    #if USE_PUSH_BUTTON
    app_button_enable();
    #endif // USE_PUSH_BUTTON
#endif //BLE_PROX_REPORTER
#if BLE_BATTERY_SERVER
    app_batt_port_reinit();
#endif //BLE_BATTERY_SERVER

#endif //BLE_APP_PRESENT
		app_button_enable();
#if (BLE_SAMPLE128)
		PM2_5[9]='u';
		PM2_5[10]='g';
		PM2_5[11]='/';
		PM2_5[12]='m';
		PM2_5[13]='3';
		
		Prameter[0]=0x55;
		Prameter[1]=0x05;
		Prameter[10]=0xAA;
#endif
		//GPIO_SetActive(LASER_STATE_PORT,LASER_STATE_PIN);
		
		if(flag_PM25)
		{
				GPIO_SetActive(LASER_STATE_PORT,LASER_STATE_PIN);
		}
		else
		{
				GPIO_SetInactive(LASER_STATE_PORT,LASER_STATE_PIN);
				#ifdef CFG_PRINTF_UART2
					SetBits16(CLK_PER_REG, UART2_ENABLE, 1);
					uart2_init(UART_BAUDRATE_115K2, 3);
					uart2_rec_data_avail_setf(1); 
				#endif
		}
		//GPIO_SetInactive(LASER_STATE_PORT,LASER_STATE_PIN);
		if(!flag_Uart2_Init)
		{
				flag_Uart2_Init=1;
				#ifdef CFG_PRINTF_UART2
					SetBits16(CLK_PER_REG, UART2_ENABLE, 1);
					uart2_init(UART_BAUDRATE_115K2, 3);
					uart2_rec_data_avail_setf(1); 
				#endif
		}
		while(!GPIO_GetPinStatus(GPIO_BUTTON_PORT,GPIO_BUTTON_PIN));
    // Enable the pads

	SetBits16(SYS_CTRL_REG, PAD_LATCH_EN, 1);
}

#endif //_PERIPH_SETUP_H_
