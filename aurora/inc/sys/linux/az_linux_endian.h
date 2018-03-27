/**
 * @file   az_linux_endian.h
 * @brief  
 * @date   05/02/16
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
 *   - 2016-02-05 fware initial create
 */

#ifndef AZ_LINUX_ENDIAN_H
#define AZ_LINUX_ENDIAN_H

#include <linux/swab.h>

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
#define AZ_DEF_BE(type, v, swap_f) \
    if (!az_isBE) {\
      v = swap_f(v);\
    }\
    return v;\

#define AZ_DEF_LE(type, v, swap_f) \
    if (az_isBE) {\
      v = swap_f(v);\
    }\
    return v;\

/* variabls exposed */
extern  az_bool_t  az_isBE;

/* inline functions */
static inline uint16_t az_sys_be16(uint16_t v) 
{
  AZ_DEF_BE(az_uint16_t, v, __swap16);
}
static inline uint32_t az_sys_be32(uint32_t v) 
{
  AZ_DEF_BE(az_uint32_t, v, __swap32);
}
static inline uint32_t az_sys_be64(uint64_t v) 
{
  AZ_DEF_BE(az_uint64_t, v, __swap64);
}

static inline uint16_t az_sys_le16(uint16_t v) 
{
  AZ_DEF_LE(az_uint16_t, v, __swap16);
}
static inline uint32_t az_sys_le32(uint32_t v) 
{
  AZ_DEF_LE(az_uint32_t, v, __swap32);
}
static inline uint32_t az_sys_le64(uint64_t v) 
{
  AZ_DEF_LE(az_uint64_t, v, __swap64);
}
#undef AZ_DEF_BE
#undef AZ_DEF_LE

/* function prototypes exposed */

#ifdef __cplusplus
}
#endif
#endif
