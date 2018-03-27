/**
 * @file   az_rw_lock.h
 * @brief  
 * @date   09/07/16
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
 *   - 2016-07-09 fware initial create
 */

#ifndef AZ_RW_LOCK_H
#define AZ_RW_LOCK_H

#include "az_sys_def.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* constants */

/* basic macros */

/* basic types */
typedef struct az_rw_lock {
  int32_t           readCount;
  az_sys_lw_lock_t  read_lock; 
  az_sys_lw_lock_t  write_lock; 
} az_rw_lock_t;

/* structures */

/* structured types */

/* macros */

/* variabls exposed */

/* inline functions */
static inline az_r_t az_rw_lock_init(az_rw_lock_t *p)
{
  az_r_t r = AZ_SUCCESS;
  az_assert(NULL != p);

  p->readCount = 0;
  az_sys_lw_lock_init(&p->read_lock);
  az_sys_lw_lock_init(&p->write_lock);

  return r;
}
static inline az_r_t az_rw_lock_deinit(az_rw_lock_t *p)
{
  az_r_t r = AZ_SUCCESS;
  az_assert(NULL != p);

  az_sys_lw_lock_deinit(&p->write_lock);
  az_sys_lw_lock_deinit(&p->read_lock);
  p->readCount = 0;

  return r;
}
static inline az_r_t az_rw_lock_read(az_rw_lock_t *p)
{
  az_r_t r = AZ_SUCCESS;
  az_assert(NULL != p);
  do {
    az_sys_lw_lock(&p->read_lock);
    if (az_atomic_fetch_and_add32(&p->readCount, 1) == 0) {
       az_sys_lw_lock(&p->write_lock);
    }
    az_sys_lw_unlock(&p->read_lock);
  } while (0);

  return r;
}
static inline az_r_t az_rw_unlock_read(az_rw_lock_t *p)
{
  az_r_t r = AZ_SUCCESS;
  az_assert(NULL != p);
  do {
    az_sys_lw_lock(&p->read_lock);
    if (az_atomic_fetch_and_sub32(&p->readCount, 1) == 1) {
      az_sys_lw_unlock(&p->write_lock);
    }
    az_sys_lw_unlock(&p->read_lock);
  } while (0);

  return r;
}
static inline az_r_t az_rw_lock_write(az_rw_lock_t *p)
{
  az_r_t r = AZ_SUCCESS;
  az_assert(NULL != p);
  do {
    az_sys_lw_lock(&p->write_lock);
  } while (0);

  return r;
}
static inline az_r_t az_rw_unlock_write(az_rw_lock_t *p)
{
  az_r_t r = AZ_SUCCESS;
  az_assert(NULL != p);
  do {
    az_sys_lw_unlock(&p->write_lock);
  } while (0);

  return r;
}

/* function prototypes exposed */

#ifdef __cplusplus
}
#endif
#endif
