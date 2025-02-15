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
#include "az_thread.h"

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

  az_thread_t xu;
} az_timer_entity_t;

typedef az_timer_entity_t *   az_timer_t;
#define AZ_TIMER_INVALID      NULL

/* structures */

/* structured types */

/* macros */

/* variabls exposed */

/* inline functions */
static inline uint64_t az_timestamp()
{
  uint32_t low, high;
#if defined(__i386__) || defined(__x86_64__) || defined(__ia64__)
  asm volatile ("rdtsc":"=a"(low), "=d"(high));
#endif
  return ((uint64_t)high << 32) | low;

}

static inline az_sys_timespec_t *az_timespec_diff(az_sys_timespec_t *e, az_sys_timespec_t *s)
{
  static az_sys_timespec_t d; 
  d.tv_nsec = e->tv_nsec - s->tv_nsec; 
  d.tv_sec = e->tv_sec - s->tv_sec;
  if (d.tv_nsec < 0) { d.tv_sec--; d.tv_nsec += 1E9; };
  return &d;
}
#define az_timespec_value(t) ((((az_sys_timespec_t *)(t))->tv_sec * 1E9) + ((az_sys_timespec_t *)(t))->tv_nsec)

/* function prototypes exposed */
extern az_ion_id_t  az_timer_create(const char *name, az_uint64_t interval, int repeat, void (*handler)(void *arg), void *arg, az_timer_t *pTimer);
extern az_r_t  az_timer_delete(az_ion_id_t id);
extern az_r_t  az_timer_start(az_ion_id_t id);
extern az_r_t  az_timer_stop(az_ion_id_t id);

#ifdef __cplusplus
}
#endif
#endif
