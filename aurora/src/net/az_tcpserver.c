/**
 * @file   az_tcpserver.c
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
#include "net/az_tcpserver.h"
#include "net/az_tcpserver_glue.h"
#include "az_printf.h"

extern int az_tcpserver_init_default(struct az_tcpserver *svr);
extern int az_tcpserver_start_default(struct az_tcpserver *svr);
extern void *az_tcpserver_run_default(void *arg);
extern int az_tcpserver_stop_default(struct az_tcpserver *svr);
extern int az_tcpserver_onClientConnection_default(void *ctx, az_socket_id_t cliSock, void *cliAddr);

/* declare global variables */
struct az_tcpserver_oprs az_tcpserver_oprs_default = {
  .init = az_tcpserver_init_default,
  .start = az_tcpserver_start_default,
  .run = az_tcpserver_run_default,
  .stop = az_tcpserver_stop_default,
  .onClientConnection = az_tcpserver_onClientConnection_default,
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
int az_tcpserver_init_default(struct az_tcpserver *svr)
{
  int r = AZ_SUCCESS;
  az_assert(NULL != svr);

  svr->state |= AZ_TCPSERVER_STATE_INIT;
  svr->config.timeout = 3000000; //us
  svr->config.backlog = 16;

  return r;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
int az_tcpserver_start_default(struct az_tcpserver *svr)
{
  az_r_t r;
  az_assert(NULL != svr);

  do {
    if (NULL != svr->thread) {
      r = AZ_ERR(AGAIN);
      break;
    }
    if (!(svr->state & AZ_TCPSERVER_STATE_INIT)) {
      az_assert(NULL != svr->oprs->init);
      r = (svr->oprs->init)(svr);
      if (r < 0) {
        break;
      }
    }
    r = az_tcpserver_thread_start(svr->config.name, svr->oprs->run, svr, &(svr->thread));
    if (r != AZ_SUCCESS) {
      svr->thread = NULL;
    } else {
      svr->state |= AZ_TCPSERVER_STATE_START;
    }
  } while (0);

  return (int)r;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
void *az_tcpserver_run_default(void *arg)
{
  az_tcpserver_t *svr = (az_tcpserver_t *)arg;
  az_assert(NULL != arg);
  int r;
  int nevt;
#ifdef  CONFIG_AZ_TCPSERVER_USE_SELECT
  int nfds;
  fd_set  fds;
  struct timeval tv;
#endif
  az_socket_t pCliSock;
  struct sockaddr_in  cliAddress;
  int svr_port_count = CONFIG_AZ_TCP_SVR_PORT_INC_MAX;

  do {
    r = az_inet_openTcpServer(svr->config.ipAddrStr, (uint16_t)svr->config.port, &(svr->sock));

    if (AZ_SUCCESS != r) {
      if (svr_port_count > 0) {
        svr_port_count--;
        svr->config.port++;
        continue;
      }
      az_sys_eprintf("tcp server on %s:%u open fail\n", svr->config.ipAddrStr,
          svr->config.port);
      break;
    }
    az_printf("tcp server on %s:%u open ok\n", svr->config.ipAddrStr,
          svr->config.port);
    r = az_inet_setSocketNonBlockMode(svr->sock->sys_socket);
    if (r < 0) {
      az_inet_closeTcpServer(svr->sock->sys_socket);
      svr->sock->sys_socket = AZ_SOCK_INVALID;
      break;
    }
    r = listen(svr->sock->sys_socket, svr->config.backlog);
    if (r < 0) {
      az_inet_closeTcpServer(svr->sock->sys_socket);
      svr->sock->sys_socket = AZ_SOCK_INVALID;
      break;
    }
    #ifdef  CONFIG_AZ_TCPSERVER_USE_SELECT
    FD_SET(svr->sock->sys_socket, &fds); nfds = svr->sock->sys_socket;
    #endif
    svr->state |= AZ_TCPSERVER_STATE_RUN;
  } while (AZ_SUCCESS != r);

  #ifdef  CONFIG_AZ_TCPSERVER_USE_SELECT
  FD_ZERO(&fds);
  while (svr->state & AZ_TCPSERVER_STATE_RUN) {
    FD_SET(svr->sock->sys_socket, &fds);
    tv.tv_sec = svr->config.timeout/1000000;
    tv.tv_usec = svr->config.timeout%1000000;
    nevt = select(nfds+1, &fds, NULL, NULL, &tv);
    if (nevt > 0) {
      if (!FD_ISSET(svr->sock->sys_socket, &fds)) continue;
      pCliSock = NULL;
      r = az_inet_getTcpConnection(svr->sock->sys_socket, &pCliSock, &cliAddress);
      if (r < 0) {
        continue;
      }
      if (svr->oprs->onClientConnection) {
        (svr->oprs->onClientConnection)(svr, pCliSock->sys_socket, (void *)&cliAddress);
      }
      continue;
    }
    if (nevt < 0) {
      break;
    }
  }
  #else

  az_sys_io_event_t ioevt;
  az_sys_xu_open_iomux();
  az_sys_xu_iomux_add(svr->sock->sys_socket, AZ_SYS_IO_IN|AZ_SYS_IO_HUP);
  while (svr->state & AZ_TCPSERVER_STATE_RUN) {
    nevt = az_thread_wait_iomux(&ioevt, 1, svr->config.timeout/1000);
    //az_printf("%ld %d\n", svr->config.timeout, nevt);
    if (nevt > 0) {
      if (ioevt.data.fd != svr->sock->sys_socket) continue;
      if (ioevt.events & AZ_SYS_IO_HUP) {
        break;
      }
      pCliSock = NULL;
      r = az_inet_getTcpConnection(svr->sock->sys_socket, &pCliSock, &cliAddress);
      if (r < 0) {
        continue;
      }
      if (svr->oprs->onClientConnection) {
        (svr->oprs->onClientConnection)(svr, pCliSock->sys_socket, (void *)&cliAddress);
      }
      continue;
    }
    if (nevt == 0) {
      continue;
    }
    if (nevt < 0) {
      az_sys_eprintf("nevt=%d\n", nevt); 
      break;
    }
  }
  
  az_sys_xu_iomux_del(svr->sock->sys_socket);
  az_sys_xu_close_iomux();
  #endif

  if (svr->state & AZ_TCPSERVER_STATE_RUN) {
    svr->state &= ~AZ_TCPSERVER_STATE_RUN;
    if (AZ_SOCK_INVALID != svr->sock->sys_socket) {
      shutdown(svr->sock->sys_socket, SHUT_RDWR);
      az_inet_closeTcpServer(svr->sock->sys_socket);
    }
  }

  return NULL;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
int az_tcpserver_stop_default(struct az_tcpserver *svr)
{
  az_r_t r = AZ_FAIL;
  az_assert(NULL != svr);

  if (svr->state & AZ_TCPSERVER_STATE_RUN) {
    shutdown(svr->sock->sys_socket, SHUT_RDWR);
    az_inet_closeTcpServer(svr->sock->sys_socket);
    svr->sock->sys_socket = AZ_SOCK_INVALID;
    svr->state &= ~AZ_TCPSERVER_STATE_RUN;
    while (svr->state & AZ_TCPSERVER_STATE_RUN) {
      ;
    }
  }
  if (svr->thread) {
    r = az_tcpserver_thread_stop(svr->thread);
    if (AZ_SUCCESS == r) {
      svr->thread = NULL;
      svr->state &= ~AZ_TCPSERVER_STATE_MASK;
    }
  } else {
    r = AZ_ERR(AGAIN);
  }

  return (int)r;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
int az_tcpserver_onClientConnection_default(void *ctx, az_socket_id_t cliSock, void *cliAddrIn)
{
  //az_tcpserver_t *svr = (az_tcpserver_t *)ctx;
  char  cliIpStr[32]; struct sockaddr_in  *cliAddr = (struct sockaddr_in  *)cliAddrIn;

  az_inet_IpAddr2Str(cliAddr, cliIpStr, sizeof(cliIpStr));
  az_printf("client from sock=%d from %s:%u\n",cliSock, cliIpStr, AZ_INET_PORT(cliAddr));

  close(cliSock);

  return AZ_SUCCESS;
}


/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
az_tcpserver_t *az_tcpserver_create(az_tcpserver_t *svr, 
    struct az_tcpserver_cfg *cfg, struct az_tcpserver_oprs *oprs, void *priv)
{
  do {
    if (NULL == oprs) oprs = &az_tcpserver_oprs_default;
    if (NULL == svr) {
      svr = az_malloc(sizeof(az_tcpserver_t));
      svr->state = AZ_TCPSERVER_STATE_DALLOC; 
    } else {
      svr->state = 0; 
    }
    if (NULL == svr) break;
    if (NULL != cfg) {
      svr->config = *cfg;
    }
    svr->sock = &svr->_sock;
    AZ_SOCKET_INIT_STATIC(svr->sock);
    svr->thread = NULL;
    if (NULL != oprs) {
      svr->oprs = oprs;
    }
    if (NULL != priv) {
      svr->priv = priv;
    }
  } while (0);

  return svr;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
int az_tcpserver_delete(az_tcpserver_t *svr)
{
  int r = AZ_SUCCESS;
  do {
    if (NULL == svr) {
      r = AZ_ERR(ARG_NULL);
      break;
    }
    if ((svr->state & ~AZ_TCPSERVER_STATE_DALLOC)) {
      r = AZ_ERR(STATE);
      break;
    }
    if (svr->state & AZ_TCPSERVER_STATE_DALLOC) {
      az_free(svr);
    }
  } while (0);

  return r;
}


/* === end of AZ_TCPSERVER_C === */
