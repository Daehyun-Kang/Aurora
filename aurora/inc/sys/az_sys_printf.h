/**
 * @file   az_sys_printf.h
 * @brief  
 * @date   28/03/18
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
 *   - 2018-03-28 dhkang initial create
 */

#ifndef AZ_SYS_PRINTF_H
#define AZ_SYS_PRINTF_H

#include "az_log_level.h"

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

#define AZ_SYS_TIMESTAMP_FMT "<%02d:%02d:%02d:%06d>  " 

#ifdef  CONFIG_AZ_SYS_PRINT_TIMESTAMP
#define az_sys_printf0(fmt) {\
  az_sys_timespec_t _ts; \
  clock_gettime(CLOCK_REALTIME, &_ts);\
  az_sys_time_t _t = (az_sys_time_t)_ts.tv_sec;\
  az_sys_datetime_t *tp = az_sys_localtime(&_t);\
  if (az_sys_log_flags & AZ_LOG_FLAGS_FILE) { \
  printf(AZ_SYS_TIMESTAMP_FMT " [%s] " fmt,tp->tm_hour, tp->tm_min, tp->tm_sec, (int)(_ts.tv_nsec/1000), AZ_FILENAME);\
  } else { \
  printf(AZ_SYS_TIMESTAMP_FMT fmt,tp->tm_hour, tp->tm_min, tp->tm_sec, (int)(_ts.tv_nsec/1000));\
  }\
}

#define az_sys_printf(fmt, ...) {\
  az_sys_timespec_t _ts; \
  clock_gettime(CLOCK_REALTIME, &_ts);\
  az_sys_time_t _t = (az_sys_time_t)_ts.tv_sec;\
  az_sys_datetime_t *tp = az_sys_localtime(&_t);\
  if (az_sys_log_flags & AZ_LOG_FLAGS_FILE) { \
  printf(AZ_SYS_TIMESTAMP_FMT " [%s] " fmt,tp->tm_hour, tp->tm_min, tp->tm_sec, (int)(_ts.tv_nsec/1000), AZ_FILENAME, __VA_ARGS__);\
  } else { \
  printf(AZ_SYS_TIMESTAMP_FMT fmt,tp->tm_hour, tp->tm_min, tp->tm_sec, (int)(_ts.tv_nsec/1000), __VA_ARGS__);\
  }\
}
#else
#define az_sys_printf0(...) printf(__VA_ARGS__)
#define az_sys_printf(...) printf(__VA_ARGS__)
#endif

#define az_sys_log_printf0(level, flags, fmt) if (level <= az_sys_log_level) {\
  if (flags & AZ_LOG_FLAGS_TIME) {\
  az_sys_timespec_t _ts; \
  clock_gettime(CLOCK_REALTIME, &_ts);\
  az_sys_time_t _t = (az_sys_time_t)_ts.tv_sec;\
  az_sys_datetime_t *tp = az_sys_localtime(&_t);\
  printf( AZ_SYS_TIMESTAMP_FMT "[%s:%d] " fmt,tp->tm_hour, tp->tm_min, tp->tm_sec, (int)(_ts.tv_nsec/1000), __FUNCTION__, __LINE__);\
  } else {\
    printf("[%s:%d] " fmt, __FUNCTION__, __LINE__);\
  }\
}
#define az_sys_log_printf(level, flags, fmt, ...) if (level <= az_sys_log_level) {\
  if (flags & AZ_LOG_FLAGS_TIME) {\
  az_sys_timespec_t _ts; \
  clock_gettime(CLOCK_REALTIME, &_ts);\
  az_sys_time_t _t = (az_sys_time_t)_ts.tv_sec;\
  az_sys_datetime_t *tp = az_sys_localtime(&_t);\
  printf(AZ_SYS_TIMESTAMP_FMT "[%s:%d] " fmt,tp->tm_hour, tp->tm_min, tp->tm_sec, (int)(_ts.tv_nsec/1000), __FUNCTION__, __LINE__, __VA_ARGS__); \
  } else {\
    printf("[%s:%d] " fmt, __FUNCTION__, __LINE__, __VA_ARGS__);\
  }\
}

#ifdef  CONFIG_AZ_SYS_PRINT_TIMESTAMP
#define az_sys_rprintf(r, fmt, ...) if (az_sys_log_level >= AZ_LOG_LEVEL_ERROR) {\
  az_sys_timespec_t _ts; \
  clock_gettime(CLOCK_REALTIME, &_ts);\
  az_sys_time_t _t = (az_sys_time_t)_ts.tv_sec;\
  az_sys_datetime_t *tp = az_sys_localtime(&_t);\
  printf(AZ_SYS_TIMESTAMP_FMT "[%s:%d] <%d:%s>" fmt,tp->tm_hour, tp->tm_min, tp->tm_sec, (int)(_ts.tv_nsec/1000), __FUNCTION__, __LINE__, (int)r, az_err_str(r), __VA_ARGS__); \
}
#else
#define az_sys_rprintf(r, fmt, ...) if (az_sys_log_level >= AZ_LOG_LEVEL_ERROR) printf("[%s:%d] <%d:%s>" fmt, __FUNCTION__, __LINE__, (int)r, az_err_str(r), __VA_ARGS__)
#endif

#define az_sys_eprintf0(fmt) az_sys_log_printf0(AZ_LOG_LEVEL_ERROR, AZ_LOG_FLAGS_ALL, fmt)

#define az_sys_eprintf(...) az_sys_log_printf(AZ_LOG_LEVEL_ERROR, AZ_LOG_FLAGS_ALL, __VA_ARGS__)

#define az_sys_dprintf0(fmt) az_sys_log_printf0(AZ_LOG_LEVEL_DEBUG, AZ_LOG_FLAGS_ALL, fmt)

#define az_sys_dprintf(...) az_sys_log_printf(AZ_LOG_LEVEL_DEBUG, AZ_LOG_FLAGS_ALL, __VA_ARGS__)


#define az_sys_vlog(...) az_sys_log_printf(AZ_LOG_LEVEL_VERBOSE, AZ_LOG_FLAGS_ALL, __VA_ARGS__)  
#define az_sys_dlog(...) az_sys_log_printf(AZ_LOG_LEVEL_DEBUG, AZ_LOG_FLAGS_ALL, __VA_ARGS__)  
#define az_sys_tlog(...) az_sys_log_printf(AZ_LOG_LEVEL_TRACE, AZ_LOG_FLAGS_TIME|AZ_LOG_FLAGS_PROC|AZ_LOG_FLAGS_FUNC, __VA_ARGS__)  
#define az_sys_ilog(...) az_sys_log_printf(AZ_LOG_LEVEL_INFO, AZ_LOG_FLAGS_TIME|AZ_LOG_FLAGS_PROC, __VA_ARGS__)  
#define az_sys_wlog(...) az_sys_log_printf(AZ_LOG_LEVEL_WARN, AZ_LOG_FLAGS_TIME|AZ_LOG_FLAGS_PROC|AZ_LOG_FLAGS_FUNC, __VA_ARGS__)  
#define az_sys_elog(...) az_sys_log_printf(AZ_LOG_LEVEL_ERROR, AZ_LOG_FLAGS_TIME|AZ_LOG_FLAGS_PROC|AZ_LOG_FLAGS_FUNC|AZ_LOG_FLAGS_LINE, __VA_ARGS__)  
#define az_sys_alog(...) az_sys_log_printf(AZ_LOG_LEVEL_ASSERT, AZ_LOG_FLAGS_ALL, __VA_ARGS__)  
#define az_sys_Slog(...) az_sys_log_printf(AZ_LOG_LEVEL_SEVERE, AZ_LOG_FLAGS_ALL, __VA_ARGS__)  

/* variabls exposed */
extern int az_sys_log_level;
extern int az_sys_log_flags;

/* inline functions */

/* function prototypes exposed */

#ifdef __cplusplus
}
#endif
#endif
