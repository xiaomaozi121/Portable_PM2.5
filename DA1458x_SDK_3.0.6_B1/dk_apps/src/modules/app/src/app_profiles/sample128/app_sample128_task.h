/**
 ****************************************************************************************
 *
 * @file app_dis_task.h
 *
 * @brief Header file - APPDISTASK.
 *
 * Copyright (C) RivieraWaves 2009-2013
 *
 *
 ****************************************************************************************
 */

#ifndef APP_SAMPLE128_TASK_H_
#define APP_SAMPLE128_TASK_H_

/**
 ****************************************************************************************
 * @addtogroup APPDISTASK Task
 * @ingroup APPDIS
 * @brief Device Information Service Application Task
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"     // SW configuration

#if (BLE_SAMPLE128)

#include "sample128_task.h"
#include "sample128.h"
#include "ke_task.h"
#include "app_sample128_task.h"
#include "app_sample128.h"
#include "app_xAPP_proj.h"
/*
 * DEFINES
 ****************************************************************************************
 */

/// Maximal number of APP DIS Task instances
#define APP_SAMPLE128_IDX_MAX        (1)

/*
 * TASK DESCRIPTOR DECLARATIONS
 ****************************************************************************************
 */

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
/**
****************************************************************************************
* @brief Handles write of 2st characteristic event indication from sample128 profile
* @return If the message was consumed or not.
****************************************************************************************
*/																			
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
												 
#endif //(BLE_DIS_SERVER)

/// @} APPDISTASK

#endif //APP_DIS_TASK_H_
