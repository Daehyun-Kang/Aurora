/**
 * @file   az_windows_sem.h
 * @brief  
 * @date   09/06/16
 * @author fware
 *
 * Copyright (c) Fware, 2013-2017 - All Rights Reserved
 * You may use, distribute and modify this code under the 
 * terms of the "Aurora Source Code License".
 * See the file LICENSE for full license details.

 * You should have received a copy of  the LICENSE with this file.

 * If not, please contact to Fware with the information in the file CONTACTS.
 *
 * History: @date 
 *   - 2016-06-09 fware initial create
 */

#ifndef AZ_WINDOWS_SEM_H
#define AZ_WINDOWS_SEM_H
#include "az_def.h"
#include "az_err.h"
#include "az_macros.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* constants */

/* basic macros */
#define AZ_SYS_SEM_OPTIONS_PSHARED  0x0001 
#define AZ_SYS_SEM_OPTIONS_EMPTY     0x0002 

/* basic types */
#ifndef AZ_WINDOWS_DEF_H
typedef HANDLE                az_sys_sem_t; 
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
  LPCTSTR wname;

  az_assert( value > 0);
  az_assert( NULL != pSem);

  do {
    AZ_GET_TSTR(name, wname, CONFIG_AZ_NAME_MAX*2);
    if (options & AZ_SYS_SEM_OPTIONS_EMPTY) {
      m = CreateSemaphore(NULL, 0, value, wname); 
    } else {
      m = CreateSemaphore(NULL, value, value, wname); 
    }

    *pSem = m;

    if (NULL == m) {
      result = AZ_ERR(CREAT);
      *pSem = NULL;
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
  BOOL bResult;
  do {
    az_if_arg_null_break(m, result);

    bResult = CloseHandle(m);
    if (!bResult) {
      result = AZ_ERR(FAIL);
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
  DWORD dResult;
  do {
    az_if_arg_null_break(m, result);

    dResult = WaitForSingleObjectEx(m, INFINITE, TRUE);
    switch (dResult) {
      case  WAIT_OBJECT_0:
            break;
      case  WAIT_FAILED:
            result = AZ_ERR(FAIL);
            break;
      case  WAIT_IO_COMPLETION:
            result = AZ_ERR(QUIT);
            break;
      case  WAIT_ABANDONED:
            result = AZ_ERR(ACCESS);
            break;
      case  WAIT_TIMEOUT:
            result = AZ_ERR(TIMEOUT);
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
static inline az_r_t  az_sys_sem_trywait(az_sys_sem_t m)
{
  az_r_t result = AZ_SUCCESS;
  DWORD dResult;
  do {
    az_if_arg_null_break(m, result);

    dResult = WaitForSingleObjectEx(m, 0, TRUE);
    switch (dResult) {
      case  WAIT_OBJECT_0:
            break;
      case  WAIT_FAILED:
            result = AZ_ERR(FAIL);
            break;
      case  WAIT_IO_COMPLETION:
            result = AZ_ERR(QUIT);
            break;
      case  WAIT_ABANDONED:
            result = AZ_ERR(ACCESS);
            break;
      case  WAIT_TIMEOUT:
            result = AZ_ERR(TIMEOUT);
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
static inline az_r_t  az_sys_sem_timedwait(az_sys_sem_t m, az_int64_t tmo_ns)
{
  az_r_t result = AZ_SUCCESS;
  DWORD tmo_ms;
  DWORD dResult;
  do {
    az_if_arg_null_break(m, result);

    if (tmo_ns >= 0) tmo_ms = tmo_ns/1000000;
    else tmo_ms = INFINITE;
    dResult = WaitForSingleObjectEx(m, tmo_ms, TRUE);
    switch (dResult) {
      case  WAIT_OBJECT_0:
            break;
      case  WAIT_FAILED:
            result = AZ_ERR(FAIL);
            break;
      case  WAIT_IO_COMPLETION:
            result = AZ_ERR(QUIT);
            break;
      case  WAIT_ABANDONED:
            result = AZ_ERR(ACCESS);
            break;
      case  WAIT_TIMEOUT:
            result = AZ_ERR(TIMEOUT);
            break;
    }
  } while (0);

  return result;

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
  BOOL bResult;
  do {
    az_if_arg_null_break(m, result);

    bResult = ReleaseSemaphore(m, (LONG)1, NULL);
    if (!bResult) {
      result = AZ_ERR(FAIL);
    }
  } while (0);

  return result;
}

/* function prototypes exposed */

#ifdef __cplusplus
}
#endif
#endif
