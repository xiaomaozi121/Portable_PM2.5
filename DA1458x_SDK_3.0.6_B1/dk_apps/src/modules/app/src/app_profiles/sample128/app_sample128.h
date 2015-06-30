/**
 ****************************************************************************************
 *
 * @file app_dis.h
 *
 * @brief Device Information Service Application entry point
 *
 * Copyright (C) RivieraWaves 2009-2013
 *
 *
 ****************************************************************************************
 */

#ifndef APP_SAMPLE128_H_
#define APP_SAMPLE128_H_

/**
 ****************************************************************************************
 * @addtogroup APP
 *
 * @brief Device Information Service Application entry point.
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"     // SW Configuration

#if (BLE_SAMPLE128)

#include <stdint.h>          // Standard Integer Definition
#include <co_bt.h>
#include "ble_580_sw_version.h"
#include "app_sw_version.h"
/*
 * DEFINES
 ****************************************************************************************
 */


#if (BLE_APP_PRESENT)

#endif //(BLE_APP_PRESENT)
#define APP_SAMPLE128_FEATURES                (0)
/*
 * FUNCTIONS DECLARATION
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 *
 * Device Information Service Application Functions
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initialize Device Information Service Application
 ****************************************************************************************
 */
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

#endif //BLE_DIS_SERVER

/// @} APP

#endif //APP_DIS_H_
