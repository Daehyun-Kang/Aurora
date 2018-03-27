/**
 * @file   az_linux_mutex.h
 * @brief  
 * @date   09/06/16
 * @author fware
 *
 * @copyright
 * Copyright (c) Fware, 2013-2017 - All Rights Reserved
 * You may use, distribute and modify this code under the 
 * terms of the "Aurora Source Code License".
 * See the file LICENSE for full license details.\n\n
 *
 * You should have received a copy of  the LICENSE with this file.\n\n
 *
 * If not, please contact to Fware with the information in the file CONTACTS.
 *
 * History:
 *   - 2016-06-09 fware initial create
 */

#ifndef AZ_LINUX_MUTEX_H
#define AZ_LINUX_MUTEX_H
#include "az_def.h"
#include "az_macros.h"
#include "az_err.h"
#include "az_malloc.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* constants */

/* basic macros */

/* basic types */
#ifndef AZ_LINUX_DEF_H
typedef pthread_mutex_t * az_sys_mutex_t; 
#define AZ_SYS_MUTEX_INVALID    NULL
#endif

/* structures */

/* structured types */

/* macros */

/* variabls exposed */

/* inline functions */
/**
 * @fn        function name
 * @brief     function-description
 * @param     input-output-parameters
 * @return    return-value
 * @warning   warnings
 * @exception none
 */
static inline az_r_t  az_sys_mutex_create(const char *name, int options, az_sys_mutex_t *pMutex)
{
  az_r_t result = AZ_SUCCESS;
  az_sys_mutex_t  m;

  do {
    m = (pthread_mutex_t *)az_malloc(sizeof(pthread_mutex_t));
    *pMutex = m;
    if (NULL == m) {
      result = AZ_ERR(MALLOC);
      break;
    }

    result = pthread_mutex_init(m, NULL);
    if (AZ_SUCCESS != result) {
      result = AZ_ERR(CREAT);
      az_free(m);
      *pMutex = NULL;
      break;
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
static inline az_r_t  az_sys_mutex_delete(az_sys_mutex_t m)
{
  az_r_t result = AZ_SUCCESS;
  do {
    az_if_arg_null_break(m, result);

    result = pthread_mutex_destroy(m);
    if (AZ_SUCCESS == result) {
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
static inline az_r_t  az_sys_mutex_lock(az_sys_mutex_t m)
{
  az_r_t result = AZ_SUCCESS;
  do {
    az_if_arg_null_break(m, result);

    result = pthread_mutex_lock(m);
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
static inline az_r_t  az_sys_mutex_trylock(az_sys_mutex_t m)
{
  az_r_t result = AZ_SUCCESS;
  do {
    az_if_arg_null_break(m, result);

    result = pthread_mutex_trylock(m);
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
static inline az_r_t  az_sys_mutex_unlock(az_sys_mutex_t m)
{
  az_r_t result = AZ_SUCCESS;
  do {
    az_if_arg_null_break(m, result);

    result = pthread_mutex_unlock(m);
  } while (0);

  return result;
}

/* function prototypes exposed */

#ifdef __cplusplus
}
#endif
#endif
