/**
 * @file   az_ion.c
 * @brief  
 * @date   12/03/16
 * @author mercury
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
 *   - 2016-03-12 mercury initial create
 */

/* include header files */
#include "az_ion.h"
#include "az_printf.h"

/* declare global variables */
#ifdef  CONFIG_AZ_DYNAMIC_ION
az_array_t *az_ion_PRefList = NULL;
#else
az_ion_list_t az_ion_list = {0};
az_ion_t  *az_ions[AZ_ION_MAX] = {0};
#endif

char *az_ion_type_names[] = AZ_ION_TYPE_NAMES; 

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
#ifdef  CONFIG_AZ_DYNAMIC_ION
az_r_t az_ion_register(az_ion_t *ion, az_ion_type_t type)
{
  az_r_t result = AZ_SUCCESS; 
  if (NULL == ion) {
    result = AZ_ERR(ARG_NULL);
  } else {
    az_ion_id_t id;

    id = (az_ion_id_t)az_array_addptr(AZ_ION_LIST(), (az_array_element_t)ion);
    if (id < 0) {
      result = AZ_ERR(OVERFLOW);
    } else {
      ion->id = id;
      ion->type = type;
      az_refcount_atomic_inc(&ion->refCount);
    }
    az_sys_eprintf("id:%d type:%d ion:%p/%d result="AZ_FMT_RET(1)"\n",id, type, 
        ion, AZ_REFCOUNT_VALUE(&ion->refCount), result); 
  }

  return result;
}

az_r_t az_ion_deregister(az_ion_t *ion)
{
  az_r_t result = AZ_SUCCESS; 

  if (NULL == ion) {
    result = AZ_ERR(ARG_NULL);
  } else {
    result = az_array_delptr(AZ_ION_LIST(), ion->id);
    if (result == AZ_SUCCESS) {
      az_refcount_atomic_dec(&ion->refCount);
    } else {
      az_sys_eprintf("%ld:%s\n", result, az_err_str(result));
    }
    az_sys_eprintf("id:%d type:%d ion:%p/%d result="AZ_FMT_RET(1)"\n",ion->id, 
        ion->type, az_ion(ion->id), AZ_REFCOUNT_VALUE(&ion->refCount), result); 
  }

  return result;
}
#else
az_r_t az_ion_register(az_ion_t *ion, az_ion_type_t type)
{
  az_r_t result = AZ_SUCCESS; 
  if (NULL == ion) {
    result = AZ_ERR(ARG_NULL);
  } else {
    az_ion_id_t id = ion->id;

    do {
      if (id < 0) {
        if (type & AZ_ION_FLAG_AUTOALLOC) {
          id = ion->id = az_sys_io_create(); 
        }
      }
      if (id < 0 || id > AZ_ION_LIST()->size) {
        result = AZ_ERR(OVERFLOW);
        break;
      } 
      if (az_refcount_atomic_inc(&ion->refCount) == 1) {
        if (az_ions[id] == NULL) {
          ion->type = type;
          az_ions[id] = ion;
          az_atomic_inc32(&AZ_ION_LIST()->count);
        } else if (ion == az_ions[id]) { 
          result = AZ_ERR(AGAIN);
        } else {
          result = AZ_ERR(INVALID);
        } 
      }
    } while (0);
    az_sys_rprintf(result, "id:%d type:%d ion:%p/%d\n",id, type, 
        ion, AZ_REFCOUNT_VALUE(&ion->refCount));
  }

  return result;
}

az_r_t az_ion_deregister(az_ion_t *ion)
{
  az_r_t result = AZ_SUCCESS; 

  if (NULL == ion) {
    result = AZ_ERR(ARG_NULL);
  } else {
    az_ion_id_t id = ion->id;

    if (id < 0 || id > AZ_ION_LIST()->size) {
      result = AZ_ERR(OVERFLOW);
    } else {
      if (ion != az_ions[id]) {
        result = AZ_ERR(INVALID);
      } else {
        if  (az_refcount_atomic_dec(&ion->refCount) == 0) {
          az_atomic_dec32(&AZ_ION_LIST()->count);
          az_ions[ion->id] = NULL;
          if (ion->type & AZ_ION_FLAG_AUTOALLOC) {
            az_sys_io_delete(ion->id); 
          }
          ion->id = AZ_SYS_IO_INVALID;
        } else if (AZ_REFCOUNT_VALUE(&ion->refCount) < 0) {
          // may reset the refcount to zero
          result = AZ_ERR(AGAIN);
        }
      }
    }
    az_sys_eprintf("id:%d type:%d ion:%p/%d result="AZ_FMT_RET(1)"\n",ion->id, 
        ion->type, az_ion(ion->id), AZ_REFCOUNT_VALUE(&ion->refCount), result); 
  }

  return result;
}
#endif

char *__az_weak az_app_ion_type_name(az_ion_type_t ion)
{
  return "UDF";
}

/* === end of AZ_ION_C === */
