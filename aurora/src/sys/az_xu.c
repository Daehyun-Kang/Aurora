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
az_r_t az_xu_create(char *name, az_xu_entry_t entry, az_xu_arg_t arg, az_xu_config_t *pCfg, az_xu_t *pXu)
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
    }

#ifdef  CONFIG_AZ_EMBED_SYS_XU
    xu->sys_xu = &(xu->_sys_xu); 
#else
    xu->sys_xu = NULL; 
#endif
    xu->state = AZ_XU_STATE_CREATED;

    if (xu->attr.startdelay <= 0) {
      result = az_xu_start(xu, NULL, NULL);
      if (AZ_SUCCESS != result) {
        az_ion_deregister(&(xu->ion));
        if (AZ_REFCOUNT_IS_ZERO(&xu->ion.refCount)) {
          az_free(xu);
          *pXu = NULL;
        }
        break;
      } 
    }
  } while (0);

  az_rprintf(result, "id:%d ref count %d\n", xu->ion.id, AZ_REFCOUNT_VALUE(&xu->ion.refCount));
  return result;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
az_r_t az_xu_start(az_xu_t xu, az_xu_entry_t entry, az_xu_arg_t arg)
{
  az_r_t result = AZ_SUCCESS;
  az_assert(NULL != xu);

  do {
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

    if (xu->attr.startdelay < 0) {
      result = az_sys_xu_init(AZ_XU_NAME(xu), az_xu_entry, xu, &(xu->attr), &(xu->sys_xu)); 
    } else {
      result = az_sys_xu_create(AZ_XU_NAME(xu), az_xu_entry, xu, &(xu->attr), &(xu->sys_xu)); 
    }
    //az_sys_printf("xu=%p sys_xu=%p, arg=%p result=%ld\n", xu, xu->sys_xu, arg, result);

    if (AZ_SUCCESS != result) {
      break;
    } 

    az_refcount_atomic_inc(&xu->ion.refCount);
    xu->state |= AZ_XU_STATE_START;
    //xu->sys_xu->priv = xu;
  } while (0);

  if (result != AZ_SUCCESS) {
    xu->state |= AZ_XU_STATE_ERROR;
    xu->cause = AZ_XU_ERROR_START;
  }

  az_rprintf(result, "id:%d ref count %d\n", xu->ion.id, AZ_REFCOUNT_VALUE(&xu->ion.refCount));
  return result;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
az_r_t  az_xu_stop(az_xu_t  xu)
{
  az_r_t  result = AZ_SUCCESS;
  az_assert(NULL != xu);

  do {
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
      az_refcount_atomic_dec(&xu->ion.refCount);
    }
    az_trz_flush(&xu->trz_list, NULL);
    result = az_sys_xu_delete(sys_xu);
    if (AZ_SUCCESS == result) {
      xu->sys_xu = NULL;
    }
  } while (0);

  az_rprintf(result, "id:%d ref count %d\n", xu->ion.id, AZ_REFCOUNT_VALUE(&xu->ion.refCount));
  return result;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
az_r_t az_xu_delete(az_xu_t xu)
{
  az_r_t result = AZ_SUCCESS;
  az_assert(NULL != xu);

  do {
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

    az_ion_deregister(&(xu->ion));

    if (AZ_REFCOUNT_VALUE(&xu->ion.refCount) == 0) {
      #ifndef  CONFIG_AZ_EMBED_SYS_XU
      xu->sys_xu = NULL;
      #endif
      az_xu_t save_xu = xu;
      if (save_xu == az_xu_self()) {
        az_xu_null();
        if (AZ_REFCOUNT_VALUE(&xu->ion.refCount) == 0) {
          az_free(xu);
        }
        az_sys_xu_exit(NULL);
      } else {
        if (sys_xu) sys_xu->arg = NULL;
        if (AZ_REFCOUNT_VALUE(&xu->ion.refCount) == 0) {
          az_free(xu);
        }
        if (sys_xu) az_sys_xu_delete(sys_xu);
      }
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
az_r_t  az_xu_setPriority(az_xu_t xu, az_xu_attr_t *pAttr)
{
  az_r_t result = AZ_SUCCESS;
  do {
    az_if_arg_null_break(xu, result);

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
az_r_t  az_xu_getPriority(az_xu_t xu, az_xu_attr_t *pAttr)
{
  az_r_t result = AZ_SUCCESS;
  do {
    az_if_arg_null_break(xu, result);

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
az_r_t  az_xu_setAffinity(az_xu_t xu, az_xu_core_mask_t core_mask)
{
  az_r_t result = AZ_SUCCESS;
  do {
    az_if_arg_null_break(xu, result);

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
az_r_t  az_xu_getAffinity(az_xu_t xu, az_xu_core_mask_t *pCoreMask)
{
  az_r_t result = AZ_SUCCESS;
  do {
    az_if_arg_null_break(xu, result);

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
az_r_t az_xu_suspend(az_xu_t xu)
{
  az_r_t result = AZ_SUCCESS;
  do {
    az_if_arg_null_break(xu, result);

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
az_r_t az_xu_resume(az_xu_t xu)
{
  az_r_t result = AZ_SUCCESS;
  do {
    az_if_arg_null_break(xu, result);

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
  return az_sys_xu_sleep(nsec);
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
      az_refcount_atomic_dec(&xu->ion.refCount);
    }

    az_trz_flush(&xu->trz_list, NULL);

    az_eprintf("id:%d ref count %d\n", xu->ion.id, AZ_REFCOUNT_VALUE(&xu->ion));

    az_ion_deregister(&(xu->ion));

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
az_r_t az_xu_sendEvent(az_xu_t xu, az_xu_event_t event)
{
  az_r_t r = AZ_SUCCESS;
  do {
    az_if_arg_null_break(xu, r);

    if (AZ_REFCOUNT_VALUE(&xu->ion.refCount) < 1) {
      r = AZ_ERR(INVALID);
      break;
    }

    r = az_sys_xu_sendEvent(xu->sys_xu, event); 
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
az_r_t az_xu_regEventHandler(az_xu_t xu, unsigned int vecno, az_xu_event_handler_t handler, az_xu_arg_t arg)
{
  az_r_t r = AZ_FAIL;
  do {
    az_if_arg_null_break(xu, r);
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
az_r_t az_xu_deregEventHandler(az_xu_t xu, unsigned int vecno)
{
  az_r_t r = AZ_FAIL;
  do {
    az_if_arg_null_break(xu, r);
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
  az_xu_t xu = (az_xu_t)arg;
  void *ret = NULL;
  az_assert(NULL != xu);

  #ifdef  CONFIG_AZ_USE_TLS
  az_xu = xu;
  #endif

  #ifdef  CONFIG_AZ_XU_EXP_HANDLE
  az_sys_xu_register_exception_handler();
  
  if (0 == az_sys_xu_save_context()) {
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
/* === end of AZ_XU_C === */
