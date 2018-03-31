/**
 * @file   az_timer.c
 * @brief  
 * @date   20/05/16
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
 *   - 2016-05-20 fware initial create
 */

/* include header files */
#include "az_timer.h"

/* declare global variables */

/* declare static variables */

/* implement static functions */

/* implement global functions */

/**
 * @fn        function name
 * @brief     function-description
 * @param     input-output-parameters
 * @return    return-value
 * @warning   warnings
 * @exception none
 */
az_ion_id_t  az_timer_create(const char *name, az_uint64_t interval,  int repeat, void (*handler)(void *arg), void *arg, az_timer_t *pTimer)
{
  az_r_t result = AZ_SUCCESS;
  az_timer_t t;

  az_assert(NULL != name);
  az_assert(0 != repeat);
  az_assert(NULL != pTimer);

  do {
    az_if_arg_null_break(name, result);
    az_if_arg_null_break(pTimer, result);

    t = *pTimer;

    if (NULL == t) {
      t = az_malloc(sizeof(az_timer_entity_t));
      az_if_alloc_null_break(t, result);
      az_ion_invalidate(&t->ion, 0);
    }
    az_assert(t->ion.id == AZ_SYS_IO_INVALID);
    strncpy(t->name, name, sizeof(t->name));
    result = az_sys_timer_create(name, interval, repeat, handler, arg, &(t->sys_timer));
    if (AZ_SUCCESS != result) {
      if (AZ_REFCOUNT_IS_ZERO(&t->ion.refCount)) {
        az_free(t);
      }
      break;
    } 
    
    t->ion.id = t->sys_timer;

    result = az_ion_register(&(t->ion), AZ_ION_TYPE_TIMER);
    if (AZ_SUCCESS != result) {
      az_sys_timer_delete(t->sys_timer);
      if (AZ_REFCOUNT_IS_ZERO(&t->ion.refCount)) {
        az_free(t);
        *pTimer = NULL;
      }
      break;
    } else {
      az_refcount_atomic_inc(&t->ion.refCount);
    }
    *pTimer = t;
  } while (0);

  return (result < 0)? (az_ion_id_t)result:t->ion.id;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
az_r_t  az_timer_delete(az_ion_id_t id)
{
  az_r_t result = AZ_SUCCESS;
  az_timer_t t = (az_timer_t)az_ion(id);
  do {
    az_if_arg_null_break(t, result);
    az_assert_ion_type(t->ion.type, AZ_ION_TYPE_TIMER);
    if (az_refcount_atomic_dec(&t->ion.refCount) <= 0) {
      result = AZ_ERR(AGAIN);
      break;
    }

    result = az_ion_deregister(&(t->ion));
    if (AZ_SUCCESS != result) {
      break;
    }
    if (AZ_REFCOUNT_VALUE(&t->ion.refCount) == 0) {
      result = az_sys_timer_delete(t->sys_timer);
    }
    if (AZ_REFCOUNT_IS_ZERO(&t->ion.refCount)) {
      az_free(t);
    }
  } while (0);

  return result;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
az_r_t  az_timer_start(az_ion_id_t id)
{
  az_r_t result = AZ_SUCCESS;
  az_timer_t t = (az_timer_t)az_ion(id);
  do {
    az_if_arg_null_break(t, result);
    az_assert_ion_type(t->ion.type, AZ_ION_TYPE_TIMER);

    if ((AZ_REFCOUNT_VALUE(&t->ion.refCount)) < 1) {
      result = AZ_ERR(INVALID);
      break;
    }

    result = az_sys_timer_start(t->sys_timer);

    if (AZ_SUCCESS == result) {
      az_refcount_atomic_inc(&t->ion.refCount);
    }
  } while (0);

  return result;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
az_r_t  az_timer_stop(az_ion_id_t id)
{
  az_r_t result = AZ_SUCCESS;
  az_timer_t t = (az_timer_t)az_ion(id);

  do {
    az_if_arg_null_break(t, result);
    az_assert_ion_type(t->ion.type, AZ_ION_TYPE_TIMER);

    if ((AZ_REFCOUNT_VALUE(&t->ion.refCount)) < 1) {
      result = AZ_ERR(INVALID);
      break;
    }
    
    result = az_sys_timer_stop(t->sys_timer);

    if (result == AZ_SUCCESS) { if (az_refcount_atomic_dec(&t->ion.refCount) == 0) {
        result = az_sys_timer_delete(t->sys_timer);
      }
      if (AZ_REFCOUNT_IS_ZERO(&t->ion.refCount)) {
        az_free(t);
      }
    }
  } while (0);

  return result;
}


/* === end of AZ_TIMER_C === */
