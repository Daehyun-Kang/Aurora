/**
 * @file   az_timer.h
 * @brief  
 * @date   20/05/16
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
 *   - 2016-05-20 fware initial create
 */

#ifndef AZ_TIMER_H
#define AZ_TIMER_H

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

/* basic types */
typedef struct {
  az_ion_t            ion;
  char      name[CONFIG_AZ_NAME_MAX];

  az_sys_timer_t         sys_timer;
  az_refcount_t  refCount;
} az_timer_entity_t;

typedef az_timer_entity_t *   az_timer_t;
#define AZ_TIMER_INVALID      NULL

/* structures */

/* structured types */

/* macros */

/* variabls exposed */

/* inline functions */
static inline az_sys_timespec_t *az_timespec_diff(az_sys_timespec_t *e, az_sys_timespec_t *s)
{
  static az_sys_timespec_t d; 
  d.tv_nsec = e->tv_nsec - s->tv_nsec; 
  d.tv_sec = e->tv_sec - s->tv_sec;
  if (d.tv_nsec < 0) { d.tv_sec--; d.tv_nsec += 1000000000; };
  return &d;
}

/* function prototypes exposed */
extern az_r_t  az_timer_create(const char *name, az_uint64_t interval, int repeat, void (*handler)(void *arg), void *arg, az_timer_t *pTimer);
extern az_r_t  az_timer_delete(az_timer_t timer);
extern az_r_t  az_timer_start(az_timer_t timer);
extern az_r_t  az_timer_stop(az_timer_t timer);

#ifdef __cplusplus
}
#endif
#endif
