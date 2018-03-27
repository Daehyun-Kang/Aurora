/**
 * @file   az_iou_msgq.c
 * @brief  
 * @date   02/08/16
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
 *   - 2016-08-02 fware initial create
 */

/* include header files */
#include "az_iou_msgq.h"

/* declare global variables */

/* declare static variables */

static struct az_iou_oprs az_iou_msgq_oprs = {
  .Create = az_iou_msgq_create,
  .Delete = az_iou_msgq_delete,
  .Open = az_iou_msgq_open,
  .Close = az_iou_msgq_close,
  .Setup = az_iou_msgq_setup,
  .Release = az_iou_msgq_release,
  .Control = az_iou_msgq_control,
  .Recv = az_iou_msgq_recv,
  .Send = az_iou_msgq_send,
};

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
az_r_t  az_iou_msgq_create(az_iou_t *iou, az_iou_config_t *cfg, az_iou_id_t *pId)
{
  az_assert(NULL != pId);
  az_assert(NULL != cfg);
  az_r_t  r = AZ_FAIL;

  do {
    if (NULL == iou) {
      if (NULL == cfg) {
        r = AZ_ERR(ARG_NULL);
        break;
      } else {
        az_iou_t *iou_exist;
        iou_exist = az_iou_find(cfg->name);
        if (NULL != iou_exist) {
          *pId = iou_exist->ion.id;
          r = AZ_ERR(AGAIN);
          break;
        }
      }
      iou = az_malloc(sizeof(az_iou_t));
      if (NULL == iou) {
        r = AZ_ERR(MALLOC);
        break;
      }
      az_ion_invalidate(&iou->ion, 0);
      iou->config = *cfg;
    } else {
      if (iou->status.state & AZ_IOU_STATE_ASSIGNED) {
        r = AZ_ERR(STATE);
        break;
      }
      if (NULL == cfg) {
        cfg = &iou->config;
      }
      if (cfg != &iou->config) {
        iou->config = *cfg;
      }
      az_iou_t *iou_exist;
      iou_exist = az_iou_find(cfg->name);
      if (NULL != iou_exist) {
        *pId = iou_exist->ion.id;
        r = AZ_ERR(AGAIN);
        break;
      }
    }
    az_refcount_atomic_inc(&iou->ion.refCount);
    az_memset(&(iou->status), 0, sizeof(iou->status));
    iou->status.state = AZ_IOU_STATE_ASSIGNED;
    iou->oprs = &az_iou_msgq_oprs;
    iou->priv = NULL;

    az_assert(iou->ion.id == AZ_SYS_IO_INVALID);
    r = az_ion_register(&iou->ion, AZ_ION_TYPE_IOU|AZ_ION_FLAG_AUTOALLOC);
    if (r < 0) {
      if (az_refcount_atomic_dec(&iou->ion.refCount) == 0) {
        az_free(iou);
        break;
      }
    }
    *pId = iou->ion.id;

    #ifdef  CONFIG_AZ_IOU_USE_EVENT_PORT
    if (iou->config.flags & AZ_IOU_FLAGS_TX) {
      if (iou->config.tx.event_id) {
        r = az_event_descr_create(&iou->tx.event_descr,
                iou->config.tx.event_id, 
                iou->config.tx.event_mask, 
                1);
        if (r < 0) {
          iou->tx.event_descr = NULL;
        } else {
          r = az_event_port_create(
                iou->config.tx.event_id, 
                iou->config.tx.event_mask, 
                AZ_EVENT_PORT_TYPE_TX,
                &iou->tx.event_port);
          if (AZ_SUCCESS == r) {
            r = az_event_port_attach(iou->tx.event_port);
          } else {
            iou->tx.event_port = NULL;
          }
          az_event_create(&iou->tx.event, iou->tx.event_descr->index, 0, NULL);
        }
      }
    }
    #endif

    #ifdef  CONFIG_AZ_IOU_USE_EVENT_PORT
    if (iou->config.flags & AZ_IOU_FLAGS_RX) {
      if (iou->config.rx.event_id) {
        r = az_event_descr_create(&iou->rx.event_descr,
                iou->config.rx.event_id, 
                iou->config.rx.event_mask, 
                1);
        if (r < 0) {
          iou->rx.event_descr = NULL;
        } else {
          az_event_create(&iou->rx.event, iou->rx.event_descr->index, 0, NULL);
          r = az_event_port_create(
                iou->config.rx.event_id, 
                iou->config.rx.event_mask, 
                AZ_EVENT_PORT_TYPE_RX,
                &iou->rx.event_port);
          if (AZ_SUCCESS == r) {
            r = az_event_port_attach(iou->rx.event_port);
          } else {
            iou->rx.event_port = NULL;
          }
        }
      }
    }
    #endif

  } while (0);

  return r;
}
az_r_t  az_iou_msgq_delete(az_iou_t *iou)
{
  az_assert(NULL != iou);
  az_assert(iou->ion.type == AZ_ION_TYPE_IOU);
  az_r_t r = AZ_SUCCESS;

  do {
    if (!(iou->status.state & AZ_IOU_STATE_ASSIGNED)) {
      r = AZ_ERR(STATE);
      break;
    }
    if ((iou->status.state & AZ_IOU_STATE_BUSY)) {
      r = AZ_ERR(BUSY);
      break;
    }
    #ifdef  CONFIG_AZ_IOU_USE_EVENT_PORT
    if (iou->config.flags & AZ_IOU_FLAGS_RX) {
      if (iou->rx.event_port) {
          r = az_event_port_detach(iou->rx.event_port);
          r = az_event_port_delete(iou->rx.event_port);
          iou->rx.event_port = NULL;
      }
      if (iou->rx.event_descr) {
        r = az_event_descr_delete(iou->rx.event_descr);
        iou->rx.event_descr = NULL;
      }
    }
    if (iou->config.flags & AZ_IOU_FLAGS_TX) {
      if (iou->tx.event_port) {
          r = az_event_port_detach(iou->tx.event_port);
          r = az_event_port_delete(iou->tx.event_port);
          iou->tx.event_port = NULL;
      }
      if (iou->tx.event_descr) {
        r = az_event_descr_delete(iou->tx.event_descr);
        iou->tx.event_descr = NULL;
      }
    }
    #endif
    r = az_ion_deregister(&iou->ion);
    if (r) break;
    if (az_refcount_atomic_dec(&iou->ion.refCount) == 0) {
      az_free(iou);
    } else {
      iou->status.state = 0;
    }
  } while (0);

  return r;
}

az_r_t  az_iou_msgq_open(az_iou_t *iou, int flags)
{
  az_assert(NULL != iou);
  az_assert(iou->ion.type == AZ_ION_TYPE_IOU);
  az_assert(iou->status.state & AZ_IOU_STATE_ASSIGNED);
  az_r_t r = AZ_SUCCESS;

  do {
    if (!(flags & AZ_IOU_FLAGS_TRX)) {
      r = AZ_ERR(INVALID_ARG);
      break;
    }
    if (iou->status.state & AZ_IOU_STATE_TX_OPEN){
      if (!(iou->config.flags & AZ_IOU_FLAGS_TX)) {
        r = AZ_ERR(NOT_SUPPORT);
        break;
      }
      if (flags & AZ_IOU_FLAGS_TX) {
        r = AZ_ERR(AGAIN);
        break;
      }
    }
    if (iou->status.state & AZ_IOU_STATE_RX_OPEN) {
      if (!(iou->config.flags & AZ_IOU_FLAGS_RX)) {
        r = AZ_ERR(NOT_SUPPORT);
        break;
      }
      if (flags & AZ_IOU_FLAGS_RX) {
        r = AZ_ERR(AGAIN);
        break;
      }
    }
    if ((flags & AZ_IOU_FLAGS_TRX) == AZ_IOU_FLAGS_TRX) {
      iou->status.state |= AZ_IOU_STATE_OPEN;
      if (!(iou->status.state & AZ_IOU_STATE_UP)) {
        az_iou_msgq_setup(iou, AZ_IOU_FLAGS_TRX);
      }
    }  else {
    if ((flags & AZ_IOU_FLAGS_TX)) {
      iou->status.state |= AZ_IOU_STATE_TX_OPEN;
      if (!(iou->status.state & AZ_IOU_STATE_TX_UP)) {
        az_iou_msgq_setup(iou, AZ_IOU_FLAGS_TX);
      }
    }
    if ((flags & AZ_IOU_FLAGS_RX)) {
      iou->status.state |= AZ_IOU_STATE_RX_OPEN;
      if (!(iou->status.state & AZ_IOU_STATE_RX_UP)) {
        az_iou_msgq_setup(iou, AZ_IOU_FLAGS_RX);
      }
    }
    }

  } while (0);

  return r;
}
az_r_t  az_iou_msgq_close(az_iou_t *iou, int flags)
{
  az_assert(NULL != iou);
  az_assert(iou->ion.type == AZ_ION_TYPE_IOU);
  az_assert(iou->status.state & AZ_IOU_STATE_ASSIGNED);
  az_r_t r = AZ_SUCCESS;

  do {
    if (!(flags & AZ_IOU_FLAGS_TRX)) {
      r = AZ_ERR(INVALID_ARG);
      break;
    }
    if (iou->status.state & AZ_IOU_STATE_TX_OPEN){
      if (!(iou->config.flags & AZ_IOU_FLAGS_TX)) {
        r = AZ_ERR(NOT_SUPPORT);
        break;
      }
      if (!(flags & AZ_IOU_FLAGS_TX)) {
        r = AZ_ERR(AGAIN);
        break;
      }
    }
    if (iou->status.state & AZ_IOU_STATE_RX_OPEN) {
      if (!(iou->config.flags & AZ_IOU_FLAGS_RX)) {
        r = AZ_ERR(NOT_SUPPORT);
        break;
      }
      if (!(flags & AZ_IOU_FLAGS_RX)) {
        r = AZ_ERR(AGAIN);
        break;
      }
    }
    if ((flags & AZ_IOU_FLAGS_TRX) == AZ_IOU_FLAGS_TRX) {
      if (iou->status.state & AZ_IOU_STATE_UP) {
        az_iou_msgq_release(iou, AZ_IOU_FLAGS_TRX);
      }
      iou->status.state &= ~AZ_IOU_STATE_OPEN;
    }  else {
      if ((flags & AZ_IOU_FLAGS_TX)) {
        if ((iou->status.state & AZ_IOU_STATE_TX_UP)) {
         az_iou_msgq_release(iou, AZ_IOU_FLAGS_TX);
        }
        iou->status.state &= ~AZ_IOU_STATE_TX_OPEN;
      }
      if ((flags & AZ_IOU_FLAGS_RX)) {
        if (!(iou->status.state & AZ_IOU_STATE_RX_UP)) {
         az_iou_msgq_release(iou, AZ_IOU_FLAGS_RX);
        }
        iou->status.state &= ~AZ_IOU_STATE_RX_OPEN;
      }
    }

  } while (0);

  return r;
}

az_r_t  az_iou_msgq_setup(az_iou_t *iou, int options)
{
  az_assert(NULL != iou);
  az_assert(iou->ion.type == AZ_ION_TYPE_IOU);
  az_assert(iou->status.state & AZ_IOU_STATE_ASSIGNED);
  az_r_t r = AZ_SUCCESS;
  char *remote = NULL, *local = NULL;
  int state = 0;

  do {
    if (options & AZ_IOU_FLAGS_TX) {
      remote = iou->config.remote_addr.name;
      state |= AZ_IOU_STATE_TX_UP;
    }
    if (options & AZ_IOU_FLAGS_RX) {
      local = iou->config.local_addr.name;
      state |= AZ_IOU_STATE_RX_UP;
    }
    iou->priv = az_iou_msgq_ctx_create(iou, iou->priv, local, remote);
    if (iou->priv) {
      az_iou_msgq_ctx_t *ctx = (az_iou_msgq_ctx_t *)(iou->priv);
      #ifdef  CONFIG_AZ_IOU_USE_EVENT_PORT
      if (NULL != ctx->local) {
        ctx->local->event_port = iou->rx.event_port; 
      }
      #endif
      iou->status.state |= state; 
    } else {
      r = AZ_ERR(CREAT);
    }
  } while (0);

  return r;
}

az_r_t  az_iou_msgq_release(az_iou_t *iou, int options)
{
  az_assert(NULL != iou);
  az_assert(iou->ion.type == AZ_ION_TYPE_IOU);
  az_assert(iou->status.state & AZ_IOU_STATE_ASSIGNED);
  az_r_t r = AZ_SUCCESS;
  char *remote = NULL, *local = NULL;
  int state = 0;

  do {
    if (options & AZ_IOU_FLAGS_TX) {
      remote = iou->config.remote_addr.name;
      state |= AZ_IOU_STATE_TX_UP;
    }
    if (options & AZ_IOU_FLAGS_RX) {
      local = iou->config.local_addr.name;
      state |= AZ_IOU_STATE_RX_UP;
    }
    iou->priv = az_iou_msgq_ctx_delete(iou, iou->priv, local, remote);
    iou->status.state &= ~state; 
  } while (0);

  return r;
}

az_size_t az_iou_msgq_recv(az_iou_t *iou, void *bp, az_size_t size)
{
  az_size_t r = (az_size_t)AZ_SUCCESS;
  az_iou_msgq_ctx_t *ctx;
  az_assert(NULL != iou);
  do {
    ctx = (az_iou_msgq_ctx_t *)(iou->priv);
    if (NULL == ctx) {
      r = (az_size_t)AZ_ERR(CTX_NULL);
      break;
    }
    if (ctx->local) {
      r = (ctx->local->oprs->Recv)(ctx->local, bp, size);
    }

  } while (0);

  return r;
}

az_size_t az_iou_msgq_send(az_iou_t *iou, void *bp, az_size_t size)
{
  az_size_t r = (az_size_t)AZ_SUCCESS;
  az_iou_msgq_ctx_t *ctx;
  az_assert(NULL != iou);
  do {
    ctx = (az_iou_msgq_ctx_t *)(iou->priv);
    if (NULL == ctx) {
      r = (az_size_t)AZ_ERR(CTX_NULL);
      break;
    }
    if (ctx->remote) {
      r = (ctx->remote->oprs->Send)(ctx->remote, bp, size);
      #ifdef  CONFIG_AZ_IOU_USE_EVENT_PORT
      if (iou->tx.event) {
        az_event_send(iou->tx.event_port, iou->tx.event);
      }
      #endif
    }

  } while (0);

  return r;
}

az_r_t az_iou_msgq_control(az_iou_t *iou, int cmd, void *arg1, void *arg2)
{
  az_size_t r = (az_size_t)AZ_SUCCESS;
  az_assert(NULL != iou);

  do {
    switch (cmd) {
      default:
        r = AZ_ERR(NOT_SUPPORT);
        break;
    }
  } while (0);

  return r;
}

/* === end of AZ_IOU_MSGQ_C === */
