/**
 * @file   az_event_bus.c
 * @brief  
 * @date   24/06/16
 * @author fware
 *
 * @copyright
 * Copyright (c) Fware, 2013-2016 - All Rights Reserved
 * You may use, distribute and modify this code under the 
 * terms of the "Aurora Source Code License".
 * See the file LICENSE for full license details.\n\n
 *
 * You should have received a copy of  the LICENSE with this file.\n\n
 *
 * If not, please contact to Fware with the information in the file CONTACTS.
 *
 * History:
 *   - 2016-06-24 fware initial create
 */

/* include header files */
#include "az_core.h"
#include "az_event_bus.h"
#include "az_printf.h"
#include "az_malloc.h"

/* declare global variables */
az_event_port_t az_event_port_list_default[AZ_EVENT_PORT_LIST_SZ];

az_event_bus_t  az_event_bus_default = {
  .state = AZ_ST_IDLE,
  .port_count = 0,
  .port_list_sz = AZ_EVENT_PORT_LIST_SZ,
  .port_list = az_event_port_list_default,
};
az_event_bus_t  *az_event_bus_pDefault = &az_event_bus_default;

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

/*
 * define event bus managing functions 
 */
az_r_t  az_event_bus_init()
{
  az_r_t r = AZ_ERR_L(STATE, 0);
  az_event_bus_t  *ebus = AZ_EVENT_BUS();

  if (!(ebus->state & (AZ_ST_INIT|AZ_ST_TINI|AZ_ST_NORM))) {
    ebus->state |= AZ_ST_INIT;
    r = AZ_SUCCESS;

    int j = 0;
    for (; j < ebus->port_list_sz; j++) {
      ebus->port_list[j] = NULL;
    }
    ebus->port_count = 0;
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
az_r_t  az_event_bus_start() 
{
  az_r_t r = AZ_ERR_L(STATE, 0);
  az_event_bus_t  *ebus = AZ_EVENT_BUS();

  if (ebus->state == AZ_ST_INIT) { 
    ebus->state = AZ_ST_NORM;
    r = AZ_SUCCESS;
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
az_r_t  az_event_bus_stop() 
{
  az_r_t r = AZ_ERR(STATE);
  az_event_bus_t  *ebus = AZ_EVENT_BUS();
  if (ebus->state == AZ_ST_NORM) {
    r = AZ_SUCCESS;
    ebus->state = AZ_ST_TINI; 
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
az_r_t  az_event_bus_denit()
{
  az_r_t r = AZ_ERR(STATE);
  az_event_bus_t  *ebus = AZ_EVENT_BUS();

  if (ebus->state & AZ_ST_TINI) {
    r = AZ_SUCCESS;

    ebus->port_count = 0;
    int j = 0;
    for (; j < ebus->port_list_sz; j++) {
      ebus->port_list[j] = NULL;
    }
    ebus->state = AZ_ST_IDLE;
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
az_event_port_t az_event_port_get(az_event_port_no_t portno)
{
  az_event_bus_t  *ebus = AZ_EVENT_BUS();
  az_assert(portno < ebus->port_list_sz);
  az_event_port_t port = ebus->port_list[portno];
  if (port) {
    az_assert(port->no == portno);
  }
  return port; 
}
/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
az_r_t az_event_port_search(az_event_id_t event_id, az_event_id_t event_mask, int type, az_event_port_t *pPort) 
{
  az_r_t r = AZ_FAIL;
  az_event_bus_t  *ebus = AZ_EVENT_BUS();

  if (ebus->state & AZ_ST_NORM) {
    r = AZ_ERR(NOT_FOUND);
    az_event_port_t port = NULL; 
    int j = 0, k = ebus->port_count;
    for (; k > 0 && j < ebus->port_list_sz; j++) {
      port = ebus->port_list[j];
      if (NULL == port) continue;
      k--;
      if (AZ_EVENT_PORT_TYPE(port) != type) continue;
      if ((port->event_id & port->event_mask) != (event_id & event_mask)) { 
        continue;
      }
      az_assert(port->no == j); 
      r = AZ_SUCCESS;
      *pPort = port;
      break;
    }
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
az_r_t az_event_port_create(az_event_id_t event_id, az_event_id_t event_mask, int type, az_event_port_t *pPort)
{
  az_event_port_t port = NULL; 
  //az_uint32_t portno;
  //az_event_bus_t  *ebus = AZ_EVENT_BUS();
  az_r_t  r = AZ_FAIL;
  int mask_size = AZ_EVENT_MASK_SIZE(event_mask);
  az_assert(mask_size > 0);

  do {
    r = az_event_port_search(event_id, event_mask, type, &port); 
    if (AZ_SUCCESS == r) {
      *pPort = port;
      r = AZ_ERR(AGAIN);
      break; 
    }
    port = *pPort;
    if (NULL != port) {
      if (port->flags.u32 != 0 && port->no != AZ_EVENT_PORT_NO_NOT_ASSIGNED) {
        r = AZ_ERR(STATE);
        break;
      }
    } else {
      port = (az_event_port_t)az_malloc(sizeof(az_event_port_entity_t));
      if (NULL == port) {
        r = AZ_ERR(MALLOC);
        break;
      }
      *pPort = port;
      port->flags.u32 = 0;
      az_refcount_init_dynamic(&port->refCount);
    }
    az_refcount_atomic_inc(&port->refCount);
    r = AZ_SUCCESS;
    port->no = AZ_EVENT_PORT_NO_NOT_ASSIGNED;
    port->flags.st.type = type;
    port->flags.st.state = AZ_EVENT_PORT_STATE_INIT;
    port->flags.st.mask_size = mask_size; 
    port->event_id = event_id;
    port->event_mask = event_mask;
    az_link_initList(&(port->receiver_list));

    port->lock = &port->_lock;
    az_event_port_lock_init(port);

    az_link_initList(&port->peer_list);
    //port->receiver.sys_port = NULL;
    port->oprs.In = az_event_port_in_default;
    port->oprs.Out = az_event_port_out_default;

    port->priv = NULL;

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
az_r_t az_event_port_attach(az_event_port_t port) 
{
  az_assert(NULL != port);
  az_event_bus_t  *ebus = AZ_EVENT_BUS();
  az_event_port_t peer;
  az_event_id_t   event_mask;
  az_r_t  r = AZ_SUCCESS;
  int j, k;

  do {
    if (NULL == port) {
      r = AZ_ERR(ENTITY_NULL);
      break;
    }
    if (port->flags.st.state != AZ_EVENT_PORT_STATE_INIT) {
      r = AZ_ERR(STATE);
      break;
    }
    az_event_port_t *list = ebus->port_list;
    for (j = 0;j < ebus->port_list_sz; j++, list++) {
      if (NULL == az_atomic_compare_and_swap_ptr(list, NULL, port)) { 
        port->no = j;
        port->flags.st.state = AZ_EVENT_PORT_STATE_ACTV;
        az_atomic_inc32(&(ebus->port_count));
        break;
      }
    }
    if (j == ebus->port_list_sz) {
      r = AZ_ERR(FULL);
      break;
    }
    if (AZ_EVENT_PORT_IS_TX(port)) {
      for (j = 0, k = ebus->port_count; k > 0 && j < ebus->port_list_sz; j++) {
        peer = ebus->port_list[j];
        if (NULL == peer) continue;
        k--;
        if (!AZ_EVENT_PORT_IS_RX(peer)) continue;
        if (port == peer) continue;
        if (port->flags.st.mask_size > peer->flags.st.mask_size) {
          event_mask = port->event_mask;
        } else {
          event_mask = peer->event_mask;
        }
        if ((port->event_id & event_mask) != (peer->event_id & event_mask)) { 
          continue;
        }
        az_event_port_link_t *link = az_event_port_link_create(peer);
        if (NULL != link) {
          az_event_port_lock_write(port);
          az_link_putLast(&port->peer_list, (az_link_t *)link);
          az_event_port_unlock_write(port);
        } else {
          r = AZ_ERR(NO_RESOURCE);
          break;
        }
      }
      if (AZ_EVENT_PORT_IS_TX_SHARED(port)) {
        port->oprs.In = az_event_port_in_shared;
      } else {
        port->oprs.In = az_event_port_in_exclusive;
      }
    }
    if (AZ_EVENT_PORT_IS_RX(port)) {
      for (j = 0, k = ebus->port_count; k > 0 && j < ebus->port_list_sz; j++) {
        peer = ebus->port_list[j];
        if (NULL == peer) continue;
        k--;
        if (!AZ_EVENT_PORT_IS_TX(peer)) continue;
        //if (port == peer) continue;
        if (port->flags.st.mask_size > peer->flags.st.mask_size) {
          event_mask = port->event_mask;
        } else {
          event_mask = peer->event_mask;
        }
        if ((port->event_id & event_mask) != (peer->event_id & event_mask)) { 
          continue;
        }
        az_link_t *link;
        az_event_port_link_t *port_link = NULL; 
        az_event_port_lock_read(peer);
        link = peer->peer_list.next;
        while (1) {
          if (AZ_EQUAL_PTR(link,&peer->peer_list)) {
            break;
          }
          if (((az_event_port_link_t *)link)->port == port) {
            port_link = (az_event_port_link_t *)link;
            break;
          }
          link = link->next;
        }; 
        az_event_port_unlock_read(peer);
        if (NULL != port_link) continue;
        port_link = az_event_port_link_create(port);
        if (NULL != port_link) {
          az_event_port_lock_write(peer);
          az_link_putLast(&peer->peer_list, (az_link_t *)port_link);
          az_event_port_unlock_write(peer);
        } else {
          r = AZ_ERR(NO_RESOURCE);
          break;
        }
      }
      if (AZ_EVENT_PORT_IS_RX_SHARED(port)) {
        port->oprs.Out = az_event_port_out_shared;
      } else {
        port->oprs.Out = az_event_port_out_exclusive;
      }
    } else if (!AZ_EVENT_PORT_IS_TX(port)) {
      r = AZ_ERR(NOT_SUPPORT);
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
az_r_t az_event_port_detach(az_event_port_t port) 
{
  az_event_bus_t  *ebus = AZ_EVENT_BUS();
  az_event_port_t peer;
  az_event_id_t   event_mask;
  az_r_t  r = AZ_FAIL;
  int j, k;

  do {
    if (NULL == port) {
      r = AZ_ERR(ENTITY_NULL);
      break;
    }
    if (port->flags.st.state != AZ_EVENT_PORT_STATE_ACTV) {
      r = AZ_ERR(STATE);
      break;
    }
    az_event_port_t *list = ebus->port_list;
    for (j = 0;j < ebus->port_list_sz; j++, list++) {
      if (port == az_atomic_compare_and_swap_ptr(list, port, NULL)) { 
        az_atomic_dec32(&(ebus->port_count));
        port->flags.st.state = AZ_EVENT_PORT_STATE_TINI;
        port->no = AZ_EVENT_PORT_NO_INVALID;
        port->oprs.In = az_event_port_in_default;
        port->oprs.Out = az_event_port_out_default;
        r = AZ_SUCCESS;
        break;
      }
    }
    if (r != AZ_SUCCESS) {
      break;
    }
    if (AZ_EVENT_PORT_IS_TX(port)) {
      for (j = 0, k = ebus->port_count; k > 0 && j < ebus->port_list_sz; j++) {
        peer = ebus->port_list[j];
        if (NULL == peer) continue;
        k--;
        if (!AZ_EVENT_PORT_IS_RX(peer)) continue;
        if (port == peer) continue;
        if (port->flags.st.mask_size > peer->flags.st.mask_size) {
          event_mask = port->event_mask;
        } else {
          event_mask = peer->event_mask;
        }
        if ((port->event_id & event_mask) != (peer->event_id & event_mask)) { 
          continue;
        }
        az_event_port_link_t *link;
        az_event_port_lock_write(port);
        do {
          link = (az_event_port_link_t *)az_link_getFirst(&port->peer_list);
          if (NULL == link) break;
          az_event_port_link_delete(link);
        } while (1);
        az_event_port_unlock_write(port);
      }
      port->oprs.In = NULL; 
    }
    if (AZ_EVENT_PORT_IS_RX(port)) {
      for (j = 0, k = ebus->port_count; k > 0 && j < ebus->port_list_sz; j++) {
        peer = ebus->port_list[j];
        if (NULL == peer) continue;
        k--;
        if (!AZ_EVENT_PORT_IS_TX(peer)) continue;
        if (port == peer) continue;
        if (port->flags.st.mask_size > peer->flags.st.mask_size) {
          event_mask = port->event_mask;
        } else {
          event_mask = peer->event_mask;
        }
        if ((port->event_id & event_mask) != (peer->event_id & event_mask)) { 
          continue;
        }
        az_link_t *link, *prev;
        az_event_port_lock_write(peer);
        prev = (az_link_t *)&peer->peer_list;
        link = peer->peer_list.next;
        while (1) {
          if (AZ_EQUAL_PTR(link, &peer->peer_list)) {
            break;
          }
          if (((az_event_port_link_t *)link)->port == port) {
            az_link_remove(&peer->peer_list, prev, link);
            az_event_port_link_delete((az_event_port_link_t*)link);
            break;
          }
          prev = link;
          link = link->next;
        }; 
        az_event_port_unlock_write(peer);
      }
      port->oprs.Out = NULL; 
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
az_r_t az_event_port_delete(az_event_port_t port) 
{
  //az_event_bus_t  *ebus = AZ_EVENT_BUS();
  az_r_t  r = AZ_ERR(NOT_FOUND);
  do {
    if (NULL == port) {
      r = AZ_ERR(ENTITY_NULL);
      break;
    }
    if (port->flags.st.state != AZ_EVENT_PORT_STATE_TINI) {
      r = AZ_ERR(STATE);
      break;
    }
    r = AZ_SUCCESS;
    az_event_port_lock_deinit(port);
    port->lock = NULL;

    az_refcount_atomic_dec(&port->refCount);
    if (AZ_REFCOUNT_IS_ZERO(&port->refCount)) {
      az_free(port);
    } else {
      port->flags.st.state = AZ_EVENT_PORT_STATE_NULL;
    }
  } while (0);

  return r;
}

struct {
  az_link_list_t  az_event_port_link_list;
  az_event_port_link_t  az_event_port_link_pool[AZ_EVENT_PORT_LINK_POOL_SZ];
  az_sys_lw_lock_t  lock; 
} az_event_port_link_store;

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
void az_event_port_link_pool_init()
{
  az_link_list_t  *list = &az_event_port_link_store.az_event_port_link_list;
  int j;
  az_event_port_link_t *link = az_event_port_link_store.az_event_port_link_pool; 
  az_link_initList(list);
  az_sys_lw_lock_init(&az_event_port_link_store.lock); 
  for (j = 0; j < array_size(az_event_port_link_store.az_event_port_link_pool); j++, link++) {
    az_link_init(&link->link);
    link->port = NULL;
    az_refcount_init_static(&link->refCount);
    az_link_putLast(list, (az_link_t *)link);
  }
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
void az_event_port_link_pool_deinit()
{
  //int j;
  az_link_list_t  *list = &az_event_port_link_store.az_event_port_link_list;
  az_event_port_link_t *link; 

  do {
    link = (az_event_port_link_t *) az_link_getFirst(list);
    if (NULL == link) break;
    if (AZ_REFCOUNT_IS_ZERO(&link->refCount)) {
      az_free(link);
    } else {
      link->port = NULL;
    }
  } while (0);
  az_sys_lw_lock_init(&az_event_port_link_store.lock); 
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
az_event_port_link_t *az_event_port_link_create(az_event_port_t port)
{
  az_link_list_t  *list = &az_event_port_link_store.az_event_port_link_list;
  az_event_port_link_t *link; 
  az_sys_lw_lock(&az_event_port_link_store.lock);
  link = (az_event_port_link_t *)az_link_getFirst(list); 
  az_sys_lw_unlock(&az_event_port_link_store.lock);
  do {
    if (NULL == link) {
      link = az_malloc(sizeof(az_event_port_link_t));
      if (NULL == link) break;
      az_refcount_init_dynamic(&link->refCount);
      az_link_init(&link->link);
    }
    az_refcount_atomic_inc(&link->refCount);
    link->port = port;
  } while (0);

  return link;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
void az_event_port_link_delete(az_event_port_link_t *link)
{
  az_link_list_t  *list = &az_event_port_link_store.az_event_port_link_list;
  do {
    if (NULL == link) break;
    az_refcount_atomic_dec(&link->refCount);
    link->port = NULL;
    az_sys_lw_lock(&az_event_port_link_store.lock);
    az_link_putLast(list, (az_link_t *)link); 
    az_sys_lw_unlock(&az_event_port_link_store.lock);
  } while (0);
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
az_r_t az_event_port_in_default(az_event_port_t port, az_event_t evt) 
{
  return AZ_ERR(NOT_ACTV);
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
az_r_t az_event_port_out_default(az_event_port_t port, az_event_t evt) 
{
  return AZ_ERR(NOT_ACTV);
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
az_r_t az_event_port_in_exclusive(az_event_port_t port, az_event_t evt) 
{
  az_r_t  r = AZ_SUCCESS;
  az_assert(NULL != port && NULL != evt);
  az_assert(AZ_EVENT_PORT_IS_TX(port));
  az_link_list_t *list = &port->peer_list;
  az_event_port_link_t *port_link = (az_event_port_link_t *)(list->next);
  do {
    if ((az_ref_t)port_link == (az_ref_t)list) {
      break;
    }
    az_assert(AZ_EVENT_PORT_IS_RX(port_link->port));
    port_link->port->oprs.Out(port_link->port, evt);
    port_link = (az_event_port_link_t *)(port_link->link.next);
  } while (1);

  return r; 
}
/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
az_r_t az_event_port_in_shared(az_event_port_t port, az_event_t evt) 
{
  az_r_t  r = AZ_SUCCESS;
  az_assert(NULL != port && NULL != evt);
  az_assert(AZ_EVENT_PORT_IS_TX(port));
  az_link_list_t *list = &port->peer_list;
  az_event_port_link_t *port_link;
  az_event_port_lock_read(port);
  port_link = (az_event_port_link_t *)(list->next);
  do {
    if ((az_ref_t)port_link == (az_ref_t)list) {
      az_event_port_unlock_read(port);
      break;
    }
    az_assert(AZ_EVENT_PORT_IS_RX(port_link->port));
    az_event_port_unlock_read(port);

    port_link->port->oprs.Out(port_link->port, evt);

    az_event_port_lock_read(port);
    port_link = (az_event_port_link_t *)(port_link->link.next);
  } while (1);

  return r; 
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
az_r_t az_event_port_out_exclusive(az_event_port_t port, az_event_t evt) 
{
  az_r_t  r = AZ_SUCCESS;
  az_assert(NULL != port && NULL != evt);
  az_assert(AZ_EVENT_PORT_IS_RX(port));
  az_link_list_t *list = &port->receiver_list;
  az_event_receiver_t *receiver = (az_event_receiver_t *)(list->next);
  do {
    if ((az_ref_t)receiver == (az_ref_t)list) {
      break;
    }
    receiver->oprs.In(receiver, evt);

    receiver = (az_event_receiver_t *)(receiver->link.next);
  } while (1);

  return r; 
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
az_r_t az_event_port_out_shared(az_event_port_t port, az_event_t evt) 
{
  az_r_t  r = AZ_SUCCESS;
  az_assert(NULL != port && NULL != evt);
  az_assert(AZ_EVENT_PORT_IS_RX(port));
  az_link_list_t *list = &port->receiver_list;
  az_event_receiver_t *receiver;
  az_event_port_lock_read(port);
  receiver = (az_event_receiver_t *)(list->next);
  do {
    if ((az_ref_t)receiver == (az_ref_t)list) {
      az_event_port_unlock_read(port);
      break;
    }
    az_event_port_unlock_read(port);

    receiver->oprs.In(receiver, evt);

    az_event_port_lock_read(port);
    receiver = (az_event_receiver_t *)(receiver->link.next);
  } while (1);

  return r; 
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
az_r_t az_event_port_add_receiver(az_event_port_t port, az_event_receiver_t *rcvr) 
{
  az_r_t  r = AZ_SUCCESS;
  az_assert(NULL != port && NULL != rcvr);
  az_link_list_t *list = &port->receiver_list;
  az_event_port_lock_write(port);
  az_link_putLast(list, &rcvr->link);
  az_event_port_unlock_write(port);

  return r; 
}
/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
az_r_t az_event_port_del_receiver(az_event_port_t port, az_event_receiver_t *rcvr){
  az_r_t  r = AZ_FAIL;
  az_assert(NULL != port && NULL != rcvr);
  az_link_list_t *list = &port->receiver_list;
  az_link_t     *prev = (az_link_t *)list;
  az_link_t     *link = list->next; 
  az_event_port_lock_write(port);
  do {
    if (AZ_EQUAL_PTR(list, link)) break;
    if (link == &rcvr->link) {
      az_link_remove(list, prev, link);
      r = AZ_SUCCESS;
      break;
    }
    prev = link;
    link = link->next;
  } while (1);
  az_event_port_unlock_write(port);

  return r; 
}

#if 0
#endif

/* === end of AZ_EVENT_BUS_C === */
