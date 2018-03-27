/**
 * @file   azm_rstdio.c
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
#include "mon/az_rstdio.h"
#include "cli/az_cli_api.h"
#include "net/az_tcpserver.h"
#include "auron.h"
#include "azm_mcheck.h"
#include "azm_rstdio.h"

typedef azm_trace_ctrl_t  azm_rstdio_ctrl_t;

/* declare global variables */
azm_rstdio_ctrl_t azm_rstdio_ctrl = {
  .state = AZ_TRACE_STATE_IDLE,
  .read_fd = AZ_SOCK_INVALID,
  .write_fd = AZ_SOCK_INVALID,
  .log_fd = AZ_SYS_FD_INVALID,
  .filepath = {'\0'},
};

int azm_rstdio_thread_state = 0;
az_xu_t azm_rstdio_thread_default = NULL;
/* declare static variables */
char  azm_rstdio_rxbuffer[2048];


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
int azm_rstdio_proc_default(void *arg)
{
  azm_rstdio_ctrl_t *ctrl = &azm_rstdio_ctrl; 
  int r;

  az_sys_printf("%s start...\n", __FUNCTION__);

  az_sys_xu_open_iomux();

  if (ctrl->read_fd != AZ_SOCK_INVALID) {
    ctrl->state |= AZ_TRACE_STATE_BUSY;
    azm_rstdio_thread_state = 1;
    az_sys_xu_iomux_add(ctrl->read_fd, AZ_SYS_IO_IN|AZ_SYS_IO_HUP);
    az_sys_xu_iomux_add(STDIN, AZ_SYS_IO_IN);
  }

  long count = 0;
  az_sys_io_event_t ioevt[2];
  while (azm_rstdio_thread_state) {
    r = az_sys_xu_wait_iomux(ioevt, 2, 3000);
    if (r > 0) {
      if (ioevt.events & AZ_SYS_IO_HUP) {
        break;
      }
      char  *bp = azm_rstdio_rxbuffer;
      int qlen = sizeof(azm_rstdio_rxbuffer);
      int nlen;
      if (ioevt.data.fd == ctrl->read_fd) { 
        nlen = read(ctrl->read_fd, bp, qlen); 
        if (nlen > 0) {
          write(STDOUT_FILENO, bp, nlen);
        }
      }
      if (ioevt.data.fd == STDIN) { 
        nlen = read(STDIN, bp, qlen); 
        if (nlen > 0) {
          write(ctrl->write_fd, bp, nlen);
        }
      }
    }
    if (r == 0) {
      //az_sys_eprintf("timeout: %ld\n", count++);
      continue;
    }
    if (r < 0) {
      az_sys_xu_iomux_del(ctrl->read_fd);
     az_sys_xu_iomux_del(STDIN);
      az_sys_socket_delete(ctrl->read_fd);
      ctrl->read_fd = AZ_SOCK_INVALID;
      ctrl->write_fd = AZ_SOCK_INVALID;
      break;
    }
  }

  if (ctrl->write_fd != AZ_SOCK_INVALID) {
    az_sys_xu_iomux_del(ctrl->read_fd);
    az_sys_xu_iomux_del(STDIN);
    az_sys_socket_delete(ctrl->write_fd);
    ctrl->read_fd = AZ_SOCK_INVALID;
    ctrl->write_fd = AZ_SOCK_INVALID;
  }
  //az_sys_xu_close_iomux();

  ctrl->state &= ~AZ_TRACE_STATE_BUSY;
  azm_rstdio_thread_state = 0;

  az_sys_printf("%s end...\n", __FUNCTION__);
  fflush(stdout);

  return r;
}

struct az_tcpserver_oprs azm_rstdio_svr_oprs;
az_tcpserver_t azm_rstdio_svr = {
  .state = 0,
  .config.name = "rstdioSvr", 
  .config.ipAddrStr = NULL, 
  .config.port = CONFIG_AZM_REMOTE_STDIO_SVR_PORT,
  .config.backlog = 16, 
  .config.flags = 0, 
  .sock = AZ_SOCK_INVALID, 
  .thread = NULL,
  .priv = NULL,
  .oprs = &azm_rstdio_svr_oprs, 
};

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
int azm_rstdio_svr_onClientConnection(void *ctx, az_sock_t cliSock, void *cliAddrIn)
{
  int r = AZ_SUCCESS;
  azm_rstdio_ctrl_t *ctrl = &azm_rstdio_ctrl;
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

    r = azm_rstdio_proc_default(ctrl);

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
int azm_rstdio_start()
{
  int r = AZ_SUCCESS;
  az_tcpserver_t *svr = (az_tcpserver_t *)&azm_rstdio_svr;
  
  azm_rstdio_svr_oprs = az_tcpserver_oprs_default;
  azm_rstdio_svr_oprs.onClientConnection = azm_rstdio_svr_onClientConnection;

  do {
    if (NULL == az_tcpserver_create(svr, NULL, &azm_rstdio_svr_oprs, NULL)) {
      az_printf("remote stdio server: %s create reault=%d:%s" AZ_NL,
          svr->config.name, r, az_err_str(r));
      r = AZ_FAIL;
      break;
    }
    r = (svr->oprs->start)(svr);
  } while (0);
  az_printf("remote stdio server: %s start reault=%d:%s" AZ_NL,
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
int azm_rstdio_stop()
{
  int r;
  az_tcpserver_t *svr = (az_tcpserver_t *)&azm_rstdio_svr;
  do {
    if (azm_rstdio_thread_state && svr->state & AZ_TCPSERVER_STATE_CLIBUSY) { 
      r = azm_rstdio_stop_default_thread();
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
int azm_rstdio_init()
{
  int r;

  r = azm_rstdio_start();
  return r;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
int azm_rstdio_deinit()
{
  int r;

  r = azm_rstdio_stop();
  return r;
}

/* === end of AZM_TRACE_C === */
