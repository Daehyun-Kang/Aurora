/**
 * @file   az_iou_msgq.h
 * @brief  
 * @date   05/08/16
 * @author fware
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
 *   - 2016-08-05 fware initial create
 */

#ifndef AZ_IOU_MSGQ_H
#define AZ_IOU_MSGQ_H
#include "az_core.h"
#include "az_sys_base.h"
#include "az_printf.h"
#include "az_bio.h"
#include "az_msgq.h"
#include "az_iou.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* constants */

/* basic macros */

/* basic types */

/* structures */
typedef struct az_iou_msgq_ctx {
  az_msgq_t *local;
  az_msgq_t *remote;

  az_iou_t  *iou;
  az_refcount_t refCount;
} az_iou_msgq_ctx_t;

/* structured types */

/* macros */

/* variabls exposed */

/* inline functions */
static inline az_iou_msgq_ctx_t *az_iou_msgq_ctx_create(az_iou_t *iou, az_iou_msgq_ctx_t *ctx, char *local_qname, char *remote_qname)
{
  do {
    if (NULL != ctx) {
      ctx = az_malloc(sizeof(az_iou_msgq_ctx_t));
      if (NULL == ctx) break;
      az_refcount_init_dynamic(&ctx->refCount);
    }
    az_refcount_atomic_inc(&ctx->refCount);
    if (NULL != remote_qname) {
      ctx->remote = az_msgq_find(remote_qname);
      if (NULL == ctx->remote) {
        ctx->remote = az_msgq_create(remote_qname, NULL,
            NULL, iou->config.tx.block_size, iou->config.tx.buffer_size,
            NULL, iou->config.tx.descr_size, iou->config.tx.descr_count);
      }
      az_refcount_atomic_inc(&ctx->remote->refCount);
    }
    if (NULL != local_qname) {
      ctx->local = az_msgq_find(local_qname);
      if (NULL == ctx->local) {
        ctx->local = az_msgq_create(local_qname, NULL,
            NULL, iou->config.rx.block_size, iou->config.rx.buffer_size,
            NULL, iou->config.rx.descr_size, iou->config.rx.descr_count);
      }
      az_refcount_atomic_inc(&ctx->local->refCount);
    }
    ctx->iou = iou;
  } while (0);

  return ctx;
}
static inline az_iou_msgq_ctx_t *az_iou_msgq_ctx_delete(az_iou_t *iou, az_iou_msgq_ctx_t *ctx, char *local, char *remote)
{
  do {
    if (NULL == ctx) break;
    if (ctx->local && local) {
      az_refcount_atomic_dec(&ctx->local->refCount);
      if (AZ_REFCOUNT_IS_ZERO(&ctx->local->refCount)) {
        az_msgq_delete(ctx->local);
      }
      ctx->local = NULL;
    }  
    if (ctx->remote && remote) {
      az_refcount_atomic_dec(&ctx->remote->refCount);
      if (AZ_REFCOUNT_IS_ZERO(&ctx->remote->refCount)) {
        az_msgq_delete(ctx->remote);
      }
      ctx->remote = NULL;
    }  
    az_refcount_atomic_dec(&ctx->refCount);
    if (AZ_REFCOUNT_IS_ZERO(&ctx->refCount)) {
      ctx->iou = NULL;
      az_free(ctx);
      ctx = NULL;
    }
  } while (0);
  return ctx;
}

/* function prototypes exposed */
extern az_r_t  az_iou_msgq_create(az_iou_t *iou, az_iou_config_t *cfg, az_iou_id_t *pId);
extern az_r_t  az_iou_msgq_delete(az_iou_t *iou);
extern az_r_t  az_iou_msgq_open(az_iou_t *iou, int flags);
extern az_r_t  az_iou_msgq_close(az_iou_t *iou, int flags);
extern az_r_t  az_iou_msgq_setup(az_iou_t *iou, int options);
extern az_r_t  az_iou_msgq_release(az_iou_t *iou, int options);
extern az_r_t az_iou_msgq_control(az_iou_t *iou, int cmd, void *arg1, void *arg2);
extern az_size_t az_iou_msgq_recv(az_iou_t *iou, void *bp, az_size_t size);
extern az_size_t az_iou_msgq_send(az_iou_t *iou, void *bp, az_size_t size);

#ifdef __cplusplus
}
#endif
#endif
