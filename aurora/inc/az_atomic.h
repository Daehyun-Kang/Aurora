/**
 * @file   az_atomic.h
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

#ifndef AZ_ATOMIC_H
#define AZ_ATOMIC_H

#include "az_config.h"
#include "az_def.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* constants */

/* basic macros */

/* basic types */

/* structures */

/* structured types */

/* macros */
#if defined(__GNUC__)
#include "az_atomic_gnuc.h"
#elif defined(_MSC_VER)
#include "az_atomic_msvc.h"
#else
#error "GNUC or MSVC needs!"
#endif

#ifdef  CONFIG_M64
#define az_atomic_inc_size(ptr)   az_atomic_inc64(ptr)
#define az_atomic_dec_size(ptr)   az_atomic_dec64(ptr)
#define az_atomic_compare_and_swap_ptr(ptr, oldval, newval) \
          az_atomic_compare_and_swap64(ptr, oldval, newval) 
#else
#define az_atomic_inc_size(ptr)   az_atomic_inc32(ptr)
#define az_atomic_dec_size(ptr)   az_atomic_dec32(ptr)
#define az_atomic_compare_and_swap_ptr(ptr, oldval, newval) \
            az_atomic_compare_and_swap32(ptr, oldval, newval) 
#endif

/* variabls exposed */

/* inline functions */

/* function prototypes exposed */

#ifdef __cplusplus
}
#endif
#endif
