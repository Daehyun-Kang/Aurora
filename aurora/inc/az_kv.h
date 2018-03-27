/**
 * @file   az_kv.h
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

#ifndef AZ_KV_H
#define AZ_KV_H

#include "az_def.h"
#include "az_assert.h"
#include "az_link.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* constants */

/* basic macros */

/* basic types */
typedef struct az_kv {
  char  *key;
  char  *value;
} az_kv_t;

/* structures */

/* structured types */

/* macros */

/* variabls exposed */

/* inline functions */

/* function prototypes exposed */
static inline void az_kv_init(az_kv_t *kv)
{
  az_assert(NULL != kv);
  kv->key = kv->value = NULL;
}
static inline void az_kv_deinit(az_kv_t *kv)
{
  az_assert(NULL != kv);
  kv->key = kv->value = NULL;
}


#ifdef __cplusplus
}
#endif
#endif
