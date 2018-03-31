/**
 * @file   az_thread.h
 * @brief  
 * @date   09/11/17
 * @author dhkang
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
 *   - 2017-11-09 dhkang initial create
 */

#ifndef AZ_THREAD_H
#define AZ_THREAD_H

#include "az_xu.h"
#include "az_thread_cfg.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* constants */
#define AZ_THREAD_STATE_CREATED     0x0001
#define AZ_THREAD_STATE_START       0x0002
#define AZ_THREAD_STATE_STOP        0x0004
#define AZ_THREAD_STATE_BUSY        0x0008
#define AZ_THREAD_STATE_ERROR       0x4000
#define AZ_THREAD_STATE_EXCP        0x8000

#define AZ_THREAD_ERROR_CREATE      0x0001
#define AZ_THREAD_ERROR_START       0x0002
#define AZ_THREAD_ERROR_STOP        0x0004
#define AZ_THREAD_ERROR_DELETE      0x0008
#define AZ_THREAD_ERROR_SNDEVT      0x0010
#define AZ_THREAD_ERROR_RCVEVT      0x0020
/* basic macros */

/* basic types */

typedef struct az_thread_status {
  int state;
  az_xu_entry_t entry;
  az_xu_arg_t arg;
  int cause;
} az_thread_status_t;

typedef struct az_thread_entity {
  az_xu_entity_t      xu;
  az_thread_status_t  status;
  az_thread_config_t  config;
} az_thread_entity_t;

typedef az_thread_entity_t  *az_thread_t;

/* structures */

/* structured types */

/* macros */

/* variabls exposed */

/* inline functions */

/* function prototypes exposed */
extern void *az_thread_entry(void *);
extern az_r_t  az_thread_create(az_thread_config_t *cfg, az_thread_t *pThr);
extern az_r_t  az_thread_start(az_thread_t thr, char *entry_name, char *arg_name, az_xu_entry_t entry, az_xu_arg_t arg, int startdelay);
extern az_r_t  az_thread_stop(az_thread_t thr);
extern az_r_t  az_thread_delete(az_thread_t thr);

#ifdef __cplusplus
}
#endif
#endif
