/**
 * @file   azm_rsh.c
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

/* include header files */
#include "aurora.h"
#include "mon/az_trace.h"
#include "cli/az_cli.h"
#include "net/az_tcpserver.h"
#include "auron.h"
#include "azm_rsh.h"


#ifdef  __GNUC__
/* declare global variables */
azm_rsh_ctrl_t azm_rsh_ctrl = {
  .state = AZ_TRACE_STATE_IDLE,
  .read_fd = AZ_SOCK_INVALID,
  .write_fd = AZ_SOCK_INVALID,
  .log_fd = AZ_SYS_FD_INVALID,
  .rshSvrIpStr = "127.0.0.1",
  .rshSvrPort = CONFIG_AZM_TRACE_PRB_SVR_PORT, 
  .filepath = {'\0'},
};

int azm_rsh_thread_state = 0;
az_thread_t azm_rsh_thread_default = NULL;
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
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
int azm_rsh_proc_default(void *arg)
{
  azm_rsh_ctrl_t *ctrl = &azm_rsh_ctrl; 
  az_trace_info_t info;
  int r;
  int resolve_symbol = 0;
  az_size_t nlen, tlen; 
  int nfds = 0;
  char buf[1500];

  az_sys_printf("%s start...\n", __FUNCTION__);

  az_sys_xu_open_iomux();

  do {
    if (ctrl->state != AZ_TRACE_STATE_IDLE) {
      break;
    }
    az_sys_socket_t cliSock;
    r = az_inet_openTcpClient(ctrl->rshSvrIpStr, ctrl->rshSvrPort, 
        NULL, 0, &cliSock);
    if (r != AZ_SUCCESS) {
      az_cli_printf("connect fail to trace server %s:%u\n",
          ctrl->rshSvrIpStr, ctrl->rshSvrPort);
      azm_rsh_thread_state = 0;
    } else {
      azm_rsh_thread_state = 1;
      ctrl->read_fd = cliSock;
      ctrl->write_fd = cliSock;
      ctrl->state |= AZ_TRACE_STATE_INIT;
      ctrl->curSh->state |= AZ_CLI_SHELL_STATE_INFRWD;
    }
  } while (0); 

  if (ctrl->read_fd != AZ_SOCK_INVALID) {
    az_sys_xu_iomux_add(ctrl->read_fd, AZ_SYS_IO_IN|AZ_SYS_IO_HUP); 
    ctrl->state |= AZ_TRACE_STATE_BUSY;
    azm_rsh_thread_state = 1;
    ctrl->curSh->rxfrwd = ctrl->write_fd;
  }

  long count = 0;
  az_sys_io_event_t ioevt;
  while (azm_rsh_thread_state) {
    r = az_sys_xu_wait_iomux(&ioevt, 1, 3000);
    if (r > 0) {
      if (ioevt.data.fd != ctrl->read_fd) continue;
      if (ioevt.events & AZ_SYS_IO_HUP) {
        azm_rsh_thread_state = 0;
        break;
      }
        char *bp = &info;
        nlen = read(ctrl->read_fd, buf, sizeof(buf));  
        if (nlen < 0) break;
        write(STDOUT_FILENO, buf, nlen);
        continue;
    }
    if (r == 0) {
      az_sys_eprintf("timeout %d\n", count++);
      if (!(ctrl->curSh->state & AZ_CLI_SHELL_STATE_INFRWD)) {
        break;
      }
      continue;
    }
    break;
  }
  ctrl->curSh->state &= ~AZ_CLI_SHELL_STATE_INFRWD;
  ctrl->curSh->rxfrwd = AZ_SOCK_INVALID; 
  az_sys_xu_iomux_del(ctrl->read_fd);
  az_sys_socket_delete(ctrl->read_fd);
  ctrl->read_fd = AZ_SOCK_INVALID;
  ctrl->write_fd = AZ_SOCK_INVALID;

  az_sys_xu_close_iomux();

  ctrl->state = AZ_TRACE_STATE_IDLE;
  azm_rsh_thread_state = 0;

  az_sys_printf("%s end...\n", __FUNCTION__);
  fflush(stdout);

  return r;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
void *azm_rsh_thread_proc_default(void *arg)
{
  int r;
  
  r = azm_rsh_proc_default(&azm_rsh_ctrl); 

  return NULL;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
az_r_t  azm_rsh_start_default_thread()
{
  int r;

  if (azm_rsh_thread_state == 0) {
    azm_rsh_thread_default = NULL;
    r = az_thread_create("rshDefault", azm_rsh_thread_proc_default, NULL, NULL, &azm_rsh_thread_default);
  } else {
    r = AZ_ERR(AGAIN);
    az_cli_printf("already started!\n");
  }

  return r;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
az_r_t  azm_rsh_stop_default_thread()
{
  int r;
  azm_rsh_ctrl_t *ctrl = &azm_rsh_ctrl;

  if (azm_rsh_thread_state) {
    azm_rsh_thread_state = 0;
    while (ctrl->state & AZ_TRACE_STATE_BUSY) {
      az_thread_sleep(1000000);
    }
    ctrl->state = AZ_TRACE_STATE_IDLE;
  } else {
    r = AZ_ERR(AGAIN);
    az_cli_printf("already stopped!\n");
  }

  return r;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
int azm_rsh_cli_cmd(int argc, char *argv[])
{
  int r = AZ_CLI_CMD_SUCCESS;
  azm_rsh_ctrl_t *ctrl = &azm_rsh_ctrl;
  
  ctrl->curSh = az_cli_thread_curShell(); 
  do {
    if (argc < 2) {
      r = AZ_CLI_CMD_SHOWHELP;
      break;
    }
    if (argc == 2) {
      ctrl->rshSvrPort = strtol(argv[1], NULL, 0);
    }
    if (argc == 3) {
      strncpy(ctrl->rshSvrIpStr , argv[1], sizeof(ctrl->rshSvrIpStr));
      ctrl->rshSvrPort = strtol(argv[2], NULL, 0);
    }
    az_cli_printf("start rsh %s on %s:%u, %p" AZ_NL, ctrl->filepath, ctrl->rshSvrIpStr, ctrl->rshSvrPort, ctrl->curSh); 
    r = azm_rsh_start_default_thread();
    if (r < 0) {
      azm_rsh_stop_default_thread();
    }
  } while (0);

  return r;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
int azm_rsh_init()
{
  int r;
  az_cli_cmd_reg("rsh", azm_rsh_cli_cmd, "rsh [port/ipaddress port]\t\t\t; remote shell", 0);

  return r;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
int azm_rsh_deinit()
{
  int r;

  return r;
}
#endif // __GNUC__

/* === end of AZM_RSH_C === */
