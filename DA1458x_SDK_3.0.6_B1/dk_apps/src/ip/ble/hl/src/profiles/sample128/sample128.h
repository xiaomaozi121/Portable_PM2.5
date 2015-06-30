/**
 ****************************************************************************************
 *
 * @file sample128.h
 *
* @brief 128 UUID service. sample code
 *
 * Copyright (C) 2013 Dialog Semiconductor GmbH and its Affiliates, unpublished work
 * This computer program includes Confidential, Proprietary Information and is a Trade Secret 
 * of Dialog Semiconductor GmbH and its Affiliates. All use, disclosure, and/or 
 * reproduction is prohibited unless authorized in writing. All Rights Reserved.
 *
 ****************************************************************************************
 */

#ifndef SAMPLE128_H_
#define SAMPLE128_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#if (BLE_SAMPLE128)

#include "ke_task.h"
#include "atts.h"
#include "prf_types.h"
#define characristic1_length 				8
#define characristic2_length 				14

typedef unsigned    char my_newer1_addChar[characristic1_length];
typedef unsigned 		char my_newer2_addChar[characristic2_length];
//typedef unsigned 		char my_newer3_addChar[characristic3_length];
/*
 * ENUMERATIONS
 ****************************************************************************************
 */

/// Handles offsets
enum
{
    SAMPLE128_1_IDX_SVC,

    SAMPLE128_1_IDX_CHAR,
    SAMPLE128_1_IDX_VAL,

    SAMPLE128_2_IDX_CHAR,
    SAMPLE128_2_IDX_VAL,
    SAMPLE128_2_IDX_CFG,
    
    SAMPLE128_1_IDX_NB,
};

///Characteristics Code for Write Indications
enum
{
    SAMPLE128_ERR_CHAR,
    SAMPLE128_1_CHAR,
    SAMPLE128_2_CFG,
		SAMPLE128_2_CHAR,
};

/*
 * STRUCTURES
 ****************************************************************************************
 */

/// sample128 environment variable
struct sample128_env_tag
{
    /// Connection Information
    struct prf_con_info con_info;

    /// Sample128  svc Start Handle
    uint16_t sample128_shdl;
    
    //Notification property status
    uint8_t feature;

};

/*
 *  SAMPLE128 PROFILE ATTRIBUTES VALUES DECLARATION
 ****************************************************************************************
 */

/// sample128 Service
extern const struct att_uuid_128 sample128_svc;
/// sample128_1 - Characteristic
extern struct att_char128_desc sample128_1_char;
/// sample128_1 - Value
extern const struct att_uuid_128 sample128_1_val;
/// sample128_2 - Characteristic
extern struct att_char128_desc sample128_2_char;
/// sample128_2 - Value
extern const struct att_uuid_128 sample128_2_val;

/*
 * GLOBAL VARIABLE DECLARATIONS
 ****************************************************************************************
 */

extern struct sample128_env_tag sample128_env;
/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initialization of the SAMPLE128 module.
 * This function performs all the initializations of the SAMPLE128 module.
 ****************************************************************************************
 */
void sample128_init(void);

/**
 ****************************************************************************************
 * @brief Send value change to application.
 * @param val Value.
 ****************************************************************************************
 */
 
void sample128_send_val(my_newer1_addChar val);

/**
 ****************************************************************************************
 * @brief Send value change to application.
 * @param val Value.
 ****************************************************************************************
 */
 void sample128_C2_send_val(my_newer2_addChar val);
/**
 ****************************************************************************************
 * @brief Disable role.
 ****************************************************************************************
 */
void sample128_disable(void);

/**
 ****************************************************************************************
 * @brief Send update char2 confirmation message
 ****************************************************************************************
 */
void sample128_upd_char2_cfm_send(uint8_t status);
#endif //BLE_SAMPLE128

#endif // SAMPLE128_H_
