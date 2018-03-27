/**
 * @file   az_cli_api.h
 * @brief  
 * @date   28/10/17
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
 *   - 2017-10-28 dhkang initial create
 */

#ifndef AZ_CLI_API_H
#define AZ_CLI_API_H
#include "cli/az_cli_cmd.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* constants */

/* basic macros */

/* basic types */
typedef int32_t (*az_cli_cmd_exec_t)(int, char *[]);
struct az_cli_cmd_table; 

/* structures */
struct az_cli_key_cmd {
  char  *key;
  int32_t (*cmd)(int, char *[]);
};

/* structured types */
typedef struct az_cli_cmd_table az_cli_cmd_table_t;

/* macros */

/* variabls exposed */

/* inline functions */

/* function prototypes exposed */
extern int az_cli_cmd_reg(char *, az_cli_cmd_exec_t exec, char *, int); 
extern int az_cli_cmd_dereg(char *);
extern int az_cli_cmd_proc(char *, az_cli_cmd_table_t *);
extern void az_cli_cmd_regBasicCmds();
extern int az_cli_printf(const char *fmt, ...);
extern uint16_t az_cli_getSvrPortNo();

#ifdef __cplusplus
}
#endif
#endif
