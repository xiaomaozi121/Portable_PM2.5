/**
****************************************************************************************
*
* @file app_proxr_proj.c
*
* @brief Proximity project source code .
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

/**
 ****************************************************************************************
 * @addtogroup APP
 * @{
 ****************************************************************************************
 */
#include "rwip_config.h"             // SW configuration
#include "periph_setup.h"             // SW configuration
#include "wkupct_quadec.h"             // SW configuration

#if (BLE_APP_PRESENT)

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "app_sec.h"
#include "app_proxr_proj.h"
#include "arch_sleep.h"

#include "co_math.h"                 // Common Maths Definition

#if (NVDS_SUPPORT)
#include "nvds.h"                    // NVDS Definitions
#endif //(NVDS_SUPPORT)
#if(BLE_SAMPLE128)
#include "sample128.h"
#include "sample128_task.h"
uint8_t sample128_placeholder = 0,g_flag_Set_Timer=0;
uint8_t sample128_LED=0;
uint8_t Sample_LED_Blink=100;
uint8_t Flag_Adv=0x00;
my_newer1_addChar sample128_my_new = {0x00};
my_newer2_addChar sample128_2_new={0x00};
uint8_t PM2_5[characristic2_length]={"PM2.5:"};
uint16_t PM2_5_val;
uint8_t uart_buf[5]={0x00};
uint8_t uart_bufcnt=0x00,uart_buf_flag=0x00;
uint8_t PM2_5Cnt=0;
uint8_t Prameter[11];
uint8_t flag_Connect,flag_Connect_kind,flag_PM25=1;
uint8_t flag_WakeUp_Function=0;
uint8_t flag_Uart2_Init=0;
#endif
#ifdef CFG_LED_PORT
#include "gpio.h"
#endif
#ifdef CFG_USER_UART
#include "user_uart.h"  
#endif
#include "arch_sleep.h"

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
*/


/**
 ****************************************************************************************
 * @brief Button press callback function. Registered in WKUPCT driver.
 *
 * @return void.
 ****************************************************************************************
 */

void app_button_press_cb(void)
{
        
#if BLE_PROX_REPORTER	
		if (alert_state.lvl != PROXR_ALERT_NONE)
		{
			app_proxr_alert_stop();	
		}
        else 
#endif         
		
        {
#if BLE_FINDME_LOCATOR            
        if (ke_state_get(TASK_FINDL) == FINDL_CONNECTED)
        {
            app_findl_set_alert();  
        }
		
#endif 
        }
        flag_PM25=0;
        if(GetBits16(SYS_STAT_REG, PER_IS_DOWN))
            periph_init(); 
        
        if (app_ble_ext_wakeup_get())
        {
        
            //Wakeup BLE here
            
#if (EXT_SLEEP_ENABLED)
            app_set_extended_sleep();
#elif (DEEP_SLEEP_ENABLED)
            app_set_deep_sleep();
#else
            app_disable_sleep();
#endif    
            
            SetBits32(GP_CONTROL_REG, BLE_WAKEUP_REQ, 1); 
            app_ble_ext_wakeup_off();
            ke_msg_send_basic(APP_WAKEUP_MSG, TASK_APP, NULL);
        }
        app_button_enable();
        
}

/**
 ****************************************************************************************
 * @brief Enable push button. Register callback function for button press event. Must be called in periph_init();
 *
 * @return void.
 ****************************************************************************************
 */


void app_button_enable(void)
{   
   wkupct_register_callback(app_button_press_cb);
   if (GPIO_GetPinStatus( GPIO_BUTTON_PORT, GPIO_BUTTON_PIN ))
        wkupct_enable_irq(0x100, 0x100, 1, 0); // P1_1, polarity low , 1 event, debouncing time = 0
}

 /**
 ****************************************************************************************
 * @brief app_api function. Project's actions in app_init
 *
 * @return void.
 ****************************************************************************************
*/

void app_init_func(void)
{	
    
#if (BLE_PROX_REPORTER)    
		app_proxr_init(GPIO_ALERT_LED_PORT, GPIO_ALERT_LED_PIN);
   
    alert_state.ll_alert_lvl = PROXR_ALERT_HIGH; //Link Loss default Alert Level 
#elif (BLE_FINDME_LOCATOR)
    app_findl_init();
#endif

#if (BLE_BATT_SERVER)    
    app_batt_init();
#endif
    
#if (BLE_DIS_SERVER)        
    app_dis_init();
#endif
    
#if (BLE_SPOTA_RECEIVER)    
	app_spotar_init();
#endif
	
#if (BLE_SAMPLE128)    
	app_sample128_init();
#endif   
}

/**
 ****************************************************************************************
 * @brief app_api function. Project's actions in app_sec_init during system initialization
 *
 * @return void.
 ****************************************************************************************
*/

void app_sec_init_func(void)
{
    
#if (BLE_APP_SEC)
	app_sec_env.auth = (GAP_AUTH_REQ_NO_MITM_BOND);
#endif
    
}


/******************************************************************************************
 * @brief app_api function. Project's actions in app_connect (device connection)
 *
 * @param[in] param      Received gapc_connection_req_ind.
 *
 * @return bool     true for connection request acception / false rejection.
 ****************************************************************************************
*/

void app_connection_func(struct gapc_connection_req_ind const *param)
{
    
    if (app_env.conidx != GAP_INVALID_CONIDX)
    {
        
        /*--------------------------------------------------------------
        * ENABLE REQUIRED PROFILES
        *-------------------------------------------------------------*/

        #if (BLE_PROX_REPORTER)
        app_proxr_enable();
        #endif //BLE_PROX_REPORTER
        
        #if BLE_BATT_SERVER
        //cur_batt_level = 0;
        app_batt_enable(cur_batt_level, USE_BAT_LEVEL_ALERT, GPIO_BAT_LED_PORT, GPIO_BAT_LED_PIN); //Battery level Alert enabled.
        
        app_batt_poll_start(6000); //10 mins
        #endif // BLE_BATT_SERVER

        #if BLE_FINDME_TARGET
        app_findt_enable();
        #endif // BLE_FINDME_TARGET
        
        #if BLE_FINDME_LOCATOR
        app_findl_enable();
        #endif //BLE_FINDME_LOCATOR	
        
        #if (BLE_APP_PRESENT)
        app_dis_enable_prf(app_env.conhdl);
        #endif
        
        #if (BLE_SPOTA_RECEIVER)
        app_spotar_enable();
        #endif //BLE_SPOTA_RECEIVER

				#if (BLE_SAMPLE128)
					app_sample128_enable();
					//这个timer 是10ms的时基100*10ms=1S
					flag_Connect_kind=1;
					ke_timer_set(APP_SAMPLE128_TIMER,TASK_APP,50);	
        #endif 
				
        ke_state_set(TASK_APP, APP_CONNECTED);
            
        // Retrieve the connection info from the parameters
        app_env.conhdl = param->conhdl;

        app_env.peer_addr_type = param->peer_addr_type;
        memcpy(app_env.peer_addr.addr, param->peer_addr.addr, BD_ADDR_LEN);

# if (BLE_APP_SEC)
            
        // send connection confirmation
        app_connect_confirm(app_sec_env.auth);
        
# else // (BLE_APP_SEC)
        // send connection confirmation
        app_connect_confirm(GAP_AUTH_REQ_NO_MITM_NO_BOND);            
# endif // (BLE_APP_SEC)

        ke_timer_clear(APP_ADV_TIMER, TASK_APP); 
    }
    else
    {
        // No connection has been establish, restart advertising
        app_adv_start();
    }
    
    return;

}

/**
 ****************************************************************************************
 * @brief app_api function. Project advertising function. Setup advertise string.
 *
 * @return void.
 ****************************************************************************************
*/

void app_adv_func(struct gapm_start_advertise_cmd *cmd)
{
    
     //  Device Name Length
    uint8_t device_name_length;
    int8_t device_name_avail_space;
    uint8_t device_name_temp_buf[64];

    cmd->op.code     = GAPM_ADV_UNDIRECT;
    cmd->op.addr_src = GAPM_PUBLIC_ADDR;
    cmd->intv_min    = APP_ADV_INT_MIN;
    cmd->intv_max    = APP_ADV_INT_MAX;
    cmd->channel_map = APP_ADV_CHMAP;

    cmd->info.host.mode = GAP_GEN_DISCOVERABLE;

    /*-----------------------------------------------------------------------------------
     * Set the Advertising Data and the Scan Response Data
     *---------------------------------------------------------------------------------*/
    cmd->info.host.adv_data_len       = APP_ADV_DATA_MAX_SIZE;
    cmd->info.host.scan_rsp_data_len  = APP_SCAN_RESP_DATA_MAX_SIZE;
    
    // Advertising Data
    #if (NVDS_SUPPORT)
    if(nvds_get(NVDS_TAG_APP_BLE_ADV_DATA, &cmd->info.host.adv_data_len,
                &cmd->info.host.adv_data[0]) != NVDS_OK)
    #endif //(NVDS_SUPPORT)
    {
        cmd->info.host.adv_data_len = APP_ADV_DATA_LEN;
        memcpy(&cmd->info.host.adv_data[0], APP_ADV_DATA, cmd->info.host.adv_data_len);
    }

    // Scan Response Data
    #if (NVDS_SUPPORT)
    if(nvds_get(NVDS_TAG_APP_BLE_SCAN_RESP_DATA, &cmd->info.host.scan_rsp_data_len,
                &cmd->info.host.scan_rsp_data[0]) != NVDS_OK)
    #endif //(NVDS_SUPPORT)
    {
        cmd->info.host.scan_rsp_data_len = APP_SCNRSP_DATA_LENGTH;
        memcpy(&cmd->info.host.scan_rsp_data[0], APP_SCNRSP_DATA, cmd->info.host.scan_rsp_data_len);
    }

    // Get remaining space in the Advertising Data - 2 bytes are used for name length/flag
    device_name_avail_space = APP_ADV_DATA_MAX_SIZE - cmd->info.host.adv_data_len - 2;

    // Check if data can be added to the Advertising data
    if (device_name_avail_space > 0)
    {
        // Get the Device Name to add in the Advertising Data (Default one or NVDS one)
        #if (NVDS_SUPPORT)
        device_name_length = NVDS_LEN_DEVICE_NAME;
        if (nvds_get(NVDS_TAG_DEVICE_NAME, &device_name_length, &device_name_temp_buf[0]) != NVDS_OK)
        #endif //(NVDS_SUPPORT)
        {
            // Get default Device Name (No name if not enough space)
            device_name_length = strlen(APP_DEVICE_NAME);
            memcpy(&device_name_temp_buf[0], APP_DEVICE_NAME, device_name_length);
        }

        if(device_name_length > 0)
        {
            // Check available space
            device_name_length = co_min(device_name_length, device_name_avail_space);

            // Fill Length
            cmd->info.host.adv_data[cmd->info.host.adv_data_len]     = device_name_length + 1;
            // Fill Device Name Flag
            cmd->info.host.adv_data[cmd->info.host.adv_data_len + 1] = '\x09';
            // Copy device name
            memcpy(&cmd->info.host.adv_data[cmd->info.host.adv_data_len + 2], device_name_temp_buf, device_name_length);

            // Update Advertising Data Length
            cmd->info.host.adv_data_len += (device_name_length + 2);
        }
    }

#if USE_PUSH_BUTTON
#if defined(CFG_EXT_SLEEP) || defined(CFG_DEEP_SLEEP)    
    app_timer_set(APP_ADV_TIMER, TASK_APP, 5000); //180000 ms -> 3 mins
#endif
#endif
    
    return;
}

/**
 ****************************************************************************************
 * @brief app_api function. Project's actions in app_disconnect
 *
 * @param[in] taskid     App task's id.
 * @param[in] param      Received gapc_disconnect_ind msg.
 *
 * @return void.            
 ****************************************************************************************
*/

void app_disconnect_func(ke_task_id_t task_id, struct gapc_disconnect_ind const *param)
{
    
    uint8_t state = ke_state_get(task_id);
    
#if BLE_BATT_SERVER
	app_batt_poll_stop();
#endif // BLE_BATT_SERVER
    
    if ((state == APP_SECURITY) || (state == APP_CONNECTED)  || (state == APP_PARAM_UPD))
    {
        // Restart Advertising
        app_adv_start();
    }
    else
    {
        // We are not in a Connected State
        ASSERT_ERR(0);
    }
						
}

/**
 ****************************************************************************************
 * @brief app_api function. Project's actions for profiles' database initialization
 *
 * @return void.
 ****************************************************************************************
*/

bool app_db_init_func(void)
{
    
    // Indicate if more services need to be added in the database
    bool end_db_create = false;
    
    // Check if another should be added in the database
    if (app_env.next_prf_init < APP_PRF_LIST_STOP)
    {
        switch (app_env.next_prf_init)
        {
            #if (BLE_PROX_REPORTER)
            case (APP_PROXR_TASK):
            {
                // Add proxr Service in the DB
                app_proxr_create_db_send();
            } break;
            #endif //BLE_PROX_REPORTER
            #if (BLE_BATT_SERVER)
            case (APP_BASS_TASK):
            {   
                app_batt_create_db();
            } break;
            #endif //BLE_BATT_SERVER
            #if (BLE_DIS_SERVER)
            case (APP_DIS_TASK):
            {
                app_dis_create_db_send();
            } break;
            #endif //BLE_DIS_SERVER
            #if (BLE_SPOTA_RECEIVER)
            case (APP_SPOTAR_TASK):
            {
                // Add spotar Service in the DB
                app_spotar_create_db();
            } break;
						#endif //BLE_SPOTA_RECEIVER
						#if (BLE_SAMPLE128)
						case (APP_SAMPLE128_TASK):
            {
                // Add spotar Service in the DB
                app_sample128_create_db_send();
            } break;
						#endif
            default:
            {
                ASSERT_ERR(0);
            } break;

        }
        // Select following service to add
        app_env.next_prf_init++;
    }
    else
    {
        end_db_create = true;
    }

    return end_db_create;
}

/**
 ****************************************************************************************
 * @brief app_api function. Called upon connection param's update rejection
 *
 * @param[in] status        Error code
 *
 * @return void.
 ****************************************************************************************
*/

void app_update_params_rejected_func(uint8_t status)
{
    return;
}

/**
 ****************************************************************************************
 * @brief app_api function. Sends request to update connection's paramaters.
 *
 * @return void.
 ****************************************************************************************
*/
void app_param_update_func(void)
{

    return;
}

/**
 ****************************************************************************************
 * @brief app_api function. Project configures GAPM. Called upon reset completion
 *
 * @param[in] task_id    id of the kernel task calling this function
 * @param[in] cmd        parameters to pass to the stack
 *
 * @return void.
 ****************************************************************************************
*/

void app_configuration_func(ke_task_id_t const task_id, struct gapm_set_dev_config_cmd *cmd)
{
    // set device configuration
    cmd->operation = GAPM_SET_DEV_CONFIG;
    // Device Role
    cmd->role = GAP_PERIPHERAL_SLV;
    // Device IRK
    // cmd->irk = ; TODO NOT set

    // Device Appearance
    #if (BLE_APP_HT)
    cmd->appearance = 728;
    #else
    // Device Appearance
    cmd->appearance = 0x0000;
    #endif

    // Device Appearance write permission requirements for peer device
    cmd->appearance_write_perm = GAPM_WRITE_DISABLE;
    // Device Name write permission requirements for peer device
    cmd->name_write_perm = GAPM_WRITE_DISABLE;
    
    // Maximum trasnimt unit size
    cmd->max_mtu = 23;

    // Peripheral only: *****************************************************************
    // Slave preferred Minimum of connection interval
    cmd->con_intv_min = 8;         // 10ms (8*1.25ms)
    // Slave preferred Maximum of connection interval
    cmd->con_intv_max = 16;        // 20ms (16*1.25ms)
    // Slave preferred Connection latency
    cmd->con_latency  = 0;
    // Slave preferred Link supervision timeout
    cmd->superv_to    = 200;

    // Privacy settings bit field
    cmd->flags = 0;
    
    return;   
}

/**
 ****************************************************************************************
 * @brief app_api function. Handles Set device's configuration complete message
 *
 * @return void.
 ****************************************************************************************
*/

void app_set_dev_config_complete_func(void)
{
    // We are now in Initialization State
    ke_state_set(TASK_APP, APP_DB_INIT);

    // Add the first required service in the database
    if (app_db_init())
    {
        // No service to add in the DB -> Start Advertising
        app_adv_start();
    }
 
    return;
}

/**
 ****************************************************************************************
 * @brief app_api function. Handles Update connection's params complete message
 *
 * @return void.
 ****************************************************************************************
*/

void app_update_params_complete_func(void)
{
    return;
}

/**
 ****************************************************************************************
 * @brief app_api function. Handles undirect advertising completion.
 *
 * @param[in] status        Command complete message status
 *
 * @return void.
 ****************************************************************************************
*/

void app_adv_undirect_complete(uint8_t status)
{
    
    ke_timer_clear(APP_ADV_TIMER, TASK_APP);
    return;
}

/**
 ****************************************************************************************
 * @brief app_api function. Handles direct advertising completion.
 *
 * @param[in] status        Command complete message status
 *
 * @return void.
 ****************************************************************************************
*/

void app_adv_direct_complete(uint8_t status)
{
    return;
}

/**
 ****************************************************************************************
 * @brief app_api function. Handles Database creation. Start application.
 *
 * @return void.
 ****************************************************************************************
*/

void app_db_init_complete_func(void)
{
    
    app_adv_start();
    
    return;
}

/**
 ****************************************************************************************
 * @brief app_api function. Handles scan procedure completion.
 *
 * @return void.
 ****************************************************************************************
*/

void app_scanning_completed_func(void)
{
    
}

/**
 ****************************************************************************************
 * @brief app_api function. Fill gapm_start_connection_cmd msg params.
 *
 * @param[in] msg        gapm_start_connection_cmd message
 *
 * @return void.
 ****************************************************************************************
*/

void app_connect_func(struct gapm_start_connection_cmd *msg)                           
{
    
}

/**
 ****************************************************************************************
 * @brief app_api function. Handles reception of gapm_adv_report_ind msg.
 *
 * @param[in] param     gapm_adv_report_ind message
 *
 * @return void.
 ****************************************************************************************
*/

void app_adv_report_ind_func(struct gapm_adv_report_ind *param)
{
    
}

/**
 ****************************************************************************************
 * @brief app_api function. Handles connection request failure.
 *
 * @return void.
 ****************************************************************************************
*/

void app_connect_failed_func(void)
{
    
}

#if (BLE_APP_SEC)
void app_send_pairing_rsp_func(struct gapc_bond_req_ind *param)
{
    
    struct gapc_bond_cfm* cfm = KE_MSG_ALLOC(GAPC_BOND_CFM, TASK_GAPC, TASK_APP, gapc_bond_cfm);

    cfm->request = GAPC_PAIRING_RSP;
    cfm->accept = true;

    // OOB information
    cfm->data.pairing_feat.oob            = GAP_OOB_AUTH_DATA_NOT_PRESENT;
    // Encryption key size
    cfm->data.pairing_feat.key_size       = KEY_LEN;
    // IO capabilities
    cfm->data.pairing_feat.iocap          = GAP_IO_CAP_NO_INPUT_NO_OUTPUT;
    // Authentication requirements
    cfm->data.pairing_feat.auth           = GAP_AUTH_REQ_NO_MITM_BOND;
    //Security requirements
    cfm->data.pairing_feat.sec_req        = GAP_NO_SEC;
    //Initiator key distribution
    cfm->data.pairing_feat.ikey_dist      = GAP_KDIST_SIGNKEY;
    //Responder key distribution
    cfm->data.pairing_feat.rkey_dist      = GAP_KDIST_ENCKEY;
    
    ke_msg_send(cfm);
}

void app_send_tk_exch_func(struct gapc_bond_req_ind *param)
{
    struct gapc_bond_cfm* cfm = KE_MSG_ALLOC(GAPC_BOND_CFM, TASK_GAPC, TASK_APP, gapc_bond_cfm);
    uint32_t pin_code = app_sec_gen_tk();
    cfm->request = GAPC_TK_EXCH;
    cfm->accept = true;
    
    memset(cfm->data.tk.key, 0, KEY_LEN);
    
    cfm->data.tk.key[3] = (uint8_t)((pin_code & 0xFF000000) >> 24);
    cfm->data.tk.key[2] = (uint8_t)((pin_code & 0x00FF0000) >> 16);
    cfm->data.tk.key[1] = (uint8_t)((pin_code & 0x0000FF00) >>  8);
    cfm->data.tk.key[0] = (uint8_t)((pin_code & 0x000000FF) >>  0);
    
    ke_msg_send(cfm);
}

void app_send_irk_exch_func(struct gapc_bond_req_ind *param)
{
    return;
}

void app_send_csrk_exch_func(struct gapc_bond_req_ind *param)
{
    struct gapc_bond_cfm* cfm = KE_MSG_ALLOC(GAPC_BOND_CFM, TASK_GAPC, TASK_APP, gapc_bond_cfm);

    // generate ltk
    app_sec_gen_ltk(param->data.key_size);

    cfm->request = GAPC_CSRK_EXCH;

    cfm->accept = true;

    memset((void *) cfm->data.csrk.key, 0, KEY_LEN);
    memcpy((void *) cfm->data.csrk.key, (void *)"\xAB\xAB\x45\x55\x23\x01", 6);

    ke_msg_send(cfm);

}

void app_send_ltk_exch_func(struct gapc_bond_req_ind *param)
{
    
    struct gapc_bond_cfm* cfm = KE_MSG_ALLOC(GAPC_BOND_CFM, TASK_GAPC, TASK_APP, gapc_bond_cfm);
    
    // generate ltk
    app_sec_gen_ltk(param->data.key_size);

    cfm->request = GAPC_LTK_EXCH;

    cfm->accept = true;

    cfm->data.ltk.key_size = app_sec_env.key_size;
    cfm->data.ltk.ediv = app_sec_env.ediv;

    memcpy(&(cfm->data.ltk.randnb), &(app_sec_env.rand_nb) , RAND_NB_LEN);
    memcpy(&(cfm->data.ltk.ltk), &(app_sec_env.ltk) , KEY_LEN);

    ke_msg_send(cfm);

}

void app_paired_func(void)
{
    return;
}

bool app_validate_encrypt_req_func(struct gapc_encrypt_req_ind *param)
{
    return true;
}

void app_sec_encrypt_ind_func(void)
{
    
    return; 
}

void app_sec_encrypt_complete_func(void)
{
    return;
}

void app_mitm_passcode_entry_func(ke_task_id_t const src_id, ke_task_id_t const dest_id)
{
    return;
}
#endif //BLE_APP_SEC

/**
 ****************************************************************************************
 * @brief Advertise timer handler. Set device in deep sleep mode
 *
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance (TASK_GAP).
 * @param[in] src_id    ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 * @return int.
 ****************************************************************************************
*/

int app_adv_timer_handler(ke_msg_id_t const msgid,
                                   void const *param,
                                   ke_task_id_t const dest_id,
                                   ke_task_id_t const src_id)
{
  
    app_adv_stop();
    
    ke_timer_clear(APP_ADV_TIMER, TASK_APP);
    
    app_set_deep_sleep();
    
    ke_timer_clear(APP_ADV_TIMER, TASK_APP);
    
    //app_proxr_alert_stop();
    
    app_ble_ext_wakeup_on();
    app_button_enable();
		flag_Connect=0;
    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles APP_WAKEUP_MSG sent when device exits deep sleep. Trigerred by button press.
 *
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance (TASK_GAP).
 * @param[in] src_id    ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 * @return int.
 ****************************************************************************************
*/

int app_wakeup_handler(ke_msg_id_t const msgid,
									void *param,
									ke_task_id_t const dest_id,
									ke_task_id_t const src_id)
{
	// If state is not idle, ignore the message
	if (ke_state_get(dest_id) == APP_CONNECTABLE)
		app_adv_start();
	/*
	if(flag_WakeUp_Function)
	{
		GPIO_SetActive(LASER_STATE_PORT,LASER_STATE_PIN);
		if(flag_Connect)
		{
				sample128_C2_val_Change();
		}
		else
		{
				app_adv_start();
		}
	}
	*/
	return (KE_MSG_CONSUMED);
}
#if( BLE_SAMPLE128)
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
 //memcpy(&req->sample128_1_val,&sample128_my_new,sizeof(my_newer1_addChar)); // default
 //req->sample128_1_val = 0x01; // default value for sample128 characteristic 1
 //req->sample128_2_val = 0xff; // default value for sample128 characteristic 2
 //memcpy(&req->sample128_1_val,&sample128_2_new,sizeof(my_newer2_addChar)); 
 req->feature = 0x00; // client CFG notify/indicate disabled
 // Send the message
 ke_msg_send(req);
}

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
	memcpy(&sample128_my_new,&param->val,sizeof(my_newer1_addChar));
	GPIO_SetInactive(GPIO_PORT_0 , GPIO_PIN_7);
	Prameter[2]=sample128_my_new[0];
	Prameter[3]=sample128_my_new[1];
	Prameter[4]=sample128_my_new[2];
	Prameter[5]=sample128_my_new[3];
	Prameter[6]=sample128_my_new[4];
	Prameter[7]=sample128_my_new[5];
	Prameter[8]=sample128_my_new[6];
	Prameter[9]=sample128_my_new[7];
	flag_Connect_kind=2;
	ke_timer_set(APP_SAMPLE128_TIMER,TASK_APP,300);	
	return (KE_MSG_CONSUMED);
}
/**
****************************************************************************************
* @brief Handles write of 2st characteristic event indication from sample128 profile
*
* @param[in] msgid Id of the message received.
* @param[in] param Pointer to the parameters of the message.
* @param[in] dest_id ID of the receiving task instance (TASK_GAP).
* @param[in] src_id ID of the sending task instance.
*
* @return If the message was consumed or not.
****************************************************************************************
*/
int sample128_C2_val_ind_handler(ke_msg_id_t const msgid,
																struct sample128_C2_val_ind const *param,
																ke_task_id_t const dest_id,
																ke_task_id_t const src_id)
{
	//sample128_placeholder = param->val;
	memcpy(&sample128_2_new,&param->val,sizeof(my_newer2_addChar));

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

	//表征是否处于连接模式
	flag_Connect=1;
	if(flag_Connect_kind==2)
	{
		uart2_write(Prameter,11,NULL);
	}
	else if(flag_Connect_kind==1)
	{
		sample128_C2_val_Change();
	}
	ke_timer_clear(APP_SAMPLE128_TIMER,TASK_APP);
	return (KE_MSG_CONSUMED);
}
void sample128_C2_val_Change(void)
{
		struct sample128_upd_char2_req *req = KE_MSG_ALLOC(
																	SAMPLE128_UPD_CHAR2_REQ,
																	TASK_SAMPLE128,
																	TASK_APP,
																	sample128_upd_char2_req
																	 );
		memcpy(&req->val,PM2_5,sizeof(my_newer2_addChar));
		req->conhdl = app_env.conhdl;
		ke_msg_send(req);
}
//CharToHex()
/////////////////////////////////////////////////////////////////////
unsigned char CharToHex(unsigned char bHex)
{
	if((bHex>=0)&&(bHex<=9))
			bHex += 0x30;
	else if((bHex>=10)&&(bHex<=15))//????
			bHex += 0x37;
	else bHex = 0xff;
	return bHex;
}
/////////////////////////////////////////////////////////////////////
//HexToChar()
/////////////////////////////////////////////////////////////////////
unsigned char HexToChar(unsigned char bChar)
{
	if((bChar>=0x30)&&(bChar<=0x39))
			bChar -= 0x30;
	else if((bChar>=0x41)&&(bChar<=0x46))//????
			bChar -= 0x37;
	else if((bChar>=0x61)&&(bChar<=0x66))//????
			bChar -= 0x57;
	else bChar = 0xff;
	return bChar;
}
#endif
#endif  //BLE_APP_PRESENT
/// @} APP
