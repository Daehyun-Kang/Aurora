/**
 * @file   az_log.h
 * @brief  
 * @date   03/05/16
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
 *   - 2016-05-03 fware initial create
 */

#ifndef AZ_LOG_H
#define AZ_LOG_H

#include "az_cirbuffer.h"
#include "sys/az_xu.h"
#include "sys/az_datetime.h"

#ifdef  CONFIG_AZ_USE_STDARG
#include <stdarg.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

/* constants */

/* basic macros */
#ifndef CONFIG_AZ_LOG_MAX 
#define CONFIG_AZ_LOG_MAX   8
#endif

#ifndef CONFIG_AZ_LOG_OUTPORT_MAX
#define CONFIG_AZ_LOG_OUTPORT_MAX 4
#endif
  
#define  AZ_LOG_USE_FORMAT_INFO_TABLE   1


#define AZ_DATETIME_STR       "%02d/%02d/%02d %02d:%02d:%02d"
#define AZ_TIMESTAMP_STR      "%02d:%02d:%02d.%06d"
#define AZ_TIMESTAMP_STR0     "%06d.%06d"

#ifdef  CONFIG_AZ_PRINTF_BUF_SZ
#define AZ_PRINTF_BUF_SZ  CONFIG_AZ_PRINTF_BUF_SZ
#else
#define AZ_PRINTF_BUF_SZ  2048
#endif

#ifdef  CONFIG_AZ_LOG_ADD_NL
#define AZ_LOG_NL  AZ_NL 
#else
#define AZ_LOG_NL 
#endif

#define AZ_LOG_STATE_INIT     0x0001
#define AZ_LOG_STATE_START    0x0002
#define AZ_LOG_STATE_READY    0x0003
#define AZ_LOG_STATE_MASK     0x0fff
#define AZ_LOG_STATE_DALLOC   0x8000
#define AZ_LOG_STATE_DBUFFER  0x4000

// include for log level and flags
#include "az_log_level.h"

/* basic types */
typedef unsigned int az_logid_t;
typedef uint8_t az_log_flag_t;
typedef uint8_t az_log_level_t;
typedef uint8_t az_log_port_type_t;
typedef uint8_t az_log_port_state_t;
typedef union { 
  az_sys_fd_t     fd;
  az_sys_file_t   file;
  az_sys_socket_t sock;
} az_log_end_point_t;

struct az_log_port_entity;
typedef int (*az_log_port_write_t)(struct az_log_port_entity *, const char *, int);

#define   AZ_LOG_PORT_TYPE_INVD   0x00
#define   AZ_LOG_PORT_TYPE_FD     0x01
#define   AZ_LOG_PORT_TYPE_FILE   0x02
#define   AZ_LOG_PORT_TYPE_SOCK   0x03

#define   AZ_LOG_PORT_IDLE      0x00
#define   AZ_LOG_PORT_ATTACHED  0x01
#define   AZ_LOG_PORT_EP_ON     0x02
#define   AZ_LOG_PORT_ACTIVE    (AZ_LOG_PORT_ATTACHED|AZ_LOG_PORT_EP_ON) 

/* structures */
typedef struct az_log_port_entity {
  az_log_port_type_t type;
  az_log_port_state_t state;
  az_log_end_point_t  ep;
  az_log_port_write_t  Write;
} az_log_port_entity_t;

typedef az_log_port_entity_t * az_log_port_t;

#define AZ_LOG_PORT_INVALID (az_log_port_t)(NULL)

typedef struct az_log_config {
  char  *name;
  az_log_flag_t   flags;
  az_log_level_t  level;
  int   buffer_size;
} az_log_config_t;

typedef struct az_log {
  az_logid_t  logid;
  int   state;

  az_log_config_t config;

  az_cirbuffer_t  buffer;
  uint8_t         *buffer_area;

  az_ion_id_t     thread_id;

  int (*out)(struct az_log *, const char *, int);

  uint8_t outport_count;
  az_log_port_entity_t  *outports[CONFIG_AZ_LOG_OUTPORT_MAX];
} az_log_t;

#include "az_log_dup.h"

/* structured types */
typedef int (*az_log_format_info_t)(const char *filename, const char *funcname, int line, char *bp, int blen);

/* macros */
#define AZ_LOG_READY(log) \
    (((log)->state & AZ_LOG_STATE_MASK) == AZ_LOG_STATE_READY)
#define AZ_LOGID_INVALID  CONFIG_AZ_LOG_MAX

/* variabls exposed */
extern az_log_t *az_logs[];

#define AZ_LOGS(id) \
  (((uint32_t)(id) < CONFIG_AZ_LOG_MAX)? az_logs[(uint32_t)(id)]:NULL)

/* inline functions */
static inline int az_print_timestampInSeconds(char *bp, int blen, const char *fmt, az_sys_timespec_t *tv)
{
  az_assert(NULL != tv && NULL != bp);
  return snprintf(bp, blen, fmt, (int)tv->tv_sec, (int)tv->tv_nsec/1000);
}
static inline int az_print_timestampInDatetime(char *bp, int blen, const char *fmt, az_sys_timespec_t *tv)
{
  az_assert(NULL != tv && NULL != bp);
  az_sys_time_t _t = (az_sys_time_t)tv->tv_sec;
  az_sys_datetime_t *tp = az_sys_localtime(&_t);
  return snprintf(bp, blen, fmt, tp->tm_hour, tp->tm_min, tp->tm_sec, (int)(tv->tv_nsec/1000));
}
static inline int az_print_timestamp(char *bp, int blen, const char *fmt, az_sys_timespec_t *tv, int opt)
{
  int r = 0;
  switch (opt) {
    case 0: r = az_print_timestampInSeconds(bp, blen, fmt, tv); break;
    case 1: r = az_print_timestampInDatetime(bp, blen, fmt, tv); break;
  }
  return r;
}


#define AZ_LOG_FORMAT_INFO_TIMESTAMP(bp, blen, tlen)\
{\
  int _nlen;\
  az_sys_timespec_t _ts;\
  clock_gettime(CLOCK_REALTIME, &_ts);\
  _nlen = az_print_timestampInDatetime(bp, blen, "<" AZ_TIMESTAMP_STR ">  ", &_ts);\
  blen -= _nlen; tlen += _nlen;\
}

#define AZ_LOG_FORMAT_INFO_PROC(bp, blen, tlen)\
{\
  int _nlen;\
  char *_bp = bp;\
  az_xu_t xu = az_xu_self();\
  char *xu_name = ((xu == NULL)? "????":AZ_XU_NAME(xu));\
  _nlen = sprintf(_bp, "[%s]  ", xu_name);\
  blen -= _nlen; tlen += _nlen;\
}

extern az_log_format_info_t az_log_format_info[]; 

#if 0
#endif
extern int az_log_port_write2fd(az_log_port_t port, const char *bp, int len);
extern int az_log_port_write2file(az_log_port_t port, const char *bp, int len);
extern int az_log_port_write2sock(az_log_port_t port, const char *bp, int len);

extern az_sys_lw_lock_t az_log_ctrl_lock;

static inline az_r_t az_log_add_port(az_log_t *log, az_log_port_t port)
{
  az_assert(NULL != log);
  az_assert(NULL != port);
  az_r_t r = AZ_SUCCESS;
  int j, k = -1;
  az_log_port_t *outports = log->outports;
  az_sys_lw_lock(&az_log_ctrl_lock);
  do {
    for (j = 0; j < CONFIG_AZ_LOG_OUTPORT_MAX; j++) {
      if (outports[j] == port) {
        r = AZ_ERR(AGAIN);
        break;
      } else if (outports[j] == NULL) {
        if (k < 0) k = j;
      }
    }
    if (k < 0) {
      r = AZ_ERR(FULL);
      break;
    }
    outports[k] = port;
    port->state |= AZ_LOG_PORT_ATTACHED;
    log->outport_count++;
  } while (0);
  az_sys_lw_unlock(&az_log_ctrl_lock);

  return r;
}
static inline int az_log_del_port(az_log_t *log, az_log_port_t port)
{
  az_assert(NULL != log);
  az_r_t r = AZ_ERR(NOT_FOUND);
  int j;
  az_log_port_t *outports = log->outports; 
  az_sys_lw_lock(&az_log_ctrl_lock);
  do {
    for (j = 0; j < CONFIG_AZ_LOG_OUTPORT_MAX; j++) {
      if (outports[j] == port) {
        r = AZ_SUCCESS;
        outports[j] = NULL;
        port->state &= ~AZ_LOG_PORT_ATTACHED;
        log->outport_count--;
        break;
      }
    }
  } while (0);
  az_sys_lw_unlock(&az_log_ctrl_lock);

  return r;
}
static inline int az_log_out2port(az_log_t *log, const char *data, int datalen) 
{
  az_assert(NULL != log);
  az_assert(NULL != data);
  int j, outport_count;
  az_log_port_t outport;
  outport_count = log->outport_count;
  for (j = 0; outport_count > 0 && j < CONFIG_AZ_LOG_OUTPORT_MAX;j++) {
    outport = log->outports[j];
    if (outport == NULL) continue;
    outport_count--;
    if (outport->state != AZ_LOG_PORT_ACTIVE) continue;
    (outport->Write)(outport, data, datalen);
  }
  return datalen;
}

static inline int az_log_out2buffer(az_log_t *log, const char *data, int datalen) 
{
  az_assert(NULL != log);
  az_assert(NULL != data);
  int tlen = 0;
  tlen = az_cirbuffer_push(&log->buffer, (uint8_t *)data, datalen);
  return tlen;
}

static inline int az_log_write(az_logid_t logid, az_log_level_t level, const char *data, int datalen) 
{
  az_log_t  *log = AZ_LOGS(logid);
  int tlen = 0;
  az_assert(NULL != log);
  az_assert(NULL != log->out);

  do {
    if (!AZ_LOG_READY(log)) {
      tlen = AZ_ERR(STATE);
      break;
    }
    if (level > log->config.level) { 
      tlen = AZ_ERR(LEVEL);
      break;
    }
    tlen = (log->out)(log, data, datalen);
  } while (0);

  return tlen;
}

#ifdef  AZ_LOG_USE_FORMAT_INFO_TABLE
static inline int az_log_printf(az_logid_t logid, az_log_level_t level, az_log_flag_t flags, const char *filename, const char *funcname, int line, const char *fmt, ...)
{
  char _logbuffer[AZ_PRINTF_BUF_SZ];
  uint8_t *bp = (uint8_t *)_logbuffer;
  int blen = AZ_PRINTF_BUF_SZ;
  int nlen = 0;
  int tlen = 0;

  az_log_t  *log = AZ_LOGS(logid);
  az_assert(NULL != log);
  az_assert(NULL != log->out);

  //az_sys_printf("logid:%p %s\n", log, fmt);
  do {
    if (!AZ_LOG_READY(log)) {
      tlen = AZ_ERR(STATE);
      break;
    }
    if (level > log->config.level) { 
      tlen = AZ_ERR(LEVEL);
      break;
    }

    if (flags) flags = flags & AZ_LOG_FLAGS_ALL;
    else flags = log->config.flags & AZ_LOG_FLAGS_ALL; 

    if (flags) {
      az_log_format_info_t  format_info = az_log_format_info[flags & AZ_LOG_FLAGS_ALL];
      nlen = format_info(filename, funcname, line, (char *)bp, blen);
      blen -= nlen; tlen += nlen;
    }

#ifdef  CONFIG_AZ_USE_STDARG
    va_list ap;
    va_start(ap, fmt);
    nlen = vsnprintf((char *)&bp[tlen], blen, fmt, ap);
    va_end(ap);
    blen -= nlen; tlen += nlen;
#endif

    tlen = (log->out)(log, (const char *)bp, tlen);
  //az_sys_printf("logid:%p tlen=%d\n", log, tlen);
  } while (0);

  return tlen;
}
#else
static inline int az_log_printf(az_logid_t logid, az_log_level_t level, az_log_flag_t flags, char *filename, char *funcname, int line, const char *fmt, ...)
{
  char _logbuffer[AZ_PRINTF_BUF_SZ];
  char *bp = _logbuffer;
  int blen = AZ_PRINTF_BUF_SZ;
  int nlen = 0;
  int tlen = 0;

  az_log_t  *log = AZ_LOGS(logid);
  az_assert(NULL != log);
  az_assert(NULL != log->out);

  do {
    if (!AZ_LOG_READY(log)) {
      tlen = AZ_ERR(STATE);
      break;
    }
    if (level > log->config.level) { 
      tlen = AZ_ERR(LEVEL);
      break;
    }
    if (flags) flags = flags & AZ_LOG_FLAGS_ALL;
    else flags = log->config.flags & AZ_LOG_FLAGS_ALL; 

    if (flags & AZ_LOG_FLAGS_TIME) {
      AZ_LOG_FORMAT_INFO_TIMESTAMP(bp, blen, tlen);
    }
    if (flags & AZ_LOG_FLAGS_PROC) {
      AZ_LOG_FORMAT_INFO_PROC(&bp[tlen], blen, tlen);
    }

    switch (flags & AZ_LOG_FLAGS_SPOT) {
      case  AZ_LOG_FLAGS_FUNC|AZ_LOG_FLAGS_LINE:
        nlen = snprintf(&bp[tlen], blen, "[%s:%d] ", funcname, line);
        blen -= nlen; tlen += nlen;
        break;
      case  AZ_LOG_FLAGS_FUNC|AZ_LOG_FLAGS_LINE|AZ_LOG_FLAGS_FILE:
        nlen = snprintf(&bp[tlen], blen, "[%s:%s:%d] ", filename, funcname, line);
        blen -= nlen; tlen += nlen;
        break;
      case  AZ_LOG_FLAGS_FUNC|AZ_LOG_FLAGS_FILE:
        nlen = snprintf(&bp[tlen], blen, "[%s:%s] ", filename, funcname);
        blen -= nlen; tlen += nlen;
        break;
      case  AZ_LOG_FLAGS_FILE|AZ_LOG_FLAGS_LINE:
        nlen = snprintf(&bp[tlen], blen, "[%s:%d] ", filename, line);
        blen -= nlen; tlen += nlen;
        break;
      case  AZ_LOG_FLAGS_FUNC:
        nlen = snprintf(&bp[tlen], blen, "[%s] ", funcname);
        blen -= nlen; tlen += nlen;
        break;
      case  AZ_LOG_FLAGS_FILE:
        nlen = snprintf(&bp[tlen], blen, "[%s  ", filename);
        blen -= nlen; tlen += nlen;
        break;
      case  AZ_LOG_FLAGS_LINE:
        nlen = snprintf(&bp[tlen], blen, "[%d] ", line);
        blen -= nlen; tlen += nlen;
        break;
    }
#ifdef  CONFIG_AZ_USE_STDARG
    va_list ap;
    va_start(ap, fmt);
    nlen = vsnprintf(&bp[tlen], blen, fmt, ap);
    va_end(ap);
    blen -= nlen; tlen += nlen;
#endif

  /*
#ifdef CONFIG_WINDOWS
  _write(STDOUT_FILENO, bp, tlen);
#else
  write(STDOUT_FILENO, bp, tlen);
#endif
*/
    tlen = (log->out)(log, bp, tlen);
  } while (0);

  return tlen;
}
#endif


extern az_log_t az_log_default;
extern az_xu_t az_log_thread_default;

/* function prototypes exposed */
extern void az_log_ctrl_init();
extern void az_log_ctrl_deinit();
extern az_log_t  *az_log_create(az_log_t *log, az_log_config_t *cfg);
extern int  az_log_init(az_log_t *log, 
    uint8_t *buffer_area, az_ion_id_t thread_id, 
    az_cirbuffer_pushdata_t pushdata, az_cirbuffer_popdata_t popdata);
extern int  az_log_deinit(az_log_t *log);
extern int az_log_delete(az_log_t  *log);

extern az_r_t  az_log_start_default_thread();
extern az_r_t  az_log_stop_default_thread();

extern az_r_t az_log_port_addFdOutput(az_logid_t logid, az_sys_fd_t fd, az_log_port_t *pPort);
extern az_r_t az_log_port_delFdOutput(az_logid_t logid, az_log_port_t port); 
extern az_r_t az_log_port_addFileOutput(az_logid_t logid, const char *path, az_log_port_t *pPort); 
extern az_r_t az_log_port_delFileOutput(az_logid_t logid, az_log_port_t port); 
extern az_r_t az_log_port_addSocketOutput(az_logid_t logid, az_sys_socket_t sock, az_log_port_t *pPort); 
extern az_r_t az_log_port_delSocketOutput(az_logid_t logid, az_log_port_t port);

extern az_r_t  az_log_start();
extern az_r_t  az_log_stop();
#ifdef __cplusplus
}
#endif
#endif
