/**
 * @file   az_sem.c
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
#include "az_sem.h"

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
az_ion_id_t  az_sem_create(const char *name, int options, unsigned int value, az_sem_t *pSem)
{
  az_r_t result = AZ_SUCCESS;
  az_sem_t m;

  do {
    az_if_arg_null_break(name, result);
    az_if_arg_null_break(pSem, result);

    m = *pSem;
    
    if (NULL == m) {
      m = az_malloc(sizeof(az_sem_entity_t));
      az_if_alloc_null_break(m, result);
      az_ion_invalidate(&m->ion, 0);
    }
    az_assert(m->ion.id == AZ_SYS_IO_INVALID);

    strncpy(m->name, name, sizeof(m->name));
    result = az_sys_sem_create(name, options, value, &(m->sys_sem));
    if (AZ_SUCCESS != result) {
      if (AZ_REFCOUNT_IS_ZERO(&m->ion.refCount)) {
        az_free(m);
      }
      break;
    }

    #ifdef  CONFIG_AZ_ION_NONIO
    result = az_ion_register(&(m->ion), AZ_ION_TYPE_SEM);
    #else
    result = az_ion_register(&(m->ion), AZ_ION_TYPE_SEM|AZ_ION_FLAG_AUTOALLOC);
    #endif
    if (AZ_SUCCESS != result) {
      az_sys_sem_delete(m->sys_sem);
      if (AZ_REFCOUNT_IS_ZERO(&m->ion.refCount)) {
        az_free(m);
      }
      break;
    } else {
      az_refcount_atomic_inc(&m->ion.refCount);
    }
    *pSem = m;
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
az_r_t  az_sem_delete(az_ion_id_t id)
{
  az_r_t result = AZ_SUCCESS;
  az_sem_t m = (az_sem_t)az_ion(id); 
  do {
    az_if_arg_null_break(m, result);
    az_assert_ion_type(m->ion.type, AZ_ION_TYPE_SEM);

    if (az_refcount_atomic_dec(&m->ion.refCount) <= 0) {
      result = AZ_ERR(AGAIN);
      break;
    }

    result = az_ion_deregister(&(m->ion));
    if (AZ_SUCCESS != result) {
      break;
    }
    if (AZ_REFCOUNT_VALUE(&m->ion.refCount) == 0) {
      result = az_sys_sem_delete(m->sys_sem);
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
az_r_t  az_sem_wait(az_ion_id_t id)
{
  az_r_t result = AZ_SUCCESS;
  az_sem_t m = (az_sem_t)az_ion(id); 
  do {
    az_if_arg_null_break(m, result);
    az_assert_ion_type(m->ion.type, AZ_ION_TYPE_SEM);

    if ((AZ_REFCOUNT_VALUE(&m->ion.refCount)) < 1) {
      result = AZ_ERR(INVALID);
      break;
    }

    result = az_sys_sem_wait(m->sys_sem);

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
az_r_t  az_sem_trywait(az_ion_id_t id)
{
  az_r_t result = AZ_SUCCESS;
  az_sem_t m = (az_sem_t)az_ion(id); 
  do {
    az_if_arg_null_break(m, result);
    az_assert_ion_type(m->ion.type, AZ_ION_TYPE_SEM);

    if ((AZ_REFCOUNT_VALUE(&m->ion.refCount)) < 1) {
      result = AZ_ERR(INVALID);
      break;
    }

    result = az_sys_sem_trywait(m->sys_sem);

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
az_r_t  az_sem_timedwait(az_ion_id_t id, az_uint64_t tmo_ns)
{
  az_r_t result = AZ_SUCCESS;
  az_sem_t m = (az_sem_t)az_ion(id); 
  do {
    az_if_arg_null_break(m, result);
    az_assert_ion_type(m->ion.type, AZ_ION_TYPE_SEM);

    if ((AZ_REFCOUNT_VALUE(&m->ion.refCount)) < 1) {
      result = AZ_ERR(INVALID);
      break;
    }

    result = az_sys_sem_timedwait(m->sys_sem, tmo_ns); 

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
az_r_t  az_sem_post(az_ion_id_t id)
{
  az_r_t result = AZ_SUCCESS;
  az_sem_t m = (az_sem_t)az_ion(id); 
  do {
    az_if_arg_null_break(m, result);
    az_assert_ion_type(m->ion.type, AZ_ION_TYPE_SEM);

    if ((AZ_REFCOUNT_VALUE(&m->ion.refCount)) < 1) {
      result = AZ_ERR(INVALID);
      break;
    }

    result = az_sys_sem_post(m->sys_sem);

    if (result == AZ_SUCCESS) {
      if (az_refcount_atomic_dec(&m->ion.refCount) == 0) {
        result = az_sys_sem_delete(m->sys_sem);
      }
      if (AZ_REFCOUNT_IS_ZERO(&m->ion.refCount)) {
        az_free(m);
      }
    }
  } while (0);

  return result;
}


/* === end of AZ_SEM_C === */
