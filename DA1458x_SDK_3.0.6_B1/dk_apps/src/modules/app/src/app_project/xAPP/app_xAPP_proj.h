/**
****************************************************************************************
*
* @file app_template_proj.h
*
* @brief Template application header file.
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

#ifndef APP_TEMPLATE_PROJ_H_
#define APP_TEMPLATE_PROJ_H_

/**
 ****************************************************************************************
 * @addtogroup APP
 * @ingroup RICOW
 *
 * @brief 
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwble_config.h"
#include "app_task.h"                  // application task
#include "gapc_task.h"                 // gap functions and messages
#include "gapm_task.h"                 // gap functions and messages
#include "app.h"                       // application definitions
#include "co_error.h"                  // error code definitions
#include "smpc_task.h"                 // error code definitions
#if (BLE_DIS_SERVER)
#include "app_dis.h"
#include "app_dis_task.h"
#endif 
#if (BLE_SAMPLE128)
#include "sample128.h"
#include "sample128_task.h"
#endif
extern uint8_t sample128_placeholder;
extern uint8_t sample128_LED;
extern uint8_t Sample_LED_Blink;
extern uint8_t Flag_Adv;
extern my_newer1_addChar sample128_my_new;
extern my_newer2_addChar sample128_2_new;
extern uint8_t PM2_5[characristic2_length];
extern uint16_t PM2_5_val;
extern uint8_t uart_buf[5];
extern uint8_t uart_bufcnt,uart_buf_flag;
/****************************************************************************
Add here supported profiles' application header files.
i.e.

*****************************************************************************/

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/*
 * DEFINES
 ****************************************************************************************
 */

/****************************************************************************
Define device name. Used in Advertise string
*****************************************************************************/

#define APP_DEVICE_NAME "DA14580 TEMPL"

/**
 * Default Advertising data
 * --------------------------------------------------------------------------------------
 * x02 - Length
 * x01 - Flags
 * x06 - LE General Discoverable Mode + BR/EDR Not Supported
 * --------------------------------------------------------------------------------------
 * x03 - Length
 * x03 - Complete list of 16-bit UUIDs available
 * x09\x18 - Health Thermometer Service UUID
 *   or
 * x00\xFF - Nebulization Service UUID
 * --------------------------------------------------------------------------------------
 */

#define APP_ADV_DATA        "\x07\x01\x03\x18"
#define APP_ADV_DATA_LEN    (0)

/**
 * Default Scan response data
 * --------------------------------------------------------------------------------------
 * x09                             - Length
 * xFF                             - Vendor specific advertising type
 * x00\x60\x52\x57\x2D\x42\x4C\x45 - "RW-BLE"
 * --------------------------------------------------------------------------------------
 */
#define APP_SCNRSP_DATA         "\x02\xFF\x00"
#define APP_SCNRSP_DATA_LENGTH  (0)
/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/// @} APP
void app_sample128_init(void);

/**
 ****************************************************************************************
 * @brief Add a Device Information Service instance in the DB
 ****************************************************************************************
 */
void app_sample128_create_db_send(void);


/**
 ****************************************************************************************
 * @brief Enable the Device Information Service
 ****************************************************************************************
 */
void app_sample128_enable(void);
/**
****************************************************************************************
* @brief Handles sample128 profile database creation confirmation.
* @return If the message was consumed or not.
****************************************************************************************
*/
int sample128_create_db_cfm_handler(ke_msg_id_t const msgid,
																			 struct sample128_create_db_cfm const *param,
																			 ke_task_id_t const dest_id,
																			 ke_task_id_t const src_id);
/**
****************************************************************************************
* @brief Handles disable indication from the sample128 profile.
* @return If the message was consumed or not.
****************************************************************************************
*/
int sample128_disable_ind_handler(ke_msg_id_t const msgid,
																			 struct sample128_disable_ind const *param,
																			 ke_task_id_t const dest_id,
																			 ke_task_id_t const src_id);
/**
****************************************************************************************
* @brief Handles write of 1st characteristic event indication from sample128 profile
* @return If the message was consumed or not.
****************************************************************************************
*/
int sample128_val_ind_handler(ke_msg_id_t const msgid,
																			 struct sample128_val_ind const *param,
																			 ke_task_id_t const dest_id,
																			 ke_task_id_t const src_id);
int sample128_C2_val_ind_handler(ke_msg_id_t const msgid,
																struct sample128_C2_val_ind const *param,
																ke_task_id_t const dest_id,
																ke_task_id_t const src_id);																			
/**
****************************************************************************************
* @brief Handles timer timeout
* @return If the message was consumed or not.
****************************************************************************************
*/																			 
int sample128_timer_handler(ke_msg_id_t const msgid,
													 struct gapm_cmp_evt const *param,
													 ke_task_id_t const dest_id,
													 ke_task_id_t const src_id);
void gpio0_callback(void);													 
unsigned char CharToHex(unsigned char bHex);
unsigned char HexToChar(unsigned char bChar);
/**
****************************************************************************************
* @brief Handles timer timeout
* @return If the message was consumed or not.
****************************************************************************************
*/
#endif //APP_TEMPLATE_PROJ_H_
