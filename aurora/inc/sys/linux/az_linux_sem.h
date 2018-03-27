/**
 * @file   az_linux_sem.h
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

#ifndef AZ_LINUX_SEM_H
#define AZ_LINUX_SEM_H
#include "az_def.h"
#include "az_err.h"
#include "az_macros.h"
#include "az_malloc.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* constants */

/* basic macros */
#define AZ_SYS_SEM_OPTIONS_PSHARED  0x0001 

/* basic types */
#ifndef AZ_LINUX_DEF_H
typedef sem_t *               az_sys_sem_t; 
#define AZ_SYS_SEM_INVALID    NULL
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
static inline az_r_t  az_sys_sem_create(const char *name, int options, unsigned int value, az_sys_sem_t *pSem)
{
  az_r_t result = AZ_SUCCESS;
  az_sys_sem_t  m;

  do {
    m = (sem_t *)az_malloc(sizeof(sem_t));
    *pSem = m;
    if (NULL == m) {
      result = AZ_ERR(MALLOC);
      break;
    }

    if (options & AZ_SYS_SEM_OPTIONS_PSHARED) {
      result = sem_init(m, 1, value);
    } else {
      result = sem_init(m, 0, value);
    }

    if (AZ_SUCCESS != result) {
      result = AZ_ERR(CREAT);
      az_free(m);
      *pSem = NULL;
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
static inline az_r_t  az_sys_sem_delete(az_sys_sem_t m)
{
  az_r_t result = AZ_SUCCESS;
  do {
    az_if_arg_null_break(m, result);

    result = sem_destroy(m);
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
static inline az_r_t  az_sys_sem_wait(az_sys_sem_t m)
{
  az_r_t result = AZ_SUCCESS;
  do {
    az_if_arg_null_break(m, result);

    result = sem_wait(m);
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
static inline az_r_t  az_sys_sem_trywait(az_sys_sem_t m)
{
  az_r_t result = AZ_SUCCESS;
  do {
    az_if_arg_null_break(m, result);

    result = sem_trywait(m);
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
static inline az_r_t  az_sys_sem_timedwait(az_sys_sem_t m, az_uint64_t tmo_ns)
{
  az_r_t result = AZ_SUCCESS;
  struct timespec abs_timeout;
  do {
    az_if_arg_null_break(m, result);

    clock_gettime(CLOCK_REALTIME, &abs_timeout);
    tmo_ns += abs_timeout.tv_nsec;
    abs_timeout.tv_sec += tmo_ns / 1000000000;
    abs_timeout.tv_nsec = tmo_ns % 1000000000;
    result = sem_timedwait(m, &abs_timeout);
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
static inline az_r_t  az_sys_sem_post(az_sys_sem_t m)
{
  az_r_t result = AZ_SUCCESS;
  do {
    az_if_arg_null_break(m, result);

    result = sem_post(m);
  } while (0);

  return result;
}

/* function prototypes exposed */

#ifdef __cplusplus
}
#endif
#endif
