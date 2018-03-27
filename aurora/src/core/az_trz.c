/**
 * @file   az_trz.c
 * @brief  
 * @date   20/03/18
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
 *   - 2018-03-20 dhkang initial create
 */

/* include header files */
#include "az_trz.h"

#ifdef  CONFIG_AZ_TRZ_LOG_SVR
#define AZ_TRZ_LOCAL_NODE CONFIG_AZ_TRZ_LOG_SVR_NODE
#else
#define AZ_TRZ_LOCAL_NODE CONFIG_AZ_TRZ_NODEID_DEFAULT
#endif

/* declare global variables */
az_trz_node_t  az_trz_node_local = {
  .nodeid = AZ_TRZ_LOCAL_NODE, 
  .port = CONFIG_AZ_TRZ_SVR_PORT_BASE + AZ_TRZ_LOCAL_NODE, 
  .ipAddr = "0.0.0.0",
  .backlog = 16,
};
az_trz_node_t  az_trz_node_log_svr = {
  .nodeid = CONFIG_AZ_TRZ_LOG_SVR_NODE, 
  .port = CONFIG_AZ_TRZ_SVR_PORT_BASE + CONFIG_AZ_TRZ_LOG_SVR_NODE, 
  .ipAddr = "0.0.0.0",
  .backlog = 16,
};

int  az_trz_state = 0;
int  az_trz_seqno = 0;
az_sys_lw_lock_t  az_trz_lock;
az_trz_conn_t az_trz_conn_default;
az_trz_conn_t *az_trz_conn_list[AZ_TRZ_NODEID_MAX];

int az_trz_thread_state = 0;
az_xu_t az_trz_thread_default = NULL;

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
void az_trz_init_vars()
{
  if (az_trz_state == 0) {
    az_trz_state = 1;
    az_sys_lw_lock_init(&az_trz_lock);
    memset(az_trz_conn_list, 0, array_size(az_trz_conn_list));
  }

  az_trz_conn_default.nodeid = AZ_TRZ_NODEID_ALL;
  az_trz_conn_default.sock = AZ_SOCK_INVALID;
  az_trz_conn_default.peer = NULL; 
  az_link_initList(&az_trz_conn_default.list);
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
void az_trz_default_req_handler(az_trz_t *trz, az_sock_t sock)
{
  az_assert(NULL != trz);
  az_r_t r = AZ_SUCCESS;
  do {
    az_trz_put(&trz->xu->trz_list, trz, NULL);
    r = az_xu_sendEvent(trz->xu, AZ_XU_EVENT_TRZ);
  } while (0);
  return r;
}

void az_trz_default_rsp_handler(az_trz_t *trz, az_sock_t sock)
{
  az_assert(NULL != trz);
  az_r_t r = AZ_SUCCESS;
  do {
    az_trz_put(&trz->xu->trz_list, trz, NULL);
    r = az_xu_sendEvent(trz->xu, AZ_XU_EVENT_TRZ);
  } while (0);
  return r;
}

void az_trz_msg_keepalive_req_handler(az_trz_t *trz, az_sock_t sock)
{
  az_assert(NULL != trz);
  az_r_t r = AZ_SUCCESS;

  az_trz_msg_hdr_t *req = (az_trz_msg_hdr_t *)trz->req;
  az_trz_msg_hdr_t rsp = {
    .nodeid = req->nodeid, 
    .seqno = req->seqno,
    .code = req->code & ~AZ_TRZ_REQ_FLAG,
    .len = 0,
  };
  do {
    r = az_trz_send_response(trz, &rsp, 0); 
    if (trz->state & AZ_TRZ_STATE_DREQ) {
      trz->req = NULL;
      trz->state &= ~AZ_TRZ_STATE_DREQ;
      az_free(req);
    }
  } while (0);
}

void az_trz_msg_keepalive_rsp_handler(az_trz_t *trz, az_sock_t sock)
{
  az_assert(NULL != trz);
  az_r_t r = AZ_SUCCESS;
  do {
    az_printf("node=%d seqno=%d code=%04x complete...\n",
        trz->nodeid, trz->seqno, trz->code);
    //note trz is already removed from wait list
    //trz = az_trz_remove(msghdr->nodeid, msghdr->seqno, msghdr->code);
    if (NULL != trz) {
      // rx msg buffer will be freed along with trz.
      az_trz_free(trz);
    }
  } while (0);
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
az_r_t az_trz_msg_keepalive_req_send(az_trz_nodeid_t nodeid) 
{
  az_r_t r = AZ_SUCCESS;
  az_trz_msg_hdr_t msghdr = {
    .nodeid = nodeid,
    .code = AZ_TRZ_CODE_KEEPALIVE,
    .len = 0,
  };

  do {
    r = az_trz_send_request(NULL, &msghdr, az_trz_msg_keepalive_rsp_handler, NULL, 3, AZ_TRZ_STATE_ONCE);
    if (AZ_SUCCESS != r) {
      az_printf("result %d:%s\n", r, az_err_str(r));
      az_trz_t *trz = az_trz_remove(msghdr.nodeid, msghdr.seqno, msghdr.code);
      if (NULL != trz) {
        az_trz_free(trz);
      }
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
az_r_t az_trz_send_request(az_trz_t *trz, az_trz_msg_hdr_t *msgp, az_trz_handler_t handler, void *priv, int timeout, int flags) 
{
  az_assert(NULL != msgp);
  az_r_t r = AZ_SUCCESS;
  az_trz_conn_t *trz_conn;

  do {
    flags = flags & AZ_TRZ_FLAGS_MASK;
    msgp->seqno = az_trz_seqno++;
    if (trz == NULL) {
      // note msgp->nodeid is the destination node
      trz = az_trz_alloc(msgp->nodeid, msgp->seqno, msgp->code & AZ_TRZ_CODE_MASK);
      if (NULL == trz) {
        r = AZ_ERR(MALLOC);
        break;
      }
      r = az_trz_add(trz, az_xu_self(), handler, priv, flags);
    } else {
      // reset the trz info to be consistent with req msg
      r = az_trz_reset(trz, msgp, msgp->code & AZ_TRZ_CODE_MASK,
          handler, priv, flags);
    }
    if (r != AZ_SUCCESS) {
      break;
    }
    trz_conn = az_trz_conn(trz->nodeid);
    if (trz_conn == NULL) {
      r = AZ_ERR(ENTITY_NULL);
      break;
    }
    trz->req = msgp;
    if (flags & AZ_TRZ_STATE_DREQ) trz->state |= AZ_TRZ_STATE_DREQ;
    if (trz_conn->sock == AZ_SOCK_INVALID) {
      az_trz_node_t  *peer = trz_conn->peer;
      if (peer == NULL) {
        r = AZ_ERR(INVALID);
        break;
      }
      r = az_inet_openTcpClient(
            peer->ipAddr, 
            peer->port, NULL, 0, &trz_conn->sock);
      if (r != AZ_SUCCESS) {
        break;
      }
      r = az_xu_iomux_add(az_trz_thread_default, trz_conn->sock, AZ_SYS_IO_IN|AZ_SYS_IO_HUP);
      if (r < 0) break;
    }

    // change node id to source node
    msgp->nodeid = az_trz_node_local.nodeid; 
    msgp->code |= AZ_TRZ_REQ_FLAG;
    r = az_send(trz_conn->sock, msgp, sizeof(*msgp) + msgp->len, 0);
    // resoter nodeid
    msgp->nodeid = trz->nodeid;
    if (r != (sizeof(*msgp) + msgp->len)) {
      trz->state |= AZ_TRZ_STATE_ESND;
      r = AZ_FAIL;
      break;
    } else {
      trz->timelimit = time(NULL) + timeout;
    }
    trz->state |= AZ_TRZ_STATE_WAIT;
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
az_r_t az_trz_send_control_msg(az_trz_node_t *peer, az_trz_msg_hdr_t *msgp, az_trz_handler_t handler, void *priv, int timeout, int flags) 
{
  az_assert(NULL != msgp);
  az_r_t r = AZ_SUCCESS;
  az_trz_t *trz;

  do {
    msgp->nodeid = AZ_TRZ_NODEID_ALL; 
    flags = flags & AZ_TRZ_FLAGS_MASK;
    msgp->seqno = az_trz_seqno++;
    // note msgp->nodeid is the destination node
    trz = az_trz_alloc(msgp->nodeid, msgp->seqno, msgp->code & AZ_TRZ_CODE_MASK);
    if (NULL == trz) {
      r = AZ_ERR(MALLOC);
      break;
    }
    r = az_trz_add(trz, az_xu_self(), handler, priv, flags);
    if (r != AZ_SUCCESS) {
      break;
    }
    trz->req = msgp;
    if (flags & AZ_TRZ_STATE_DREQ) trz->state |= AZ_TRZ_STATE_DREQ;
    if (peer->sock == AZ_SOCK_INVALID) {
      if (peer == NULL) {
        r = AZ_ERR(INVALID);
        break;
      }
      r = az_inet_openTcpClient(
            peer->ipAddr, 
            peer->port, NULL, 0, &peer->sock);
      if (r != AZ_SUCCESS) {
        break;
      }
      r = az_xu_iomux_add(az_trz_thread_default, peer->sock, AZ_SYS_IO_IN|AZ_SYS_IO_HUP);
      if (r < 0) break;
    }

    // change node id to source node
    msgp->code |= AZ_TRZ_REQ_FLAG;
    r = az_send(peer->sock, msgp, sizeof(*msgp) + msgp->len, 0);
    if (r != (sizeof(*msgp) + msgp->len)) {
      trz->state |= AZ_TRZ_STATE_ESND;
      r = AZ_FAIL;
      break;
    } else {
      trz->timelimit = time(NULL) + timeout;
    }
    trz->state |= AZ_TRZ_STATE_WAIT;
  } while (0);

  return r;
}

az_r_t az_trz_msg_query_nodeid_req_send(az_trz_node_t *peer)
{
  az_r_t r = AZ_SUCCESS;
  struct az_trz_msg_query_nodeid txmsg;
  txmsg.hdr.nodeid = AZ_TRZ_NODEID_ALL;
  txmsg.hdr.seqno = AZ_TRZ_SEQNO_ALL;
  txmsg.hdr.code = AZ_TRZ_CODE_REQ(QUERY_NODEID);
  txmsg.hdr.len = sizeof(txmsg) - sizeof(txmsg.hdr); 

  do {
    r = az_trz_send_control_msg(peer, &txmsg, az_trz_msg_query_nodeid_rsp_handler,  NULL, 1, 0); 
    if (AZ_SUCCESS != r) {
      az_printf("result %d:%s\n", r, az_err_str(r));
      az_trz_t *trz = az_trz_remove(txmsg.hdr.nodeid, txmsg.hdr.seqno, txmsg.hdr.code);
      if (NULL != trz) {
        az_trz_free(trz);
      }
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
void az_trz_msg_query_nodeid_req_handler(az_trz_t *trz, az_sock_t sock)
{
  az_assert(NULL != trz);
  az_r_t r = AZ_SUCCESS;
  az_trz_conn_t *trz_conn;

  az_trz_msg_hdr_t *req = (az_trz_msg_hdr_t *)trz->req;
  struct az_trz_msg_query_nodeid rsp; 

  do {
    rsp.hdr = *req;
    rsp.hdr.code = AZ_TRZ_CODE_RSP(QUERY_NODEID);
    trz_conn = az_trz_conn_find(sock);
    if (trz_conn == NULL) {
      trz_conn = az_trz_conn_getfree(sock, NULL);
      if (trz_conn == NULL) {
        rsp.hdr.code |= AZ_TRZ_ERR_FLAG;
      }
    }
    rsp.local_nodeid = trz_conn->nodeid;
    rsp.remote_nodeid = az_trz_node_local.nodeid;
    rsp.hdr.len = sizeof(rsp) - sizeof(rsp.hdr);
    r = az_send(trz_conn->sock, &rsp, sizeof(rsp), 0);
    trz->state &= ~AZ_TRZ_STATE_BUSY;

    if (trz->state & AZ_TRZ_STATE_DREQ) {
      trz->req = NULL;
      trz->state &= ~AZ_TRZ_STATE_DREQ;
      az_free(req);
    }
  } while (0);

}

void az_trz_msg_query_nodeid_rsp_handler(az_trz_t *trz, az_sock_t sock)
{
  az_assert(NULL != trz);
  az_r_t r = AZ_SUCCESS;
  az_trz_conn_t *trz_conn;

  do {
    struct az_trz_msg_query_nodeid *rsp = (struct az_trz_msg_query_nodeid *)trz->rsp; 
    if (rsp->hdr.code & AZ_TRZ_ERR_FLAG) {
      az_printf("nodeid query failed remote=%d\n", rsp->remote_nodeid);
      break;
    }
    az_trz_node_log_svr.nodeid = rsp->remote_nodeid; 
    trz_conn = az_trz_conn(rsp->remote_nodeid);
    if (NULL == trz_conn) {
      r = az_trz_conn_register(rsp->remote_nodeid, sock, &az_trz_node_log_svr);
      // register transaction handler from remote node
      az_trz_t *trz = az_trz_alloc(rsp->remote_nodeid, AZ_TRZ_SEQNO_ALL, AZ_TRZ_CODE_REQ(KEEPALIVE));
      if (trz) { 
        az_trz_add(trz, az_xu_self(), az_trz_msg_keepalive_req_handler, NULL, 0);
      }
      az_printf("register trz conn for remote=%d result=%ld\n", rsp->remote_nodeid, r);
    }
    if (az_trz_node_local.nodeid != rsp->local_nodeid) {
      az_trz_node_local.nodeid = rsp->local_nodeid;
      az_printf("local trz nodeid changed to %d\n", az_trz_node_local.nodeid); 
    }
    if (NULL != trz) {
      // rx msg buffer will be freed along with trz.
      az_trz_free(trz);
    }
  } while (0);
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
az_r_t az_trz_proc_request(az_trz_msg_hdr_t *msgp, az_sock_t sock)
{
  az_assert(NULL != msgp);
  az_r_t r = AZ_SUCCESS;
  az_trz_conn_t *trz_conn;
  az_trz_t *trz;

  do {
    trz_conn = az_trz_conn(msgp->nodeid);
    if (trz_conn == NULL) {
      // TODO: peer node info found and set on new trz_conn
      r = az_trz_conn_register(msgp->nodeid, sock, NULL);
      if (r < 0) {
        az_free(msgp);
        break;
      }
      trz_conn = az_trz_conn(msgp->nodeid); 
      assert(trz_conn != NULL);
    }
    trz = az_trz_find(msgp->nodeid, AZ_TRZ_SEQNO_ALL, msgp->code, 0);
    if (NULL == trz) {
      r = AZ_ERR(NOT_FOUND);
      struct az_trz_msg_error_report erpt;
      erpt.hdr.nodeid = az_trz_node_local.nodeid,
      erpt.hdr.seqno = msgp->seqno;
      erpt.hdr.code = trz->code | AZ_TRZ_ERR_FLAG,
      erpt.hdr.len = sizeof(erpt) - sizeof(erpt.hdr);
      erpt.errcode = (int)r; 
      az_send(trz_conn->sock, &erpt, sizeof(erpt), 0);
      
      az_free(msgp);
      break;
    }
    trz->state &= ~AZ_TRZ_STATE_WAIT;
    trz->state |= AZ_TRZ_STATE_BUSY;
    trz->req = msgp;
    trz->state |= AZ_TRZ_STATE_DREQ;
    (trz->handler)(trz, sock);
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
az_r_t az_trz_proc_response(az_trz_msg_hdr_t *msgp, az_sock_t sock)
{
  az_assert(NULL != msgp);
  az_r_t r = AZ_SUCCESS;
  az_trz_conn_t *trz_conn;
  az_trz_t *trz;

  do {
    trz_conn = az_trz_conn(msgp->nodeid);
    if (trz_conn == NULL) {
      r = AZ_ERR(ENTITY_NULL);
      az_free(msgp);
      break;
    }
    trz = az_trz_find(msgp->nodeid, msgp->seqno, msgp->code & AZ_TRZ_CODE_MASK, 0);
    if (NULL == trz) {
      r = AZ_ERR(NOT_FOUND);
      az_free(msgp);
      break;
    }
    trz->state &= ~AZ_TRZ_STATE_WAIT;
    trz->state |= AZ_TRZ_STATE_BUSY;
    trz->rsp = msgp;
    trz->state |= AZ_TRZ_STATE_DRSP;
    (trz->handler)(trz, sock);
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
az_r_t az_trz_send_response(az_trz_t *trz, az_trz_msg_hdr_t *msgp, int flags) 
{
  az_r_t r = AZ_SUCCESS;
  az_assert(NULL != trz);
  az_trz_conn_t *trz_conn = az_trz_conn(trz->nodeid);

  do {
    if (trz_conn == NULL) {
      r = AZ_ERR(ENTITY_NULL);
      break;
    }
    assert(msgp->nodeid == trz->nodeid);
    // when msgp is static, AZ_TRZ_STATE_DRSP shoul not be set on flags
    trz->rsp = msgp;
    if (flags & AZ_TRZ_STATE_DRSP) trz->state |= AZ_TRZ_STATE_DRSP;
    // change node id to source node
    msgp->nodeid = az_trz_node_local.nodeid; 
    // additional settings 
    msgp->seqno = trz->seqno;
    msgp->code &= ~AZ_TRZ_REQ_FLAG;
    r = az_send(trz_conn->sock, msgp, sizeof(*msgp)+msgp->len, 0);
    // restore node id to destination node
    msgp->nodeid = trz->nodeid;
    if (r != (sizeof(*msgp) + msgp->len)) {
      r = AZ_FAIL;
      break;
    }
    trz->state &= ~AZ_TRZ_STATE_BUSY;
  } while (0);

  return r;
}

int az_trz_recv_proc(az_sock_t sock)
{
  az_r_t r;
  int nlen;
  int tlen = 0;
  az_trz_msg_hdr_t msghdr;
  char *bp;
  int msgcount = 0;

  do {
    bp = &msghdr;
    tlen = az_recv(sock, bp, sizeof(msghdr), MSG_PEEK);  
    if (tlen != sizeof(msghdr)) {
      r = AZ_ERR(NOT_READY);
      break;
    }
    tlen = sizeof(msghdr) + msghdr.len;
    bp = az_malloc(tlen);
    if (NULL == bp) {
      r = AZ_ERR(MALLOC);
      break;
    }
    while (tlen > 0) {
      nlen = az_recv(sock, bp, tlen, 0); 
      if (nlen < 0) break;
      tlen -= nlen; bp += nlen;
    }
    if (tlen > 0) {
      r = AZ_FAIL;
      break;
    }

    msgcount++;
    if (msghdr.code & AZ_TRZ_REQ_FLAG) {
      r = az_trz_proc_request((az_trz_msg_hdr_t *)bp, sock);
    } else {
      r = az_trz_proc_response((az_trz_msg_hdr_t *)bp, sock);
    }

  } while (1);

  return msgcount;
}


/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
void *az_trz_thread_proc_default(void *arg)
{
  az_xu_event_t received;
  az_xu_event_t emask;
  az_r_t  r;
  az_int64_t  tmo_ns = 1000000000; // 1 sec
  int tmo_count = 0; 
  az_trz_node_t  *local = &az_trz_node_local;
  az_trz_node_t  *svr = &az_trz_node_log_svr;
  az_sock_t localSvrSock;

  az_trz_thread_state = 1;
  az_sys_eprintf("%s start...\n", __FUNCTION__);

  az_trz_init_vars();

  do {
    r = az_inet_openTcpServer(local->ipAddr, local->port, &localSvrSock); 
    if (AZ_SUCCESS != r) {
      az_sys_eprintf("tcp server on %s:%u open fail\n", local->ipAddr, local->port);
      break;
    }
    r = az_inet_setSocketNonBlockMode(localSvrSock);
    if (r < 0) {
      r = az_inet_closeTcpServer(localSvrSock);
      local->sock = AZ_SOCK_INVALID;
      break;
    }
    r = listen(localSvrSock, local->backlog);
    if (r < 0) {
      r = az_inet_closeTcpServer(localSvrSock);
      local->sock = AZ_SOCK_INVALID;
      break;
    }
  } while (0);


  int nevt;
  int j;
  az_sock_t cliSock;
  struct sockaddr_in cliAddress;

  az_sys_io_event_t ioevt[AZ_TRZ_WAIT_EVENT_MAX];
  az_sys_xu_open_iomux();
  if (local->sock != AZ_SOCK_INVALID) {
    az_sys_xu_iomux_add(localSvrSock, AZ_SYS_IO_IN|AZ_SYS_IO_HUP);
  }

  az_trz_t *trz = az_trz_alloc(AZ_TRZ_NODEID_ALL, AZ_TRZ_SEQNO_ALL, AZ_TRZ_CODE_REQ(QUERY_NODEID));
  if (trz) {
    az_trz_add(trz, az_xu_self(), az_trz_msg_query_nodeid_req_handler, NULL, 0);
  }

  if (local->nodeid != svr->nodeid) {
    // query nodeid
    r = az_trz_msg_query_nodeid_req_send(svr);
  }

  while (az_trz_thread_state) {
    nevt = az_sys_xu_wait_iomux(ioevt, AZ_TRZ_WAIT_EVENT_MAX, tmo_ns/1000000); 

    if (nevt > 0) {
      for (j = 0; j < nevt; j++) {
        if (ioevt[j].data.fd == localSvrSock) {
          r = az_inet_getTcpConnection(localSvrSock, &cliSock, &cliAddress);
          if (r < 0) {
            continue;
          }
          az_sys_xu_iomux_add(cliSock, AZ_SYS_IO_IN|AZ_SYS_IO_HUP);
          continue;
        }
        if (ioevt[j].data.fd == az_sys_xu_io_default()) {
          continue;
        }
        cliSock = ioevt[j].data.fd;
        if (ioevt[j].events & AZ_SYS_IO_HUP) {
          // close corresponding trz_connection
          az_sys_xu_iomux_del(cliSock);
          continue;
        }
        az_trz_recv_proc(cliSock);
        // optionally close cliSock
      }
    }
    if (nevt < 0) {
      az_sys_eprintf("nevt=%d\n", nevt);
      break;
    }
    tmo_count++;
    // scan response time expired transactions
  }

  if (localSvrSock != AZ_SOCK_INVALID) {
    az_sys_xu_iomux_del(localSvrSock);
  }

  az_sys_eprintf("%s end...\n", __FUNCTION__);

  return NULL;
}

az_r_t  az_trz_start_default_thread()
{
  int r;

  r = az_xu_create("trzDefault", az_trz_thread_proc_default, NULL, NULL, &az_trz_thread_default);
  //az_sys_printf("%s: %p\n", __FUNCTION__, az_trz_thread_default);

  return r;
}

az_r_t  az_trz_stop_default_thread()
{
  int r;

  az_trz_thread_state = 0;
  r = az_xu_delete(az_trz_thread_default);

  return r;
}


/**
 * @fn        function name
 * @brief     function-description
 * @param     input-output-parameters
 * @return    return-value
 * @warning   warnings
 * @exception none
 */


/* === end of AZ_TRZ_C === */
