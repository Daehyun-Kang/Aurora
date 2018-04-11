/**
 * @file   az_xu.c
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
#include "az_xu.h"
#include "az_trz.h"
#include "mon/az_probe.h"

/* declare global variables */
#ifdef  CONFIG_AZ_USE_TLS
AZ_SYS_TLS  az_xu_t az_xu = NULL;
AZ_SYS_TLS  char    az_xu_prtbuf[CONFIG_AZ_XU_PRTBUF_SZ];
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
az_ion_id_t az_xu_create(char *name, az_xu_entry_t entry, az_xu_arg_t arg, az_xu_config_t *pCfg, az_xu_t *pXu)
{
  az_r_t result = AZ_SUCCESS;
  az_xu_t xu;
  az_assert(NULL != pXu);

  do {
    xu = *pXu;
    if (NULL == xu) {
      xu = (az_xu_t)az_malloc(sizeof(az_xu_entity_t));
      az_if_alloc_null_break(xu, result);
      *pXu = xu;
      az_ion_invalidate(&xu->ion, 0);
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
    
    xu->wait_xu_id = (az_xu_self() == NULL)? AZ_ION_ID_INVALID:az_xu_self()->ion.id;
    #if (CONFIG_AZ_XU_MSG_SZ == 64)
    az_ring_init(&xu->msgq, AZ_RING_TYPE_DS64, CONFIG_AZ_XU_MSG_QUEUE_SZ, xu->msgs);
    #else
    az_ring_init(&xu->msgq, AZ_RING_TYPE_DS32, CONFIG_AZ_XU_MSG_QUEUE_SZ, xu->msgs);
    #endif
    strncpy(AZ_XU_NAME(xu), name, sizeof(AZ_XU_NAME(xu)));
    xu->flags = AZ_XU_FLAGS_INI_VAL;
    xu->evectors = 0;
    #ifdef  CONFIG_AZ_EMBED_XU_EVECTORS
    xu->evectable = xu->_evectable; 
    #else
    xu->evectable = NULL;
    #endif
    az_link_initList(&xu->trz_list);
    result = az_ion_register(&(xu->ion), AZ_ION_TYPE_XU|AZ_ION_FLAG_AUTOALLOC);
    if (AZ_SUCCESS != result) {
      if (AZ_REFCOUNT_IS_ZERO(&xu->ion.refCount)) {
        az_free(xu);
        *pXu = NULL;
      }
      break;
    } else {
      result = (az_r_t)xu->ion.id;
    }

#ifdef  CONFIG_AZ_EMBED_SYS_XU
    xu->sys_xu = &(xu->_sys_xu); 
#else
    xu->sys_xu = NULL; 
#endif
    xu->state = AZ_XU_STATE_CREATED;

    if (xu->attr.startdelay <= 0) {
      result = az_xu_start(xu->ion.id, NULL, NULL);
      if (AZ_SUCCESS != result) {
        if (AZ_REFCOUNT_VALUE(&xu->ion.refCount) > 0) {
          az_ion_deregister(&(xu->ion));
        }
        if (AZ_REFCOUNT_VALUE(&xu->ion.refCount) == 0) {
          xu = az_xu_empty(xu);
        }
        *pXu = NULL;
        break;
      } else {
        result = (az_r_t)xu->ion.id;
      }
    }
  } while (0);

  if (xu) {
    az_rprintf(result, "id:%d ref count %d\n", xu->ion.id, AZ_REFCOUNT_VALUE(&xu->ion.refCount));
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
az_r_t az_xu_start(az_ion_id_t id, az_xu_entry_t entry, az_xu_arg_t arg)
{
  az_r_t result = AZ_SUCCESS;
  az_xu_t xu = (az_xu_t)az_ion(id);

  do {
    if (xu == NULL) {
      result = AZ_ERR(ENTITY_NULL);
      break;
    }
    az_assert_ion_type(xu->ion.type, AZ_ION_TYPE_XU);
    if (xu->state != AZ_XU_STATE_CREATED) {
      result = AZ_ERR(STATE);
      break;
    }
    if (xu->state & AZ_XU_STATE_START) {
      result = AZ_ERR(AGAIN);
      break;
    }
    if (NULL != entry) xu->entry = entry;
    if (NULL != arg) xu->arg = arg;

    az_refcount_atomic_inc(&xu->ion.refCount);
    if (xu->attr.startdelay < 0) {
      result = az_sys_xu_init(AZ_XU_NAME(xu), az_xu_entry, xu, &(xu->attr), &(xu->sys_xu)); 
    } else {
      result = az_sys_xu_create(AZ_XU_NAME(xu), az_xu_entry, xu, &(xu->attr), &(xu->sys_xu)); 
    }
    //az_sys_printf("xu=%p sys_xu=%p, arg=%p result=%ld\n", xu, xu->sys_xu, arg, result);

    if (AZ_SUCCESS == result) {
      if (AZ_XU_ON_EXCPT(xu)) {
        if (AZ_REFCOUNT_VALUE(&xu->ion.refCount) > 0) {
          az_ion_deregister(xu);
        }
        if (AZ_REFCOUNT_VALUE(&xu->ion.refCount) == 0) {
          xu = az_xu_empty(xu);
        }
        result = AZ_ERR(EXCEPTION);
      } else {
        xu->state |= AZ_XU_STATE_START;
      }
    } else {
      az_refcount_atomic_dec(&xu->ion.refCount);
      xu->state |= AZ_XU_STATE_ERROR;
      xu->cause = AZ_XU_ERROR_START;
    }
  } while (0);

  if (xu) {
    az_rprintf(result, "id:%d ref count %d\n", id, AZ_REFCOUNT_VALUE(&xu->ion.refCount));
  } else {
    az_rprintf(result, "id:%d\n", id);
  }
  return result;
}

void  az_xu_empty_sys_xu(az_xu_t xu)
{
  az_sys_xu_t sys_xu = xu->sys_xu;
  az_assert(xu->state & AZ_XU_STATE_STOP);
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
az_r_t  az_xu_stop(az_ion_id_t  id)
{
  az_r_t  result = AZ_SUCCESS;
  az_xu_t xu = (az_xu_t)az_ion(id);

  do {
    if (xu == NULL) {
      result = AZ_ERR(ENTITY_NULL);
      break;
    }
    az_assert_ion_type(xu->ion.type, AZ_ION_TYPE_XU);
    if (!(xu->state & AZ_XU_STATE_START)) {
      result = AZ_ERR(STATE);
      break;
    }
    if (xu->state & AZ_XU_STATE_STOP) {
      result = AZ_ERR(AGAIN);
      break;
    }
    az_sys_xu_t sys_xu = xu->sys_xu;

    xu->state |= AZ_XU_STATE_STOP;
    if (result != AZ_SUCCESS) {
      xu->state |= AZ_XU_STATE_ERROR;
      xu->cause = AZ_XU_ERROR_STOP|AZ_XU_ERROR_SNDEVT;
      break;
    } else {
      xu->state &= ~AZ_XU_STATE_START;
      az_refcount_atomic_dec(&xu->ion.refCount);
    }
    if (AZ_REFCOUNT_VALUE(&xu->ion.refCount) == 1) {
      az_xu_empty_sys_xu(xu);
    }
  } while (0);

  az_rprintf(result, "id:%d ref count %d\n", xu->ion.id, AZ_REFCOUNT_VALUE(&xu->ion.refCount));
  return result;
}

az_xu_t  az_xu_empty(az_xu_t xu)
{
  az_sys_xu_t     sys_xu = xu->sys_xu;

  #ifndef  CONFIG_AZ_EMBED_SYS_XU
  xu->sys_xu = NULL;
  #endif
  
  az_xu_t save_xu = xu;
  if (save_xu == az_xu_self()) {
    az_xu_null();
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
az_r_t az_xu_delete(az_ion_id_t id)
{
  az_r_t result = AZ_SUCCESS;
  az_xu_t xu = (az_xu_t)az_ion(id);

  do {
    if (xu == NULL) {
      result = AZ_ERR(ENTITY_NULL);
      break;
    }
    az_assert_ion_type(xu->ion.type, AZ_ION_TYPE_XU);
    if (!(xu->state &AZ_XU_STATE_CREATED)) {
      result = AZ_ERR(STATE);
      break;
    }
    if (!(xu->state &AZ_XU_STATE_STOP)) {
      result = AZ_ERR(STATE);
      break;
    }
	  if (xu == az_xu_self()) {
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
      xu = az_xu_empty(xu);
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
az_r_t  az_xu_setPriority(az_ion_id_t id, az_xu_attr_t *pAttr)
{
  az_r_t result = AZ_SUCCESS;
  az_xu_t xu = (az_xu_t)az_ion(id);
  do {
    az_if_arg_null_break(xu, result);
    az_assert_ion_type(xu->ion.type, AZ_ION_TYPE_XU);
    if (AZ_XU_ON_EXCPT(xu)) {
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
az_r_t  az_xu_getPriority(az_ion_id_t id, az_xu_attr_t *pAttr)
{
  az_r_t result = AZ_SUCCESS;
  az_xu_t xu = (az_xu_t)az_ion(id);
  do {
    az_if_arg_null_break(xu, result);
    az_assert_ion_type(xu->ion.type, AZ_ION_TYPE_XU);
    if (AZ_XU_ON_EXCPT(xu)) {
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
az_r_t  az_xu_setAffinity(az_ion_id_t id, az_xu_core_mask_t core_mask)
{
  az_r_t result = AZ_SUCCESS;
  az_xu_t xu = (az_xu_t)az_ion(id);
  do {
    az_if_arg_null_break(xu, result);
    az_assert_ion_type(xu->ion.type, AZ_ION_TYPE_XU);
    if (AZ_XU_ON_EXCPT(xu)) {
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
az_r_t  az_xu_getAffinity(az_ion_id_t id, az_xu_core_mask_t *pCoreMask)
{
  az_r_t result = AZ_SUCCESS;
  az_xu_t xu = (az_xu_t)az_ion(id);
  do {
    az_if_arg_null_break(xu, result);
    az_assert_ion_type(xu->ion.type, AZ_ION_TYPE_XU);
    if (AZ_XU_ON_EXCPT(xu)) {
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
az_r_t  az_xu_getcpu()
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
az_xu_t az_xu_self()
{
  az_xu_t xu = NULL;
  az_sys_xu_t sys_xu;

  sys_xu = az_sys_xu_self();
  if (NULL != sys_xu) {
    xu = (az_xu_t)(sys_xu->arg);
  }

  return xu;
}
void az_xu_null()
{
  az_xu_t xu = NULL;
  az_sys_xu_t sys_xu;

  sys_xu = az_sys_xu_self();
  if (NULL != sys_xu) {
     sys_xu->arg = NULL;
  }
}
az_xu_trace_info_t *az_xu_traceinfo()
{
  az_xu_t xu = az_xu_self();
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
void *az_xu_getarg()
{
  az_xu_t xu = az_xu_self();
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
az_r_t az_xu_suspend(az_ion_id_t id)
{
  az_r_t result = AZ_SUCCESS;
  az_xu_t xu = (az_xu_t)az_ion(id);
  do {
    az_if_arg_null_break(xu, result);
    az_assert_ion_type(xu->ion.type, AZ_ION_TYPE_XU);
    if (AZ_XU_ON_EXCPT(xu)) {
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
az_r_t az_xu_resume(az_ion_id_t id)
{
  az_r_t result = AZ_SUCCESS;
  az_xu_t xu = (az_xu_t)az_ion(id);
  do {
    az_if_arg_null_break(xu, result);
    az_assert_ion_type(xu->ion.type, AZ_ION_TYPE_XU);
    if (AZ_XU_ON_EXCPT(xu)) {
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
az_r_t az_xu_sleep(az_int64_t nsec)
{
  AZ_PROBE_INC();
  az_r_t r = az_sys_xu_sleep(nsec);
  AZ_PROBE_DEC();

  return r;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
void az_xu_cleanup(az_xu_t xu)
{
  az_assert(NULL != xu);

  #ifdef CONFIG_AZ_DEBUG_NATIVE_APP  
  do {
	  if (NULL == xu) break;
    //az_sys_xu_t     sys_xu = xu->sys_xu;

    if (xu->state & AZ_XU_STATE_START) {
      xu->state |= AZ_XU_STATE_STOP;
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
      az_xu_null();
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
void az_xu_exit(az_xu_ret_t ret)
{
  do {
	  az_xu_t xu = az_xu_self();

	  if (NULL == xu) break;
    //az_sys_xu_t     sys_xu = xu->sys_xu;

    if (xu->state & AZ_XU_STATE_START) {
      xu->state |= AZ_XU_STATE_STOP;
      xu->state &= ~AZ_XU_STATE_START;
      az_refcount_atomic_dec(&xu->ion.refCount);
    }

    az_trz_flush(&xu->trz_list, NULL);

    az_eprintf("id:%d ref count %d\n", xu->ion.id, AZ_REFCOUNT_VALUE(&xu->ion));

    if (AZ_REFCOUNT_VALUE(&xu->ion.refCount) > 0) {
      az_ion_deregister(&(xu->ion));
    }

    if (AZ_REFCOUNT_VALUE(&xu->ion.refCount) == 0) {
      xu->sys_xu = NULL;
      az_xu_null();
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
az_r_t az_xu_sendEvent(az_ion_id_t id, az_xu_event_t event)
{
  az_r_t r = AZ_SUCCESS;
  az_xu_t xu = (az_xu_t)az_ion(id);
  do {
    az_if_arg_null_break(xu, r);
    az_assert_ion_type(xu->ion.type, AZ_ION_TYPE_XU);

    // need to use lock not free xu by others
    if (AZ_XU_ON_EXCPT(xu)) {
      r = AZ_ERR(EXCEPTION);
      break;
    }

    if (xu->state & AZ_XU_STATE_STOP) {
      r = AZ_ERR(STATE);
      break;
    }

    az_refcount_atomic_inc(&xu->ion.refCount);

    r = az_sys_xu_sendEvent(xu->sys_xu, event); 

    if (xu->state & (AZ_XU_STATE_STOP|AZ_XU_STATE_EXCPT)) {
      if  (az_refcount_atomic_dec(&xu->ion.refCount) == 0) {
        az_ion_empty(&xu->ion);
        az_xu_empty(xu);
      } else if (AZ_REFCOUNT_VALUE(&xu->ion.refCount) == 1) {
        az_xu_empty_sys_xu(xu);
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
az_r_t az_xu_sendMsg(az_ion_id_t id, az_xu_msg_t msg)
{
  az_r_t r = AZ_SUCCESS;
  az_xu_t xu = (az_xu_t)az_ion(id);
  do {
    if (msg == 0) {
      r = AZ_ERR(EMPTY);
      break;
    }
    az_if_arg_null_break(xu, r);
    az_assert_ion_type(xu->ion.type, AZ_ION_TYPE_XU);

    // need to use lock not free xu by others
    if (AZ_XU_ON_EXCPT(xu)) {
      r = AZ_ERR(EXCEPTION);
      break;
    }

    if (xu->state & AZ_XU_STATE_STOP) {
      r = AZ_ERR(STATE);
      break;
    }

    az_refcount_atomic_inc(&xu->ion.refCount);
    #if (CONFIG_AZ_XU_MSG_SZ == 64)
    r = az_ring_push64(&xu->msgq, &msg);
    #else
    r = az_ring_push32(&xu->msgq, &msg);
    #endif
    if (r < 0) {
      break;
    }
    r = az_sys_xu_sendEvent(xu->sys_xu, AZ_XU_EVENT_MSG); 
    if (xu->state & (AZ_XU_STATE_STOP|AZ_XU_STATE_EXCPT)) {
      if  (az_refcount_atomic_dec(&xu->ion.refCount) == 0) {
        az_ion_empty(&xu->ion);
        az_xu_empty(xu);
      } else if (AZ_REFCOUNT_VALUE(&xu->ion.refCount) == 1) {
        az_xu_empty_sys_xu(xu);
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
az_r_t az_xu_recvMsg(az_xu_msg_t *pMsg)
{
  az_r_t r = AZ_SUCCESS;
  az_xu_t xu = az_xu_self(); 
  az_assert(NULL != pMsg);
  do {
    if (NULL == xu) {
      r = AZ_ERR(ENTITY_NULL);
      break;
    }
    #if (CONFIG_AZ_XU_MSG_SZ == 64)
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
az_r_t az_xu_recvEvent(az_xu_event_t toReceive, az_int8_t options,
    az_int64_t nsec, az_xu_event_t *pReceived)
{
  az_r_t r =  az_sys_xu_recvEvent(toReceive, options, nsec, pReceived); 
  return r;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
az_r_t az_xu_waitEvent(az_xu_event_t toReceive, az_int8_t options,
    az_int64_t nsec, az_xu_event_t *pReceived)
{
  az_xu_t xu = az_xu_self();
  az_xu_event_t revents = az_sys_xu_readEvent(xu->evectors);
  az_xu_event_t evecmask;
  struct az_xu_evector *evector; 
  int j, evecount;
  if (revents) {
    evecmask = 1; evecount = xu->evecount;
    for (j = 0, evecmask = 1; j < AZ_XU_EVENT_VECTORS && evecount > 0; j++) {
      if (evecmask & revents) {
        evecount--;
        evector = &xu->evectable[j];
        (evector->handler)(j, evector->arg);
      }
      evecmask <<= 1;
    }
  }

  az_r_t r =  az_sys_xu_waitEvent(xu->evectors, toReceive, options, nsec, &revents, pReceived); 

  if (revents) {
    evecmask = 1; evecount = xu->evecount;
    for (j = 0, evecmask = 1; j < AZ_XU_EVENT_VECTORS && evecount > 0; j++) {
      if (evecmask & revents) {
        evecount--;
        evector = &xu->evectable[j];
        (evector->handler)(j, evector->arg);
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
az_r_t az_xu_regEventHandler(az_ion_id_t id, unsigned int vecno, az_xu_event_handler_t handler, az_xu_arg_t arg)
{
  az_r_t r = AZ_FAIL;
  az_xu_t xu = (az_xu_t)az_ion(id);
  do {
    az_if_arg_null_break(xu, r);
    az_assert_ion_type(xu->ion.type, AZ_ION_TYPE_XU);
    if (AZ_XU_ON_EXCPT(xu)) {
      r = AZ_ERR(EXCEPTION);
    }

    if (vecno > AZ_XU_EVENT_VECTORS) {
      r = AZ_ERR_L(OOR, 0);
      break;
    }
#ifndef  CONFIG_AZ_EMBED_XU_EVECTORS
    if (xu->evectable == NULL) {
      xu->evectable = az_malloc(sizeof(struct  az_xu_evector) * AZ_XU_EVENT_VECTORS);
      if (xu->evectable == NULL) {
        r = AZ_ERR_L(MALLOC, 0);
        break;
      }
    }
#endif
    if (xu->evectable[vecno].handler) {
      r = AZ_ERR_L(AGAIN, 0);
    }
    xu->evectable[vecno].handler = handler;
    xu->evectable[vecno].arg = arg;
    xu->evecount++;
    xu->evectors |= (1 << vecno);
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
az_r_t az_xu_deregEventHandler(az_ion_id_t id, unsigned int vecno)
{
  az_r_t r = AZ_FAIL;
  az_xu_t xu = (az_xu_t)az_ion(id);
  do {
    az_if_arg_null_break(xu, r);
    az_assert_ion_type(xu->ion.type, AZ_ION_TYPE_XU);
    if (AZ_XU_ON_EXCPT(xu)) {
      r = AZ_ERR(EXCEPTION);
    }
    if (vecno > AZ_XU_EVENT_VECTORS) {
      r = AZ_ERR_L(OOR, 0);
      break;
    }
    az_assert(NULL != xu->evectable);
    if (NULL == xu->evectable[vecno].handler) {
      r = AZ_ERR_L(AGAIN, 0);
    }
    xu->evectors &= ~(1 << vecno);
    xu->evecount--;
    xu->evectable[vecno].handler = NULL; 
    xu->evectable[vecno].arg = NULL;
  } while (0);

  return r;
}

int az_xu_comparebyname(char *name, az_xu_t xu)
{
  int r = 0;
  do {
    if (NULL == name) { r = -1; break; }
    if (NULL == xu) { r = -2; break; }
    //az_sys_eprintf("%s %s type:%d %d\n", name, xu->name, xu->ion.type, AZ_ION_TYPE_XU);
    if (xu->ion.type != AZ_ION_TYPE_XU) {
      r = 1; 
      break;
    }
    r = strcmp(name, AZ_XU_NAME(xu));
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
az_xu_t az_xu_find(char *name) 
{
  az_xu_t xu =  (az_xu_t)az_ion_find(name, (az_ion_compare_t)az_xu_comparebyname);
  //az_sys_eprintf("%p \n", xu); 
  return xu;
}


/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */

void *az_xu_entry(void *arg)
{
  volatile az_xu_t xu = (az_xu_t)arg;
  volatile void *ret = NULL;
  az_assert(NULL != xu);

  #ifdef  CONFIG_AZ_USE_TLS
  az_xu = xu;
  #endif

  #ifdef  CONFIG_AZ_XU_EXP_HANDLE
  az_sys_xu_register_exception_handler();
  
  if (0 == AZ_SYS_XU_SAVE_CONTEXT()) {
  #endif
   do {
    /*
     * allocate resources...
     */

    xu->state |= AZ_XU_STATE_BUSY;

    ret = (xu->entry)(xu->arg);

    xu->state &= ~AZ_XU_STATE_BUSY;

    /*
     * release resources...
     */

   } while (0);
  #ifdef  CONFIG_AZ_XU_EXP_HANDLE
   az_sys_xu_remove_context();
  } else {
    az_xu_sendMsg(xu->wait_xu_id, AZ_XU_MSG_EXCEPTION|xu->ion.id);
    az_sys_dlog("%s\n", AZ_XU_NAME(xu));
  }
  #endif

  az_xu_exit(ret);

  return ret;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
void  az_xu_set_cleanup()
{
#ifdef  CONFIG_AZ_TRACE_PTHREAD
  az_xu_t xu = az_xu_self();
  if (xu != NULL) {
    pthread_cleanup_push (az_xu_cleanup, xu);
  }

#endif
}

void  az_xu_set_state_excpt(void *excpt_point)
{
  az_xu_t xu = az_xu_self();
  if (xu != NULL) {
    xu->state |= AZ_XU_STATE_EXCPT; 
    xu->excpt_point = excpt_point;
  }
}
void  az_xu_reset_state_excpt(az_xu_t xu)
{
  if (xu != NULL) {
    xu->state &= ~AZ_XU_STATE_EXCPT; 
  }
}
/* === end of AZ_XU_C === */
