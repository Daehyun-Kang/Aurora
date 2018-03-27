/**
 * @file   az_linux_lock.h
 * @brief  
 * @date   30/06/16
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
 *   - 2016-06-30 fware initial create
 */

#ifndef AZ_LINUX_LOCK_H
#define AZ_LINUX_LOCK_H
#include "az_sys_linux.h"
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
typedef struct az_sys_lw_lock {
#ifdef  CONFIG_AZ_SYS_LW_USE_SEM
  sem_t sem;
#else
  int   counter;
  int   sem;
#endif
} az_sys_lw_lock_t;

/* macros */

/* variabls exposed */

/* inline functions */
static inline az_r_t az_sys_lw_lock_init(az_sys_lw_lock_t *p)
{
  az_r_t r = AZ_SUCCESS;
  az_assert(NULL != p);

#ifdef  CONFIG_AZ_SYS_LW_USE_SEM
  r = (az_r_t)sem_init(&p->sem, 0, 1);
#else
  p->counter = 0;
  p->sem = 0; 
#endif
  return r;
}
static inline az_r_t az_sys_lw_lock_deinit(az_sys_lw_lock_t *p)
{
  az_r_t r = AZ_SUCCESS;
  az_assert(NULL != p);

#ifdef  CONFIG_AZ_SYS_LW_USE_SEM
  r = (az_r_t)sem_destroy(&p->sem);
#else
  p->counter = 0;
  p->sem = 0;
#endif
  return r;
}

static inline az_r_t az_sys_lw_lock(az_sys_lw_lock_t *p)
{
  az_r_t r = AZ_SUCCESS;
  az_assert(NULL != p);

#ifdef  CONFIG_AZ_SYS_LW_USE_SEM
    r = (az_r_t)sem_wait(&p->sem);
#else
  if (az_atomic_inc32(&(p->counter)) > 1) {
    futex(&(p->sem), FUTEX_WAIT, 0, NULL, NULL, 0);
    az_atomic_dec32(&(p->sem));
  }
#endif

  return r;
}

static inline az_r_t az_sys_lw_unlock(az_sys_lw_lock_t *p)
{
  az_r_t r = AZ_SUCCESS;
  az_assert(NULL != p);

#ifdef  CONFIG_AZ_SYS_LW_USE_SEM
    r = (az_r_t)sem_post(&p->sem);
#else
  if (az_atomic_dec32(&(p->counter)) > 0) {
    az_atomic_inc32(&(p->sem));
    futex(&(p->sem), FUTEX_WAKE, 1, NULL, NULL, 0);
  }
#endif

  return r;
}

/* function prototypes exposed */

#ifdef __cplusplus
}
#endif
#endif
