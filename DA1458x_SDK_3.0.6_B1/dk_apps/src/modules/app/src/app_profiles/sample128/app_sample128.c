/**
 ****************************************************************************************
 *
 * @file app_dis.c
 *
 * @brief Device Information Service Application entry point
 *
 * Copyright (C) RivieraWaves 2009-2013
 *
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @addtogroup APP
 * @{
 ****************************************************************************************
 */

#include "rwip_config.h"     // SW configuration

#if (BLE_SAMPLE128)

/*
 * INCLUDE FILES
 ****************************************************************************************
*/

#include "app_sample128.h"           // Device Information Service Application Definitions
#include "app_sample128_task.h"      // Device Information Service Application Task API
#include "app.h"                     // Application Definitions
#include "app_task.h"                // Application Task Definitions
#include "sample128_task.h"          // Health Thermometer Functions
#include "sample128.h"
#include "app_task_handlers.h"
#include "app_xAPP_proj.h"
/*
 * LOCAL VARIABLES DECLARATIONS
 ****************************************************************************************
 */

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

void app_sample128_init(void)
{
    return;
}

void app_sample128_create_db_send(void)
{
 struct sample128_create_db_req *req = KE_MSG_ALLOC(
																									SAMPLE128_CREATE_DB_REQ,
																									TASK_SAMPLE128,
																									TASK_APP,
																									sample128_create_db_req
																								);
 ke_msg_send(req);
}
void app_sample128_enable(void)
{
 // Allocate the message
 struct sample128_enable_req* req = KE_MSG_ALLOC(
																									SAMPLE128_ENABLE_REQ,
																									TASK_SAMPLE128,
																									TASK_APP,
																									sample128_enable_req
																									);
 req->conhdl = app_env.conhdl;
 req->sec_lvl = PERM(SVC, ENABLE);
 memcpy(&req->sample128_1_val,&sample128_my_new,sizeof(my_new_characristic)); // default
 //req->sample128_1_val = 0x01; // default value for sample128 characteristic 1
 //req->sample128_2_val = 0xff; // default value for sample128 characteristic 2
 memcpy(&req->sample128_1_val,&sample128_2_new,sizeof(my_newer_addChar)); 
 req->feature = 0x00; // client CFG notify/indicate disabled
 // Send the message
 ke_msg_send(req);
}

#endif //BLE_DIS_SERVER

/// @} APP
