/**
 * @file   az_msgq.h
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

#ifndef AZ_MSGQ_H
#define AZ_MSGQ_H
#include "az_core.h"
#include "az_sys_base.h"
#include "az_printf.h"
#include "az_bio.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* constants */

/* basic macros */
#ifdef  CONFIG_AZ_NAME_MAX
#define AZ_MSGQ_NAME_LEN  CONFIG_AZ_NAME_MAX
#else
#define AZ_MSGQ_NAME_LEN  64
#endif

/* basic types */
struct az_msgq;
typedef az_size_t (*az_msgq_send_t)(struct az_msgq *, void *bp, az_size_t size);
typedef az_size_t (*az_msgq_recv_t)(struct az_msgq *, void *bp, az_size_t size);

struct az_msgq_oprs {
  az_msgq_send_t  Send;
  az_msgq_recv_t  Recv;
};

/* structures */
typedef struct az_msgq {
  az_ion_t  ion;

  az_bio_t  bio;

  #ifdef  CONFIG_AZ_MSGQ_USE_EVENT_PORT
  az_event_port_t event_port;
  #endif

  char  name[AZ_MSGQ_NAME_LEN];

  struct az_msgq_oprs *oprs;

  az_refcount_t refCount;
  az_ref_t  _data_buffer;
  az_ref_t  _descr_list;
} az_msgq_t;

/* structured types */

/* macros */

/* variabls exposed */
extern az_size_t az_msgq_send32(struct az_msgq *, void *bp, az_size_t size);
extern az_size_t az_msgq_recv32(struct az_msgq *, void *bp, az_size_t size);
extern az_size_t az_msgq_send64(struct az_msgq *, void *bp, az_size_t size);
extern az_size_t az_msgq_recv64(struct az_msgq *, void *bp, az_size_t size);

extern struct az_msgq_oprs az_msgq_oprs32; 
extern struct az_msgq_oprs az_msgq_oprs64; 

/* inline functions */
static inline az_msgq_t *az_msgq_find(char *name)
{
  extern int az_msgq_comparebyname(char *name, az_msgq_t *msgq);
  return (az_msgq_t *)az_ion_find(name, (az_ion_compare_t)az_msgq_comparebyname);
}

static inline az_msgq_t *az_msgq_create(char *name, az_msgq_t *msgq,
    az_ref_t data_buffer, az_size_t block_size, az_size_t buffer_size, 
    az_ref_t descr_list, az_size_t descr_size, az_size_t descr_count)
{
  az_assert(NULL != name);
  az_r_t r;
  do {
    az_msgq_t *msgq_exist = az_msgq_find(name);
    if (msgq_exist) {
      msgq = msgq_exist;
      break;
    }
    if (NULL == msgq) {
      msgq = az_malloc(sizeof(az_msgq_t));
      if (NULL == msgq) {
        break;
      }
      az_refcount_init_dynamic(&msgq->ion.refCount);
    } 
    az_refcount_atomic_inc(&msgq->ion.refCount);
    r = az_ion_register(&msgq->ion, AZ_ION_TYPE_MSGQ);
    if (r < 0) {
      az_refcount_atomic_dec(&msgq->ion.refCount);
      if (AZ_REFCOUNT_IS_ZERO(&msgq->ion.refCount)) {
        az_free(msgq);
        msgq = NULL;
        break;
      }
    }
    if (NULL == data_buffer) {
      msgq->_data_buffer = data_buffer = az_malloc(block_size*buffer_size);
    } else {
      msgq->_data_buffer = NULL;
    }
    if (NULL == data_buffer) {
      az_refcount_atomic_dec(&msgq->ion.refCount);
      if (AZ_REFCOUNT_IS_ZERO(&msgq->ion.refCount)) {
        az_free(msgq);
        msgq = NULL;
        break;
      }
    }
    if (NULL == descr_list) {
      msgq->_descr_list = descr_list = az_malloc(descr_size*descr_count);
    } else {
      msgq->_descr_list = NULL;
    }
    if (NULL == descr_list) {
      if (msgq->_data_buffer) {
        az_free(msgq->_data_buffer);
      }
      msgq->_data_buffer = NULL;
      az_refcount_atomic_dec(&msgq->ion.refCount);
      if (AZ_REFCOUNT_IS_ZERO(&msgq->ion.refCount)) {
        az_free(msgq);
        msgq = NULL;
        break;
      }
    }
    strncpy(msgq->name, name, sizeof(msgq->name));

    msgq->bio.cfg.buffer_size = buffer_size;
    msgq->bio.cfg.block_size = block_size;
    msgq->bio.cfg.buffer_data = data_buffer;
    msgq->bio.cfg.descr_count = descr_count;
    msgq->bio.cfg.descr_type = (descr_size == sizeof(int32_t))? AZ_RING_TYPE_DS32:AZ_RING_TYPE_DS64;
    msgq->bio.cfg.descr_list = descr_list;

    if (msgq->bio.cfg.descr_type == AZ_RING_TYPE_DS32) {
      msgq->oprs = &az_msgq_oprs32;
    } else {
      msgq->oprs = &az_msgq_oprs64;
    }
    
    az_bio_init(&(msgq->bio), &(msgq->bio.cfg));

    #ifdef  CONFIG_AZ_MSGQ_USE_EVENT_PORT
    msgq->event_port = NULL;
    #endif

    az_refcount_init_dynamic(&msgq->refCount);
  } while (0);

  return msgq;
}
static inline void az_msgq_delete(az_msgq_t *msgq)
{
  //az_r_t r;
  az_assert(NULL != msgq);
  do {
    #ifdef  CONFIG_AZ_MSGQ_USE_EVENT_PORT
    msgq->event_port = NULL;
    #endif
    az_bio_deinit(&(msgq->bio));
    if (msgq->_descr_list) {
      az_free(msgq->_descr_list);
      msgq->_descr_list = NULL;
    }
    if (msgq->_data_buffer) {
      az_free(msgq->_data_buffer);
      msgq->_data_buffer = NULL;
    }

    //r = az_ion_deregister(&msgq->ion);
    (void)az_ion_deregister(&msgq->ion);
    az_refcount_atomic_dec(&msgq->ion.refCount);
    if (AZ_REFCOUNT_IS_ZERO(&msgq->ion.refCount)) {
      az_free(msgq);
    }
  } while (0);
}

/* function prototypes exposed */

#ifdef __cplusplus
}
#endif
#endif
