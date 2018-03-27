/**
 * @file   az_var.h
 * @brief  
 * @date   10/03/16
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
 *   - 2016-03-10 fware initial create
 */

#ifndef AZ_VAR_H
#define AZ_VAR_H

#include "az_def.h"
#include "az_err.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* constants */

/* basic macros */
#define AZ_VAR_DFT_DOUBLE_FMT   "%.6f" 
#define AZ_VAR_DFT_FLOAT_FMT    "%.3f" 
#define AZ_VAR_DFT_INT_FMT      "%d" 
#define AZ_VAR_DFT_INT64_FMT    "%ld" 
#define AZ_VAR_DFT_UINT_FMT     "%u" 
#define AZ_VAR_DFT_UINT64_FMT   "%lu" 
#define AZ_VAR_DFT_HEX_FMT      "%x" 
#define AZ_VAR_DFT_HEX64_FMT    "%lx" 
#define AZ_VAR_DFT_CHAR_FMT     "%c" 
#define AZ_VAR_DFT_PTR_FMT      "%p" 

#ifdef  CONFIG_M64
#define AZ_VAR_DFT_SIZE_FMT   "%ld"
#define AZ_VAR_DFT_POS_FMT    "%ld"
#define AZ_VAR_DFT_USIZE_FMT  "%lu"
#define AZ_VAR_DFT_UPOS_FMT   "%lu"
#else
#define AZ_VAR_DFT_SIZE_FMT   "%d"
#define AZ_VAR_DFT_POS_FMT    "%d"
#define AZ_VAR_DFT_USIZE_FMT  "%u"
#define AZ_VAR_DFT_UPOS_FMT   "%u"
#endif

#define AZ_VAR_PRINT_KV_LINE_WIDTH   64

/* basic types */
typedef enum {
  AZ_VAR_TYPE_NULL = 0,
  AZ_VAR_TYPE_INT8,
  AZ_VAR_TYPE_INT16,
  AZ_VAR_TYPE_INT32,
  AZ_VAR_TYPE_INT64,
  AZ_VAR_TYPE_UINT8,
  AZ_VAR_TYPE_UINT16,
  AZ_VAR_TYPE_UINT32,
  AZ_VAR_TYPE_UINT64,
  AZ_VAR_TYPE_BIT8,
  AZ_VAR_TYPE_BIT16,
  AZ_VAR_TYPE_BIT32,
  AZ_VAR_TYPE_BIT64,
  AZ_VAR_TYPE_FLOAT,
  AZ_VAR_TYPE_DOUBLE,
  AZ_VAR_TYPE_CHAR,
  AZ_VAR_TYPE_SIZE,
  AZ_VAR_TYPE_POS,
  AZ_VAR_TYPE_USIZE,
  AZ_VAR_TYPE_UPOS,
  AZ_VAR_TYPE_BOOL,
  AZ_VAR_TYPE_ENUM,
  AZ_VAR_TYPE_RET,
  AZ_VAR_TYPE_REF,  // void *
  AZ_VAR_TYPE_ARRAY,
  AZ_VAR_TYPE_STRING,
  AZ_VAR_TYPE_VECTOR,
  AZ_VAR_TYPE_DESCR,
  AZ_VAR_TYPE_MAX,
} az_var_type_t;

/* structures */
typedef union {
  az_int8_t         i8;
  az_int16_t        i16;
  az_int32_t        i32;
  az_int64_t        i64;
  az_uint8_t        u8;
  az_uint16_t       u16;
  az_uint32_t       u32;
  az_uint64_t       u64;
  az_uint8_t        b8;
  az_uint16_t       b16;
  az_uint32_t       b32;
  az_uint64_t       b64;
  float             f;
  double            d;
  char              c;
  az_size_t         size;
  az_pos_t          pos;
  az_usize_t        usize;
  az_upos_t         upos;
  az_bool_t         b;
  az_ref_t          ref;
  az_r_t            r;
  az_ref_t          a;
  char*             s;
  az_ref_t          v;
} az_var_value_t;

typedef struct az_var {
  az_var_type_t   type;
  az_var_value_t  value;
} az_var_t;

typedef int (*az_var_print_scalar_value_t)(char *, az_size_t, void *, int); 

/* structured types */
typedef struct az_var_descr {
  char    *name;
  az_var_type_t type;
  az_bool_t   onoff;
  az_pos_t    offset;
  az_size_t   size;
} az_var_descr_t;

/* macros */
#define AZ_VAR_DESCR_SET(_n, _t, _o, _s, _e) \
  { .name = _n, .type = _t, .offset = _o, .size = _s, .onoff=_e }

#define AZ_VAR_DESCR_DEF(NAME, TYPE, MEMBER, descr_type) \
  { .name = #NAME, \
    .type = descr_type, \
    .offset = (az_pos_t)&(((TYPE *)0)->MEMBER),\
    .size = sizeof(((TYPE *)0)->MEMBER),\
    .onoff=1 }
#define AZ_VAR_DESCR_ARRAY(NAME, TYPE, MEMBER, var_size) \
  { .name = #NAME, \
    .type = AZ_VAR_TYPE_ARRAY, \
    .offset = (az_pos_t)&(((TYPE *)0)->MEMBER),\
    .size = var_size,\
    .onoff=1 }
#define AZ_VAR_DESCR_REF(descrs, TYPE, MEMBER, var_size) \
  { .name = (char *)descrs, \
    .type = AZ_VAR_TYPE_DESCR, \
    .offset = (az_pos_t)&(((TYPE *)0)->MEMBER),\
    .size = var_size,\
    .onoff=1 }
#define AZ_VAR_DESCR_NULL AZ_VAR_DESCR_SET(NULL, AZ_VAR_TYPE_NULL, 0, 0, 0)

typedef enum az_var_print_mode {
  AZ_VAR_PRINT_KV,
  AZ_VAR_PRINT_COL,
  AZ_VAR_PRINT_ROW,
} az_var_print_mode_t;

typedef char *(*az_var_value2Str_t)(void *);

typedef struct az_var_print_format {
  az_var_print_mode_t mode;
  char  *tag;
  char  *title_fmt;
  int   content_width;
  int   line_width;
  char  *width_list;
  az_var_value2Str_t *value2StrList;
  az_var_value2Str_t value2Str;
} az_var_print_format_t;

#define AZ_VAR_PRINT_KV_FMT_DEFAULT(_tag, _title_fmt, _content_width) \
  {\
    .mode = AZ_VAR_PRINT_KV,\
    .tag = _tag,\
    .title_fmt = _title_fmt,\
    .width_list = NULL,\
    .content_width = _content_width,\
    .line_width = AZ_VAR_PRINT_KV_LINE_WIDTH,\
    .value2StrList = NULL,\
    .value2Str = NULL } 

#define AZ_VAR_PRINT_ROW_FMT_DEFAULT(_tag, _wlist, _content_width) \
  {\
    .mode = AZ_VAR_PRINT_ROW,\
    .tag = _tag,\
    .title_fmt = NULL,\
    .width_list = _wlist, \
    .content_width = _content_width,\
    .line_width = 256 ,\
    .value2StrList = NULL,\
    .value2Str = NULL } 


/* variabls exposed */
extern char *az_var_type_names[];

/* inline functions */

/* function prototypes exposed */
extern az_size_t az_var_print(az_uint8_t *pVar, az_var_descr_t *pDescr, 
    az_var_print_format_t *format, char *bp, az_size_t blen);

extern az_size_t az_var_printVars(az_uint8_t *pVar, az_var_descr_t *pDescr, 
    az_var_print_format_t *format, char *bp, az_size_t blen,
    void (*init_descr)(az_uint8_t *, az_var_descr_t *)); 
#ifdef __cplusplus
}
#endif
#endif
