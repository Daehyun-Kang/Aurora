/**
 * @file   azm_trace.h
 * @brief  
 * @date   06/03/18
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
 *   - 2018-03-06 dhkang initial create
 */

#ifndef AZM_TRACE_H
#define AZM_TRACE_H

#ifdef __cplusplus
extern "C"
{
#endif

/* constants */

/* basic macros */

/* basic types */

/* structures */

/* structured types */
typedef struct azm_trace_ctrl {
  uint16_t state;
  uint16_t flags;
  az_sys_socket_t read_fd;
  az_sys_socket_t write_fd;
  az_sys_fd_t log_fd;
  char        prbSvrIpStr[CONFIG_AZ_NAME_MAX];
  uint16_t    prbSvrPort;
  uint16_t    monSvrPort;
  char        filepath[CONFIG_AZ_PATH_MAX];
} azm_trace_ctrl_t;

/* macros */
#define AZM_TRACE_DISP_XU_NAME        1
#define AZM_TRACE_DISP_XU             2
#define AZM_TRACE_DISP_TIME           4
#define AZM_TRACE_DISP_WHERE          8

#define AZM_TRACE_DISP_SHORT_FORMAT  (\
            AZM_TRACE_DISP_XU_NAME  |\
            AZM_TRACE_DISP_TIME     ) 
#define AZM_TRACE_DISP_LONG_FORMAT  (\
            AZM_TRACE_DISP_XU_NAME  |\
            AZM_TRACE_DISP_XU       |\
            AZM_TRACE_DISP_TIME     |\
            AZM_TRACE_DISP_WHERE    ) 

/* variabls exposed */

/* inline functions */

/* function prototypes exposed */
extern az_r_t  azm_trace_start_default_thread();
extern az_r_t  azm_trace_stop_default_thread();
extern int azm_trace_send_query_cmd(short query, char *data, int datalen);
extern int azm_trace_proc_query_rsp(short query, char *data, int datalen);
extern int azm_trace_init();
extern int azm_trace_deinit();

#ifdef __cplusplus
}
#endif
#endif
