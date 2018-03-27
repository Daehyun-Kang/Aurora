/**
 * @file   az_cli_cmd.c
 * @brief  
 * @date   25/09/17
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
 *   - 2017-09-25 dhkang initial create
 */

/* include header files */
#include  "cli/az_cli.h"
#include  "cli/az_cli_cmd.h"

/* declare global variables */
az_cli_cmd_table_t  az_cli_cmd_table = {
  0
  //.list = {0},
  //.count = 0,
};

/* declare static variables */


/* implement static functions */

/**
 * @fn        function name
 * @brief     function-description
 * @param     input-output-parameters
 * @return    return-value
 * @warning   warnings
 * @exception none
 */


/* implement global functions */

/**
 * @fn        function name
 * @brief     function-description
 * @param     input-output-parameters
 * @return    return-value
 * @warning   warnings
 * @exception none
 */
int az_cli_cmd_reg(char *name, az_cli_cmd_exec_t exec, char *help, int mask)
{
  az_cli_cmd_table_t  *tbl = &az_cli_cmd_table;
  az_cli_cmd_descr_t  *descr = tbl->list;
  az_cli_cmd_descr_t  *empty_descr = NULL; 
  int j, k;
  int r = AZ_SUCCESS;

  for (j = 0, k = 0; j < CONFIG_AZ_CLI_MAX_CMDS; j++, descr++) {
    if (NULL == descr->name) {
      if (NULL == empty_descr) {
        empty_descr = descr;
      }
      break;
    }
    //az_sys_printf("%s %s %s\n", __FUNCTION__, descr->name, name); 
    if (!strcmp(descr->name, name)) {
      r = AZ_ERR(AGAIN);
      break;
    }
    k++;
    if (k == tbl->count) {
      if (k == CONFIG_AZ_CLI_MAX_CMDS) {
        break;
      }
      if (NULL == empty_descr) {
        empty_descr = descr++;
      }
      break;
    }
  }
  do {
    if (AZ_SUCCESS != r) break; 
    if (NULL == empty_descr) {
      r = AZ_ERR(FULL);
    }
    descr->name = name;
    descr->exec = exec;
    descr->help = help;
    descr->mask = mask;
    tbl->count++;
  } while (0);
  //az_sys_printf("%s %p cmd=%s count=%d\n", __FUNCTION__, descr, name, tbl->count);

  return r;
}
int az_cli_cmd_dereg(char *name)
{
  az_cli_cmd_table_t  *tbl = &az_cli_cmd_table;
  az_cli_cmd_descr_t  *descr = tbl->list;
  int j, k;
  int r = AZ_ERR(NOT_FOUND);

  for (j = 0, k = 0; j < CONFIG_AZ_CLI_MAX_CMDS; j++, descr++) {
    if (NULL == descr->name) {
      continue;
    }
    k++;
    if (k == tbl->count) {
      break;
    }
    if (!strcmp(descr->name, name)) {
      az_cli_cmd_descr_init(descr);
      tbl->count--;
      r = AZ_SUCCESS;
      break;
    }
  }

  return r;
}

int az_cli_cmd_parse(char *argbuf, char *argv[])
{
  int ntok = 0;
  char *p;

  for (p = argbuf; *p != '\0' && ntok < CONFIG_AZ_CLI_MAX_ARGS; p++) {
    while (*p == ' ' || *p == '\t') p++;
    if (*p == '\0' || *p == '\n' || *p == 0x0a || *p == '\r') break;
    argv[ntok] = p;
    ntok++;
    while (*p != '\0' && *p != ' ' && *p != '\t' &&
        *p != ':' && *p != '\n' && *p != '\r' && *p != 0x0a) p++;
    if (*p == '\0' || *p == '\n' || *p == 0x0a || *p == '\r') break;
    *p = 0;
  }
  *p = 0;

  return ntok;
}

int az_cli_cmd_proc(char *cmd_buffer,  az_cli_cmd_table_t  *tbl)
{
  az_cli_cmd_descr_t  *descr = tbl->list;
  int j, k;
  int r = AZ_CLI_CMD_NOTFOUND; 

  do {
    tbl->Argc = az_cli_cmd_parse(cmd_buffer, tbl->Argv);
    if (tbl->Argc ==  0) break;
    for (j = 0, k = 0; j < CONFIG_AZ_CLI_MAX_CMDS; j++, descr++) {
      if (NULL == descr->name) {
        continue;
      }
      if (!strcmp(descr->name, tbl->Argv[0])) {
        if (descr->exec) {
          r = (descr->exec)(tbl->Argc, tbl->Argv);
        }
        break;
      }
      k++;
      if (k == tbl->count) {
        break;
      }
    }
  } while (0);

  return r;
}

int az_cli_cmd_ls(int argc, char *argv[])
{
  az_cli_cmd_table_t  *tbl = &az_cli_cmd_table;
  az_cli_cmd_descr_t  *descr = tbl->list;
  int j, k;
  int r = AZ_CLI_CMD_SUCCESS; 

  do {
    for (j = 0, k = 0; j < CONFIG_AZ_CLI_MAX_CMDS; j++, descr++) {
      if (NULL == descr->name) {
        continue; 
      }
      az_cli_printf("%10s", descr->name);
      k++;
      if (k == tbl->count) {
        break;
      }
      if ((k % 8) == 0) {
        az_cli_write(AZ_NL, strlen(AZ_NL));
      }
    }
  } while (0);
  az_cli_write(AZ_NL, strlen(AZ_NL));

  return r;
}

int az_cli_cmd_prt(int argc, char *argv[])
{
  int r = AZ_CLI_CMD_SUCCESS;
  az_cli_shell_t *pSh =  az_cli_thread_curShell();
  int onoff;
  char *onoffstr[] = {"disabled", "enabled"};
  do {
    if (argc < 2) {
      az_cli_printf("cli %s print status\n", pSh->name);
      onoff = (pSh->state & AZ_CLI_SHELL_STATE_BCAST)? 1:0;
      az_cli_printf("print to remote %s\n", onoffstr[onoff]);
      onoff = (pSh->state & AZ_CLI_SHELL_STATE_RXON)? 1:0;
      az_cli_printf("print from remote %s\n", onoffstr[onoff]);
      onoff = (pSh->state & AZ_CLI_SHELL_STATE_RXSTDOUT)? 1:0;
      az_cli_printf("print from stdout %s\n", onoffstr[onoff]);
      break;
    }
    if (argc < 3) {
      r = AZ_CLI_CMD_SHOWHELP;
      break;
    }
    onoff = strtol(argv[2], NULL, 10);
    if (!strcmp(argv[1], "bcast")) {
      az_cli_shell_enable_bcast(pSh, onoff);
      onoff = (pSh->state & AZ_CLI_SHELL_STATE_BCAST)? 1:0;
      az_cli_printf("cli %s: print to remote %s\n", pSh->name, onoffstr[onoff]);
      break;
    }
    if (!strcmp(argv[1], "remote")) {
      az_cli_shell_enable_rxon(pSh, onoff);
      onoff = (pSh->state & AZ_CLI_SHELL_STATE_RXON)? 1:0;
      az_cli_printf("cli %s: print from remote %s\n", pSh->name, onoffstr[onoff]);
      break;
    }
    if (!strcmp(argv[1], "stdout")) {
      az_cli_shell_enable_rxstdout(pSh, onoff);
      onoff = (pSh->state & AZ_CLI_SHELL_STATE_RXSTDOUT)? 1:0;
      az_cli_printf("cli %s: print from stdout %s\n", pSh->name, onoffstr[onoff]);
      break;
    }
    r = AZ_CLI_CMD_SHOWHELP;
  } while (0);

  return r;
}

int az_cli_showhelp(char *cmd)
{
  az_cli_cmd_table_t  *tbl = &az_cli_cmd_table;
  az_cli_cmd_descr_t  *descr = tbl->list;
  int j, k;
  int r = AZ_CLI_CMD_SUCCESS; 

  do {
    for (j = 0, k = 0; j < CONFIG_AZ_CLI_MAX_CMDS; j++, descr++) {
      if (NULL == descr->name) {
        continue;
      }
      if (NULL == cmd) {
        az_cli_printf(" %s" AZ_NL, descr->help);
      } else {
        if (!strcmp(cmd, descr->name)) {
          az_cli_printf("Usage: %s" AZ_NL, descr->help);
          break;
        }
      }
      k++;
      if (k == tbl->count) {
        break;
      }
    }
  } while (0);

  return r;
}

int az_cli_cmd_help(int argc, char *argv[])
{
  az_cli_cmd_table_t  *tbl = &az_cli_cmd_table;
  az_cli_cmd_descr_t  *descr = tbl->list;
  int j, k;
  int r = AZ_CLI_CMD_SUCCESS; 

  if (argc < 2) {
    r = az_cli_showhelp(NULL);
  } else {
    r = az_cli_showhelp(argv[1]);
  }

  return r;
}

int az_cli_cmd_quit(int argc, char *argv[])
{
  az_cli_printf("%s", "exit...");

  return AZ_CLI_CMD_QUIT;
}

extern char *az_cli_cmd_memdisp_help;
extern int az_cli_cmd_memdisp(int argc, char *argv[]);

void az_cli_cmd_regBasicCmds()
{
  az_cli_cmd_reg("ls", az_cli_cmd_ls, "ls\t\t\t\t\t\t;list commands", 0);
  az_cli_cmd_reg("help", az_cli_cmd_help, "help [cmd]\t\t\t\t\t;show command help", 0);
  az_cli_cmd_reg("prt", az_cli_cmd_prt, "prt [bcast/remote/stdout] [0/1]\t\t;prt control", 0);
  az_cli_cmd_reg("quit", az_cli_cmd_quit, "quit\t\t\t\t\t\t;quit shell", 0);
  az_cli_cmd_reg("md", az_cli_cmd_memdisp, az_cli_cmd_memdisp_help, 0);
}

/* === end of AZ_CLI_CMD_C === */
