/**
 * @file   az_def.h
 * @brief  
 * @date   05/02/16
 * @author Daehyun Kang
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
 *   - 2016-02-05 Daehyun Kang initial create
 */


#include "az_config.h"
#include "az_sys.h"
#include "az_args.h"

#ifndef AZ_DEF_H
#define AZ_DEF_H

#ifdef __cplusplus
extern "C"
{
#endif

/* constants */

/* basic macros */
#if defined(__GNUC__)
  #define AZ_PACKED_STRUCT(name)  struct __attribute__((packed)) name
#elif defined(_MSC_VER)
  #define AZ_PACKED_STRUCT(name)  __declspec(align(1)) struct name 
#else
  #error "not supported system"
#endif

/* basic types */
#ifdef  CONFIG_M64
typedef int64_t   az_size_t;
typedef uint64_t  az_usize_t;
typedef int64_t   az_pos_t;
typedef uint64_t  az_upos_t;
#elif CONFIG_M16
typedef int16_t   az_size_t;
typedef uint16_t  az_usize_t;
typedef int16_t   az_pos_t;
typedef uint16_t  az_upos_t;
#elif CONFIG_M8
typedef int8_t   az_size_t;
typedef uint8_t  az_usize_t;
typedef int8_t   az_pos_t;
typedef uint8_t  az_upos_t;
#else
typedef int32_t   az_size_t;
typedef uint32_t  az_usize_t;
typedef int32_t   az_pos_t;
typedef uint32_t  az_upos_t;
#endif

typedef int8_t   az_int8_t;
typedef int16_t  az_int16_t;
typedef int32_t  az_int32_t;
typedef int64_t  az_int64_t;
typedef uint8_t   az_uint8_t;
typedef uint16_t  az_uint16_t;
typedef uint32_t  az_uint32_t;
typedef uint64_t  az_uint64_t;

typedef enum {
  AZ_FALSE = 0,
  AZ_TRUE = 1,
} az_bool_t; 

typedef enum {
  AZ_ST_IDLE = 0,
  AZ_ST_INIT = 1,
  AZ_ST_TINI = 2,
  AZ_ST_NORM = 3
} az_state_t;

#include "az_refcount.h"

typedef char *    az_str_t;

typedef void  *   az_ref_t;

typedef az_size_t      az_r_t;

typedef struct az_range {
  az_pos_t  location;
  az_size_t length;
} az_range_t;
#define AZ_MAKE_RANGE(loc, len) {.location=loc, .length = len }
#define AZ_IN_RANGE(r, i) ((r)->location >= (i) && (i) < ((r)->location + (r)->length))

/* structures */

/* structured types */

/* macros */
#define AZ_STR(a)    #a

#ifndef   PATH_MAX
#define   PATH_MAX    512
#endif

#ifndef PATH_COMP_MAX
#define PATH_COMP_MAX   64
#endif

#define AZ_EQUAL_PTR(a, b) ((void *)(a) == (void *)(b))

#ifndef __az_section
#define __section(S)    __attribute__((__section(#S)))
#endif

#ifndef __az_used
#define __used    __attribute__((__used__))
#endif

#ifndef __az_unused
#define __unused    __attribute__((unused))
#endif

#ifndef __az_weak
#define __az_weak __attribute__((weak))
#endif

#ifndef offsetof
#ifdef  __compiler_offsetof
#define offsetof(type, member)  __compiler_offsetof(type, member)
#else
#define offsetof(type, member)  ((az_size_t)&(((type *)0)->member))
#endif
#endif

#define az_addr(base, offset) ((az_ref_t)((az_pos_t)(base) + (az_pos_t)(offset)))

#define container_of(ptr, type, member) \
    (type *)((char *)(ptr) - offsetof(type, member))

#define get_aligned_size(len, size) ((((len) + (size) - 1)/(size)) * (size))

#define array_size(arr) (sizeof(arr)/sizeof(arr[0]))

#ifdef  BASE_FILENAME
#define AZ_FILENAME BASE_FILENAME
#else
#define AZ_FILENAME __FILE__
#endif

#define _AZ_FORMAT(_tlen, _bp, _blen, _format_f, ...) { \
      az_size_t  _nlen;\
      if ((_blen) > 0) {\
        _nlen = _format_f(_bp, _blen, __VA_ARGS__);\
        _bp += _nlen; _blen -= _nlen; _tlen += _nlen;\
      }\
    }

#define _AZ_SNPRINTF(_tlen, _bp, _blen, ...) {\
  _AZ_FORMAT(_tlen, _bp, _blen, snprintf, __VA_ARGS__); } 


#define _AZ_FORMAT_UPDATE(_tlen, _bp, _blen, __nlen) { \
      if ((__nlen) > 0) {\
        az_size_t _nlen = __nlen;\
        _bp += _nlen; _blen -= _nlen; _tlen += _nlen;\
      }\
    }

/* variabls exposed */

/* inline functions */

/* function prototypes exposed */

#ifdef __cplusplus
}
#endif

#include "az_sys_def.h"

#endif
