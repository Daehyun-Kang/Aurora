/**
 * @file   az_trace.h
 * @brief  
 * @date   04/03/18
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
 *   - 2018-03-04 dhkang initial create
 */

#ifndef AZ_TRACE_H
#define AZ_TRACE_H

#include "az_thread.h"
#include "mon/az_mcheck.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* constants */

/* basic macros */
#define AZ_TRACE_SOCK_SVR_NAME  "/tmp/az_trace_svr"
#define AZ_TRACE_SOCK_CLI_NAME  "/tmp/az_trace_cli"

#define AZ_TRACE_STATE_IDLE     0
#define AZ_TRACE_STATE_INIT     1
#define AZ_TRACE_STATE_BUSY     2
#define AZ_TRACE_STATE_ACTV     (AZ_TRACE_STATE_INIT|AZ_TRACE_STATE_BUSY) 
#define AZ_TRACE_STATE_SNDF     4

#define AZ_TRACE_CODE_CMD_START   1 
#define AZ_TRACE_CODE_CMD_STOP    2 
#define AZ_TRACE_CODE_CMD_ENABLE  3 
#define AZ_TRACE_CODE_CMD_QUERY   5 
#define AZ_TRACE_CODE_RSP_QUERY   6 
#define AZ_TRACE_CODE_FILENAME    7 
#define AZ_TRACE_CODE_RPT_MALLOC  8 
#define AZ_TRACE_CODE_RPT_FREE    9 

#define AZ_TRACE_CODE_FUNC_ENTER  'E' 
#define AZ_TRACE_CODE_FUNC_EXIT   'X' 

#define AZ_TRACE_QUERY_CLIPORT    1 
#define AZ_TRACE_QUERY_UNKNOWN    0 

/* basic types */

/* structures */

/* structured types */
typedef struct az_trace_ctrl {
  int state;
  az_sys_fd_t read_fd;
  az_sys_fd_t write_fd;
  az_sys_fd_t log_fd;

  char      monSvrIpStr[CONFIG_AZ_NAME_MAX];
  uint16_t  monSvrPort;

  az_socket_entity_t  _sock;
} az_trace_ctrl_t;

typedef AZ_PACKED_STRUCT(az_trace_info) {
  short code;
  unsigned short length;
  union {
    struct {
      short depth;
      struct timespec ts;
      #ifdef  CONFIG_AZ_TRACE_PTHREAD
      pthread_t xu; 
      #else
      az_thread_t xu; 
      #endif
      void    *func;
      void    *caller;
      char    xu_name[CONFIG_AZ_NAME_MAX];
    };
    struct {
      short query;
      char  data[CONFIG_AZ_NAME_MAX*2];
    };
    /*
    struct {
      short mchk_count;
      unsigned long mchk_nTotal;
      az_ref_t mchk_info[config_az_name_max*2/sizeof(az_ref_t)];
    }
    */
  }
} az_trace_info_t;
typedef AZ_PACKED_STRUCT(az_trace_fileinfo) {
  short code;
  unsigned short length;
  char filename[AZ_PATH_MAX];
} az_trace_fileinfo_t;

#define AZ_TRACE_INFO_LEN_DFT   sizeof(az_trace_info_t)
typedef union {
  az_trace_info_t trace;
  az_mchk_info_t  mchk;
} az_diag_info_t;

/* macros */

#define AZ_TRACE_IS_ACTV() ((az_trace_ctrl.state & AZ_TRACE_STATE_ACTV) == AZ_TRACE_STATE_ACTV)
/* variabls exposed */
extern az_trace_ctrl_t az_trace_ctrl;

/* inline functions */
#ifdef CONFIG_AZ_TRACE_PTHREAD
static inline int az_attr_no_instrument az_trace_enable_on_pthread(int onoff, char *name)
{
  int r = 0;
  do {
    if (az_pthread_trace_info == NULL) {
      az_pthread_trace_info = az_malloc(sizeof(az_thread_trace_info_t));
      if (az_pthread_trace_info == NULL) {
        r = AZ_ERR(MALLOC);
        break;
      }
      az_pthread_trace_info->thread = pthread_self();
    }
    if (NULL != name) {
      pthread_setname_np(az_pthread_trace_info->thread, name);
    }
    if (onoff) {
      AZ_THREAD_TRACE_ENABLE(az_pthread_trace_info);
    } else {
      AZ_THREAD_TRACE_DISABLE(az_pthread_trace_info);
    }

  } while (0);

  return r;
}
#endif

/* function prototypes exposed */
extern int az_trace_start();
extern int az_trace_stop();

extern az_r_t az_trace_start_default_thread();
extern az_r_t az_trace_stop_default_thread();
#ifdef __cplusplus
}
#endif
#endif
