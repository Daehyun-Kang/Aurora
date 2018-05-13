/**
 * @file   az_toStr.h
 * @brief  
 * @date   07/05/18
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
 *   - 2018-05-07 dhkang initial create
 */

#ifndef AZ_TOSTR_H
#define AZ_TOSTR_H

#include  "az_def.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* constants */
#define AZ_TOSTR_SEP  ":"
#define AZ_TOSTR_FMT_KV(kfmt,vfmt)  kfmt AZ_TOSTR_SEP vfmt
#define AZ_TOSTR_FMT_TYPE(vw, t, sp)        "%" vw AZ_TOSTR_FMT_#t sp
#define AZ_TOSTR_FMT_KV_TYPE(kw, vw, t, sp)     AZ_TOSTR_FMT_KV("%" kw "s", AZ_TOSTR_FMT_TYPE(vw, t, sp)) 

#define AZ_TOSTR(tlen, bp, blen, kw, vw, sp, type, name, value) \
    _AZ_SNPRINTF(tlen, bp, blen, AZ_TOSTR_FMT_KV_TYPE(kw, vw, type, sp), name, value)

#define AZ_TOSTR_MEMBER(tlen, bp, blen, kw, vw, sp, ptype, ptr, type, name, value) \
  if (NULL == (void *)(ptr)) {\
    _AZ_SNPRINTF(tlen, bp, blen, AZ_TOSTR_FMT_KV_TYPE(kw, vw, type, sp), name, value);\
  } else { \
    _AZ_SNPRINTF(tlen, bp, blen, AZ_TOSTR_FMT_KV_TYPE(kw, vw, type, sp), name, ((ptype *)(ptr))->value);\
  }
#define AZ_TOSTR_VALUE(tlen, bp, blen, vw, sp, type, value) \
    _AZ_SNPRINTF(tlen, bp, blen, AZ_TOSTR_FMT_TYPE(vw, type, sp), value)

#define AZ_TOSTR_FMT_char            "c"
#define AZ_TOSTR_FMT_uchar           "u"
#define AZ_TOSTR_FMT_wchar_t         "C" // "%lc"
#define AZ_TOSTR_FMT_az_wchar_t      "C" // "%lc"
#define AZ_TOSTR_FMT_short           "d"
#define AZ_TOSTR_FMT_ushort          "u"
#define AZ_TOSTR_FMT_int             "d"
#define AZ_TOSTR_FMT_uint            "u"
#define AZ_TOSTR_FMT_long            "ld"
#define AZ_TOSTR_FMT_ulong           "lu"
#define AZ_TOSTR_FMT_float           "f"
#define AZ_TOSTR_FMT_double          "f"
#define AZ_TOSTR_FMT_Float           "e"
#define AZ_TOSTR_FMT_Double          "e"
#define AZ_TOSTR_FMT_FLOAT           "E"
#define AZ_TOSTR_FMT_DOUBLE          "E"
#define AZ_TOSTR_FMT_Float_t         "g"
#define AZ_TOSTR_FMT_Double_t        "g"
#define AZ_TOSTR_FMT_FLOAT_t         "G"
#define AZ_TOSTR_FMT_DOUBLE_t        "G"
#define AZ_TOSTR_FMT_str             "s"
#define AZ_TOSTR_FMT_wstr            "S" // "%ls"
#define AZ_TOSTR_FMT_hex             "#x" 
#define AZ_TOSTR_FMT_HEX             "#X" 
#define AZ_TOSTR_FMT_lhex            "#lx" 
#define AZ_TOSTR_FMT_LHEX            "#lX" 

#define AZ_TOSTR_FMT_int8_t          "d"
#define AZ_TOSTR_FMT_uint8_t         "u"
#define AZ_TOSTR_FMT_az_int8_t       "d"
#define AZ_TOSTR_FMT_az_uint8_t      "u"
#define AZ_TOSTR_FMT_int16_t         "d"
#define AZ_TOSTR_FMT_uint16_t        "u"
#define AZ_TOSTR_FMT_az_int16_t      "d"
#define AZ_TOSTR_FMT_az_uint16_t     "u"
#define AZ_TOSTR_FMT_int32_t         "d"
#define AZ_TOSTR_FMT_uint32_t        "u"
#define AZ_TOSTR_FMT_az_int32_t      "d"
#define AZ_TOSTR_FMT_az_uint32_t     "u"
#define AZ_TOSTR_FMT_int64_t         "ld"
#define AZ_TOSTR_FMT_uint64_t        "lu"
#define AZ_TOSTR_FMT_az_int64_t      "ld"
#define AZ_TOSTR_FMT_az_uint64_t     "lu"
#define AZ_TOSTR_FMT_ptr             "p"
#define AZ_TOSTR_FMT_az_ref_t        "p"
#define AZ_TOSTR_FMT_az_str_t        "s"
#define AZ_TOSTR_FMT_az_wstr_t       "S"

#ifdef  CONFIG_M64
#define AZ_TOSTR_FMT_az_size_t       "ld"
#define AZ_TOSTR_FMT_az_pos_t        "ld"
#define AZ_TOSTR_FMT_az_usize_t      "lu"
#define AZ_TOSTR_FMT_az_upos_t       "lu"
#define AZ_TOSTR_FMT_az_r_t          "ld"
#else
#define AZ_TOSTR_FMT_az_size_t       "d"
#define AZ_TOSTR_FMT_az_pos_t        "d"
#define AZ_TOSTR_FMT_az_usize_t      "u"
#define AZ_TOSTR_FMT_az_upos_t       "u"
#define AZ_TOSTR_FMT_az_r_t          "d"
#endif

/* basic macros */

/* basic types */

/* structures */

/* structured types */

/* macros */

/* variabls exposed */
extern char *az_bool_t_toStr[];
extern char *az_state_t_toStr[];

/* inline functions */

/* function prototypes exposed */

#ifdef __cplusplus
}
#endif
#endif

/* === end of AZ_TOSTR_H === */
