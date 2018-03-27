/**
 * @file   az_cli.c
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

/* include header files */
#include  "cli/az_cli.h"
#include  "az_inet.h"

/* declare global variables */
az_cli_shell_t  az_cli_shell_default =
{
  .name = {'c','l','i','S','h','0', '\0'},
  .state = AZ_CLI_SHELL_STATE_INIT,
  .cmd_table = &az_cli_cmd_table,
  .txport = STDOUT_FILENO,
  .rxport = STDIN_FILENO,
  .logport = AZ_SYS_FD_INVALID,
  .Read = (az_cli_read_t)read,
  .Write = (az_cli_write_t)write,
  .prompt = AZ_CLI_SHELL_PROMPT_DFT,
  .Cleanup = NULL,
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
int az_cli_shell_init(az_cli_shell_t *pSh, 
    az_cli_port_t rxport, az_cli_read_t Read,
    az_cli_port_t txport, az_cli_write_t  Write,
    az_cli_cleanup_t Cleanup)
{
  az_assert(NULL != pSh);
  //int j;
  int r = AZ_SUCCESS;

  if (pSh->state & AZ_CLI_SHELL_STATE_INIT) {
    r = AZ_ERR(AGAIN);
  } else {
    pSh->state |= AZ_CLI_SHELL_STATE_INIT;

    pSh->cmd_table = &az_cli_cmd_table;
    pSh->rxport = rxport;
    pSh->Read = Read;
    pSh->txport = txport;
    pSh->Write = Write;

    pSh->prompt = AZ_CLI_SHELL_PROMPT_DFT;

    pSh->Cleanup = Cleanup;
  }

  return r;
}

int az_cli_shell_start(az_cli_shell_t *pSh)
{
  int result ;
  az_assert(NULL != pSh);

  az_cli_cmd_regBasicCmds();
  if (pSh->state & AZ_CLI_SHELL_STATE_START) {
    result = AZ_ERR(AGAIN);
  } else {
    pSh->state |= AZ_CLI_SHELL_STATE_START;
    pSh->state |= (AZ_CLI_SHELL_STATE_START|AZ_CLI_SHELL_STATE_START_DFT) ;  
    result = az_cli_thread_start(az_cli_shell_run, pSh);
    if (result < 0) {
      az_sys_rprintf(result, "%s", "cli thread start" AZ_NL);
      pSh->state &= ~AZ_CLI_SHELL_STATE_START;  
    }
  }

  return result;
}


int az_cli_shell_stop(az_cli_shell_t *pSh)
{
  int result = AZ_SUCCESS;
  az_assert(NULL != pSh);
  if (pSh->state > 0) {
    pSh->state |= AZ_CLI_SHELL_STATE_STOP;  
    result = AZ_ERR(AGAIN);
  }
  return result;
}


void *az_cli_shell_run(az_cli_shell_t *pSh)
{
  az_assert(NULL != pSh);
  az_assert(NULL != pSh->Read);
  int r = 0;
  char  ch = 0;
  int promptlen = strlen(pSh->prompt);

#ifdef  CONFIG_AZ_TRACE
  az_xu_set_trace(az_xu_traceinfo(), 0);
#endif

  az_cli_shell_add(pSh);

  pSh->cmdpos = 0 ;
  do {
    if (pSh->state & AZ_CLI_SHELL_STATE_STOP) {
      break;
    }
    #ifdef  CONFIG_AZ_CLI_INPUT_FRWD
    if (pSh->state & AZ_CLI_SHELL_STATE_INFRWD) {
      char line[128];
      r = (pSh->Read)(pSh->rxport, line, sizeof(line)-1); 
      if (0 == r) continue;
      if (r < 0) {
        az_sys_rprintf(r, "%s", "connection" AZ_NL);
        break;
      }
      if (pSh->rxfrwd == AZ_SOCK_INVALID) {
          pSh->state &= ~AZ_CLI_SHELL_STATE_INFRWD;
      } else {
        r = az_inet_tcpWrite(pSh->rxfrwd, line, r);
        if (r < 0 || line[0] == 'q') {
          pSh->state &= ~AZ_CLI_SHELL_STATE_INFRWD;
        }
      }
      continue;
    }
    #endif
    if (pSh->cmdpos == 0 && ch == '\n') {
      pSh->Write(pSh->txport, pSh->prompt, promptlen); 
      if (pSh->state & AZ_CLI_SHELL_STATE_BCAST) {
        az_cli_shell_bcast(pSh,pSh->prompt, promptlen);
      } 
    }
    r = (pSh->Read)(pSh->rxport, &ch, 1); 
    if (0 == r) {
      continue;
    }
    if (r < 0) {
      az_sys_rprintf(r, "%s", "connection" AZ_NL);
      break;
    }
    if (pSh->state & AZ_CLI_SHELL_STATE_LOG) {
      pSh->Write(pSh->logport, &ch, 1); 
    }
    //if (r > 0) {
    //  if (pSh->state & AZ_CLI_SHELL_STATE_BCAST) {
    //    az_cli_shell_bcast(pSh, &ch, 1);
    //  } 
    //}
    if (ch == '\n' || ch == '\r') {
      pSh->cmdbuffer[pSh->cmdpos] = 0;
      if (pSh->cmdpos == 0) {
        if (pSh->state & AZ_CLI_SHELL_STATE_BCAST) {
          az_cli_shell_bcast(pSh,AZ_NL, strlen(AZ_NL)); 
        }
        continue;
      }
      if (pSh->state & AZ_CLI_SHELL_STATE_BCAST) {
        if (pSh->cmdpos > 0) {
          az_cli_shell_bcast(pSh,pSh->cmdbuffer, pSh->cmdpos); 
        }
        az_cli_shell_bcast(pSh,AZ_NL, strlen(AZ_NL)); 
      } 
      r = az_cli_cmd_proc(pSh->cmdbuffer, pSh->cmd_table);
      if (r == AZ_CLI_CMD_QUIT) {
        if (pSh->rxport != STDIN_FILENO) {
          break;
        }
      } else 
      if (r == AZ_CLI_CMD_NOTFOUND) {
        char *msg = "Command not found!\n";
        pSh->Write(pSh->txport, msg, strlen(msg));
        if (pSh->state & AZ_CLI_SHELL_STATE_BCAST) {
          az_cli_shell_bcast(pSh, msg, strlen(msg)); 
        } 
      } else
      if (r == AZ_CLI_CMD_SHOWHELP) {
        extern int az_cli_showhelp(char *cmd);
        az_cli_showhelp(pSh->cmd_table->Argv[0]);
      }
      pSh->cmdpos = 0;
      continue;
    }
    if (ch == '\b') {
      if (pSh->cmdpos > 0) {
        pSh->cmdpos--;
      }
    } else {
      pSh->cmdbuffer[pSh->cmdpos++] = ch;
    }

  } while (1);

  az_cli_shell_del(pSh);
  
  az_cli_shell_terminated(pSh);
  return NULL;
}

int az_cli_shell_terminated(az_cli_shell_t *pSh)
{
  int r = AZ_SUCCESS;
  //TODO:report cli shel termination

  if (pSh->Cleanup) {
    (pSh->Cleanup)(pSh);
  }
  return r;
}
#if 0
#endif

/* === end of AZ_CLI_C === */
