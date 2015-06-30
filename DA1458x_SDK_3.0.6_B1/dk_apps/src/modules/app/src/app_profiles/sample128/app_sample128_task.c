/**
 ****************************************************************************************
 *
 * @file app_dis_task.c
 *
 * @brief Device Information Service Application Task
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

#include "rwip_config.h"        // SW Configuration
#include <string.h>             // srtlen()

#if (BLE_SAMPLE128)

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "sample128_task.h"          // Device Information Service Server Task API
#include "sample128.h"               // Device Information Service Definitions
//#include "app_sample128.h"           // Device Information Service Application Definitions
//#include "app_sample128_task.h"      // Device Information Service Application Task API
#include "app_task.h"           		 // Application Task API
#include "gapm_task.h"
#include "gpio.h"
#include "app_xAPP_proj.h"

#include "gap.h"
#include "gapc.h"
#include "gattc_task.h"
#include "atts_util.h"
#include "attm_cfg.h"
#include "attm_db.h"
#include "prf_utils.h"
/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Handles DIS Server profile database creation confirmation.
 *
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance (TASK_GAP).
 * @param[in] src_id    ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */

/**
****************************************************************************************
* @brief Handles Sample128 profile database creation confirmation.
*
* @param[in] msgid Id of the message received.
* @param[in] param Pointer to the parameters of the message.
* @param[in] dest_id ID of the receiving task instance .
* @param[in] src_id ID of the sending task instance.
*
* @return If the message was consumed or not.
****************************************************************************************
*/
int sample128_create_db_cfm_handler(ke_msg_id_t const msgid,
																			 struct sample128_create_db_cfm const *param,
																				ke_task_id_t const dest_id,
																			 ke_task_id_t const src_id)
{
	 // If state is not idle, ignore the message
	 if (ke_state_get(dest_id) == APP_DB_INIT)
	 {
		 // Inform the Application Manager
		 struct app_module_init_cmp_evt *cfm = KE_MSG_ALLOC(APP_MODULE_INIT_CMP_EVT,
		 TASK_APP, TASK_APP,
		 app_module_init_cmp_evt);
		 cfm->status = param->status;
		 ke_msg_send(cfm);
	 }
	 return (KE_MSG_CONSUMED);
}
/**
****************************************************************************************
* @brief Handles disable indication from Sample128 profile.
*
* @param[in] msgid Id of the message received.
* @param[in] param Pointer to the parameters of the message.
* @param[in] dest_id ID of the receiving task instance.
* @param[in] src_id ID of the sending task instance.
*
* @return If the message was consumed or not.
****************************************************************************************
*/
int sample128_disable_ind_handler(ke_msg_id_t const msgid,
																 struct sample128_disable_ind const *param,
																 ke_task_id_t const dest_id,
																 ke_task_id_t const src_id)
{
 return (KE_MSG_CONSUMED);
}
/**
****************************************************************************************
* @brief Handles write of 1st characteristic event indication from sample128 profile
*
* @param[in] msgid Id of the message received.
* @param[in] param Pointer to the parameters of the message.
* @param[in] dest_id ID of the receiving task instance (TASK_GAP).
* @param[in] src_id ID of the sending task instance.
*
* @return If the message was consumed or not.
****************************************************************************************
*/
int sample128_val_ind_handler(ke_msg_id_t const msgid,
																struct sample128_val_ind const *param,
																ke_task_id_t const dest_id,
																ke_task_id_t const src_id)
{
	//sample128_placeholder = param->val;
	memcpy(&sample128_my_new,&param->val,sizeof(my_new_characristic));
  return (KE_MSG_CONSUMED);
}

/**
****************************************************************************************
* @brief Handles timer timeout
* @return If the message was consumed or not.
****************************************************************************************
*/
int sample128_timer_handler(ke_msg_id_t const msgid,
																 struct gapm_cmp_evt const *param,
																 ke_task_id_t const dest_id,
																 ke_task_id_t const src_id)
{
	uint8_t Tmpi;
	my_newer_addChar Tmp_Buff;
	ke_timer_set(APP_SAMPLE128_TIMER,TASK_APP,50);
	sample128_placeholder++;
	struct sample128_upd_char2_req *req = KE_MSG_ALLOC(
																	SAMPLE128_UPD_CHAR2_REQ,
																	TASK_SAMPLE128,
																	TASK_APP,
																	sample128_upd_char2_req
																	 );
	sample128_LED++;
	if(sample128_LED%2==0)
	{
		GPIO_SetActive(GPIO_PORT_1,GPIO_PIN_0);
	}
	else
	{
		GPIO_SetInactive(GPIO_PORT_1,GPIO_PIN_0);
	}
	for(Tmpi=0;Tmpi<characristic2_length;Tmpi++)
	{
		Tmp_Buff[Tmpi]=(uint8_t)(sample128_LED+Tmpi);
	}
	memcpy(&req->val,Tmp_Buff,sizeof(my_newer_addChar));
	//req->val = sample128_placeholder;
	req->conhdl = app_env.conhdl;
	ke_msg_send(req);
	return (KE_MSG_CONSUMED);
}
 /* GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

#endif //(BLE_DIS_SERVER)

/// @} APP
