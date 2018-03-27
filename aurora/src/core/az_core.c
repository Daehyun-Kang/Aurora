/**
 * @file   az_core.c
 * @brief  
 * @date   01/05/16
 * @author fware
 *
 * @copyright
 * Copyright (c) Fware, 2013-2016 - All Rights Reserved
 * You may use, distribute and modify this code under the 
 * terms of the "Aurora Source Code License".
 * See the file LICENSE for full license details.\n\n
 *
 * You should have received a copy of  the LICENSE with this file.\n\n
 *
 * If not, please contact to Fware with the information in the file CONTACTS.
 *
 * History:
 *   - 2016-05-01 fware initial create
 */

/* include header files */
#define  ALLOC_ERR_STR
#include "aurora.h"
#undef  ALLOC_ERR_STR

/* declare global variables */
//struct az_malloc_stats az_malloc_stats;

/* declare static variables */
static az_state_t az_core_state = AZ_ST_IDLE;
#ifdef  CONFIG_AZ_DYNAMIC_ION
static az_array_t az_ion_list = {
  .list = NULL,
  .size = AZ_ION_MAX,
  .count = 0,
  .refCount = AZ_REFCOUNT_STATIC_ZERO
};
#endif

/* implement static functions */

/**
 * @fn        function name
 * @brief     function-description
 * @param     input-output-parameters
 * @return    return-value
 * @warning   warnings
 * @exception none
 */


/* implement global functions */

/**
 * @fn        function name
 * @brief     function-description
 * @param     input-output-parameters
 * @return    return-value
 * @warning   warnings
 * @exception none
 */
az_r_t az_core_init()
{
  az_r_t result = AZ_SUCCESS;
  switch (az_core_state) {
    case AZ_ST_IDLE:
      az_core_state = AZ_ST_INIT;
      az_malloc_init();
      az_sys_init();
#ifdef  CONFIG_AZ_LOG
      az_log_ctrl_init();
#endif
      az_ion_init(&az_ion_list);
      az_event_store_init();
      az_event_port_link_pool_init();
      az_event_bus_init();
      az_event_bus_start();

      az_core_state = AZ_ST_NORM;
      break;
    //case AZ_ST_INIT:
    //  result = AZ_ERR(AGAIN);
    //  break;
    default:
      result = AZ_ERR(STATE);
      break;
  }

  return result;
}

az_r_t az_core_deinit()
{
  az_r_t result = AZ_SUCCESS;
  switch (az_core_state) {
    case AZ_ST_IDLE:
      result = AZ_ERR(STATE);
      break;
    case AZ_ST_TINI:
      result = AZ_ERR(AGAIN);
      break;
    default:
      az_core_state = AZ_ST_TINI;
      az_event_bus_stop();
      az_event_bus_denit();
      az_event_port_link_pool_deinit();
      az_event_store_deinit();
      az_ion_deinit();
#ifdef  CONFIG_AZ_LOG
      az_log_ctrl_deinit();
#endif
      az_sys_deinit();

      az_core_state = AZ_ST_IDLE;
      break;
  }

  return result;
}

az_state_t az_core_get_state()
{
  return az_core_state;
}


/* === end of AZ_CORE_C === */
