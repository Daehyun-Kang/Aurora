/**
 * @file   az_windows_timer.h
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

#ifndef AZ_WINDOWS_TIMER_H
#define AZ_WINDOWS_TIMER_H
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

typedef struct {
  HANDLE          timer;
  az_uint64_t     interval_ms;
  int             repeat;
  void            (*handler)(void *);
  void            *arg;
  int32_t         owner;

  const char      *name
  int             state;
} az_sys_timer_entity_t;
typedef az_sys_timer_entity_t*   az_sys_timer_t;
#define AZ_SYS_TIMER_INVALID    NULL

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

/* function prototypes exposed */
extern az_r_t  az_sys_timer_init();
extern az_r_t  az_sys_timer_deinit();
extern az_r_t  az_sys_timer_create(const char *name, az_uint64_t interval,  int repeat, void (*handler)(void *arg), void *arg, az_sys_timer_t *pTimer);
extern az_r_t  az_sys_timer_delete(az_sys_timer_t t);
extern az_r_t  az_sys_timer_start(az_sys_timer_t t);
extern az_r_t  az_sys_timer_stop(az_sys_timer_t t);

#ifdef __cplusplus
}
#endif
#endif
