/**
 * @file   az_windows_mutex.h
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

#ifndef AZ_WINDOWS_MUTEX_H
#define AZ_WINDOWS_MUTEX_H
#include "az_def.h"
#include "az_err.h"
#include "az_macros.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* constants */

/* basic macros */

/* basic types */
#ifndef AZ_WINDOWS_DEF_H
typedef HANDLE   az_sys_mutex_t; 
//#define AZ_SYS_MUTEX_INVALID    INVALID_HANDLE_VALUE
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
  LPCTSTR wname;

  az_assert(NULL != name && NULL != pMutex);
  do {
    AZ_GET_TSTR(name, wname, CONFIG_AZ_NAME_MAX*2);
    m = CreateMutex(NULL, FALSE, wname);
    if (NULL == m) {
      result = AZ_ERR(CREAT);
      *pMutex = NULL;
    } else {
      *pMutex = m;
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
static inline az_r_t  az_sys_mutex_lock(az_sys_mutex_t m)
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
static inline az_r_t  az_sys_mutex_trylock(az_sys_mutex_t m)
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
static inline az_r_t  az_sys_mutex_unlock(az_sys_mutex_t m)
{
  az_r_t result = AZ_SUCCESS;
  BOOL bResult;
  do {
    az_if_arg_null_break(m, result);

    bResult = ReleaseMutex(m);
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
