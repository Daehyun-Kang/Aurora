/**
 * @file   az_log.c
 * @brief  
 * @date   26/09/17
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
 *   - 2017-09-26 dhkang initial create
 */

/* include header files */
#include "az_def.h"
#include "az_err.h"
#include "az_assert.h"
#include "az_macros.h"
#include "az_malloc.h"
#include "az_atomic.h"
#include "az_string.h"
#include "az_log.h"
#include "az_printf.h"

#ifdef CONFIG_AZ_LOG

/* declare global variables */

az_log_t  *az_logs[CONFIG_AZ_LOG_MAX] = {0};
int       az_logs_count = 0;
uint8_t   az_log_ctrl_state = 0;
az_sys_lw_lock_t  az_log_ctrl_lock;

az_log_port_entity_t  az_log_port_stdout = {
  .state = AZ_LOG_PORT_ACTIVE,
  .ep.fd = STDOUT_FILENO,
  .Write = (az_log_port_write_t)(az_log_port_write2fd),
};

az_log_t  az_log_default = {
  .state = 0,
  .config.name = "aur",
  .config.flags = AZ_LOG_FLAGS_ALL, 
  .config.level = CONFIG_AZ_LOG_LEVEL_DFT,
  .config.buffer_size = 20480,
  .buffer_area = NULL,
  .out = az_log_out2buffer,
  .outport_count = 1,
  .outports[0] = &az_log_port_stdout,
  .outports[1] = AZ_LOG_PORT_INVALID,
  .outports[2] = AZ_LOG_PORT_INVALID,
  .outports[3] = AZ_LOG_PORT_INVALID,
};
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
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
void az_log_ctrl_init()
{
  if (0 == az_log_ctrl_state) {
    az_log_ctrl_state = 1;
    az_sys_lw_lock_init(&az_log_ctrl_lock);
  }
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
void az_log_ctrl_deinit()
{
  if (az_log_ctrl_state) {
    az_sys_lw_lock_deinit(&az_log_ctrl_lock);
    az_log_ctrl_state = 0;
  }
}
/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
int az_log_pushdata_default(void *priv, int push, uint8_t *src, int len)
{
  az_log_t *log = (az_log_t *)priv;
  az_assert(NULL != log);

  //az_sys_printf("log=%p start=%p\n", log, log->buffer_area);
  do {
    az_memcpy(log->buffer_area + push, src, len);
  } while (0);

  if (AZ_ION_ID_INVALID != log->thread_id) {
    az_r_t r = az_xu_sendEvent(log->thread_id, 1 << log->logid);
    if (r < 0) {
      az_sys_rprintf(r, "thread_id=%d" AZ_NL, log->thread_id);
    }
  } else {
    az_sys_eprintf("%s" AZ_NL, "thread invalid");
  }
  return len;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
int az_log_popdata_default(void *priv, int pop, uint8_t *dst, int len)
{
  az_log_t *log = (az_log_t *)priv;
  az_assert(NULL != log);
  do {
    az_memcpy(dst, log->buffer_area + pop, len);
  } while (0);

  return len;
}


/**
 * @fn        function name
 * @brief     function-description
 * @param     input-output-parameters
 * @return    return-value
 * @warning   warnings
 * @exception none
 */

az_log_t  *az_log_create(az_log_t *log, az_log_config_t *cfg)
{
  az_logid_t logid;
  do {
    if (az_logs_count >= CONFIG_AZ_LOG_MAX) {
      log = NULL;
      break;
    }
    if (NULL == log) {
      log = az_malloc(sizeof(az_log_t));
      log->state = AZ_LOG_STATE_DALLOC;
    } else {
      log->state = 0; 
    }
    if (NULL == log) break;
    if (NULL != cfg) {
      log->config = *cfg;
    }
    log->thread_id = AZ_ION_ID_INVALID;
    az_log_t  **pLog = az_logs;
    for (logid = 0; logid < CONFIG_AZ_LOG_MAX; logid++, pLog++) {
      if (*pLog == NULL) {
        *pLog = log;
        log->logid = logid;
        az_logs_count++;
        break;
      }
#if 0
      if (NULL == az_atomic_compare_and_swap_ptr(pLog, NULL, log)) { 
        //TODO: not atomic
        az_atomic_inc32(&az_logs_count);
        log->logid = logid;
        break;
      }
#endif
    }
  } while (0);

  //az_sys_printf("logid %d %p\n", logid, AZ_LOGS(logid)); 

  return log;
}


/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
int  az_log_init(az_log_t *log, uint8_t *buffer_area, az_ion_id_t thread_id,
    az_cirbuffer_pushdata_t pushdata, az_cirbuffer_popdata_t popdata)
{
  int r = AZ_SUCCESS;
  az_assert(NULL != log);
  do {
    if (log->state & AZ_LOG_STATE_INIT) {
      r = AZ_ERR(AGAIN);
      break;
    }
    log->state |= AZ_LOG_STATE_INIT;
    if (NULL == buffer_area) {
      log->buffer_area = az_malloc(log->config.buffer_size);
      if (NULL == log->buffer_area) {
        r = AZ_ERR(MALLOC);
        break;
      }
      log->state |= AZ_LOG_STATE_DBUFFER;
    } else {
      log->buffer_area = buffer_area;
    }
    az_cirbuffer_init_s(&(log->buffer), log->config.buffer_size, NULL);
    if (NULL == pushdata) pushdata = az_log_pushdata_default;
    if (NULL == popdata) popdata = az_log_popdata_default;
    az_cirbuffer_setcallbacks(&(log->buffer), log, pushdata, popdata);

    log->thread_id = thread_id;
    //az_sys_printf("%s: thread_id=%p bp=%p\n", __FUNCTION__, thread_id, log->buffer_area); 

    log->state |= AZ_LOG_STATE_START;
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
int  az_log_deinit(az_log_t *log)
{
  int r = AZ_SUCCESS;
  az_assert(NULL != log);
  do {
    log->state &= ~AZ_LOG_STATE_START;
    if (!(log->state & AZ_LOG_STATE_INIT)) {
      r = AZ_ERR(AGAIN);
      break;
    }
    log->state &= ~AZ_LOG_STATE_INIT;

    log->thread_id = AZ_ION_ID_INVALID;
    az_cirbuffer_deinit(&(log->buffer));
    if (log->state & AZ_LOG_STATE_DBUFFER) {
      az_free(log->buffer_area);
    }
    log->buffer_area = NULL;

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
int az_log_delete(az_log_t  *log)
{
  int r = AZ_SUCCESS;
  az_logid_t logid;
  do {
    az_if_arg_null_break(log, r);
    if (log->state & ~AZ_LOG_STATE_DALLOC) {
      r = AZ_ERR(STATE);
      break;
    }
    logid = log->logid;
    az_log_t  **pLog = &az_logs[logid];
    if (log == az_atomic_compare_and_swap_ptr(pLog, log, NULL)) {
      //TODO: not atomic
      az_atomic_dec32(&az_logs_count);
      log->logid = AZ_LOGID_INVALID;
      break;
    }
    if (log->state & AZ_LOG_STATE_DALLOC) {
      az_free(log);
    } 
  } while (0);

  return r;
}


/*
 * define log thread ...
 */
int az_log_proc_data_default(az_log_t *log)
{
  char  _logbuffer[AZ_PRINTF_BUF_SZ];
  char *bp = _logbuffer;
  int len = AZ_PRINTF_BUF_SZ;
  int processed = 0;

  do {
    len = az_cirbuffer_pop(&log->buffer, (uint8_t *)bp, len); 
    if (len <= 0) break;

    len = az_log_out2port(log, bp, len);

    processed += len;
  } while (1);
  

  return processed;
}

int az_log_thread_state = 0;
az_xu_t az_log_thread_default = NULL;

void *az_log_thread_proc_default(void *arg)
{
  az_xu_event_t received;
  az_xu_event_t emask;
  az_r_t  r;
  az_int64_t  tmo_ns = 2000000000;
  az_logid_t  logid;
  int tmo_count = 0; 

  az_log_thread_state = 1;
  az_sys_printf("%s start..." AZ_NL, __FUNCTION__);
  while (az_log_thread_state) {
    r = az_xu_recvEvent(0xffff, 0, tmo_ns, &received);  
    if (az_log_thread_state & 2) continue;
    //az_sys_printf("%s: r=%d received=%x\n", __FUNCTION__, r, received);
    if (r < 0) {
      //az_sys_printf("%s: tmo\n", __FUNCTION__);
      if (++tmo_count == 3) {
        az_xu_reset_flags(AZ_XU_FLAGS_LOG_ERR);
      }
      continue;
    }
    for (logid = 0, emask=1; logid < CONFIG_AZ_LOG_MAX; logid++, emask <<= 1) {
      if (!(received & emask)) continue;
      // process log data ...
      az_log_proc_data_default(az_logs[logid]);
    }
    received = 0;
  }

  return NULL;
}

az_r_t  az_log_start_default_thread()
{
  az_r_t r;

  r = (az_r_t)az_xu_create("logDefault", az_log_thread_proc_default, NULL, NULL, &az_log_thread_default);
  //az_sys_printf("%s: %p\n", __FUNCTION__, az_log_thread_default);

  return (r < AZ_SUCCESS)? r:AZ_SUCCESS;
}

az_r_t  az_log_stop_default_thread()
{
  int r;

  az_log_thread_state = 0;
  r = az_xu_delete(az_log_thread_default);

  return r;
}

az_r_t az_err_log_and_set(const char *f, const char *s, int n, az_r_t err, int level)
{
  if (level > 0) {
    do {
      char *err_str = az_err_str(err);
      az_xu_t xu = az_xu_self(); 
      if (xu && !(xu->flags & AZ_XU_FLAGS_LOG_ERR)) {
        break;
      }
      char *xu_name = ((xu == NULL)? "????":AZ_XU_NAME(xu));
      if (az_logs_count > 0) {
        az_log_printf(0, 0, f, s, n, "[%s] error:%s(%ld)\n", xu_name, err_str, err);
      } else {
        printf("%s:%s:%d" "[%s] error:%s(%ld)\n", f, s, n, xu_name, err_str, err);
      }
    } while (0);
  }

  return err;
}

az_log_port_entity_t az_log_file_port_entity_default = {
  .state = AZ_LOG_PORT_IDLE,
  .ep.file = AZ_SYS_FILE_INVALID,
  .Write = NULL
};
az_log_port_t az_log_file_port_default = &az_log_file_port_entity_default; 

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
extern az_r_t  az_log_dup_start_default_thread();
az_r_t  az_log_start()
{
  az_r_t r = AZ_SUCCESS;
  do {
    if (NULL == az_log_create(&az_log_default, NULL)) {
      r = AZ_FAIL;
      break;
    }
    r = az_log_start_default_thread();
    if (r != AZ_SUCCESS) break;

    r = (az_r_t)az_log_init(&az_log_default, NULL, az_log_thread_default->ion.id, NULL, NULL);
    if (r != AZ_SUCCESS) break;
    r = (az_r_t)az_log_port_addFileOutput(az_log_default.logid, "aurora.log", &az_log_file_port_default); 
  
    az_log_dup_start_default_thread();
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
az_r_t  az_log_stop()
{
  az_r_t r = AZ_SUCCESS;
  do {
    r = (az_r_t)az_log_port_delFileOutput(az_log_default.logid, az_log_file_port_default); 
    r = (az_r_t)az_log_deinit(&az_log_default);
    r = az_log_stop_default_thread();

    az_log_delete(&az_log_default);
  } while (0);

  return r;
}
#endif

/* === end of AZ_LOG_C === */
