/**
 * @file   az_xu.h
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

#ifndef AZ_XU_H
#define AZ_XU_H

#include "az_def.h"
#include "az_err.h"
#include "az_macros.h"
#include "az_malloc.h"
#include "az_xu_types.h"
#include "az_link.h"
#include "az_dllib.h"
#include "az_xcfg.h"
#include "az_xu_cfg.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* constants */
#define AZ_XU_STATE_CREATED     0x0001
#define AZ_XU_STATE_START       0x0002
#define AZ_XU_STATE_STOP        0x0004
#define AZ_XU_STATE_BUSY        0x0008
#define AZ_XU_STATE_ERROR       0x4000
#define AZ_XU_STATE_EXCPT       0x8000

#define AZ_XU_ERROR_CREATED     0x0001
#define AZ_XU_ERROR_START       0x0002
#define AZ_XU_ERROR_STOP        0x0004
#define AZ_XU_ERROR_DELETE      0x0008
#define AZ_XU_ERROR_SNDEVT      0x0010
#define AZ_XU_ERROR_RCVEVT      0x0010

#define AZ_XU_EVENT_VECTORS         (sizeof(az_xu_event_t)*8)
#define AZ_XU_EVENT_THR_START       0x00010000
#define AZ_XU_EVENT_THR_STOP        0x00020000
#define AZ_XU_EVENT_TRZ             0x00040000
#define AZ_XU_EVENT_EVTBUS          0x00080000
#define AZ_XU_EVENT_MSG             0x00100000

#if (CONFIG_AZ_XU_MSG_SZ == 64)
#define AZ_XU_MSG_EXCEPTION         (0x80000000L << 32) 
#define AZ_XU_MSG_STATECHG          (0x40000000L << 32) 
#define AZ_XU_MSG_CAT_MASK          (0xffffffffL << 32) 
#else
#define AZ_XU_MSG_EXCEPTION         (0x80000000) 
#define AZ_XU_MSG_STATECHG          (0x40000000) 
#define AZ_XU_MSG_CAT_MASK          (0xffff0000) 
#endif

#define AZ_XU_FLAGS_LOG_ERR         0x00000001
#define AZ_XU_FLAGS_TRACE           0x00000002
#ifdef CONFIG_AZ_TRACE_TASKS           
#define AZ_XU_FLAGS_INI_VAL         (AZ_XU_FLAGS_LOG_ERR|AZ_XU_FLAGS_TRACE) 
#else
#define AZ_XU_FLAGS_INI_VAL         (AZ_XU_FLAGS_LOG_ERR) 
#endif

#define AZ_XU_TRACE_ENABLED(xu)     ((xu)->flags & AZ_XU_FLAGS_TRACE)           
#define AZ_XU_TRACE_ENABLE(xu)     ((xu)->flags |= AZ_XU_FLAGS_TRACE)           
#define AZ_XU_TRACE_DISABLE(xu)     ((xu)->flags &= ~AZ_XU_FLAGS_TRACE)          
/* basic macros */
#define AZ_XU_EVENT_OPT_AND_ALL     AZ_SYS_EVENT_OPT_AND_ALL 
#define AZ_XU_EVENT_OPT_PRESERVE    AZ_SYS_EVENT_OPT_PRESERVE 

/* basic types */
typedef short az_xu_state_t;
typedef short az_xu_cause_t;
typedef int   az_xu_flags_t;
typedef az_sys_event_t  az_xu_event_t;
#if (CONFIG_AZ_XU_MSG_SZ == 64)
typedef uint64_t        az_xu_msg_t;
#else
typedef uint32_t        az_xu_msg_t;
#endif

typedef void *az_xu_arg_t;
typedef void *az_xu_ret_t;
typedef az_xu_ret_t (*az_xu_entry_t)(az_xu_arg_t);
typedef void (*az_xu_event_handler_t)(int, az_xu_arg_t);
struct az_xu_evector {
  az_xu_event_handler_t handler;
  az_xu_arg_t           arg;
};

#ifdef  CONFIG_AZ_TRACE
typedef struct az_xu_trace_info {
  short flags;
  short depth;
  #ifdef  CONFIG_AZ_TRACE_PTHREAD
  pthread_t thread;
  #endif
  char            name[CONFIG_AZ_NAME_MAX];
  void  *callstack[CONFIG_AZ_TRACE_DEPTH_MAX];
} az_xu_trace_info_t;
#endif

typedef struct az_xu_entity {
  az_ion_t        ion;
#ifndef  CONFIG_AZ_TRACE
  char            name[CONFIG_AZ_NAME_MAX];
#endif

  az_xu_state_t   state;
  az_xu_cause_t   cause;
  az_xu_flags_t   flags;

  az_xu_entry_t   entry;
  az_xu_arg_t     arg;

  az_sys_xu_t     sys_xu;

  az_xu_event_t   evectors;
  int             evecount;
  struct az_xu_evector *evectable; 
  az_xu_attr_t    attr;

#ifdef  CONFIG_AZ_EMBED_SYS_XU
  az_sys_xu_entity_t  _sys_xu;
#endif

#ifdef  CONFIG_AZ_EMBED_XU_EVECTORS
  struct az_xu_evector _evectable[AZ_XU_EVENT_VECTORS];
#endif

#ifdef  CONFIG_AZ_TRACE
  az_xu_trace_info_t trace_info; 
#endif

  az_link_list_t  trz_list;

  az_rw_lock_t    lock;
  void *excpt_point;

  az_ion_id_t wait_xu_id;
  az_ring_t   msgq;
  az_xu_msg_t msgs[CONFIG_AZ_XU_MSG_QUEUE_SZ]; 

} az_xu_entity_t;
typedef   az_xu_entity_t *  az_xu_t;
#define   AZ_XU_INVALID     NULL

#ifdef  CONFIG_AZ_TRACE
#define   AZ_XU_NAME(xu)    (xu)->trace_info.name
#else
#define   AZ_XU_NAME(xu)    (xu)->name
#endif

/* structures */

/* structured types */

/* macros */
#define AZ_XU_EVENT_OPT_AND_ALL    AZ_SYS_EVENT_OPT_AND_ALL 
#define AZ_XU_EVENT_OPT_PRESERVE   AZ_SYS_EVENT_OPT_PRESERVE

#define AZ_XU_IS_STATE_EXCPT(xu) (((xu) == NULL)? 0:((xu)->state & AZ_XU_STATE_EXCPT))
#define AZ_XU_EXCPT_POINT(xu)   (((xu)==NULL)? NULL:((xu)->excpt_point))
#define AZ_XU_ON_EXCPT(xu)      AZ_XU_IS_STATE_EXCPT(xu) 
#define AZ_XU_REPORT_EXCPT()    az_xu_sendMsg(az_xu_self()->wait_xu_id, AZ_XU_MSG_EXCEPTION|az_xu_self()->ion.id)

#define AZ_XU_MSG_CATEGORY(msg)  ((msg) & AZ_XU_MSG_CAT_MASK)
#define AZ_XU_MSG_GET_ION_ID(msg) (az_ion_id_t)((msg) & 0xffffffff)

/* variabls exposed */

#ifdef  CONFIG_AZ_USE_TLS
extern AZ_SYS_TLS az_xu_t az_xu;
extern AZ_SYS_TLS  char   az_xu_prtbuf[CONFIG_AZ_XU_PRTBUF_SZ];

#define az_xu_self()  az_xu
#define az_xu_null()  az_xu = NULL
#define az_xu_traceinfo() ((az_xu == NULL)? NULL:&az_xu->trace_info)
#else
extern az_xu_t az_xu_self();
extern void az_xu_null();
extern az_xu_trace_info_t *az_xu_traceinfo();
#endif

/* inline functions */
static inline void az_attr_no_instrument az_xu_set_flags(int flags)
{
  az_xu_t xu = az_xu_self();
  if (xu) {
    xu->flags |= flags;
  }
}
static inline void az_attr_no_instrument az_xu_reset_flags(int flags)
{
  az_xu_t xu = az_xu_self();
  if (xu) {
    xu->flags &= ~flags;
  }
}

#ifdef  CONFIG_AZ_TRACE
static inline int az_attr_no_instrument az_xu_set_trace(az_xu_trace_info_t *info, int onoff)
{
  if (onoff) {
    info->flags |= AZ_XU_FLAGS_TRACE; 
  } else {
    info->flags &= ~AZ_XU_FLAGS_TRACE; 
  }
  return AZ_XU_TRACE_ENABLED(info); 
}
#define AZ_XU_TRACE_SET_CURFUNC(traceinfo, func) {\
  if ((unsigned)(traceinfo->depth) < CONFIG_AZ_TRACE_DEPTH_MAX) {\
    traceinfo->callstack[traceinfo->depth] = func;\
  }\
}
#endif

static inline az_r_t az_xu_iomux_add(az_xu_t xu, az_sys_io_t fd, uint32_t ioevt)
{
  az_assert(NULL != xu);
  return (az_r_t)az_sys_iomux_add(xu->sys_xu->iomux, fd, ioevt);
}
static inline az_r_t az_xu_iomux_del(az_xu_t xu, az_sys_io_t fd)
{
  az_assert(NULL != xu);
  return (az_r_t)az_sys_iomux_del(xu->sys_xu->iomux, fd);
}


/* function prototypes exposed */
extern void az_sys_xu_register_exception_handler();

extern az_ion_id_t az_xu_create(char *name, az_xu_entry_t entry, az_xu_arg_t arg, az_xu_config_t *pCfg, az_xu_t *pXU);
extern az_r_t az_xu_start(az_ion_id_t id, az_xu_entry_t entry, az_xu_arg_t arg);
extern az_r_t  az_xu_stop(az_ion_id_t id);
extern az_r_t az_xu_delete(az_ion_id_t id);

extern az_r_t az_xu_setPriority(az_ion_id_t id, az_xu_attr_t *pAttr);
extern az_r_t az_xu_getPriority(az_ion_id_t id, az_xu_attr_t *pAttr);
extern az_r_t az_xu_setAffinity(az_ion_id_t id, az_xu_core_mask_t core_mask);
extern az_r_t az_xu_getAffinity(az_ion_id_t id, az_xu_core_mask_t *pCoreMask);

extern az_r_t az_xu_suspend(az_ion_id_t id);
extern az_r_t az_xu_resume(az_ion_id_t id);
extern az_r_t az_xu_sleep(az_int64_t nsec);
extern az_r_t az_xu_sendEvent(az_ion_id_t id, az_xu_event_t event);
extern az_r_t az_xu_sendMsg(az_ion_id_t id, az_xu_msg_t msg);
extern az_r_t az_xu_recvMsg(az_xu_msg_t *msg);
extern az_r_t az_xu_recvEvent(az_xu_event_t toReceive, az_int8_t options, az_int64_t nsec, az_xu_event_t *pReceived);
extern az_r_t az_xu_waitEvent(az_xu_event_t toReceive, az_int8_t options, az_int64_t nsec, az_xu_event_t *pReceived);
extern az_r_t az_xu_regEventHandler(az_ion_id_t id, unsigned int vecno, az_xu_event_handler_t handler, az_xu_arg_t arg); 
extern az_r_t az_xu_deregEventHandler(az_ion_id_t id, unsigned int vecno);

extern az_xu_t az_xu_find(char *name);

extern void* az_xu_getarg();
extern az_r_t az_xu_getcpu();

extern void  az_xu_empty_sys_xu(az_xu_t xu);
extern az_xu_t  az_xu_empty(az_xu_t xu);

extern void az_xu_cleanup(az_xu_t xu);
extern void    az_xu_exit(az_xu_ret_t ret);

extern void *az_xu_entry(void *arg);

extern void  az_xu_set_cleanup();
extern void  az_xu_set_state_excpt(void *excpt_point);
extern void  az_xu_reset_state_excpt(az_xu_t xu);
#ifdef __cplusplus
}
#endif
#endif
