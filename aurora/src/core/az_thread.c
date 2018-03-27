/**
 * @file   az_thread.c
 * @brief  
 * @date   10/11/17
 * @author dhkang
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
 *   - 2017-11-10 dhkang initial create
 */

/* include header files */
#include "az_core.h"
#include "az_thread.h"

/* declare global variables */

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

az_r_t  az_thread_create(az_thread_config_t *cfg, az_thread_t *pThr)
{
  az_r_t  r = AZ_FAIL;
  az_thread_entity_t  *pEntity;

  do {
    az_assert(NULL != cfg);
    az_assert(NULL != pThr);
    pEntity = *pThr;
#ifdef  CONFIG_AZ_DYN_THR_ALLOC
    if (NULL == pEntity) {
      pEntity = (az_thread_t)az_malloc(sizeof(az_thread_entity_t));
      if (pEntity == NULL) {
        r = AZ_ERR(MALLOC);
        break;
      }
      az_refcount_init_dynamic(&pEntity->xu.ion.refCount);
      *pThr = pEntity;
      pEntity->config = *cfg;
    } else {
      if (&pEntity->config != cfg) {
        pEntity->config = *cfg;
      }
    }
#else
    if (NULL == pEntity) {
      r = AZ_ERR(ENTITY_NULL);
      break;
    } else {
      if (&pEntity->config != cfg) {
        pEntity->config = *cfg;
      }
    }
#endif
    
    az_refcount_inc(&pEntity->xu.ion.refCount);
    if (pEntity->config.entry_name[0]) {
      r = az_dllib_getSymbol(pEntity->config.entry_name, (az_ref_t *)&pEntity->status.entry);
    }
    if (pEntity->config.arg_name[0]) {
      r = az_dllib_getSymbol(pEntity->config.arg_name, (az_ref_t *)&pEntity->status.arg);
    }
    pEntity->status.state = AZ_THREAD_STATE_CREATED;
    if (pEntity->config.startdelay >= 0) {
      r = az_thread_start(pEntity, NULL, NULL, NULL, NULL, 0);
    }

  } while (0);


  return r;
}

az_r_t  az_thread_start(az_thread_t thr, char *entry_name, char *arg_name,
    az_xu_entry_t entry, az_xu_arg_t arg, int startdelay)
{
  az_r_t  r = AZ_FAIL;
  az_assert(NULL != thr);

  do {
    if (thr->status.state != AZ_THREAD_STATE_CREATED) {
      r = AZ_ERR(STATE);
      break;
    }
    if (thr->status.state & AZ_THREAD_STATE_START) {
      r = AZ_ERR(AGAIN);
      break;
    }
    if (thr->status.entry == NULL) {
      if (entry == NULL || entry_name == NULL) {
        r = AZ_ERR(ENTRY);
        break;
      } else {
        strncpy(thr->config.entry_name, entry_name, sizeof(thr->config.entry_name)-1);
        thr->config.entry_name[sizeof(thr->config.entry_name)-1] = 0;
        thr->status.entry = entry;
      }
    } else {
      if (!(entry == NULL || entry_name == NULL)) {
        strncpy(thr->config.entry_name, entry_name, sizeof(thr->config.entry_name)-1);
        thr->config.entry_name[sizeof(thr->config.entry_name)-1] = 0;
        thr->status.entry = entry;
      }
    }

    if (thr->config.startdelay < 0) {
      if (startdelay < 0) {
        r = AZ_ERR(PARAM);
        break;
      } else {
        thr->config.startdelay = startdelay;
      }
    }
    az_xu_t xu = &thr->xu;
    r = az_xu_create(thr->config.name, az_thread_entry, thr, &thr->config, &xu);
    if (r < 0) {
      thr->status.state |= AZ_THREAD_STATE_ERROR;
      thr->status.cause = AZ_THREAD_ERROR_START;
      break;
    } else {
      thr->status.state |= AZ_THREAD_STATE_START;
    }
  } while (0);

  return r;
}

az_r_t  az_thread_stop(az_thread_t thr)
{
  az_r_t  r = AZ_FAIL;
  az_assert(NULL != thr);

  do {
    if (!(thr->status.state & AZ_THREAD_STATE_CREATED)) {
      r = AZ_ERR(STATE);
      break;
    }
    if (thr->status.state & AZ_THREAD_STATE_STOP) {
      r = AZ_ERR(AGAIN);
      break;
    }
    thr->status.state |= AZ_THREAD_STATE_STOP;
    r = az_xu_sendEvent(&thr->xu, AZ_XU_EVENT_THR_STOP);
    if (r < 0) {
      thr->status.state |= AZ_THREAD_STATE_ERROR;
      thr->status.cause = AZ_THREAD_ERROR_STOP|AZ_THREAD_ERROR_SNDEVT;
    }

  } while (0);

  return r;
}
az_r_t  az_thread_delete(az_thread_t thr)
{
  az_r_t  r = AZ_FAIL;
  az_assert(NULL != thr);

  do {
    if (!(thr->status.state & AZ_THREAD_STATE_CREATED)) {
      r = AZ_ERR(STATE);
      break;
    }
    if ((thr->status.state & AZ_THREAD_STATE_START)) {
      r = AZ_ERR(STATE);
      break;
    }
    if ((thr->status.state & AZ_THREAD_STATE_BUSY)) {
      r = AZ_ERR(STATE);
      break;
    }
    if (thr->status.state & AZ_THREAD_STATE_STOP) {
      r = az_xu_delete(&thr->xu);
      if (r < 0) {
        thr->status.state |= AZ_THREAD_STATE_ERROR;
        thr->status.cause = AZ_THREAD_ERROR_DELETE;
        break;
      } else {
        thr->status.state &= ~AZ_THREAD_STATE_STOP;
      }
    }
    az_refcount_dec(&thr->xu.ion.refCount);

#ifdef  CONFIG_AZ_DYN_THR_ALLOC
    if (AZ_REFCOUNT_IS_ZERO(&thr->xu.ion.refCount)) {
      az_free(thr);
    }
#endif

  } while (0);

  return r;
}

void *az_thread_entry(void *arg)
{
  az_thread_t thr = (az_thread_t)arg;
  void *ret = NULL;

  do {
    if (NULL == thr) {
      break;
    }
    thr->status.state |= AZ_THREAD_STATE_START;

    /*
     * allocate resources...
     */

    thr->status.state |= AZ_THREAD_STATE_BUSY;

    ret = (thr->status.entry)(thr->status.arg);

    thr->status.state &= ~AZ_THREAD_STATE_BUSY;

    /*
     * release resources...
     */

    thr->status.state &= ~AZ_THREAD_STATE_START;
  } while (0);

  return ret;
}

/* === end of AZ_THREAD_C === */
