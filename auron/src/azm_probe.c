/**
 * @file   azm_probe.c
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
#include "mon/az_probe.h"
#include "cli/az_cli_api.h"
#include "net/az_tcpserver.h"
#include "auron.h"


#ifdef  __GNUC__
/* declare global variables */
typedef az_probe_ctrl_t azm_probe_ctrl_t;

az_probe_ctrl_t azm_probe_ctrl = {
  .state = AZ_PROBE_STATE_IDLE,
  .ctrl_sock = AZ_SOCK_INVALID,
  .data_sock = AZ_SOCK_INVALID,
};

int azm_probe_thread_state = 0;
az_thread_t azm_probe_thread_default = NULL;

extern az_r_t  azm_probe_stop_default_thread();
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

static az_probe_sample_t buf[AZ_PROBE_SAMPLE_SEND_MAX];
int azm_probe_proc_recv(int fd)
{
  azm_probe_ctrl_t *ctrl = &azm_probe_ctrl; 
  az_probe_sample_t *sample = buf;
  int rcount;
  int j;
  int src, level;
  uint64_t tstamp;

  do {
    rcount = recv(ctrl->data_sock, buf, sizeof(buf), 0);
    az_assert((rcount % sizeof(az_probe_sample_t)) == 0);
    if (rcount < 0) {
      return AZ_ERR(FAIL);
      break;
    }
    if (rcount == 0) break;

    if (fd > 0) write(fd, buf, rcount);

    rcount /= sizeof(az_probe_sample_t);
    sample = buf;
    for (j = 0; j < rcount; j++, sample++) {
      src = AZ_PROBE_SRC(*sample);
      level = AZ_PROBE_LEVEL(*sample);
      tstamp = AZ_PROBE_TSTAMP(*sample);
      
      az_printf("%lx: src:%d level:%d  tstamp:%ld\n", *sample, src, level, tstamp);
    }
  } while (0);

  return rcount;
}
/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
int azm_probe_proc_default(void *arg)
{
  azm_probe_ctrl_t *ctrl = &azm_probe_ctrl; 
  int r;

  az_sys_printf("%s start...\n", __FUNCTION__);

  az_sys_xu_open_iomux();

  int fd = open("/tmp/yyy", O_RDWR|O_CREAT|O_CLOEXEC, 0666);  
  
  if (ctrl->ctrl_sock != AZ_SOCK_INVALID) {
    azm_probe_thread_state = 1;
    az_sys_xu_iomux_add(ctrl->ctrl_sock, AZ_SYS_IO_IN|AZ_SYS_IO_HUP|AZ_SYS_IO_RDHUP);
  }
  if (ctrl->data_sock != AZ_SOCK_INVALID) {
    az_sys_xu_iomux_add(ctrl->data_sock, AZ_SYS_IO_IN|AZ_SYS_IO_HUP|AZ_SYS_IO_RDHUP);
  }

  ctrl->state |= AZ_PROBE_STATE_ENAB;

  long count = 0;
  az_sys_io_event_t ioevt;
  while (azm_probe_thread_state) {
    r = az_sys_xu_wait_iomux(&ioevt, 1, 3000);
    if (r > 0) {
      if (ioevt.events & (AZ_SYS_IO_HUP|AZ_SYS_IO_RDHUP)) {
          az_sys_eprintf("disconnected %x\n", ioevt.events);
          break;
      }
      if (ioevt.events & AZ_SYS_IO_IN) {
        r =  azm_probe_proc_recv(fd);
      } else {
        az_sys_eprintf("events %x\n", ioevt.events);
        break;
      }

    }
    if (r == 0) {
      //az_sys_eprintf("timeout: %ld\n", count++);
      continue;
    }
    if (r < 0) {
      az_sys_xu_iomux_del(ctrl->ctrl_sock);
      az_sys_socket_delete(ctrl->data_sock);
      ctrl->data_sock = AZ_SOCK_INVALID;
      ctrl->ctrl_sock = AZ_SOCK_INVALID;
      break;
    }
  }
  if (fd > 0) close(fd);

  if (ctrl->ctrl_sock != AZ_SOCK_INVALID) {
    az_sys_xu_iomux_del(ctrl->ctrl_sock);
    az_sys_socket_delete(ctrl->data_sock);
    ctrl->ctrl_sock = AZ_SOCK_INVALID;
    ctrl->data_sock = AZ_SOCK_INVALID;
  }
  //az_sys_xu_close_iomux();

  azm_probe_thread_state = 0;

  ctrl->state &= ~AZ_PROBE_STATE_ENAB;

  az_sys_printf("%s end...\n", __FUNCTION__);
  fflush(stdout);

  return r;
}

struct az_tcpserver_oprs azm_probe_svr_oprs;
az_tcpserver_t azm_probe_svr = {
  .state = 0,
  .config.name = "rprobeSvr", 
  .config.ipAddrStr = NULL, 
  .config.port = CONFIG_AZ_PROBE_SVR_PORT,
  .config.backlog = 16, 
  .config.flags = 0, 
  .sock = AZ_SOCK_INVALID, 
  .thread = NULL,
  .priv = NULL,
  .oprs = &azm_probe_svr_oprs, 
};

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
int azm_probe_svr_onClientConnection(void *ctx, az_sock_t cliSock, void *cliAddrIn)
{
  int r = AZ_SUCCESS;
  azm_probe_ctrl_t *ctrl = &azm_probe_ctrl;
  struct sockaddr_in *cliAddr = (struct sockaddr_in *)cliAddrIn;
  char cliIpStr[32] = {0};
  az_tcpserver_t *svr = (az_tcpserver_t *)ctx;


  if (ctrl->ctrl_sock == AZ_SOCK_INVALID) {
    ctrl->ctrl_sock = cliSock;
    az_inet_IpAddr2Str(cliAddr, cliIpStr, sizeof(cliIpStr));
    az_sys_eprintf("new probe ctrl connection from %s:%u" AZ_NL,
        cliIpStr, AZ_INET_PORT(cliAddr));
    return;
  } else if (ctrl->data_sock == AZ_SOCK_INVALID) {
    ctrl->data_sock = cliSock;
    az_inet_IpAddr2Str(cliAddr, cliIpStr, sizeof(cliIpStr));
    az_sys_eprintf("new probe data connection from %s:%u" AZ_NL,
        cliIpStr, AZ_INET_PORT(cliAddr));
  } else {
    az_inet_IpAddr2Str(cliAddr, cliIpStr, sizeof(cliIpStr));
    az_sys_eprintf("extra probe data connection from %s:%u, reject..." AZ_NL,
        cliIpStr, AZ_INET_PORT(cliAddr));
    az_sys_socket_delete(cliSock);
    return;
  }

  svr->state |= AZ_TCPSERVER_STATE_CLIBUSY;

  do {
    if (ctrl->state != AZ_PROBE_STATE_IDLE) {
      az_sys_eprintf("probe mon state=%x..., reject connection" AZ_NL,
          ctrl->state);
      r = AZ_ERR(AGAIN);
    }
    ctrl->state |= AZ_PROBE_STATE_INIT;

    r = azm_probe_proc_default(ctrl);

    ctrl->state &= ~AZ_PROBE_STATE_INIT;
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
int azm_probe_start()
{
  int r = AZ_SUCCESS;
  az_tcpserver_t *svr = (az_tcpserver_t *)&azm_probe_svr;
  
  azm_probe_svr_oprs = az_tcpserver_oprs_default;
  azm_probe_svr_oprs.onClientConnection = azm_probe_svr_onClientConnection;

  do {
    if (NULL == az_tcpserver_create(svr, NULL, &azm_probe_svr_oprs, NULL)) {
      az_printf("probe mon server: %s create reault=%d:%s" AZ_NL,
          svr->config.name, r, az_err_str(r));
      r = AZ_FAIL;
      break;
    }
    r = (svr->oprs->start)(svr);
  } while (0);
  az_printf("probe mon server: %s start reault=%d:%s" AZ_NL,
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
int azm_probe_stop()
{
  int r;
  az_tcpserver_t *svr = (az_tcpserver_t *)&azm_probe_svr;
  do {
    if (azm_probe_thread_state && svr->state & AZ_TCPSERVER_STATE_CLIBUSY) { 
      r = azm_probe_stop_default_thread();
    }
    r = (svr->oprs->stop)(svr);
    if (r) break;
    r = az_tcpserver_delete(svr);
  } while (0);
  az_printf("probe mon server: %s stop reault=%d:%s" AZ_NL,
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

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
void *azm_probe_thread_proc_default(void *arg)
{
  int r;
  
  r = azm_probe_proc_default(&azm_probe_ctrl); 

  return NULL;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
az_r_t  azm_probe_start_default_thread()
{
  int r;

  if (azm_probe_thread_state == 0) {
    azm_probe_thread_default = NULL;
    r = az_thread_create("rprobeDefault", azm_probe_thread_proc_default, NULL, NULL, &azm_probe_thread_default);
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
az_r_t  azm_probe_stop_default_thread()
{
  int r;
  azm_probe_ctrl_t *ctrl = &azm_probe_ctrl;

  if (azm_probe_thread_state) {
    azm_probe_thread_state = 0;
    while (ctrl->state & AZ_PROBE_STATE_ENAB) {
      az_thread_sleep(1000000);
    }
    ctrl->state = AZ_PROBE_STATE_IDLE;
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
int azm_probe_init()
{
  int r;

  r = azm_probe_start();
  return r;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
int azm_probe_deinit()
{
  int r;

  r = azm_probe_stop();
  return r;
}
#if 0
#endif
#endif // __GNUC__

/* === end of AZM_PROBE_C === */
