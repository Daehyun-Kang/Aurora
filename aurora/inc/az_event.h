/**
 * @file   az_event.h
 * @brief  
 * @date   18/06/16
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
 *   - 2016-06-18 fware initial create
 */

#ifndef AZ_EVENT_H
#define AZ_EVENT_H

#include "az_link.h"
#include "az_ring.h"
#include "az_rw_lock.h"
#include "az_bits.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* constants */

/* basic macros */
#ifdef  CONFIG_AZ_EVENT_DESCR_POOL_SZ
#define AZ_EVENT_DESCR_POOL_SZ   CONFIG_AZ_EVENT_DESCR_POOL_SZ   
#else
#define AZ_EVENT_DESCR_POOL_SZ   2048   
#endif

#define AZ_EVENT_PORT_LINK_POOL_SZ   (AZ_EVENT_DESCR_POOL_SZ * 4)   

/* basic types */
#ifdef  CONFIG_AZ_EVENT_SZ_32BIT 
  typedef uint32_t  az_event_id_t;
  #define AZ_EVENT_MASK_SIZE(m) az_bitmask_rfilled_size32(m)
  #define AZ_FMT_EVENT_ID(_v) "%" #_v "x"
                                   //1234567890
  #define AZ_EVENT_ID_SYS_FLAG     0x80000000L
  #define AZ_EVENT_SYSTEM(e)  ((e) & AZ_EVENT_ID_SYS_FLAG) 
#else
  typedef uint64_t  az_event_id_t;
  #define AZ_EVENT_MASK_SIZE(m) az_bitmask_rfilled_size64(m)
  #define AZ_FMT_EVENT_ID(_v) "%" #_v "lx"
                                   //123456789abcdef0
  #define AZ_EVENT_ID_SYS_FLAG     0x8000000000000000L
  #define AZ_EVENT_SYSTEM(e)  ((e) & AZ_EVENT_ID_SYS_FLAG) 
#endif
typedef uint32_t  az_event_node_t;
typedef az_event_id_t  az_event_mask_t;

struct az_event_port_entity; 
typedef uint32_t  az_event_descr_id_t;

#define AZ_EVENT_ID_INVALID           0
#define AZ_EVENT_ID(e)  ((e)->event_id)

#define AZ_EVENT_FLAGS_EXPORT    0x0080 //accessible from other node
#define AZ_EVENT_FLAGS_ASSERT    0x0001 //
#define AZ_EVENT_FLAGS_LEVEL     0x0002 // level or edge

typedef AZ_PACKED_STRUCT(az_event_descr) {
  az_event_descr_id_t   index;
  az_uint32_t           flags;
  az_refcount_t         refCount;

  az_event_node_t       event_node;
  az_event_id_t         event_id;

  struct az_event_port_entity *port; 
  az_ring_t             event_cache;
  //event_cache_list
} az_event_descr_t;

#define AZ_EVENT_FLAGS_BUFFER_DYNAMIC     0x0004 

typedef AZ_PACKED_STRUCT(az_event_entity) {
  az_event_descr_id_t   descr_id;
  az_uint32_t           flags;
  az_refcount_t         refCount;

  az_event_node_t       event_node;
  az_event_id_t         event_id;

  union {
    struct {
      az_uint32_t       mux_index;
      az_uint32_t       level_count;
    };
    struct {
      az_uint32_t       size;
      az_ref_t          data;
    } buffer;
  };

} az_event_entity_t;

typedef az_event_entity_t  * az_event_t;

#define AZ_EVENT_DESCR_ID(evt)  (((az_event_t)(evt))->descr_id)
#define AZ_EVENT_BUFFER_SIZE(evt)  (((az_event_t)(evt))->buffer.size)
#define AZ_EVENT_BUFFER_DATA(evt)  (((az_event_t)(evt))->buffer.data)

#define AZ_EVENT_IS_EXPORT(e)    \
  ((e)->flags & AZ_EVENT_FLAGS_EXPORT)
#define AZ_EVENT_SET_EXPORT(e)    \
  do { (e)->flags |= AZ_EVENT_FLAGS_EXPORT;} while (0) 
#define AZ_EVENT_RESET_EXPORT(e)    \
  do { (e)->flags &= ~AZ_EVENT_FLAGS_EXPORT;} while (0) 

#define AZ_EVENT_IS_TYPE_LEVEL(e)    \
  ((e)->flags & AZ_EVENT_FLAGS_LEVEL)
#define AZ_EVENT_IS_TYPE_EDGE(e)    \
  (!((e)->flags & AZ_EVENT_FLAGS_LEVEL))
#define AZ_EVENT_SET_TYPE_LEVEL(e)    \
  do { (e)->flags |= AZ_EVENT_FLAGS_LEVEL;} while (0) 
#define AZ_EVENT_SET_TYPE_EDGE(e)     \
  do { (e)->flags &= ~AZ_EVENT_FLAGS_LEVEL; } while(0) 

#define AZ_EVENT_IS_ASSERT(e)  \
  ((e)->flags & AZ_EVENT_FLAGS_ASSERT)
#define AZ_EVENT_SET_ASSERT(e)  \
  do { (e)->flags |= AZ_EVENT_FLAGS_ASSERT; } while (0)
#define AZ_EVENT_RESET_ASSERT(e)  \
  do { (e)->flags &= ~AZ_EVENT_FLAGS_ASSERT; } while (0)

#define AZ_EVENT_EQUAL_WITH_MASK(a, b, mask) \
  (((a) & (mask)) == ((b) & (mask)))

/* structures */

/*
 * define event port stuffs
 */
typedef az_uint32_t   az_event_port_no_t;

#define AZ_EVENT_PORT_NO_INVALID        (az_event_port_no_t)(-1)
#define AZ_EVENT_PORT_NO_NOT_ASSIGNED   (az_event_port_no_t)(-2)

#define AZ_EVENT_PORT_STATE_NULL  AZ_ST_IDLE 
#define AZ_EVENT_PORT_STATE_INIT  AZ_ST_INIT 
#define AZ_EVENT_PORT_STATE_TINI  AZ_ST_TINI 
#define AZ_EVENT_PORT_STATE_ACTV  AZ_ST_NORM

#define AZ_EVENT_PORT_TYPE_RX          1
#define AZ_EVENT_PORT_TYPE_TX          2
#define AZ_EVENT_PORT_TYPE_RX_SHARED   4
#define AZ_EVENT_PORT_TYPE_TX_SHARED   8

typedef union {
  az_uint32_t   u32;
#if __BYTE_ORDER == __LITTLE_ENDIAN 
  struct {
    az_uint32_t state:8;
    az_uint32_t substate:8;
    az_uint32_t mask_size:8;
    az_uint32_t reserved:4;
    az_uint32_t type:4;
  } st;
#else
  struct {
    az_uint32_t type:4;
    az_uint32_t reserved:4;
    az_uint32_t mask_size:8;
    az_uint32_t substate:8;
    az_uint32_t state:8;
  } st;
#endif
} az_event_port_flags_t;

typedef struct az_event_receiver_stats {
  struct {
    int qfull;
    int empty;
  } err;
} az_event_receiver_stats_t;

typedef union az_event_mux {
  struct {
    uint16_t size;
    uint16_t count;
    int32_t level_count;
  };
  az_event_t  event;
} az_event_mux_t;

struct az_event_receiver; 
typedef az_r_t (*az_event_receiver_in_t)(struct az_event_receiver *, az_event_t );
typedef az_r_t (*az_event_receiver_out_t)(struct az_event_receiver *, az_event_t *);

typedef struct az_event_receiver {
  az_link_t         link;
  az_sys_ep_t       *sys_port;
  az_sys_event_t    signo;

  az_event_t        event;
  az_event_mux_t    *event_mux;
  az_ring_t         *event_q;

  az_rw_lock_t      *lock;
  struct {
    az_event_receiver_in_t In;
    az_event_receiver_out_t Out;
  } oprs;

  az_event_receiver_stats_t stats;
} az_event_receiver_t;

static inline az_r_t az_event_mux_add_event(az_event_mux_t *mux, az_event_t evt)
{
  az_r_t r = AZ_SUCCESS;
  az_event_t    *evt_list;
  uint16_t mux_index;
  az_assert(NULL != mux);
  az_assert(NULL != evt);

  do {
    if (!AZ_EVENT_IS_TYPE_LEVEL(evt)) {
      r = AZ_ERR(INVALID);
      break;
    }
    mux_index = evt->mux_index;
    evt_list = (az_event_t *)(mux + 1);
    if (mux->size < 0) {
      r = AZ_ERR(EMPTY);
      break;
    }
    if (mux_index >= mux->size) {
      r = AZ_ERR(OOR);
      break;
    }
    if (NULL != evt_list[mux_index]) {
      r = AZ_ERR(NOT_EMPTY);
      break;
    }
    evt->level_count = 0;
    evt_list[mux_index] = evt;
    mux->count++;
  } while (0);
  return r;
}
static inline az_r_t az_event_mux_del_event(az_event_mux_t *mux, az_event_t *pEvt)
{
  az_r_t r = AZ_SUCCESS;
  az_event_t    *evt_list;
  az_event_t    evt;
  uint16_t mux_index;
  az_assert(NULL != mux);
  az_assert(NULL != pEvt);

  do {
    evt = *pEvt;
    if (NULL == evt) {
      r = AZ_ERR(ENTITY_NULL);
      break;
    }
    if (!AZ_EVENT_IS_TYPE_LEVEL(evt)) {
      r = AZ_ERR(INVALID);
      break;
    }
    mux_index = evt->mux_index;
    evt_list = (az_event_t *)(mux + 1);
    if (mux->size < 0) {
      r = AZ_ERR(EMPTY);
      break;
    }
    if (mux_index >= mux->size) {
      r = AZ_ERR(OOR);
      break;
    }
    if (NULL == evt_list[mux_index]) {
      r = AZ_ERR(EMPTY);
      break;
    }
    *pEvt = evt_list[mux_index];
    evt_list[mux_index] = NULL;
    mux->count--;
  } while (0);
  return r;
}

static inline az_r_t az_event_mux_push(az_event_mux_t *mux, az_event_t evt)
{
  az_r_t r = AZ_SUCCESS;
  az_assert(NULL != mux);
  az_assert(NULL != evt);
  az_event_t    *evt_list;
  uint16_t mux_index = evt->mux_index;
  az_assert(AZ_EVENT_IS_TYPE_LEVEL(evt)); 
  az_assert(mux_index < mux->size);

  do {
    evt_list = (az_event_t *)(mux + 1);
    evt = evt_list[mux_index];
    if (NULL == evt) {
      r = AZ_ERR(EMPTY);
      break;
    }
    evt->level_count++;
    mux->level_count++;
  } while (0);
  return r;
}

static inline az_r_t az_event_mux_pop(az_event_mux_t *mux, az_event_t *pEvt)
{
  az_r_t r = AZ_SUCCESS;
  az_event_t evt = NULL;
  az_assert(NULL != pEvt);
  az_assert(NULL != mux);
  az_event_t    *evt_list;
  int16_t mux_index, mux_count;
  az_assert(AZ_EVENT_IS_TYPE_LEVEL(evt)); 

  do {
    evt_list = (az_event_t *)(mux + 1);
    if (mux->level_count <= 0) {
      r = AZ_ERR(NO_DATA);
      break;
    }
    r = AZ_ERR(NOT_FOUND);
    *pEvt = NULL;
    mux_count = mux->count;
    for (mux_index = 0; mux_index < mux->size && mux_count > 0; mux_index++) {
      evt = evt_list[mux_index];
      if (NULL == evt) continue;
      mux_count--;
      if (evt->level_count > 0) {
        mux->level_count--;
        evt->level_count--;
        *pEvt = evt;
        break;
      }
    }
  } while (0);
  return r;
}

typedef az_r_t (*az_event_port_in_t)(struct az_event_port_entity *, az_event_t);
typedef az_r_t (*az_event_port_out_t)(struct az_event_port_entity *, az_event_t);

typedef struct az_event_port_entity {
  az_event_port_no_t    no;
  az_event_port_flags_t flags;

  az_event_id_t         event_id;
  az_event_id_t         event_mask;

  az_rw_lock_t          *lock;
  az_rw_lock_t          _lock;

  az_refcount_t         refCount;

  az_link_list_t        peer_list;
  az_link_list_t        receiver_list;

  struct {
    az_event_port_in_t  In;
    az_event_port_out_t Out;
  } oprs;

  void *priv;
} az_event_port_entity_t;

typedef az_event_port_entity_t * az_event_port_t;
#define AZ_EVENT_PORT_TYPE(port) ((port)->flags.st.type) 

#define AZ_EVENT_PORT_IS_TX(port) ((port)->flags.st.type & AZ_EVENT_PORT_TYPE_TX) 
#define AZ_EVENT_PORT_IS_TX_SHARED(port) ((port)->flags.st.type & AZ_EVENT_PORT_TYPE_TX_SHARED) 
#define AZ_EVENT_PORT_IS_RX(port) ((port)->flags.st.type & AZ_EVENT_PORT_TYPE_RX)
#define AZ_EVENT_PORT_IS_RX_SHARED(port) ((port)->flags.st.type & AZ_EVENT_PORT_TYPE_RX_SHARED)

typedef struct az_event_port_link {
  az_link_t        link;
  az_event_port_entity_t  *port;
  az_refcount_t         refCount;
} az_event_port_link_t;

#define AZ_EVENT_PORT_DECL(eport) \
  az_event_port_t eport = { \
    .no = AZ_EVENT_PORT_NO_NOT_ASSIGNED,\
    .flags.u32 = 0,\
    .lock = NULL,\
    .oprs = NULL,\
  };

#define az_event_port_lock_init(port) \
            if (port->lock) { az_rw_lock_init(port->lock); } 
#define az_event_port_lock_deinit(port) \
            if (port->lock) { az_rw_lock_deinit(port->lock); } 
#define az_event_port_lock_read(port) \
            if (port->lock) { az_rw_lock_read(port->lock); } 
#define az_event_port_unlock_read(port) \
            if (port->lock) { az_rw_unlock_read(port->lock); } 
#define az_event_port_lock_write(port) \
            if (port->lock) { az_rw_lock_write(port->lock); } 
#define az_event_port_unlock_write(port) \
            if (port->lock) { az_rw_unlock_write(port->lock); } 

/* structured types */

/* macros */

/* variabls exposed */

/* inline functions */

/* function prototypes exposed */
extern az_r_t  az_event_store_init();
extern az_r_t  az_event_store_deinit();
extern az_event_descr_t *az_event_descr_get(az_event_descr_id_t descr_id);
extern az_event_descr_t *az_event_descr_find(az_event_id_t event_id);
extern az_r_t  az_event_descr_create(az_event_descr_t **pDescr, az_event_id_t event_id, az_event_node_t event_node, int cache_size);
extern az_r_t  az_event_descr_delete(az_event_descr_t *descr);
extern az_event_t  az_event_alloc(az_event_descr_t *descr, az_uint32_t buffer_size, az_ref_t buffer_data);
extern az_r_t  az_event_create(az_event_t *pEvt, az_event_descr_id_t descr_id, az_uint32_t buffer_size, az_ref_t buffer_data);
extern void  az_event_free(az_event_t evt, az_event_descr_t *descr);
extern az_r_t  az_event_delete(az_event_t evt);

extern az_r_t az_event_send(az_event_port_t port, az_event_t evt);
extern az_r_t az_event_recv(az_event_receiver_t *rcvr, az_event_t *pEvt);

extern az_r_t az_event_receiver_init(az_event_receiver_t *rcvr, az_event_t event, az_event_mux_t *event_mux, az_ring_t *event_q);
extern az_r_t az_event_receiver_in_default(az_event_receiver_t *rcvr, az_event_t evt);
extern az_r_t az_event_receiver_in_single(az_event_receiver_t *rcvr, az_event_t evt);
extern az_r_t az_event_receiver_in_edge(az_event_receiver_t *rcvr, az_event_t evt);
extern az_r_t az_event_receiver_in_level(az_event_receiver_t *rcvr, az_event_t evt);
extern az_r_t az_event_receiver_out_default(az_event_receiver_t *rcvr, az_event_t *pEvt);
extern az_r_t az_event_receiver_out_single(az_event_receiver_t *rcvr, az_event_t *pEvt);
extern az_r_t az_event_receiver_out_level(az_event_receiver_t *rcvr, az_event_t *pEvt);
extern az_r_t az_event_receiver_out_edge(az_event_receiver_t *rcvr, az_event_t *pEvt);

extern int az_event_toStr(char *bp, int blen, void *fmt, az_event_t evt);

#ifdef __cplusplus
}
#endif
#endif
