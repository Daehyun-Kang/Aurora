/**
 * @file   az_cli_cmd.h
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

#ifndef AZ_CLI_CMD_H
#define AZ_CLI_CMD_H

#include "az_def.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* constants */
#ifndef AZ_NL
#define AZ_NL AZ_SYS_NL 
#endif

#ifndef	CONFIG_AZ_CLI_MAX_CMDS
#define	CONFIG_AZ_CLI_MAX_CMDS	128
#endif

#ifndef	CONFIG_AZ_CLI_CMDBUF_SZ
#define	CONFIG_AZ_CLI_CMDBUF_SZ	128
#endif

#ifndef	CONFIG_AZ_CLI_MAX_ARGS	
#define	CONFIG_AZ_CLI_MAX_ARGS	32
#endif

#define	AZ_CLI_CMD_SUCCESS	0
#define	AZ_CLI_CMD_NOTFOUND	-1	
#define	AZ_CLI_CMD_SHOWHELP	-2	
#define	AZ_CLI_CMD_QUIT		-3	
#define	AZ_CLI_CMD_ERROR		-4	

#define AZ_CLI_CMD_MASK_DEBUG   0x0001 
#define AZ_CLI_CMD_MASK_LOCAL   0x0008 
#define AZ_CLI_CMD_MASK_FRW     0x0002 
#define AZ_CLI_CMD_MASK_APP     0x0004 

/* basic macros */

/* basic types */
typedef int32_t (*az_cli_cmd_exec_t)(int, char *[]);
typedef struct az_cli_cmd_descr {
  const char *name;
  az_cli_cmd_exec_t exec;
  const char  *help;
  int mask;
} az_cli_cmd_descr_t;

/* structures */
typedef struct az_cli_cmd_table {
  az_cli_cmd_descr_t list[CONFIG_AZ_CLI_MAX_CMDS];
  int  count;
  int   Argc;
  char  *Argv[CONFIG_AZ_CLI_MAX_ARGS];
} az_cli_cmd_table_t;

/* structured types */

/* macros */

/* variabls exposed */
extern az_cli_cmd_table_t az_cli_cmd_table;

/* inline functions */
static inline void az_cli_cmd_descr_init(az_cli_cmd_descr_t *pDescr)
{
  pDescr->name = NULL;
  pDescr->exec = NULL;
  pDescr->help = NULL;
  pDescr->mask = 0;
}

/* function prototypes exposed */
#include "az_cli_api.h"
/*
extern int az_cli_cmd_reg(char *, az_cli_cmd_exec_t exec, char *, int); 
extern int az_cli_cmd_dereg(char *);
extern int az_cli_cmd_proc(char *, az_cli_cmd_table_t *);

extern void az_cli_cmd_regBasicCmds();
*/
#ifdef __cplusplus
}
#endif
#endif
