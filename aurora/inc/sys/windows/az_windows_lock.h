/**
 * @file   az_windows_lock.h
 * @brief  
 * @date   30/06/16
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
 *   - 2016-06-30 fware initial create
 */

#ifndef AZ_WINDOWS_LOCK_H
#define AZ_WINDOWS_LOCK_H
#include "az_sys_windows.h"
#include "az_def.h"
#include "az_err.h"
#include "az_assert.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* constants */

/* basic macros */

/* basic types */

/* structures */

/* structured types */
/*
 * define light weight lock
 */
typedef struct az_sys_lw_lock {
  #ifdef  CONFIG_AZ_LW_LOCK_USE_SEMAPHORE
  LONG    counter;
  HANDLE  sem;
  #else
  CRITICAL_SECTION lock;
  #endif
} az_sys_lw_lock_t;

/* macros */

/* variabls exposed */

/* inline functions */
static inline az_r_t az_sys_lw_lock_init(az_sys_lw_lock_t *p)
{
  az_r_t r = AZ_SUCCESS;
  az_assert(NULL != p);

  #ifdef  CONFIG_AZ_LW_LOCK_USE_SEMAPHORE
  p->counter = 0;
  p->sem = CreateSemaphore(NULL, 0, 1, NULL);
  #else
  InitializeCriticalSection(&(p->lock));
  #endif

  return r;
}
static inline az_r_t az_sys_lw_lock_deinit(az_sys_lw_lock_t *p)
{
  az_r_t r = AZ_SUCCESS;
  az_assert(NULL != p);

  #ifdef  CONFIG_AZ_LW_LOCK_USE_SEMAPHORE
  CloseHandle(p->sem);
  #else
  DeleteCriticalSection(&(p->lock));
  #endif

  return r;
}

static inline az_r_t az_sys_lw_lock(az_sys_lw_lock_t *p)
{
  az_r_t r = AZ_SUCCESS;
  az_assert(NULL != p);

  #ifdef  CONFIG_AZ_LW_LOCK_USE_SEMAPHORE
  if (_InterlockedIncrement(&(p->counter)) > 1) {
    WaitForSingleObjectEx(p->sem, INFINITE, TRUE);
    r = AZ_ERR(INTERRUPT);
  }
  #else
  EnterCriticalSection(&(p->lock));
  #endif

  return r;
}

static inline az_r_t az_sys_lw_unlock(az_sys_lw_lock_t *p)
{
  az_r_t r = AZ_SUCCESS;
  az_assert(NULL != p);

  #ifdef  CONFIG_AZ_LW_LOCK_USE_SEMAPHORE
  if (_InterlockedDecrement(&(p->counter)) > 0) {
    ReleaseSemaphore(p->sem, 1, NULL);
  }
  #else
  LeaveCriticalSection(&(p->lock));
  #endif

  return r;
}

/* function prototypes exposed */

#ifdef __cplusplus
}
#endif
#endif
