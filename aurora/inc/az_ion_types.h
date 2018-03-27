/**
 * @file   az_ion_types.h
 * @brief  
 * @date   10/04/16
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
 *   - 2016-04-10 fware initial create
 */

#ifndef AZ_ION_TYPES_H
#define AZ_ION_TYPES_H

#include "az_def.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* constants */
#define   AZ_ION_FLAG_AUTOALLOC   0x8000
#define   AZ_ION_TYPE_MASK        0x0fff
#define   AZ_ION_TYPE_XU          0x0001
#define   AZ_ION_TYPE_TIMER       0x0002
#define   AZ_ION_TYPE_MUTEX       0x0003
#define   AZ_ION_TYPE_SEM         0x0004
#define   AZ_ION_TYPE_DIR         0x0005
#define   AZ_ION_TYPE_FILE        0x0006
#define   AZ_ION_TYPE_MSGQ        0x0007
#define   AZ_ION_TYPE_IOU         0x0008
#define   AZ_ION_TYPE_UDP         0x0009
#define   AZ_ION_TYPE_TCP         0x000A
#define   AZ_ION_TYPE_LOCAL       0x000B
#define   AZ_ION_TYPE_MAX         0x000C

#define   AZ_ION_TYPE_NAMES       {"NDF", "XU", "TIMER", "MUTEX", "SEM", "DIR", "FILE", "MSGQ", "IOU", "UDP", "TCP", "LOCAL"}
/* basic macros */

/* basic types */
typedef int az_ion_id_t;
typedef az_uint32_t az_ion_type_t;

/* structures */
typedef struct az_ion {
  az_ion_id_t   id;
  az_ion_type_t type;
  az_refcount_t refCount;
} az_ion_t;
typedef az_ion_t  * az_ion_ref_t;

/* structures */

/* structured types */

/* macros */
 
/* variabls exposed */

/* inline functions */
extern char *az_ion_type_names[];
static inline char * az_ion_type_name(az_ion_type_t type) {
  extern char *__az_weak az_app_ion_type_name(az_ion_type_t);
  type = type & AZ_ION_TYPE_MASK; 
  return (type < AZ_ION_TYPE_MAX)? az_ion_type_names[type]:az_app_ion_type_name(type);
}

/* function prototypes exposed */

#ifdef __cplusplus
}
#endif
#endif
