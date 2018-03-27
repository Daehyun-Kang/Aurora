/**
 * @file   az_printf.h
 * @brief  
 * @date   04/05/16
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
 *   - 2016-05-04 fware initial create
 */

#ifndef AZ_PRINTF_H
#define AZ_PRINTF_H

#include "az_log.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* constants */
#ifndef AZ_NL
#define AZ_NL   AZ_SYS_NL
#endif

/* basic macros */
#define AZ_FMT_KV_STR(_k, _v)  "%" #_k "s = %" #_v "s"
#define AZ_FMT_KV_INT(_k, _v)  "%" #_k "s = %" #_v "d"
#define AZ_FMT_KV_UINT(_k, _v)  "%" #_k "s = %" #_v "u"
#define AZ_FMT_KV_INT64(_k, _v)  "%" #_k "s = %" #_v "ld"
#define AZ_FMT_KV_UINT64(_k, _v)  "%" #_k "s = %" #_v "lu"
#define AZ_FMT_KV_PTR(_k, _v)  "%" #_k "s = %" #_v "p"
#define AZ_FMT_KV_HEX(_k, _v)  "%" #_k "s = %" #_v "x"
#define AZ_FMT_KV_LHEX(_k, _v)  "%" #_k "s = %" #_v "lx"
#define AZ_FMT_KV_FLOAT(_k, _v)  "%" #_k "s = %" #_v "f"
#define AZ_FMT_KV_EXP(_k, _v)  "%" #_k "s = %" #_v "e"
#define AZ_FMT_KV_EXP2(_k, _v)  "%" #_k "s = %" #_v "E"

#ifdef  CONFIG_M64
  #define AZ_FMT_KV_SIZE(_k, _v)  "%" #_k "s = %" #_v "ld"
  #define AZ_FMT_KV_POS(_k, _v)  "%" #_k "s = %" #_v "ld"
  #define AZ_FMT_KV_USIZE(_k, _v)  "%" #_k "s = %" #_v "lu"
  #define AZ_FMT_KV_UPOS(_k, _v)  "%" #_k "s = %" #_v "lu"

  #define AZ_FMT_SIZE(_v) "%" #_v "ld"
  #define AZ_FMT_POS(_v) "%" #_v "ld"
#else
  #define AZ_FMT_KV_SIZE(_k, _v)  "%" #_k "s = %" #_v "d"
  #define AZ_FMT_KV_POS(_k, _v)  "%" #_k "s = %" #_v "d"
  #define AZ_FMT_KV_USIZE(_k, _v)  "%" #_k "s = %" #_v "u"
  #define AZ_FMT_KV_UPOS(_k, _v)  "%" #_k "s = %" #_v "u"

  #define AZ_FMT_SIZE(_v) "%" #_v "d"
  #define AZ_FMT_POS(_v) "%" #_v "d"
#endif

#define AZ_FMT_RET(_v) AZ_FMT_SIZE(_v) 


#define az_printf(...)   {\
  extern int az_logs_count;\
  if (az_logs_count > 0) {\
    az_log_printf(0, 0, AZ_LOG_FLAGS_TIME|AZ_LOG_FLAGS_PROC, AZ_FILENAME, __FUNCTION__, __LINE__, __VA_ARGS__);\
  } else {\
    az_sys_printf(__VA_ARGS__);\
  }}

#define az_eprintf(...)   {\
  extern int az_logs_count;\
  if (az_logs_count > 0) {\
    az_log_printf(0, AZ_LOG_LEVEL_ERROR, AZ_LOG_FLAGS_ALL, AZ_FILENAME, __FUNCTION__, __LINE__, __VA_ARGS__);\
  } else {\
    az_sys_printf(__VA_ARGS__);\
  }}

#define az_dprintf(...)   {\
  extern int az_logs_count;\
  if (az_logs_count > 0) {\
    az_log_printf(0, AZ_LOG_LEVEL_DEBUG, AZ_LOG_FLAGS_ALL, AZ_FILENAME, __FUNCTION__, __LINE__, __VA_ARGS__);\
  } else {\
    az_sys_printf(__VA_ARGS__);\
  }}

#define az_rprintf(r, fmt, ...) {\
  extern int az_logs_count;\
  if (az_logs_count > 0) {\
    az_dprintf("<%d:%s> " fmt, r, az_err_str(r), __VA_ARGS__); \
  } else {\
    az_sys_eprintf("<%d:%s> " fmt, r, az_err_str(r), __VA_ARGS__); \
  }\
}

#define az_log(level, flags, ...)   {\
  extern int az_logs_count;\
  if (az_logs_count > 0) {\
    az_log_printf(0, level, flags, AZ_FILENAME, __FUNCTION__, __LINE__, __VA_ARGS__);\
  } else {\
    az_sys_printf(__VA_ARGS__);\
  }\
}
#define AZ_LOG_LEVEL_SEVERE   10
#define AZ_LOG_LEVEL_ASSERT   20
#define AZ_LOG_LEVEL_ERROR    30
#define AZ_LOG_LEVEL_WARN     40
#define AZ_LOG_LEVEL_INFO     50
#define AZ_LOG_LEVEL_TRACE    60
#define AZ_LOG_LEVEL_DEBUG    70
#define AZ_LOG_LEVEL_VERBOSE  80

#define az_vlog(...) az_log(AZ_LOG_LEVEL_VERBOSE, AZ_LOG_FLAGS_ALL, __VA_ARGS__)  
#define az_dlog(...) az_log(AZ_LOG_LEVEL_DEBUG, AZ_LOG_FLAGS_ALL, __VA_ARGS__)  
#define az_tlog(...) az_log(AZ_LOG_LEVEL_TRACE, AZ_LOG_FLAGS_TIME|AZ_LOG_FLAGS_PROC|AZ_LOG_FLAGS_FUNC, __VA_ARGS__)  
#define az_ilog(...) az_log(AZ_LOG_LEVEL_INFO, AZ_LOG_FLAGS_TIME|AZ_LOG_FLAGS_PROC, __VA_ARGS__)  
#define az_wlog(...) az_log(AZ_LOG_LEVEL_WARN, AZ_LOG_FLAGS_TIME|AZ_LOG_FLAGS_PROC|AZ_LOG_FLAGS_FUNC, __VA_ARGS__)  
#define az_elog(...) az_log(AZ_LOG_LEVEL_ERROR, AZ_LOG_FLAGS_TIME|AZ_LOG_FLAGS_PROC|AZ_LOG_FLAGS_FUNC|AZ_LOG_FLAGS_LINE, __VA_ARGS__)  
#define az_alog(...) az_log(AZ_LOG_LEVEL_ASSERT, AZ_LOG_FLAGS_ALL, __VA_ARGS__)  
#define az_Slog(...) az_log(AZ_LOG_LEVEL_SEVERE, AZ_LOG_FLAGS_ALL, __VA_ARGS__)  

/* basic types */

/* structures */

/* structured types */

/* macros */

/* variabls exposed */

/* inline functions */

/* function prototypes exposed */

#ifdef __cplusplus
}
#endif
#endif
