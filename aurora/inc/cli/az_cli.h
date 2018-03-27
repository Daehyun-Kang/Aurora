/**
 * @file   az_cli.h
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

#ifndef AZ_CLI_H
#define AZ_CLI_H

#include "az_def.h"
#include "cli/az_cli_cmd.h"

#ifdef __cplusplus
extern "C"
{
#endif


/* constants */ 
#ifdef  CONFIG_AZ_CLI_SVR_PORT
#define AZ_CLI_SVR_PORT     CONFIG_AZ_CLI_SVR_PORT 
#else
#define AZ_CLI_SVR_PORT     50000 
#endif

#ifndef CONFIG_AZ_CLI_TX_BUFFER_SZ  
#define CONFIG_AZ_CLI_TX_BUFFER_SZ  2048
#endif

#ifdef  CONFIG_AZ_CLI_SHELL_PROMPT
#define AZ_CLI_SHELL_PROMPT_DFT       CONFIG_AZ_CLI_SHELL_PROMPT
#else
#define AZ_CLI_SHELL_PROMPT_DFT       ">>"
#endif
#define AZ_CLI_SHELL_STATE_INIT       0x0001
#define AZ_CLI_SHELL_STATE_START      0x0002
#define AZ_CLI_SHELL_STATE_STOP       0x0008
#define AZ_CLI_SHELL_STATE_BCAST      0x0010
#define AZ_CLI_SHELL_STATE_RXON       0x0020
#define AZ_CLI_SHELL_STATE_RXSTDOUT   0x0040
#define AZ_CLI_SHELL_STATE_LOG        0x0100
#ifdef  CONFIG_AZ_CLI_INPUT_FRWD
  #define AZ_CLI_SHELL_STATE_INFRWD     0x0200
#endif
#define AZ_CLI_SHELL_STATE_DMALLOC    0x8000

#define AZ_CLI_SHELL_STATE_START_DFT (AZ_CLI_SHELL_STATE_BCAST|AZ_CLI_SHELL_STATE_RXON|AZ_CLI_SHELL_STATE_RXSTDOUT)
/* basic macros */

/* basic types */
typedef int az_cli_port_t; 
struct az_cli_shell;

typedef int (*az_cli_read_t)(az_cli_port_t, char *, int);
typedef int (*az_cli_write_t)(az_cli_port_t, const char *, int);
typedef void (*az_cli_cleanup_t)(struct az_cli_shell *);

/* structures */
typedef struct az_cli_shell {
  char  name[CONFIG_AZ_NAME_MAX];
  int state;
  az_cli_cmd_table_t *cmd_table;
  char  cmdbuffer[CONFIG_AZ_CLI_CMDBUF_SZ];	
  int   cmdpos;

  az_cli_port_t   txport, rxport;
  az_cli_read_t   Read;
  az_cli_write_t  Write;
  
  #ifdef  CONFIG_AZ_CLI_INPUT_FRWD
  az_cli_port_t   rxfrwd;
  #endif

  az_sys_fd_t     logport;

  const char  *prompt;

  char  buffer[CONFIG_AZ_CLI_TX_BUFFER_SZ];

  void  *thread;

  az_cli_cleanup_t  Cleanup;
} az_cli_shell_t;


/* structured types */

/* macros */

/* variabls exposed */
extern az_cli_shell_t az_cli_shell_default;

#include "az_cli_glue.h"

/* inline functions */

/* function prototypes exposed */
extern int az_cli_shell_init(az_cli_shell_t *pSh, 
    az_cli_port_t rxport, az_cli_read_t Read,
    az_cli_port_t txport, az_cli_write_t  Write,
    az_cli_cleanup_t Cleanup);
extern int az_cli_shell_start(az_cli_shell_t *pSh);
extern int az_cli_shell_stop(az_cli_shell_t *pSh);
extern void *az_cli_shell_run(az_cli_shell_t *);
extern int az_cli_shell_terminated(az_cli_shell_t *pSh);

extern int az_cli_svr_start();
extern int az_cli_svr_stop();
#ifdef __cplusplus
}
#endif
#endif
