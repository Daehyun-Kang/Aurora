/**
 * @file   az_trz.h
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

#ifndef AZ_TRZ_H
#define AZ_TRZ_H

#include "az_def.h"
#include "az_link.h"
#include "sys/az_inet.h"
#include "az_thread.h"
#include "az_printf.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* constants */

/* basic macros */
#ifdef  CONFIG_AZ_TRZ_NODEID_MAX
#define AZ_TRZ_NODEID_MAX     CONFIG_AZ_TRZ_NODEID_MAX
#else
#define AZ_TRZ_NODEID_MAX     256
#endif

#define AZ_TRZ_REQ_FLAG       0x8000
#define AZ_TRZ_ERR_FLAG       0x4000
#define AZ_TRZ_CODE_MASK      0x3fff

#define AZ_TRZ_STATE_IDLE      0x00
#define AZ_TRZ_STATE_BUSY      0x01
#define AZ_TRZ_STATE_WAIT      0x02
#define AZ_TRZ_STATE_ONCE      0x04
#define AZ_TRZ_STATE_LINK      0x08
#define AZ_TRZ_STATE_ETMO      0x10
#define AZ_TRZ_STATE_ESND      0x20
#define AZ_TRZ_STATE_DREQ      0x40 // request msg is malloced  
#define AZ_TRZ_STATE_DRSP      0x80 // response  msg is malloced
#define AZ_TRZ_FLAGS_MASK      (AZ_TRZ_STATE_ONCE|AZ_TRZ_STATE_DREQ|AZ_TRZ_STATE_DRSP)

#define AZ_TRZ_NODEID_ALL      0xffff 
#define AZ_TRZ_SEQNO_ALL       0xffff 

#define AZ_TRZ_WAIT_EVENT_MAX  32

/* basic types */
typedef uint16_t  az_trz_nodeid_t;
typedef uint16_t  az_trz_seqno_t;
typedef uint16_t  az_trz_code_t;
typedef uint16_t  az_trz_len_t;
struct az_transaction; 
typedef void (*az_trz_handler_t)(struct az_transaction *, az_socket_id_t );

/* structures */

/* structured types */
typedef AZ_PACKED_STRUCT(az_transaction_msg_header) {
  az_trz_nodeid_t  nodeid; //source nodeid
  az_trz_seqno_t seqno;
  az_trz_code_t  code;
  az_trz_len_t  len;
} az_trz_msg_hdr_t;

#include "az_trz_msg.h"

typedef struct az_transaction {
  az_trz_nodeid_t   nodeid; //destnation nodeid
  az_trz_seqno_t  seqno; 
  az_trz_code_t   code;
  uint16_t        state;
  void            *req;
  void            *rsp;

  time_t           timelimit;

  az_ion_id_t      xu_id;
  az_trz_handler_t handler;
  void *priv;

  az_link_t link;
} az_trz_t;

typedef struct az_transaction_node {
  az_trz_nodeid_t nodeid; //destination nodeid
  uint16_t  port;
  char      *ipAddr;
  int       backlog;

  az_socket_t sock;
  az_socket_entity_t  _sock;
} az_trz_node_t;

typedef struct az_transaction_connection {
  az_trz_nodeid_t  nodeid; //destination nodeid
  az_socket_t      sock;

  az_trz_node_t *peer;

  az_link_list_t list;
  az_socket_entity_t  _sock;
} az_trz_conn_t;

/* macros */

/* variabls exposed */
extern int az_trz_state;
extern int az_trz_seqno;
extern az_sys_lw_lock_t az_trz_lock;
extern az_trz_conn_t az_trz_conn_default;
extern az_trz_conn_t *az_trz_conn_list[];

#define az_trz_conn(nodeid) ((az_trz_conn_t *)((nodeid < AZ_TRZ_NODEID_MAX)? az_trz_conn_list[nodeid]:&az_trz_conn_default)) 

/* inline functions */
static inline az_trz_t *az_trz_alloc(az_trz_nodeid_t nodeid, az_trz_seqno_t seqno, az_trz_code_t code)
{
  az_trz_t *trz =  (az_trz_t *)az_malloc(sizeof(az_trz_t));
  if (trz) {
    trz->nodeid = nodeid;
    trz->seqno = seqno;
    trz->code = code;
    trz->state = AZ_TRZ_STATE_IDLE;
    trz->req = NULL;
    trz->rsp = NULL;
    az_link_init(&trz->link);
  }
  return trz;
}
static inline void az_trz_free(az_trz_t *trz)
{
  if (trz) {
    if ((trz->state & AZ_TRZ_STATE_DREQ) && NULL != trz->req) {
      az_free(trz->req);
    }
    if ((trz->state & AZ_TRZ_STATE_DRSP) && NULL != trz->rsp) {
      az_free(trz->rsp);
    }
    az_free(trz);
  }
}


static inline az_r_t az_trz_conn_register(az_trz_nodeid_t nodeid, az_socket_id_t sockid, az_trz_node_t *node)
{
  az_trz_conn_t *trz_conn = az_trz_conn(nodeid);
  az_r_t r = AZ_SUCCESS;
  do {
    if (trz_conn == NULL) {
      trz_conn = (az_trz_conn_t *)az_malloc(sizeof(az_trz_conn_t));
      if (trz_conn == NULL) {
        r = AZ_ERR(MALLOC);
        break;
      }
      trz_conn->nodeid = nodeid;
      trz_conn->sock = (az_socket_t)az_ion(sockid);
      trz_conn->peer = node;

      az_link_initList(&trz_conn->list);
      break;
    }
    if (trz_conn->nodeid != nodeid || trz_conn->sock->sys_socket != sockid) {
      r = AZ_ERR(INVALID);
      break;
    }
    r = AZ_ERR(AGAIN);
  } while (0);

  return r;
}
static inline az_r_t az_trz_conn_deregister(az_trz_nodeid_t nodeid)
{
  az_trz_conn_t *trz_conn = az_trz_conn_list[nodeid];
  az_r_t r = AZ_SUCCESS;
  do {
    if (trz_conn == NULL) {
      r = AZ_ERR(NOT_FOUND);
      break;
    }
    if (trz_conn->list.count > 0) {
      r = AZ_ERR(BUSY);
      break;
    }
    az_trz_conn_list[nodeid] = NULL;
    az_free(trz_conn);
  } while (0);

  return r;
}
static inline az_trz_conn_t *az_trz_conn_find(az_socket_id_t sockid)
{
  az_trz_conn_t *trz_conn;
  int nodeid = 0;
  for (; nodeid < AZ_TRZ_NODEID_MAX; nodeid++) {
    trz_conn = az_trz_conn_list[nodeid];
    if (trz_conn == NULL) continue;
    if (trz_conn->sock->sys_socket == sockid) {
      break;
    }
  }
  if (nodeid == AZ_TRZ_NODEID_MAX) trz_conn = NULL;

  return trz_conn;
}
static inline az_trz_conn_t *az_trz_conn_getfree(az_socket_id_t sockid, az_trz_node_t *node)
{
  extern az_trz_node_t az_trz_node_local;
  az_trz_conn_t *trz_conn;
  az_trz_nodeid_t nodeid = 0;
  az_r_t r;

  for (; nodeid < AZ_TRZ_NODEID_MAX; nodeid++) {
    if (nodeid == az_trz_node_local.nodeid) continue;
    trz_conn = az_trz_conn_list[nodeid];
    if (trz_conn == NULL) {
      r = az_trz_conn_register(nodeid, sockid, node);
      if (r == AZ_SUCCESS) break;
    }
  }
  if (nodeid == AZ_TRZ_NODEID_MAX) trz_conn = NULL;

  return trz_conn;
}

static inline void az_trz_put(az_link_list_t *list, az_trz_t *trz, az_sys_lw_lock_t *lock)
{
  az_assert(NULL != trz);
  if (!(trz->state & AZ_TRZ_STATE_LINK)) {
    if (NULL == lock) lock = &az_trz_lock;
    az_sys_lw_lock(lock);
    az_link_putLast(list, &trz->link);
    trz->state |= AZ_TRZ_STATE_LINK;
    az_sys_lw_unlock(lock);
  }
}
static inline az_trz_t *az_trz_get(az_link_list_t *list, az_sys_lw_lock_t *lock)
{
  az_trz_t *trz = NULL;
  az_link_t *link;
  
  if (NULL == lock) lock = &az_trz_lock;
  az_sys_lw_lock(lock);
  link = az_link_getFirst(list);
  az_sys_lw_unlock(lock);
  if (link) {
    trz = container_of(link, az_trz_t, link); 
    trz->state &= ~AZ_TRZ_STATE_LINK;
  }
  return trz;
}
static inline int az_trz_flush(az_link_list_t *list, az_sys_lw_lock_t *lock)
{
  az_trz_t *trz;
  int count = 0;
  if (NULL == lock) lock = &az_trz_lock;
  do {
    trz = az_trz_get(list, lock);
    if (trz == NULL) break;
    count++;
    az_trz_free(trz);
  } while (1);

  return count;
}

static inline az_r_t az_trz_add(az_trz_t *trz, az_ion_id_t xu_id, az_trz_handler_t handler, void *priv, int istate)
{
  az_assert(NULL != trz);
  az_trz_conn_t *trz_conn = az_trz_conn(trz->nodeid);
  az_r_t r = AZ_SUCCESS;
  do {
    if (trz_conn == NULL) {
      r = AZ_ERR(NOT_FOUND);
      break;
    }
    if (trz->link.next != NULL) {
      r = AZ_ERR(BUSY);
      break;
    }
    if (trz->state != AZ_TRZ_STATE_IDLE) {
      r = AZ_ERR(STATE);
      break;
    }
    trz->state |= AZ_TRZ_STATE_LINK;
    if (istate) trz->state |= istate;
    trz->timelimit = 0;
    trz->xu_id = xu_id;
    trz->handler = handler;
    trz->priv = priv;
    az_sys_lw_lock(&az_trz_lock);
    az_link_putLast(&trz_conn->list, &trz->link);
    az_sys_lw_unlock(&az_trz_lock);
  } while (0);

  return r;
}

static inline az_trz_t *az_trz_find(az_trz_nodeid_t nodeid, az_trz_seqno_t seqno, az_trz_code_t code, int remove)
{
  az_trz_conn_t *trz_conn = az_trz_conn(nodeid);
  az_r_t r = AZ_SUCCESS;
  az_trz_t *trz = NULL;
  az_link_list_t *list;
  az_link_t *link, *prev;
  do {
    if (trz_conn == NULL) {
      r = AZ_ERR(NOT_FOUND);
      break;
    }
    list = &trz_conn->list;
    prev = list;
    az_sys_lw_lock(&az_trz_lock);
    link = list->next;
    while (link != list->next) {
      trz = container_of(link, az_trz_t, link); 
      if (trz->seqno == seqno && trz->code == code) {
        if (remove || (trz->state & AZ_TRZ_STATE_ONCE)) {
          az_link_remove(list, prev, link);
          trz->state &= ~AZ_TRZ_STATE_LINK;
        }
        break;
      }
      trz = NULL;
      prev = link;
      link = prev->next;
    }
    az_sys_lw_unlock(&az_trz_lock);
  } while (0);

  return trz;
}

static inline az_trz_t *az_trz_remove(az_trz_nodeid_t nodeid, az_trz_seqno_t seqno,
    az_trz_code_t code)
{
  return az_trz_find(nodeid, seqno, code, 1);
}

static inline az_r_t az_trz_reset(az_trz_t *trz, az_trz_msg_hdr_t *msgp, az_trz_code_t code, az_trz_handler_t handler, void *priv, int flags)
{
  az_r_t r = AZ_SUCCESS;
  do {
    if (trz->state & (AZ_TRZ_STATE_BUSY|AZ_TRZ_STATE_WAIT))
    {
      r = AZ_ERR(BUSY);
      break;
    }
    if (trz->nodeid != msgp->nodeid) {
      if (trz->state & AZ_TRZ_STATE_LINK) {
        az_trz_remove(trz->nodeid, trz->seqno, trz->code);
      }
      trz->nodeid = msgp->nodeid;
    }
    trz->seqno = msgp->seqno;
    trz->code = code; 
    if (trz->state & AZ_TRZ_STATE_LINK) {
      trz->xu_id = az_thread_self()->ion.id;
      trz->handler = handler; 
      trz->priv = priv; 
      trz->state |= flags;
    } else {
      r = az_trz_add(trz, az_thread_self()->ion.id, handler, priv, flags);
    }
  } while (0);
  return r;
}

/* function prototypes exposed */
extern  void az_trz_init_vars();
extern az_r_t az_trz_send_request(az_trz_t *trz, az_trz_msg_hdr_t *msgp, 
    az_trz_handler_t handler, void *priv, int timeout, int flags); 
extern az_r_t az_trz_proc_request(az_trz_msg_hdr_t *msgp, az_socket_id_t sockid);
extern az_r_t az_trz_proc_response(az_trz_msg_hdr_t *msgp, az_socket_id_t sockid);
extern az_r_t az_trz_send_response(az_trz_t *trz, az_trz_msg_hdr_t *msgp, int flags); 

extern az_r_t az_trz_send_control_msg(az_trz_node_t *peer, az_trz_msg_hdr_t *msgp, az_trz_handler_t handler, void *priv, int timeout, int flags);
extern void az_trz_msg_query_nodeid_rsp_handler(az_trz_t *trz, az_socket_id_t sockid);
#ifdef __cplusplus
}
#endif
#endif
