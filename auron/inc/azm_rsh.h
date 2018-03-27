/**
 * @file   azm_rsh.h
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

#ifndef AZM_RSH_H
#define AZM_RSH_H

#ifdef __cplusplus
extern "C"
{
#endif

/* constants */

/* basic macros */

/* basic types */

/* structures */

/* structured types */
typedef struct azm_rsh_ctrl {
  uint16_t state;
  uint16_t flags;
  az_sys_socket_t read_fd;
  az_sys_socket_t write_fd;
  az_sys_fd_t log_fd;
  char        rshSvrIpStr[CONFIG_AZ_NAME_MAX];
  uint16_t    rshSvrPort;
  char        filepath[CONFIG_AZ_PATH_MAX];
  az_cli_shell_t  *curSh;
} azm_rsh_ctrl_t;

/* macros */
/* variabls exposed */

/* inline functions */

/* function prototypes exposed */
extern az_r_t  azm_rsh_start_default_thread();
extern az_r_t  azm_rsh_stop_default_thread();
extern int azm_rsh_init();
extern int azm_rsh_deinit();

#ifdef __cplusplus
}
#endif
#endif
