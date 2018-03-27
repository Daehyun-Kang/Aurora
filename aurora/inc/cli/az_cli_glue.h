/**
 * @file   az_cli_glue.h
 * @brief  
 * @date   24/09/17
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
 *   - 2017-09-24 dhkang initial create
 */

#ifndef AZ_CLI_GLUE_H
#define AZ_CLI_GLUE_H

#include "sys/az_xu.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* constants */
#ifdef  CONFIG_AZ_CLI_SHELL_MAX
#define AZ_CLI_SHELL_MAX  CONFIG_AZ_CLI_SHELL_MAX
#else
#define AZ_CLI_SHELL_MAX  8
#endif

/* basic macros */

/* basic types */

/* structures */

/* structured types */

/* macros */

/* variabls exposed */
extern int az_cli_shell_nr;
extern az_cli_shell_t  *az_cli_shell_list[];

/* inline functions */
static inline az_cli_shell_t *az_cli_thread_curShell()
{
  return (az_cli_shell_t *)az_xu_getarg();
}

static inline az_cli_shell_t  *az_cli_get_default()
{
  return &az_cli_shell_default;
}

/* function prototypes exposed */
extern int az_cli_shell_add(az_cli_shell_t *pSh);
extern int az_cli_shell_del(az_cli_shell_t *pSh);
extern int az_cli_shell_bcast(az_cli_shell_t *pSh, char *dp, int len);
extern int az_cli_shell_bcast_stdout(char *dp, int len);
extern int az_cli_shell_enable_bcast(az_cli_shell_t *pSh, int onoff);
extern int az_cli_shell_enable_rxon(az_cli_shell_t *pSh, int onoff);
extern int az_cli_shell_enable_rxstdout(az_cli_shell_t *pSh, int onoff);
extern int az_cli_thread_start(void *(*run_f)(az_cli_shell_t *), az_cli_shell_t *);
extern int az_cli_thread_stop(void *);

extern az_sys_fd_t az_cli_shell_change_txport(az_cli_shell_t *pSh, az_sys_fd_t fd, int logOn);

extern int az_cli_printf(const char *fmt, ...);
extern int az_cli_write(char *dp, int len);

#ifdef __cplusplus
}
#endif
#endif
