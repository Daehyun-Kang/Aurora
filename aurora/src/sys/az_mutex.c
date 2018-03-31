/**
 * @file   az_mutex.c
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
#include "az_mutex.h"

/* declare global variables */

/* declare static variables */


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
az_ion_id_t  az_mutex_create(const char *name, int options, az_mutex_t *pMutex)
{
  az_r_t result = AZ_SUCCESS;
  az_mutex_t m;

  do {
    az_if_arg_null_break(name, result);
    az_if_arg_null_break(pMutex, result);

    m = *pMutex;
    if (NULL == m) {
      m = az_malloc(sizeof(az_mutex_entity_t));
      az_if_alloc_null_break(m, result);
      az_ion_invalidate(&m->ion, 0);
    }
    az_assert(m->ion.id == AZ_SYS_IO_INVALID);
    strncpy(m->name, name, sizeof(m->name));
    result = az_sys_mutex_create(name, options, &(m->mutex));
    if (AZ_SUCCESS != result) {
      if (AZ_REFCOUNT_IS_ZERO(&m->ion.refCount)) {
        az_free(m);
      }
      break;
    }
    #ifdef  CONFIG_AZ_ION_NONIO
    result = az_ion_register(&(m->ion), AZ_ION_TYPE_MUTEX);
    #else
    result = az_ion_register(&(m->ion), AZ_ION_TYPE_MUTEX|AZ_ION_FLAG_AUTOALLOC);
    #endif
    if (AZ_SUCCESS != result) {
      az_sys_mutex_delete(m->mutex);
      if (AZ_REFCOUNT_IS_ZERO(&m->ion.refCount)) {
        az_free(m);
        *pMutex = NULL;
      }
      break;
    } else {
      az_refcount_atomic_inc(&m->ion.refCount);
    }
    
    *pMutex = m;
  } while (0);

  return (result < 0)? (az_ion_id_t)result:m->ion.id;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
az_r_t  az_mutex_delete(az_ion_id_t id)
{
  az_r_t result = AZ_SUCCESS;
  az_mutex_t m = (az_mutex_t)az_ion(id); 
  do {
    az_if_arg_null_break(m, result);
    az_assert_ion_type(m->ion.type, AZ_ION_TYPE_MUTEX);
    
    if (az_refcount_atomic_dec(&m->ion.refCount) <= 0) {
      result = AZ_ERR(AGAIN);
      break;
    }
    
    result = az_ion_deregister(&(m->ion));

    if (AZ_SUCCESS != result) {
      break;
    }
    if (AZ_REFCOUNT_VALUE(&(m->ion.refCount)) == 0) {
      result = az_sys_mutex_delete(m->mutex);
    }
    if (AZ_REFCOUNT_IS_ZERO(&m->ion.refCount)) {
      az_free(m);
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
az_r_t  az_mutex_lock(az_ion_id_t id)
{
  az_r_t result = AZ_SUCCESS;
  az_mutex_t m = (az_mutex_t)az_ion(id); 
  do {
    az_if_arg_null_break(m, result);
    az_assert_ion_type(m->ion.type, AZ_ION_TYPE_MUTEX);

    if ((AZ_REFCOUNT_VALUE(&m->ion.refCount)) < 1) {
      result = AZ_ERR(INVALID);
      break;
    }

    result = az_sys_mutex_lock(m->mutex);
    if (result == AZ_SUCCESS) {
      az_refcount_atomic_inc(&m->ion.refCount);
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
az_r_t  az_mutex_trylock(az_ion_id_t id)
{
  az_r_t result = AZ_SUCCESS;
  az_mutex_t m = (az_mutex_t)az_ion(id); 
  do {
    az_if_arg_null_break(m, result);
    az_assert_ion_type(m->ion.type, AZ_ION_TYPE_MUTEX);

    if ((AZ_REFCOUNT_VALUE(&m->ion.refCount)) < 1) {
      result = AZ_ERR(INVALID);
      break;
    }

    result = az_sys_mutex_trylock(m->mutex);
    if (result == AZ_SUCCESS) {
      az_refcount_atomic_inc(&m->ion.refCount);
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
az_r_t  az_mutex_unlock(az_ion_id_t id)
{
  az_r_t result = AZ_SUCCESS;
  az_mutex_t m = (az_mutex_t)az_ion(id); 
  do {
    az_if_arg_null_break(m, result);
    az_assert_ion_type(m->ion.type, AZ_ION_TYPE_MUTEX);

    if ((AZ_REFCOUNT_VALUE(&m->ion.refCount)) < 1) {
      result = AZ_ERR(INVALID);
      break;
    }

    result = az_sys_mutex_unlock(m->mutex);
    if (result == AZ_SUCCESS) {
      if (az_refcount_atomic_dec(&m->ion.refCount) == 0) {
        result = az_sys_mutex_delete(m->mutex);
      }
      if (AZ_REFCOUNT_IS_ZERO(&m->ion.refCount)) {
        az_free(m);
      }
    }
  } while (0);

  return result;
}


/* === end of AZ_MUTEX_C === */
