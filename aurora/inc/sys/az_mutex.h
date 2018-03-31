/**
 * @file   az_mutex.h
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

#ifndef AZ_MUTEX_H
#define AZ_MUTEX_H
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

  az_sys_mutex_t     mutex;       
} az_mutex_entity_t;

typedef az_mutex_entity_t *        az_mutex_t;
#define AZ_MUTEX_INVALID   NULL

/* structures */

/* structured types */

/* macros */

/* variabls exposed */

/* inline functions */

/* function prototypes exposed */
extern az_ion_id_t  az_mutex_create(const char *name, int options, az_mutex_t *pMutex);
extern az_r_t  az_mutex_delete(az_ion_id_t id);
extern az_r_t  az_mutex_lock(az_ion_id_t id);
extern az_r_t  az_mutex_trylock(az_ion_id_t id);
extern az_r_t  az_mutex_unlock(az_ion_id_t id);

#ifdef __cplusplus
}
#endif
#endif
