/**
 * @file   az_thread.h
 * @brief  
 * @date   10/04/16
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
 *   - 2016-04-10 fware initial create
 */

#ifndef AZ_THREAD_H
#define AZ_THREAD_H

#include "az_def.h"
#include "az_err.h"
#include "az_macros.h"
#include "az_rw_lock.h"
#include "az_malloc.h"
#include "az_thread_types.h"
#include "az_link.h"
#include "az_dllib.h"
#include "az_xcfg.h"
#include "az_thread_cfg.h"
#include "az_ring.h"
#include "az_ion.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* constants */
#define AZ_THREAD_STATE_CREATED     0x0001
#define AZ_THREAD_STATE_START       0x0002
#define AZ_THREAD_STATE_STOP        0x0004
#define AZ_THREAD_STATE_BUSY        0x0008
#define AZ_THREAD_STATE_ERROR       0x4000
#define AZ_THREAD_STATE_EXCPT       0x8000

#define AZ_THREAD_ERROR_CREATED     0x0001
#define AZ_THREAD_ERROR_START       0x0002
#define AZ_THREAD_ERROR_STOP        0x0004
#define AZ_THREAD_ERROR_DELETE      0x0008
#define AZ_THREAD_ERROR_SNDEVT      0x0010
#define AZ_THREAD_ERROR_RCVEVT      0x0010

#define AZ_THREAD_BEAM_VECTORS         (sizeof(az_thread_beam_t)*8)
#define AZ_THREAD_BEAM_THR_START       0x00010000
#define AZ_THREAD_BEAM_THR_STOP        0x00020000
#define AZ_THREAD_BEAM_TRZ             0x00040000
#define AZ_THREAD_BEAM_EVTBUS          0x00080000
#define AZ_THREAD_BEAM_MSG             0x00100000

#if (CONFIG_AZ_THREAD_MSG_SZ == 64)
#define AZ_THREAD_MSG_EXCEPTION         (0x80000000L << 32) 
#define AZ_THREAD_MSG_STATECHG          (0x40000000L << 32) 
#define AZ_THREAD_MSG_CAT_MASK          (0xffffffffL << 32) 
#else
#define AZ_THREAD_MSG_EXCEPTION         (0x80000000) 
#define AZ_THREAD_MSG_STATECHG          (0x40000000) 
#define AZ_THREAD_MSG_CAT_MASK          (0xffff0000) 
#endif

#define AZ_THREAD_FLAGS_LOG_ERR         0x00000001
#define AZ_THREAD_FLAGS_TRACE           0x00000002

#ifdef CONFIG_AZ_TRACE_TASKS           
#define AZ_THREAD_FLAGS_INI_VAL         (AZ_THREAD_FLAGS_LOG_ERR|AZ_THREAD_FLAGS_TRACE) 
#else
#define AZ_THREAD_FLAGS_INI_VAL         (AZ_THREAD_FLAGS_LOG_ERR) 
#endif

#define AZ_THREAD_TRACE_ENABLED(xu)     ((xu)->flags & AZ_THREAD_FLAGS_TRACE)           
#define AZ_THREAD_TRACE_ENABLE(xu)     ((xu)->flags |= AZ_THREAD_FLAGS_TRACE)           
#define AZ_THREAD_TRACE_DISABLE(xu)     ((xu)->flags &= ~AZ_THREAD_FLAGS_TRACE)          
#define AZ_THREAD_ID_INVALID            AZ_ION_ID_INVALID

/* basic macros */
#define AZ_THREAD_BEAM_OPT_AND_ALL     AZ_SYS_EVENT_OPT_AND_ALL 
#define AZ_THREAD_BEAM_OPT_PRESERVE    AZ_SYS_EVENT_OPT_PRESERVE 


/* basic types */
typedef az_ion_id_t     az_thread_id_t;

typedef short az_thread_state_t;
typedef short az_thread_cause_t;
typedef int   az_thread_flags_t;

typedef az_sys_event_t  az_thread_beam_t;

#if (CONFIG_AZ_THREAD_MSG_SZ == 64)
typedef uint64_t        az_thread_msg_t;
#else
typedef uint32_t        az_thread_msg_t;
#endif

typedef void *az_thread_arg_t;
typedef void *az_thread_ret_t;
typedef az_thread_ret_t (*az_thread_entry_t)(az_thread_arg_t);
typedef void (*az_thread_beam_handler_t)(int, az_thread_arg_t);
struct az_thread_beam {
  az_thread_beam_handler_t handler;
  az_thread_arg_t           arg;
};

#ifdef  CONFIG_AZ_TRACE
typedef struct az_thread_trace_info {
  short flags;
  short depth;
  #ifdef  CONFIG_AZ_TRACE_PTHREAD
  pthread_t thread;
  #endif
  char            name[CONFIG_AZ_NAME_MAX];
  void  *callstack[CONFIG_AZ_TRACE_DEPTH_MAX];
} az_thread_trace_info_t;
#endif

typedef struct az_thread_entity {
  az_ion_t        ion;
#ifndef  CONFIG_AZ_TRACE
  char            name[CONFIG_AZ_NAME_MAX];
#endif

  az_thread_state_t   state;
  az_thread_cause_t   cause;
  az_thread_flags_t   flags;

  az_thread_entry_t   entry;
  az_thread_arg_t     arg;

  az_sys_xu_t     sys_xu;

  az_thread_beam_t   beams;
  int             beamcount;
  struct az_thread_beam *beamtable; 
  az_thread_attr_t    attr;

#ifdef  CONFIG_AZ_EMBED_SYS_THREAD
  az_sys_xu_entity_t  _sys_xu;
#endif

#ifdef  CONFIG_AZ_EMBED_THREAD_EVECTORS
  struct az_thread_beam _beamtable[AZ_THREAD_BEAM_VECTORS];
#endif

#ifdef  CONFIG_AZ_TRACE
  az_thread_trace_info_t trace_info; 
#endif

  az_link_list_t  trz_list;

  az_rw_lock_t    lock;
  void *excpt_point;

  az_ion_id_t wait_thread_id;
  az_ring_t   msgq;
  az_thread_msg_t msgs[CONFIG_AZ_THREAD_MSG_QUEUE_SZ]; 

} az_thread_entity_t;
typedef   az_thread_entity_t *  az_thread_t;
#define   AZ_THREAD_INVALID     NULL

#ifdef  CONFIG_AZ_TRACE
#define   AZ_THREAD_NAME(xu)    (xu)->trace_info.name
#else
#define   AZ_THREAD_NAME(xu)    (xu)->name
#endif

/* structures */

/* structured types */

/* macros */

#define AZ_THREAD_IS_STATE_EXCPT(xu) (((xu) == NULL)? 0:((xu)->state & AZ_THREAD_STATE_EXCPT))
#define AZ_THREAD_EXCPT_POINT(xu)   (((xu)==NULL)? NULL:((xu)->excpt_point))
#define AZ_THREAD_ON_EXCPT(xu)      AZ_THREAD_IS_STATE_EXCPT(xu) 
#define AZ_THREAD_REPORT_EXCPT()    az_thread_sendMsg(az_thread_self()->wait_thread_id, AZ_THREAD_MSG_EXCEPTION|az_thread_self()->ion.id)

#define AZ_THREAD_MSG_CATEGORY(msg)  ((msg) & AZ_THREAD_MSG_CAT_MASK)
#define AZ_THREAD_MSG_GET_ION_ID(msg) (az_ion_id_t)((msg) & 0xffffffff)

/* variabls exposed */

#ifdef  CONFIG_AZ_USE_TLS
extern AZ_SYS_TLS az_thread_t az_thread;
extern AZ_SYS_TLS  char   az_thread_prtbuf[CONFIG_AZ_THREAD_PRTBUF_SZ];

#define az_thread_self()  az_thread
#define az_thread_null()  az_thread = NULL
#define az_thread_traceinfo() ((az_thread == NULL)? NULL:&az_thread->trace_info)
#else
extern az_thread_t az_thread_self();
extern void az_thread_null();
extern az_thread_trace_info_t *az_thread_traceinfo();
#endif

/* inline functions */
static inline void az_attr_no_instrument az_thread_set_flags(int flags)
{
  az_thread_t xu = az_thread_self();
  if (xu) {
    xu->flags |= flags;
  }
}
static inline void az_attr_no_instrument az_thread_reset_flags(int flags)
{
  az_thread_t xu = az_thread_self();
  if (xu) {
    xu->flags &= ~flags;
  }
}

#ifdef  CONFIG_AZ_TRACE
static inline int az_attr_no_instrument az_thread_set_trace(az_thread_trace_info_t *info, int onoff)
{
  if (onoff) {
    info->flags |= AZ_THREAD_FLAGS_TRACE; 
  } else {
    info->flags &= ~AZ_THREAD_FLAGS_TRACE; 
  }
  return AZ_THREAD_TRACE_ENABLED(info); 
}
#define AZ_THREAD_TRACE_SET_CURFUNC(traceinfo, func) {\
  if ((unsigned)(traceinfo->depth) < CONFIG_AZ_TRACE_DEPTH_MAX) {\
    traceinfo->callstack[traceinfo->depth] = func;\
  }\
}
#endif

static inline az_r_t az_thread_iomux_add(az_thread_t xu, az_sys_io_t fd, uint32_t ioevt)
{
  az_assert(NULL != xu);
  return (az_r_t)az_sys_iomux_add(xu->sys_xu->iomux, fd, ioevt);
}
static inline az_r_t az_thread_iomux_del(az_thread_t xu, az_sys_io_t fd)
{
  az_assert(NULL != xu);
  return (az_r_t)az_sys_iomux_del(xu->sys_xu->iomux, fd);
}


/* function prototypes exposed */

extern az_ion_id_t az_thread_create(char *name, az_thread_entry_t entry, az_thread_arg_t arg, az_thread_config_t *pCfg, az_thread_t *pXU);
extern az_r_t az_thread_start(az_ion_id_t id, az_thread_entry_t entry, az_thread_arg_t arg);
extern az_r_t  az_thread_stop(az_ion_id_t id);
extern az_r_t az_thread_delete(az_ion_id_t id);

extern az_r_t az_thread_setPriority(az_ion_id_t id, az_thread_attr_t *pAttr);
extern az_r_t az_thread_getPriority(az_ion_id_t id, az_thread_attr_t *pAttr);
extern az_r_t az_thread_setAffinity(az_ion_id_t id, az_thread_core_mask_t core_mask);
extern az_r_t az_thread_getAffinity(az_ion_id_t id, az_thread_core_mask_t *pCoreMask);

extern az_r_t az_thread_suspend(az_ion_id_t id);
extern az_r_t az_thread_resume(az_ion_id_t id);
extern az_r_t az_thread_sleep(az_int64_t nsec);

extern az_r_t az_thread_send_beam(az_ion_id_t id, az_thread_beam_t event);
extern az_r_t az_thread_recv_beam(az_thread_beam_t toReceive, az_int8_t options, az_int64_t nsec, az_thread_beam_t *pReceived);
extern az_r_t az_thread_wait_beam(az_thread_beam_t toReceive, az_int8_t options, az_int64_t nsec, az_thread_beam_t *pReceived);

extern az_r_t az_thread_regBeamHandler(az_ion_id_t id, unsigned int vecno, az_thread_beam_handler_t handler, az_thread_arg_t arg); 
extern az_r_t az_thread_deregBeamHandler(az_ion_id_t id, unsigned int vecno);

extern az_r_t az_thread_send_msg(az_ion_id_t id, az_thread_msg_t msg);
extern az_r_t az_thread_recv_msg(az_thread_msg_t *msg);

extern az_thread_id_t az_thread_find(char *name);

extern void* az_thread_getarg();
extern az_r_t az_thread_getcpu();

extern void  az_thread_empty_sys_xu(az_thread_t xu);
extern az_thread_t  az_thread_empty(az_thread_t xu);

extern void az_thread_cleanup(az_thread_t xu);
extern void    az_thread_exit(az_thread_ret_t ret);

extern void *az_thread_entry(void *arg);

extern void  az_thread_set_cleanup();
extern void  az_thread_set_state_excpt(void *excpt_point);
extern void  az_thread_reset_state_excpt(az_thread_t xu);
#ifdef __cplusplus
}
#endif
#endif
