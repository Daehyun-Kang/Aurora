/**
 * @file   az_sem.h
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

#ifndef AZ_SEM_H
#define AZ_SEM_H
#include "az_def.h"
#include "az_err.h"
#include "az_macros.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* constants */

/* basic macros */
#define AZ_SEM_OPTIONS_PSHARED  AZ_SYS_SEM_OPTIONS_PSHARED 

/* basic types */
typedef struct {
  az_ion_t            ion;
  char      name[CONFIG_AZ_NAME_MAX];

  az_sys_sem_t              sys_sem; 
} az_sem_entity_t;
typedef az_sem_entity_t *                  az_sem_t;
#define AZ_SEM_INVALID   NULL

/* structures */

/* structured types */

/* macros */

/* variabls exposed */

/* inline functions */

/* function prototypes exposed */
extern az_ion_id_t  az_sem_create(const char *name, int options, unsigned int value, az_sem_t *pMutex);
extern az_r_t  az_sem_delete(az_ion_id_t id);
extern az_r_t  az_sem_wait(az_ion_id_t id);
extern az_r_t  az_sem_trywait(az_ion_id_t id);
extern az_r_t  az_sem_timedwait(az_ion_id_t id, az_uint64_t tmo_ns);
extern az_r_t  az_sem_post(az_ion_id_t id);

#ifdef __cplusplus
}
#endif
#endif
