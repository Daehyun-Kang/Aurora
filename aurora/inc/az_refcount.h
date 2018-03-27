/**
 * @file   az_refcount.h
 * @brief  
 * @date   11/12/17
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
 *   - 2017-12-11 dhkang initial create
 */

#ifndef AZ_REFCOUNT_H
#define AZ_REFCOUNT_H

#include "az_atomic.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* constants */

/* basic macros */

/* basic types */
typedef struct az_refcount {
  int32_t   count;
  uint8_t   is_static:1;
} az_refcount_t;

/* structures */

/* structured types */

/* macros */
#define AZ_REFCOUNT_STATIC_ZERO  {.is_static = 1, .count = 0} 
#define AZ_REFCOUNT_DYNAMIC_ZERO  {.is_static = 0, .count = 0} 
#define AZ_REFCOUNT_IS_STATIC(p)  (((az_refcount_t *)p)->is_static)
#define AZ_REFCOUNT_IS_DYNAMIC(p)  (!((az_refcount_t *)p)->is_static)
#define AZ_REFCOUNT_VALUE(p)    ((int32_t)(((az_refcount_t *)p)->count))
#define AZ_REFCOUNT_IS_ZERO(p)  (*((int32_t *)p) == 0 && AZ_REFCOUNT_IS_DYNAMIC(p))

/* variabls exposed */

/* inline functions */
static inline void az_refcount_init_static(az_refcount_t *p)
{
  p->is_static = 1;
  p->count = 0;
}
static inline void az_refcount_init_dynamic(az_refcount_t *p)
{
  p->is_static = 0;
  p->count = 0;
}
#define az_refcount_atomic_inc(p) az_atomic_inc32((int32_t *)p)
#define az_refcount_atomic_dec(p) az_atomic_dec32((int32_t *)p)

/* function prototypes exposed */

#ifdef __cplusplus
}
#endif
#endif
