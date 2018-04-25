/**
 * @file   azs_rstdio.c
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
#include "cli/az_cli_api.h"
#include "net/az_tcpserver.h"
#include "auros.h"
#include "azs_rstdio.h"

/* declare global variables */
azs_rstdio_ctrl_t azs_rstdio_ctrl = {
  .state = AZ_TRACE_STATE_IDLE,
  .read_fd = AZ_SOCK_INVALID,
  .write_fd = AZ_SOCK_INVALID,
  .log_fd = AZ_SYS_FD_INVALID,
  .filepath = {'\0'},
};

int azs_rstdio_thread_state = 0;
az_thread_t azs_rstdio_thread_default = NULL;
/* declare static variables */
char  azs_rstdio_rxbuffer[2048];


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
int azs_rstdio_proc_default(void *arg)
{
  azs_rstdio_ctrl_t *ctrl = &azs_rstdio_ctrl; 
  int r;

  az_sys_printf("%s start...\n", __FUNCTION__);

  az_sys_xu_open_iomux();

  if (ctrl->read_fd != AZ_SOCK_INVALID) {
    ctrl->state |= AZ_TRACE_STATE_BUSY;
    azs_rstdio_thread_state = 1;
    az_sys_xu_iomux_add(ctrl->read_fd, AZ_SYS_IO_IN|AZ_SYS_IO_HUP);
    az_sys_xu_iomux_add(STDIN_FILENO, AZ_SYS_IO_IN);
  }

  long count = 0;
  az_sys_io_event_t ioevt[2];
  while (azs_rstdio_thread_state) {
    r = az_sys_xu_wait_iomux(ioevt, 2, 3000);
    if (r > 0) {
      int j;
      for (j = 0; j < r; j++) {
        char  *bp = azs_rstdio_rxbuffer;
        int qlen = sizeof(azs_rstdio_rxbuffer);
        int nlen;
        if (ioevt[j].events & AZ_SYS_IO_HUP) {
          azs_rstdio_thread_state = 0;
          break;
        }
        if (ioevt[j].data.fd == ctrl->read_fd) { 
          nlen = read(ctrl->read_fd, bp, qlen); 
          if (nlen > 0) {
          write(STDOUT_FILENO, bp, nlen);
          }
          continue;
        }
        if (ioevt[j].data.fd == STDIN_FILENO) { 
          nlen = read(STDIN_FILENO, bp, qlen); 
          if (nlen > 0) {
            write(ctrl->write_fd, bp, nlen);
          }
        }
      }
    }
    if (r == 0) {
      //az_sys_eprintf("timeout: %ld\n", count++);
      continue;
    }
    if (r < 0) {
      az_sys_xu_iomux_del(ctrl->read_fd);
     az_sys_xu_iomux_del(STDIN_FILENO);
      az_socket_delete(ctrl->read_fd);
      ctrl->read_fd = AZ_SOCK_INVALID;
      ctrl->write_fd = AZ_SOCK_INVALID;
      break;
    }
  }

  if (ctrl->write_fd != AZ_SOCK_INVALID) {
    az_sys_xu_iomux_del(ctrl->read_fd);
    az_sys_xu_iomux_del(STDIN_FILENO);
    az_socket_delete(ctrl->write_fd);
    ctrl->read_fd = AZ_SOCK_INVALID;
    ctrl->write_fd = AZ_SOCK_INVALID;
  }
  //az_sys_xu_close_iomux();

  ctrl->state &= ~AZ_TRACE_STATE_BUSY;
  azs_rstdio_thread_state = 0;

  az_sys_printf("%s end...\n", __FUNCTION__);
  fflush(stdout);

  return r;
}

struct az_tcpserver_oprs azs_rstdio_svr_oprs;
az_tcpserver_t azs_rstdio_svr = {
  .state = 0,
  .config.name = "rstdioSvr", 
  .config.ipAddrStr = NULL, 
  .config.port = CONFIG_AZ_REMOTE_STDIO_SVR_PORT,
  .config.backlog = 16, 
  .config.flags = 0, 
  .sock = NULL, 
  .thread = NULL,
  .priv = NULL,
  .oprs = &azs_rstdio_svr_oprs, 
};

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
int azs_rstdio_svr_onClientConnection(void *ctx, az_socket_id_t cliSock, void *cliAddrIn)
{
  int r = AZ_SUCCESS;
  azs_rstdio_ctrl_t *ctrl = &azs_rstdio_ctrl;
  struct sockaddr_in *cliAddr = (struct sockaddr_in *)cliAddrIn;
  char cliIpStr[32] = {0};
  az_tcpserver_t *svr = (az_tcpserver_t *)ctx;

  svr->state |= AZ_TCPSERVER_STATE_CLIBUSY;

  az_inet_IpAddr2Str(cliAddr, cliIpStr, sizeof(cliIpStr));
  az_sys_eprintf("new remote stdio connection from %s:%u" AZ_NL,
        cliIpStr, AZ_INET_PORT(cliAddr));

  do {
    if (ctrl->state != AZ_TRACE_STATE_IDLE) {
      az_sys_eprintf("rstdio busy state=%x..., reject connection" AZ_NL,
          ctrl->state);
      r = AZ_ERR(AGAIN);
    }
    ctrl->state |= AZ_TRACE_STATE_INIT;
    ctrl->read_fd = cliSock;
    ctrl->write_fd = cliSock;

    r = azs_rstdio_proc_default(ctrl);

    ctrl->state &= ~AZ_TRACE_STATE_INIT;
  } while (0);

  svr->state &= ~AZ_TCPSERVER_STATE_CLIBUSY;
  return r;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
int azs_rstdio_start()
{
  int r = AZ_SUCCESS;
  az_tcpserver_t *svr = (az_tcpserver_t *)&azs_rstdio_svr;
  
  azs_rstdio_svr_oprs = az_tcpserver_oprs_default;
  azs_rstdio_svr_oprs.onClientConnection = azs_rstdio_svr_onClientConnection;

  do {
    if (NULL == az_tcpserver_create(svr, NULL, &azs_rstdio_svr_oprs, NULL)) {
      az_printf("remote stdio server: %s create reault=%d:%s" AZ_NL,
          svr->config.name, r, az_err_str(r));
      r = AZ_FAIL;
      break;
    }
    r = (svr->oprs->start)(svr);
  } while (0);
  az_printf("remote stdio server: %s on port %u start reault=%d:%s" AZ_NL,
          svr->config.name, svr->config.port, r, az_err_str(r));

  return r;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
int azs_rstdio_stop()
{
  int r;
  az_tcpserver_t *svr = (az_tcpserver_t *)&azs_rstdio_svr;
  do {
    if (azs_rstdio_thread_state && svr->state & AZ_TCPSERVER_STATE_CLIBUSY) { 
      azs_rstdio_thread_state = 0;
      az_thread_sleep(3000000000);
    }
    r = (svr->oprs->stop)(svr);
    if (r) break;
    r = az_tcpserver_delete(svr);
  } while (0);
  az_printf("remote stdio server: %s stop reault=%d:%s" AZ_NL,
          svr->config.name, r, az_err_str(r));

  return r;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
int azs_rstdio_init()
{
  int r;

  r = azs_rstdio_start();
  return r;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
int azs_rstdio_deinit()
{
  int r;

  r = azs_rstdio_stop();
  return r;
}

/* === end of AZS_RSTDIO_C === */
