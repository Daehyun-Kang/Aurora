/**
 * @file   az_thread.c
 * @brief  
 * @date   10/04/16
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
 *   - 2016-04-10 fware initial create
 */

/* include header files */
#include "az_thread.h"
#include "az_trz.h"
#include "sys/az_sys_probe.h"

/* declare global variables */
#ifdef  CONFIG_AZ_USE_TLS
AZ_SYS_TLS  az_thread_t az_thread = NULL;
AZ_SYS_TLS  char    az_thread_prtbuf[CONFIG_AZ_THREAD_PRTBUF_SZ];
#endif

/* declare static variables */

/* implement static functions */

/* implement global functions */

/**
 * @fn        function name
 * @brief     function-description
 * @param     input-output-parameters
 * @return    return-value
 * @warning   warnings
 * @exception none
 */
az_ion_id_t az_thread_create(char *name, az_thread_entry_t entry, az_thread_arg_t arg, az_thread_config_t *pCfg, az_thread_t *pXu)
{
  az_r_t result = AZ_SUCCESS;
  az_thread_t xu = NULL;

  do {
    if (pXu) {
      xu = *pXu;
    }
    if (NULL == xu) {
      xu = (az_thread_t)az_malloc(sizeof(az_thread_entity_t));
      az_if_alloc_null_break(xu, result);
      if (pXu) *pXu = xu;
      az_ion_invalidate(&xu->ion, 0);
    } else {
      if (!AZ_ION_IS_IDLE_VALID(&xu->ion, AZ_ION_TYPE_THREAD)) {
        result = AZ_ERR(INVALID_ARG);
        break;
      }
    }
    az_assert(xu->ion.id == AZ_SYS_IO_INVALID);

    xu->entry = entry;
    xu->arg = arg;
#ifdef  CONFIG_AZ_TRACE
    xu->trace_info.depth = 0;
    xu->trace_info.callstack[0] = NULL;
#endif

    if (NULL == pCfg) {
      az_if_arg_null_break(name, result);
      az_if_arg_null_break(entry, result);
      xu->attr.stack_size = 0; 
      xu->attr.startdelay = 0; 
    } else {
      if (NULL == name) {
        name = pCfg->name;
      }
      if (NULL == entry) {
        result = az_dllib_getSymbol(pCfg->entry_name, (az_ref_t *)&xu->entry);
      } 
      if (NULL == arg) {
        if (pCfg->arg_name[0]) {
          result = az_dllib_getSymbol(pCfg->arg_name, (az_ref_t *)&xu->arg);
        }
      }
      if (result != AZ_SUCCESS) break;
      xu->attr.stack_size = pCfg->stackSize;
      xu->attr.sched_policy = pCfg->policy;
      xu->attr.priority = pCfg->priority;
      xu->attr.core_mask = pCfg->coremask;
      xu->attr.startdelay = pCfg->startdelay;
    }
    az_assert(NULL != xu->entry);
    
    xu->wait_thread_id = (az_thread_self() == NULL)? AZ_ION_ID_INVALID:az_thread_self()->ion.id;
    #if (CONFIG_AZ_THREAD_MSG_SZ == 64)
    az_ring_init(&xu->msgq, AZ_RING_TYPE_DS64, CONFIG_AZ_THREAD_MSG_QUEUE_SZ, xu->msgs);
    #else
    az_ring_init(&xu->msgq, AZ_RING_TYPE_DS32, CONFIG_AZ_THREAD_MSG_QUEUE_SZ, xu->msgs);
    #endif
    strncpy(AZ_THREAD_NAME(xu), name, sizeof(AZ_THREAD_NAME(xu)));
    xu->flags = AZ_THREAD_FLAGS_INI_VAL;
    xu->beams = 0;
    #ifdef  CONFIG_AZ_EMBED_THREAD_EVECTORS
    xu->beamtable = xu->_beamtable; 
    #else
    xu->beamtable = NULL;
    #endif
    az_link_initList(&xu->trz_list);
    result = az_ion_register(&(xu->ion), AZ_ION_TYPE_THREAD|AZ_ION_FLAG_AUTOALLOC);
    if (AZ_SUCCESS != result) {
      if (AZ_REFCOUNT_IS_ZERO(&xu->ion.refCount)) {
        az_free(xu);
        if (pXu) *pXu = NULL;
      }
      break;
    } else {
      if (pCfg) {
        xu->ion.tag = pCfg->tag;
        if (pCfg->flags) {
          xu->flags = (pCfg->flags & AZ_THREAD_FLAGS_MASK);
        }
      }
      result = (az_r_t)xu->ion.id;
    }

#ifdef  CONFIG_AZ_EMBED_SYS_THREAD
    xu->sys_xu = &(xu->_sys_xu); 
#else
    xu->sys_xu = NULL; 
#endif
    xu->state = AZ_THREAD_STATE_CREATED;

    if (xu->attr.startdelay <= 0) {
      result = az_thread_start(xu->ion.id, NULL, NULL);
      if (AZ_SUCCESS != result) {
        if (AZ_REFCOUNT_VALUE(&xu->ion.refCount) > 0) {
          az_ion_deregister(&(xu->ion));
        }
        if (AZ_REFCOUNT_VALUE(&xu->ion.refCount) == 0) {
          xu = az_thread_empty(xu);
        }
        if (pXu) *pXu = NULL;
        break;
      } else {
        result = (az_r_t)xu->ion.id;
      }
    }
  } while (0);

  if (xu) {
    if (result < 0) { 
      az_rprintf(result, "id:%d ref count %d\n", xu->ion.id, AZ_REFCOUNT_VALUE(&xu->ion.refCount));
    }
  } else {
    az_rprintf(result, "id:%d \n", (az_ion_id_t)result); 
  }
  return (az_ion_id_t)result;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
az_r_t az_thread_start(az_ion_id_t id, az_thread_entry_t entry, az_thread_arg_t arg)
{
  az_r_t result = AZ_SUCCESS;
  az_thread_t xu = (az_thread_t)az_ion(id);

  do {
    if (xu == NULL) {
      result = AZ_ERR(ENTITY_NULL);
      break;
    }
    az_assert_ion_type(xu->ion.type, AZ_ION_TYPE_THREAD);
    if (xu->state != AZ_THREAD_STATE_CREATED) {
      result = AZ_ERR(STATE);
      break;
    }
    if (xu->state & AZ_THREAD_STATE_START) {
      result = AZ_ERR(AGAIN);
      break;
    }
    if (NULL != entry) xu->entry = entry;
    if (NULL != arg) xu->arg = arg;

    az_refcount_atomic_inc(&xu->ion.refCount);
    if (xu->attr.startdelay < 0) {
      result = az_sys_xu_init(AZ_THREAD_NAME(xu), az_thread_entry, xu, &(xu->attr), &(xu->sys_xu)); 
    } else {
      result = az_sys_xu_create(AZ_THREAD_NAME(xu), az_thread_entry, xu, &(xu->attr), &(xu->sys_xu)); 
    }
    //az_sys_printf("xu=%p sys_xu=%p, arg=%p result=%ld\n", xu, xu->sys_xu, arg, result);

    if (AZ_SUCCESS == result) {
      if (AZ_THREAD_ON_EXCPT(xu)) {
        if (AZ_REFCOUNT_VALUE(&xu->ion.refCount) > 0) {
          az_ion_deregister(xu);
        }
        if (AZ_REFCOUNT_VALUE(&xu->ion.refCount) == 0) {
          xu = az_thread_empty(xu);
        }
        result = AZ_ERR(EXCEPTION);
      } else {
        xu->state |= AZ_THREAD_STATE_START;
      }
    } else {
      az_refcount_atomic_dec(&xu->ion.refCount);
      xu->state |= AZ_THREAD_STATE_ERROR;
      xu->cause = AZ_THREAD_ERROR_START;
    }
  } while (0);

  if (xu) {
    if (result < 0) { 
      az_rprintf(result, "id:%d ref count %d\n", id, AZ_REFCOUNT_VALUE(&xu->ion.refCount));
    }
  } else {
    az_rprintf(result, "id:%d\n", id);
  }
  return result;
}

void  az_thread_empty_sys_xu(az_thread_t xu)
{
  az_sys_xu_t sys_xu = xu->sys_xu;
  az_assert(xu->state & AZ_THREAD_STATE_STOP);
  az_trz_flush(&xu->trz_list, NULL);
  if (xu->sys_xu != NULL) {
    az_r_t result = az_sys_xu_delete(sys_xu);
    if (AZ_SUCCESS == result) {
      xu->sys_xu = NULL;
    }
  }
}
/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
az_r_t  az_thread_stop(az_ion_id_t  id)
{
  az_r_t  result = AZ_SUCCESS;
  az_thread_t xu = (az_thread_t)az_ion(id);

  do {
    if (xu == NULL) {
      result = AZ_ERR(ENTITY_NULL);
      break;
    }
    az_assert_ion_type(xu->ion.type, AZ_ION_TYPE_THREAD);
    if (!(xu->state & AZ_THREAD_STATE_START)) {
      result = AZ_ERR(STATE);
      break;
    }
    if (xu->state & AZ_THREAD_STATE_STOP) {
      result = AZ_ERR(AGAIN);
      break;
    }
    az_sys_xu_t sys_xu = xu->sys_xu;

    xu->state |= AZ_THREAD_STATE_STOP;
    if (result != AZ_SUCCESS) {
      xu->state |= AZ_THREAD_STATE_ERROR;
      xu->cause = AZ_THREAD_ERROR_STOP|AZ_THREAD_ERROR_SNDEVT;
      break;
    } else {
      xu->state &= ~AZ_THREAD_STATE_START;
      az_refcount_atomic_dec(&xu->ion.refCount);
    }
    if (AZ_REFCOUNT_VALUE(&xu->ion.refCount) == 1) {
      az_thread_empty_sys_xu(xu);
    }
  } while (0);

  az_rprintf(result, "id:%d ref count %d\n", xu->ion.id, AZ_REFCOUNT_VALUE(&xu->ion.refCount));
  return result;
}

az_thread_t  az_thread_empty(az_thread_t xu)
{
  az_sys_xu_t     sys_xu = xu->sys_xu;

  #ifndef  CONFIG_AZ_EMBED_SYS_THREAD
  xu->sys_xu = NULL;
  #endif
  
  az_thread_t save_thread = xu;
  if (save_thread == az_thread_self()) {
    az_thread_null();
    if (AZ_REFCOUNT_IS_ZERO(&xu->ion.refCount) == 0) {
      az_free(xu);
      xu = NULL;
    }
    az_sys_xu_exit(NULL);
  } else {
    if (sys_xu) sys_xu->arg = NULL;
    if (AZ_REFCOUNT_IS_ZERO(&xu->ion.refCount) == 0) {
      az_free(xu);
      xu = NULL;
    }
    if (sys_xu) az_sys_xu_delete(sys_xu);
  }
  return xu;
}
/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
az_r_t az_thread_delete(az_ion_id_t id)
{
  az_r_t result = AZ_SUCCESS;
  az_thread_t xu = (az_thread_t)az_ion(id);

  do {
    if (xu == NULL) {
      result = AZ_ERR(ENTITY_NULL);
      break;
    }
    az_assert_ion_type(xu->ion.type, AZ_ION_TYPE_THREAD);
    if (!(xu->state &AZ_THREAD_STATE_CREATED)) {
      result = AZ_ERR(STATE);
      break;
    }
    if (!(xu->state &AZ_THREAD_STATE_STOP)) {
      result = AZ_ERR(STATE);
      break;
    }
	  if (xu == az_thread_self()) {
      result = AZ_ERR(INVALID);
      break;
    }
    if (AZ_REFCOUNT_VALUE(&xu->ion.refCount) < 1) {
      result = AZ_ERR(AGAIN);
      break;
    }

    az_trz_flush(&xu->trz_list, NULL);

    az_eprintf("id:%d ref count %d\n", xu->ion.id, AZ_REFCOUNT_VALUE(&xu->ion.refCount));

    az_sys_xu_t     sys_xu = xu->sys_xu;

    if (AZ_REFCOUNT_VALUE(&xu->ion.refCount) > 0) {
      az_ion_deregister(&(xu->ion));
    }

    if (AZ_REFCOUNT_VALUE(&xu->ion.refCount) == 0) {
      xu = az_thread_empty(xu);
    } else {
    
      az_eprintf("id:%d ref count %d\n", xu->ion.id, AZ_REFCOUNT_VALUE(&xu->ion.refCount));
      fflush(stdout);
    }
  } while (0);

  if (result) {
   az_rprintf(result, "id:%d ref count %d\n", xu->ion.id, AZ_REFCOUNT_VALUE(&xu->ion.refCount));
  }
  return result;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
az_r_t  az_thread_setPriority(az_ion_id_t id, az_thread_attr_t *pAttr)
{
  az_r_t result = AZ_SUCCESS;
  az_thread_t xu = (az_thread_t)az_ion(id);
  do {
    az_if_arg_null_break(xu, result);
    az_assert_ion_type(xu->ion.type, AZ_ION_TYPE_THREAD);
    if (AZ_THREAD_ON_EXCPT(xu)) {
      result = AZ_ERR(EXCEPTION);
    }

    result = az_sys_xu_setPriority(xu->sys_xu, pAttr);
  } while (0);

  return result;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
az_r_t  az_thread_getPriority(az_ion_id_t id, az_thread_attr_t *pAttr)
{
  az_r_t result = AZ_SUCCESS;
  az_thread_t xu = (az_thread_t)az_ion(id);
  do {
    az_if_arg_null_break(xu, result);
    az_assert_ion_type(xu->ion.type, AZ_ION_TYPE_THREAD);
    if (AZ_THREAD_ON_EXCPT(xu)) {
      result = AZ_ERR(EXCEPTION);
    }

    result = az_sys_xu_getPriority(xu->sys_xu, pAttr);
  } while (0);

  return result;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
az_r_t  az_thread_setAffinity(az_ion_id_t id, az_thread_core_mask_t core_mask)
{
  az_r_t result = AZ_SUCCESS;
  az_thread_t xu = (az_thread_t)az_ion(id);
  do {
    az_if_arg_null_break(xu, result);
    az_assert_ion_type(xu->ion.type, AZ_ION_TYPE_THREAD);
    if (AZ_THREAD_ON_EXCPT(xu)) {
      result = AZ_ERR(EXCEPTION);
    }

    result = az_sys_xu_setAffinity(xu->sys_xu, core_mask);
  } while (0);

  return result;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
az_r_t  az_thread_getAffinity(az_ion_id_t id, az_thread_core_mask_t *pCoreMask)
{
  az_r_t result = AZ_SUCCESS;
  az_thread_t xu = (az_thread_t)az_ion(id);
  do {
    az_if_arg_null_break(xu, result);
    az_assert_ion_type(xu->ion.type, AZ_ION_TYPE_THREAD);
    if (AZ_THREAD_ON_EXCPT(xu)) {
      result = AZ_ERR(EXCEPTION);
    }

    result = az_sys_xu_getAffinity(xu->sys_xu, pCoreMask);
  } while (0);

  return result;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
az_r_t  az_thread_getcpu()
{
  return az_sys_xu_getcpu();
}

#ifndef  CONFIG_AZ_USE_TLS
/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
az_thread_t az_thread_self()
{
  az_thread_t xu = NULL;
  az_sys_xu_t sys_xu;

  sys_xu = az_sys_xu_self();
  if (NULL != sys_xu) {
    xu = (az_thread_t)(sys_xu->arg);
  }

  return xu;
}
void az_thread_null()
{
  az_thread_t xu = NULL;
  az_sys_xu_t sys_xu;

  sys_xu = az_sys_xu_self();
  if (NULL != sys_xu) {
     sys_xu->arg = NULL;
  }
}
az_thread_trace_info_t *az_thread_traceinfo()
{
  az_thread_t xu = az_thread_self();
  return ((xu == NULL)? NULL:&xu->trace_info);
}
#endif

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
void *az_thread_getarg()
{
  az_thread_t xu = az_thread_self();
  void *arg = NULL;

  if (NULL != xu) {
     arg = (xu->arg);
  }
  //az_sys_printf("xu=%p arg=%p\n", xu, xu->arg);

  return arg;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
az_r_t az_thread_suspend(az_ion_id_t id)
{
  az_r_t result = AZ_SUCCESS;
  az_thread_t xu = (az_thread_t)az_ion(id);
  do {
    az_if_arg_null_break(xu, result);
    az_assert_ion_type(xu->ion.type, AZ_ION_TYPE_THREAD);
    if (AZ_THREAD_ON_EXCPT(xu)) {
      result = AZ_ERR(EXCEPTION);
    }

    result = az_sys_xu_suspend(xu->sys_xu);
  } while (0);

  return result;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
az_r_t az_thread_resume(az_ion_id_t id)
{
  az_r_t result = AZ_SUCCESS;
  az_thread_t xu = (az_thread_t)az_ion(id);
  do {
    az_if_arg_null_break(xu, result);
    az_assert_ion_type(xu->ion.type, AZ_ION_TYPE_THREAD);
    if (AZ_THREAD_ON_EXCPT(xu)) {
      result = AZ_ERR(EXCEPTION);
    }

    result = az_sys_xu_resume(xu->sys_xu); 
  } while (0);

  return result;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
az_r_t az_thread_sleep(az_int64_t nsec)
{
  AZ_PROBE_DEC_SYS(THREAD, 1);
  az_r_t r = az_sys_xu_sleep(nsec);
  AZ_PROBE_INC_SYS(THREAD, 1);

  return r;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
void az_thread_cleanup(az_thread_t xu)
{
  az_assert(NULL != xu);

  #ifdef CONFIG_AZ_DEBUG_NATIVE_APP  
  do {
	  if (NULL == xu) break;
    //az_sys_xu_t     sys_xu = xu->sys_xu;

    if (xu->state & AZ_THREAD_STATE_START) {
      xu->state |= AZ_THREAD_STATE_STOP;
      az_refcount_atomic_dec(&xu->ion.refCount);
    } 

    az_trz_flush(&xu->trz_list, NULL);

    if (AZ_REFCOUNT_VALUE(&xu->ion.refCount) <= 0) {
      break;
    }

    az_sys_eprintf("id:%d ref count %d\n", xu->ion.id, AZ_REFCOUNT_VALUE(&xu->ion));

    az_ion_deregister(&(xu->ion));

    if (AZ_REFCOUNT_VALUE(&xu->ion.refCount) == 0) {
      xu->sys_xu = NULL;
      az_thread_null();
      if (AZ_REFCOUNT_IS_ZERO(&xu->ion.refCount)) {
        az_free(xu);
      }
      az_sys_xu_cleanup();
    } else {
      az_sys_eprintf("id:%d ref count %d\n", xu->ion.id, AZ_REFCOUNT_VALUE(&xu->ion.refCount));
      fflush(stdout);
    }
  } while (0);
  #endif
}
/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
void az_thread_exit(az_thread_ret_t ret)
{
  do {
	  az_thread_t xu = az_thread_self();

	  if (NULL == xu) break;
    //az_sys_xu_t     sys_xu = xu->sys_xu;

    if (xu->state & AZ_THREAD_STATE_START) {
      xu->state |= AZ_THREAD_STATE_STOP;
      xu->state &= ~AZ_THREAD_STATE_START;
      az_refcount_atomic_dec(&xu->ion.refCount);
    }

    az_trz_flush(&xu->trz_list, NULL);

    az_eprintf("id:%d ref count %d\n", xu->ion.id, AZ_REFCOUNT_VALUE(&xu->ion));

    if (AZ_REFCOUNT_VALUE(&xu->ion.refCount) > 0) {
      az_ion_deregister(&(xu->ion));
    }

    if (AZ_REFCOUNT_VALUE(&xu->ion.refCount) == 0) {
      xu->sys_xu = NULL;
      az_thread_null();
      if (AZ_REFCOUNT_IS_ZERO(&xu->ion.refCount)) {
        az_free(xu);
      }
      az_sys_xu_exit((void *)ret);
    } else {
      az_eprintf("id:%d ref count %d\n", xu->ion.id, AZ_REFCOUNT_VALUE(&xu->ion.refCount));
      fflush(stdout);
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
az_r_t az_thread_send_beam(az_ion_id_t id, az_thread_beam_t event)
{
  az_r_t r = AZ_SUCCESS;
  az_thread_t xu = (az_thread_t)az_ion(id);
  do {
    az_if_arg_null_break(xu, r);
    az_assert_ion_type(xu->ion.type, AZ_ION_TYPE_THREAD);

    // need to use lock not free xu by others
    if (AZ_THREAD_ON_EXCPT(xu)) {
      r = AZ_ERR(EXCEPTION);
      break;
    }

    if (xu->state & AZ_THREAD_STATE_STOP) {
      r = AZ_ERR(STATE);
      break;
    }

    az_refcount_atomic_inc(&xu->ion.refCount);

    r = az_sys_xu_sendEvent(xu->sys_xu, event); 

    if (xu->state & (AZ_THREAD_STATE_STOP|AZ_THREAD_STATE_EXCPT)) {
      if  (az_refcount_atomic_dec(&xu->ion.refCount) == 0) {
        az_ion_empty(&xu->ion);
        az_thread_empty(xu);
      } else if (AZ_REFCOUNT_VALUE(&xu->ion.refCount) == 1) {
        az_thread_empty_sys_xu(xu);
      }
    } else {
      az_refcount_atomic_dec(&xu->ion.refCount);
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
az_r_t az_thread_send_msg(az_ion_id_t id, az_thread_msg_t msg)
{
  az_r_t r = AZ_SUCCESS;
  az_thread_t xu = (az_thread_t)az_ion(id);
  do {
    if (msg == 0) {
      r = AZ_ERR(EMPTY);
      break;
    }
    az_if_arg_null_break(xu, r);
    az_assert_ion_type(xu->ion.type, AZ_ION_TYPE_THREAD);

    // need to use lock not free xu by others
    if (AZ_THREAD_ON_EXCPT(xu)) {
      r = AZ_ERR(EXCEPTION);
      break;
    }

    if (xu->state & AZ_THREAD_STATE_STOP) {
      r = AZ_ERR(STATE);
      break;
    }

    az_refcount_atomic_inc(&xu->ion.refCount);
    #if (CONFIG_AZ_THREAD_MSG_SZ == 64)
    r = az_ring_push64(&xu->msgq, &msg);
    #else
    r = az_ring_push32(&xu->msgq, &msg);
    #endif
    if (r < 0) {
      break;
    }
    r = az_sys_xu_sendEvent(xu->sys_xu, AZ_THREAD_BEAM_MSG); 
    if (xu->state & (AZ_THREAD_STATE_STOP|AZ_THREAD_STATE_EXCPT)) {
      if  (az_refcount_atomic_dec(&xu->ion.refCount) == 0) {
        az_ion_empty(&xu->ion);
        az_thread_empty(xu);
      } else if (AZ_REFCOUNT_VALUE(&xu->ion.refCount) == 1) {
        az_thread_empty_sys_xu(xu);
      }
    } else {
      az_refcount_atomic_dec(&xu->ion.refCount);
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
az_r_t az_thread_recv_msg(az_thread_msg_t *pMsg)
{
  az_r_t r = AZ_SUCCESS;
  az_thread_t xu = az_thread_self(); 
  az_assert(NULL != pMsg);
  do {
    if (NULL == xu) {
      r = AZ_ERR(ENTITY_NULL);
      break;
    }
    #if (CONFIG_AZ_THREAD_MSG_SZ == 64)
    r = az_ring_pop64(&xu->msgq, pMsg);
    #else
    r = az_ring_push32(&xu->msgq, pMsg);
    #endif

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
az_r_t az_thread_recv_beam(az_thread_beam_t toReceive, az_int8_t options,
    az_int64_t nsec, az_thread_beam_t *pReceived)
{
  AZ_PROBE_DEC_SYS(THREAD, 1);
  az_r_t r =  az_sys_xu_recvEvent(toReceive, options, nsec, pReceived); 
  AZ_PROBE_INC_SYS(THREAD, 1);
  return r;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
az_r_t az_thread_wait_beam(az_thread_beam_t toReceive, az_int8_t options,
    az_int64_t nsec, az_thread_beam_t *pReceived)
{
  az_thread_t xu = az_thread_self();
  az_thread_beam_t revents = az_sys_xu_readEvent(xu->beams);
  az_thread_beam_t evecmask;
  struct az_thread_beam *beam; 
  int j, beamcount;
  if (revents) {
    evecmask = 1; beamcount = xu->beamcount;
    for (j = 0, evecmask = 1; j < AZ_THREAD_BEAM_VECTORS && beamcount > 0; j++) {
      if (evecmask & revents) {
        beamcount--;
        beam = &xu->beamtable[j];
        (beam->handler)(j, beam->arg);
      }
      evecmask <<= 1;
    }
  }

  AZ_PROBE_DEC_SYS(THREAD, 1);
  az_r_t r =  az_sys_xu_waitEvent(xu->beams, toReceive, options, nsec, &revents, pReceived); 
  AZ_PROBE_INC_SYS(THREAD, 1);

  if (revents) {
    evecmask = 1; beamcount = xu->beamcount;
    for (j = 0, evecmask = 1; j < AZ_THREAD_BEAM_VECTORS && beamcount > 0; j++) {
      if (evecmask & revents) {
        beamcount--;
        beam = &xu->beamtable[j];
        (beam->handler)(j, beam->arg);
      }
      evecmask <<= 1;
    }
  }
  return r;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    non
 */
az_r_t az_thread_regBeamHandler(az_ion_id_t id, unsigned int vecno, az_thread_beam_handler_t handler, az_thread_arg_t arg)
{
  az_r_t r = AZ_FAIL;
  az_thread_t xu = (az_thread_t)az_ion(id);
  do {
    az_if_arg_null_break(xu, r);
    az_assert_ion_type(xu->ion.type, AZ_ION_TYPE_THREAD);
    if (AZ_THREAD_ON_EXCPT(xu)) {
      r = AZ_ERR(EXCEPTION);
    }

    if (vecno > AZ_THREAD_BEAM_VECTORS) {
      r = AZ_ERR_L(OOR, 0);
      break;
    }
#ifndef  CONFIG_AZ_EMBED_THREAD_EVECTORS
    if (xu->beamtable == NULL) {
      xu->beamtable = az_malloc(sizeof(struct  az_thread_beam) * AZ_THREAD_BEAM_VECTORS);
      if (xu->beamtable == NULL) {
        r = AZ_ERR_L(MALLOC, 0);
        break;
      }
    }
#endif
    if (xu->beamtable[vecno].handler) {
      r = AZ_ERR_L(AGAIN, 0);
    }
    xu->beamtable[vecno].handler = handler;
    xu->beamtable[vecno].arg = arg;
    xu->beamcount++;
    xu->beams |= (1 << vecno);
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
az_r_t az_thread_deregBeamHandler(az_ion_id_t id, unsigned int vecno)
{
  az_r_t r = AZ_FAIL;
  az_thread_t xu = (az_thread_t)az_ion(id);
  do {
    az_if_arg_null_break(xu, r);
    az_assert_ion_type(xu->ion.type, AZ_ION_TYPE_THREAD);
    if (AZ_THREAD_ON_EXCPT(xu)) {
      r = AZ_ERR(EXCEPTION);
    }
    if (vecno > AZ_THREAD_BEAM_VECTORS) {
      r = AZ_ERR_L(OOR, 0);
      break;
    }
    az_assert(NULL != xu->beamtable);
    if (NULL == xu->beamtable[vecno].handler) {
      r = AZ_ERR_L(AGAIN, 0);
    }
    xu->beams &= ~(1 << vecno);
    xu->beamcount--;
    xu->beamtable[vecno].handler = NULL; 
    xu->beamtable[vecno].arg = NULL;
  } while (0);

  return r;
}

int az_thread_comparebyname(char *name, az_thread_t xu)
{
  int r = 0;
  do {
    if (NULL == name) { r = -1; break; }
    if (NULL == xu) { r = -2; break; }
    //az_sys_eprintf("%s %s type:%d %d\n", name, xu->name, xu->ion.type, AZ_ION_TYPE_THREAD);
    if (xu->ion.type != AZ_ION_TYPE_THREAD) {
      r = 1; 
      break;
    }
    r = strcmp(name, AZ_THREAD_NAME(xu));
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
az_thread_id_t az_thread_find(char *name) 
{
  az_thread_t xu =  (az_thread_t)az_ion_find(name, (az_ion_compare_t)az_thread_comparebyname);
  //az_sys_eprintf("%p \n", xu); 
  return ((xu == NULL)? AZ_THREAD_ID_INVALID:xu->ion.id);
}


/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */

void *az_thread_entry(void *arg)
{
  volatile az_thread_t xu = (az_thread_t)arg;
  volatile void *ret = NULL;
  az_assert(NULL != xu);

  #ifdef  CONFIG_AZ_USE_TLS
  az_thread = xu;
  #endif

  AZ_PROBE_INC_SYS(THREAD, 1);
  #ifdef  CONFIG_AZ_THREAD_EXP_HANDLE
  az_sys_xu_register_exception_handler();
  
  if (0 == AZ_SYS_XU_SAVE_CONTEXT()) {
  #endif
   do {
    /*
     * allocate resources...
     */

    xu->state |= AZ_THREAD_STATE_BUSY;

    ret = (xu->entry)(xu->arg);

    xu->state &= ~AZ_THREAD_STATE_BUSY;

    /*
     * release resources...
     */

   } while (0);
  #ifdef  CONFIG_AZ_THREAD_EXP_HANDLE
   az_sys_xu_remove_context();
  } else {
    az_thread_send_msg(xu->wait_thread_id, AZ_THREAD_MSG_EXCEPTION|xu->ion.id);
    az_sys_dlog("%s\n", AZ_THREAD_NAME(xu));
  }
  #endif
  AZ_PROBE_DEC_SYS(THREAD, 1);

  az_thread_exit(ret);

  return ret;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
void  az_thread_set_cleanup()
{
#ifdef  CONFIG_AZ_TRACE_PTHREAD
  az_thread_t xu = az_thread_self();
  if (xu != NULL) {
    pthread_cleanup_push (az_thread_cleanup, xu);
  }

#endif
}

void  az_thread_set_state_excpt(void *excpt_point)
{
  az_thread_t xu = az_thread_self();
  if (xu != NULL) {
    xu->state |= AZ_THREAD_STATE_EXCPT; 
    xu->excpt_point = excpt_point;
  }
}
void  az_thread_reset_state_excpt(az_thread_t xu)
{
  if (xu != NULL) {
    xu->state &= ~AZ_THREAD_STATE_EXCPT; 
  }
}
/* === end of AZ_THREAD_C === */
