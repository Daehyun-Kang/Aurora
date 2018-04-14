/**
 * @file   az_event.c
 * @brief  
 * @date   02/07/16
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
 *   - 2016-07-02 fware initial create
 */

/* include header files */
#include "az_core.h"
#include "az_printf.h"
#include "az_thread.h"

/* declare global variables */
// define az_event_store
az_array_t        az_event_descr_pool;
az_event_descr_t  *az_event_descr_list[AZ_EVENT_DESCR_POOL_SZ];

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
az_r_t  az_event_store_init()
{
  az_memset(&az_event_descr_pool, 0, sizeof(az_event_descr_pool));
  az_memset(&az_event_descr_list, 0, sizeof(az_event_descr_list));

  az_array_init(&az_event_descr_pool, AZ_EVENT_DESCR_POOL_SZ, 
      (az_array_element_ref_t)az_event_descr_list, NULL, 
      sizeof(az_event_descr_t *));

  return AZ_SUCCESS;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
az_r_t  az_event_store_deinit()
{
  az_array_deinit(&az_event_descr_pool);

  return AZ_SUCCESS;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
az_event_descr_t *az_event_descr_get(az_event_descr_id_t index)
{
  az_event_descr_t  *descr = (az_event_descr_t *)az_array_get_element_ptr_at(&az_event_descr_pool, index);
  return descr;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
az_event_descr_t  *az_event_descr_find(az_event_id_t event_id)
{
  az_event_descr_t  *descr;
  int j, k;
  int found = 0;

  do {
    for (j = 0, k = az_event_descr_pool.count; j < az_event_descr_pool.size && k > 0;j++) {
      descr = (az_event_descr_t *)az_array_get_element_ptr_at(&az_event_descr_pool, j);
      if (NULL == descr) continue;
      k--;
      if (0) az_sys_printf("%d: "AZ_FMT_EVENT_ID(1)" "AZ_FMT_EVENT_ID(1)"\n",j, descr->event_id, event_id); 
      if (descr->event_id == event_id) {
        found = 1;
        break;
      }
    }
  } while (0);
  if (!found) descr = NULL;

  return descr;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
az_r_t  az_event_descr_create(az_event_descr_t **pDescr, az_event_id_t event_id,
    az_event_node_t event_node, int cache_size)
{
  az_r_t r = AZ_SUCCESS;
  az_assert(NULL != pDescr);
  az_event_descr_t  *descr = *pDescr;

  do {
    if (NULL == descr) {
      descr = az_malloc(sizeof(*descr) + sizeof(az_ref_t)*cache_size);
      if (NULL == descr) {
        r = AZ_ERR(MALLOC);
        break;
      }
      *pDescr = descr;
      az_refcount_init_dynamic(&descr->refCount);
      az_ring_init(&descr->event_cache, AZ_RING_TYPE_DSREF, cache_size, descr+1);
      //az_ring_toStr(az_thread_prtbuf, sizeof(az_thread_prtbuf), &descr->event_cache);
      //az_sys_printf("event_cache: %s\n", az_thread_prtbuf);
    }
    az_refcount_atomic_inc(&descr->refCount);

    descr->flags = 0;
    descr->event_id = event_id;
    descr->event_node = event_node;
    descr->port = NULL;
    descr->index = az_array_addptr(&az_event_descr_pool, descr);
    if (descr->index < 0) {
      az_refcount_atomic_dec(&descr->refCount);
      if (AZ_REFCOUNT_IS_ZERO(&descr->refCount)) {
        az_free(descr);
        *pDescr = descr = NULL;
      }
      r = AZ_ERR(NO_SPACE);
      break;
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
az_r_t  az_event_descr_delete(az_event_descr_t *descr)
{
  az_r_t r = AZ_SUCCESS;

  do {
    if (az_refcount_atomic_dec(&descr->refCount) > 0) {
      r = AZ_ERR(BUSY);
      break;
    }
    r = az_array_del(&az_event_descr_pool, descr->index); 
    if (AZ_REFCOUNT_IS_ZERO(&descr->refCount)) {
      az_free(descr);
    } else {
      descr->event_id = AZ_EVENT_ID_INVALID;
      descr->port = NULL;
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
az_event_t az_event_alloc(az_event_descr_t *descr, az_uint32_t buffer_size,
    az_ref_t buffer_data)
{
  az_assert(NULL != descr);
  az_event_t  evt = NULL;
  
  do {
      //az_ring_toStr(az_thread_prtbuf, sizeof(az_thread_prtbuf), &descr->event_cache);
      //az_sys_printf("event_cache: %s\n", az_thread_prtbuf);
    az_ring_pop(&descr->event_cache, &evt);
    if (NULL != evt) { 
      if (evt->buffer.size >= buffer_size) {
        break;
      } else {
        if (AZ_REFCOUNT_IS_ZERO(&evt->refCount)) {
          az_event_free(evt, descr);
          evt = NULL;
        }
      }
    }
    if (NULL == buffer_data) {
      evt = (az_event_t)az_malloc(sizeof(*evt) + buffer_size); 
      if (NULL == evt) {
        break; 
      } 
      evt->flags = descr->flags;
      AZ_EVENT_BUFFER_SIZE(evt) = buffer_size; 
      if (buffer_size > 0) {
        AZ_EVENT_BUFFER_DATA(evt) = evt + 1; 
      } else {
        AZ_EVENT_BUFFER_DATA(evt) = NULL; 
      }
    } else {
      az_assert(buffer_size != 0);
      evt = (az_event_t)az_malloc(sizeof(*evt));
      if (NULL == evt) {
        break;
      }
      evt->flags = descr->flags;
      AZ_EVENT_BUFFER_SIZE(evt) = buffer_size; 
      AZ_EVENT_BUFFER_DATA(evt) = buffer_data; 
    }
    az_refcount_init_dynamic(&evt->refCount);
  } while (0);

  return evt;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
az_r_t az_event_create(az_event_t *pEvt, az_event_descr_id_t descr_id,
    az_uint32_t buffer_size, az_ref_t buffer_data)
{
  az_r_t r = AZ_SUCCESS;
  az_assert(NULL != pEvt);
  az_event_t evt = *pEvt;
  az_event_descr_t *descr = az_event_descr_get(descr_id);
  az_assert(NULL != descr);

  do {
    if (NULL == evt) {
      *pEvt = evt = az_event_alloc(descr, buffer_size, buffer_data);
      if (NULL == evt) {
        r = AZ_ERR(ALLOC);
        break;
      }
    } else if (NULL == buffer_data) {
      // this is the case static event with dynamic event data buffer
      if (buffer_size > 0) {
        buffer_data = az_malloc(buffer_size);
        if (NULL == buffer_data) {
          r = AZ_ERR(NO_RESOURCE);
          break;
        } else {
          evt->flags |= AZ_EVENT_FLAGS_BUFFER_DYNAMIC;
          AZ_EVENT_BUFFER_SIZE(evt) = buffer_size;
          AZ_EVENT_BUFFER_DATA(evt) = buffer_data;
        }
      } else {
          evt->flags &= ~AZ_EVENT_FLAGS_BUFFER_DYNAMIC;
          AZ_EVENT_BUFFER_SIZE(evt) = 0;
          AZ_EVENT_BUFFER_DATA(evt) = NULL;
      }
    } 
    evt->descr_id = descr_id;
    evt->event_node = descr->event_node;
    evt->event_id = descr->event_id;
    az_refcount_atomic_inc(&evt->refCount);
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
void az_event_free(az_event_t evt, az_event_descr_t *descr)
{
  az_assert(NULL != evt && NULL != descr);
  az_assert(evt->descr_id == descr->index);
  
  do {
    if (az_ring_push(&descr->event_cache, evt) >= 0) {
      break;
    }
    if (NULL == AZ_EVENT_BUFFER_DATA(evt)) {
      az_free(evt);
      break;
    }
    if (evt->flags & AZ_EVENT_FLAGS_BUFFER_DYNAMIC) { 
      az_free(AZ_EVENT_BUFFER_DATA(evt));
    }
    az_free(evt);
  } while (0);
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
az_r_t az_event_delete(az_event_t evt)
{
  az_r_t r = AZ_SUCCESS;
  az_assert(NULL != evt);
  az_event_descr_t *descr = az_event_descr_get(AZ_EVENT_DESCR_ID(evt));
  az_assert(NULL != descr);

  do {
    az_refcount_atomic_dec(&evt->refCount);
    if (AZ_REFCOUNT_IS_ZERO(&evt->refCount)) {
      az_event_free(evt, descr);
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
az_r_t az_event_send(az_event_port_t port, az_event_t evt)
{
  az_r_t  r = AZ_SUCCESS;
  az_assert(NULL != port);
  az_assert(NULL != evt);

  r = port->oprs.In(port, evt);

  return r;
}


/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
az_r_t az_event_recv(az_event_receiver_t *rcvr, az_event_t *pEvt)
{
  az_r_t  r = AZ_SUCCESS;
  az_assert(NULL != rcvr);
  az_assert(NULL != pEvt);

  r = rcvr->oprs.Out(rcvr, pEvt);

  return r;
}

az_r_t az_event_receiver_init(az_event_receiver_t *rcvr, az_event_t event, az_event_mux_t *event_mux, az_ring_t *event_q)
{
  az_assert(NULL != rcvr);
  az_r_t r = AZ_SUCCESS;

  az_link_init(&rcvr->link);
  rcvr->sys_port = az_sys_xu_eport();
  rcvr->signo = AZ_THREAD_BEAM_EVTBUS;
  rcvr->event = event;
  rcvr->event_mux = event_mux;
  rcvr->event_q = event_q;
  rcvr->oprs.In = az_event_receiver_in_default;
  rcvr->oprs.Out = az_event_receiver_out_default;

  return r;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
az_r_t az_event_receiver_in_default(az_event_receiver_t *rcvr, az_event_t evt)
{
  az_r_t  r = AZ_SUCCESS;
  az_assert(NULL != rcvr);
  az_assert(NULL != evt);

  do {
    if (AZ_EVENT_IS_TYPE_EDGE(evt)) {
      r = az_ring_push(rcvr->event_q, &evt);
      //az_ring_toStr(az_thread_prtbuf, sizeof(az_thread_prtbuf), rcvr->event_q);
      //az_rprintf(r, "rcvr %p ring: %s\n", rcvr, az_thread_prtbuf);
      if (r < 0) {
        rcvr->stats.err.qfull++;
        break;
      }
      az_refcount_atomic_inc(&evt->refCount);
      break;
    } 
    if (rcvr->event) {
      if (evt->descr_id == rcvr->event->descr_id) {
        rcvr->event->level_count++;
        break;
      }
    } 
    r = az_event_mux_push(rcvr->event_mux, evt);
    if (r < 0) {
      rcvr->stats.err.empty++;
      break;
    }
  } while (0);

  if (r >= AZ_SUCCESS) {
    az_assert(NULL != rcvr->sys_port); 
    r = az_sys_ep_send(rcvr->sys_port, rcvr->signo);
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
az_r_t az_event_receiver_in_single(az_event_receiver_t *rcvr, az_event_t evt)
{
  az_r_t  r = AZ_SUCCESS;
  az_assert(NULL != rcvr);
  az_assert(NULL != evt);

  do {
    az_assert(AZ_EVENT_IS_TYPE_LEVEL(evt));
    az_assert(NULL != rcvr->event);
    az_assert(evt->descr_id == rcvr->event->descr_id); 
    rcvr->event->level_count++;

    az_assert(NULL != rcvr->sys_port); 
    r = az_sys_ep_send(rcvr->sys_port, rcvr->signo);
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
az_r_t az_event_receiver_in_edge(az_event_receiver_t *rcvr, az_event_t evt)
{
  az_r_t  r = AZ_SUCCESS;
  az_assert(NULL != rcvr);
  az_assert(NULL != evt);

  do {
    az_assert(AZ_EVENT_IS_TYPE_EDGE(evt));
    r = az_ring_push(rcvr->event_q, evt);
    if (r < 0) {
      rcvr->stats.err.qfull++;
      break;
    }
    az_refcount_atomic_inc(&evt->refCount);
    az_assert(NULL != rcvr->sys_port); 
    r = az_sys_ep_send(rcvr->sys_port, rcvr->signo);
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
az_r_t az_event_receiver_in_level(az_event_receiver_t *rcvr, az_event_t evt)
{
  az_r_t  r = AZ_SUCCESS;
  az_assert(NULL != rcvr);
  az_assert(NULL != evt);

  do {
    az_assert(AZ_EVENT_IS_TYPE_LEVEL(evt)); 
    if (rcvr->event) {
      if (evt->descr_id == rcvr->event->descr_id) {
        rcvr->event->level_count++;
        break;
      }
    } 
    r = az_event_mux_push(rcvr->event_mux, evt);
    if (r < 0) {
      rcvr->stats.err.empty++;
      break;
    }
  } while (0);

  if (r >= AZ_SUCCESS) {
    az_assert(NULL != rcvr->sys_port); 
    r = az_sys_ep_send(rcvr->sys_port, rcvr->signo);
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
az_r_t az_event_receiver_out_default(az_event_receiver_t *rcvr, az_event_t *pEvt)
{
  az_r_t  r = AZ_SUCCESS;
  az_assert(NULL != rcvr);
  az_assert(NULL != pEvt);

  do {
    if (rcvr->event) {
      if (rcvr->event->level_count > 0) {
        *pEvt = rcvr->event;
        rcvr->event--;
        break;
      }
    }
    if (rcvr->event_mux) {
      r = az_event_mux_pop(rcvr->event_mux, pEvt);
      if (r >= 0) {
        break;
      }
    }
    r = az_ring_pop(rcvr->event_q, pEvt);
    //az_ring_toStr(az_thread_prtbuf, sizeof(az_thread_prtbuf), rcvr->event_q);
    //az_rprintf(r, "rcvr %p ring: %s\n", rcvr, az_thread_prtbuf);
    if (r < 0) {
      r = AZ_ERR(NO_DATA);
      break;
    }
    az_event_t evt = *pEvt;
    az_refcount_atomic_dec(&evt->refCount);
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
az_r_t az_event_receiver_out_single(az_event_receiver_t *rcvr, az_event_t *pEvt)
{
  az_r_t  r = AZ_SUCCESS;
  az_assert(NULL != rcvr);
  az_assert(NULL != pEvt);

  do {
    if (rcvr->event) {
      if (rcvr->event->level_count > 0) {
        *pEvt = rcvr->event;
        rcvr->event->level_count--;
        break;
      } else {
        r = AZ_ERR(EMPTY);
      }
    } else {
      r = AZ_ERR(INVALID);
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
az_r_t az_event_receiver_out_level(az_event_receiver_t *rcvr, az_event_t *pEvt)
{
  az_r_t  r = AZ_SUCCESS;
  az_assert(NULL != rcvr);
  az_assert(NULL != pEvt);

  do {
    if (rcvr->event) {
      if (rcvr->event->level_count > 0) {
        *pEvt = rcvr->event;
        rcvr->event->level_count--;
        break;
      }
    }
    if (rcvr->event_mux) {
      r = az_event_mux_pop(rcvr->event_mux, pEvt);
      if (r >= 0) break;
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
az_r_t az_event_receiver_out_edge(az_event_receiver_t *rcvr, az_event_t *pEvt)
{
  az_r_t  r = AZ_SUCCESS;
  az_assert(NULL != rcvr);
  az_assert(NULL != pEvt);

  do {
    if (az_ring_pop(rcvr->event_q, pEvt) < 0) {
      r = AZ_ERR(NO_DATA);
      break;
    }
    az_event_t evt = *pEvt;
    az_refcount_atomic_dec(&evt->refCount);
  } while (0);

  return r;
}

int az_event_toStr(char *bp, int blen, az_event_t evt)
{
  int tlen = 0;
  _AZ_SNPRINTF(tlen, bp, blen, "descr_id=%d flags=%x refCount=%d:%d node=%d id=%x\n",
      evt->descr_id,
      evt->flags,
      evt->refCount.is_static,
      evt->refCount.count,
      evt->event_node,
      evt->event_id);
  return tlen;
}
#if 0
#endif


/* === end of AZ_EVENT_C === */
