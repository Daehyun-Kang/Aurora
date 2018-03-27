/**
 * @file   az_frw_fsm.c
 * @brief  
 * @date   16/11/17
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
 *   - 2017-11-16 dhkang initial create
 */

/* include header files */
#include "aurora.h"
#include "frw/az_frw.h"
#include "cli/az_cli.h"
#include "frw/az_frw_cmd.h"
#include "az_ver.h"

static az_fsm_state_t az_frw_fsm_handleOnIdle(void *, az_fsm_state_t, az_event_t);
static az_fsm_state_t az_frw_fsm_handleOnInit(void *, az_fsm_state_t, az_event_t);
static az_fsm_state_t az_frw_fsm_handleOnCnfg(void *, az_fsm_state_t, az_event_t);
static az_fsm_state_t az_frw_fsm_handleOnSync(void *, az_fsm_state_t, az_event_t);
static az_fsm_state_t az_frw_fsm_handleOnIerr(void *, az_fsm_state_t, az_event_t);
static az_fsm_state_t az_frw_fsm_handleOnNorm(void *, az_fsm_state_t, az_event_t);
static az_fsm_state_t az_frw_fsm_handleOnDown(void *, az_fsm_state_t, az_event_t);

/* declare global variables */
#define AZ_FRW_FSM_VECTOR(state, handle) {AZ_FRW_STATE_##state, az_frw_fsm_##handle}
az_fsm_vector_t az_frw_fsm_table[] = {
  AZ_FRW_FSM_VECTOR(IDLE, handleOnIdle),
  AZ_FRW_FSM_VECTOR(INIT, handleOnInit),
  AZ_FRW_FSM_VECTOR(CNFG, handleOnCnfg),
  AZ_FRW_FSM_VECTOR(SYNC, handleOnSync),
  AZ_FRW_FSM_VECTOR(IERR, handleOnIerr),
  AZ_FRW_FSM_VECTOR(NORM, handleOnNorm),
  AZ_FRW_FSM_VECTOR(DOWN, handleOnDown),
};

az_fsm_t  az_frw_fsm = 
  AZ_FSM_INIT_SEQ("frw_fsm", array_size(az_frw_fsm_table), az_frw_fsm_table, &az_frw_fsm_table[AZ_FRW_STATE_IDLE], NULL, NULL);

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
static az_fsm_state_t az_frw_fsm_handleOnIdle(void *ctx, az_fsm_state_t state, az_event_t evt)
{
  az_assert(NULL != evt);
  az_event_id_t evtid = AZ_EVENT_ID(evt);
  az_event_descr_t  *edescr = az_event_descr_get(evt->descr_id);
  az_r_t r = AZ_SUCCESS;

  az_assert(NULL != edescr);

  az_tlog("%s\n", az_fsm_probe(ctx, 0, evt));
  do {
    switch (evtid) {
      case AZ_FRW_CMD_INIT:
        if (az_core_get_state() != AZ_ST_NORM) {
          r = az_core_init();
        }
        if (r == AZ_ERR(AGAIN) || r == AZ_SUCCESS) {
          state = AZ_FRW_STATE_INIT;
          r = az_frw_sendEvent(AZ_FRW_CMD_CONF, 0, NULL);
        } else {
          state = AZ_FRW_STATE_IERR;
        }
        break;
      default:
        r = AZ_ERR_L(INVALID, 1);
        break;
    }
  } while (0);

  az_event_delete(evt);

  AZ_ERR_LOG(r);

  return state;
}


/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
static az_fsm_state_t az_frw_fsm_handleOnInit(void *ctx, az_fsm_state_t state, az_event_t evt)
{
  az_assert(NULL != evt);
  az_event_id_t evtid = AZ_EVENT_ID(evt);
  az_event_descr_t  *edescr = az_event_descr_get(evt->descr_id);
  az_r_t r;

  az_assert(NULL != edescr);

  az_tlog("%s\n", az_fsm_probe(ctx, 1, evt));
  do {
    switch (evtid) {
      case AZ_FRW_CMD_CONF:
        r = az_frw_init();

        if (r != AZ_SUCCESS && r != AZ_ERR(NO_APP_CFG)) {
          state = AZ_FRW_STATE_IERR;
          break;
        }

        #ifdef CONFIG_AZ_LOG
        r = az_log_start();
        if (r != AZ_SUCCESS) {
          state = AZ_FRW_STATE_IERR;
          break;
        }
        #endif

        #ifdef CONFIG_AZ_CLI
        az_cli_shell_start(&az_cli_shell_default);
        r = az_cli_svr_start();
        az_sys_printf("default cli shell=%p/%p\n", 
            &az_cli_shell_default, az_cli_shell_default.thread);
        az_frw_cmd_init();
        #endif

        if (r == AZ_SUCCESS) {
          state = AZ_FRW_STATE_CNFG;
          r = az_frw_sendEvent(AZ_FRW_CMD_SYNC, 0, NULL);
        } else {
          state = AZ_FRW_STATE_IERR;
        }
        break;
      default:
        r = AZ_ERR_L(INVALID, 1);
        break;
    }
  } while (0);

  az_event_delete(evt);

  AZ_ERR_LOG(r);

  return state;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
static az_fsm_state_t az_frw_fsm_handleOnCnfg(void *ctx, az_fsm_state_t state, az_event_t evt)
{
  az_assert(NULL != evt);
  az_event_id_t evtid = AZ_EVENT_ID(evt);
  az_event_descr_t  *edescr = az_event_descr_get(evt->descr_id);
  az_r_t r = AZ_SUCCESS;

  az_assert(NULL != edescr);

  az_tlog("%s\n", az_fsm_probe(ctx, 2, evt));
  do {
    switch (evtid) {
      case AZ_FRW_CMD_SYNC:
        r = az_frw_start_sync(); 
        break;
      case AZ_FRW_EVT_SYNC:
        state = AZ_FRW_STATE_NORM;
        r = az_frw_sendEvent(AZ_FRW_CMD_SAPP, 0, NULL);
        break;
      default:
        r = AZ_ERR_L(INVALID, 1);
        break;
    }
  } while (0);

  az_event_delete(evt);

  AZ_ERR_LOG(r);

  return state;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
static az_fsm_state_t az_frw_fsm_handleOnNorm(void *ctx, az_fsm_state_t state, az_event_t evt)
{
  az_assert(NULL != evt);
  az_event_id_t evtid = AZ_EVENT_ID(evt);
  az_event_descr_t  *edescr = az_event_descr_get(evt->descr_id);
  az_event_t  sevt = NULL;
  az_r_t r = AZ_SUCCESS;

  az_assert(NULL != edescr);

  az_tlog("%s\n", az_fsm_probe(ctx, 3, evt));
  do {
    switch (evtid) {
      case AZ_FRW_CMD_SAPP:
        //r = az_start_app(); 
        break;
      case AZ_FRW_CMD_TAPP:
        //r = az_stop_app(); 
        break;
      case AZ_FRW_CMD_DOWN:
        //r = az_stop_app(); 
        //r = az_frw_stop_sync(); 
        state = AZ_FRW_STATE_DOWN;
        r = az_frw_sendEvent(AZ_FRW_CMD_TINI, 0, NULL);
        break;
      default:
        r = AZ_ERR_L(INVALID, 1);
        break;
    }
  } while (0);

  az_event_delete(evt);

  AZ_ERR_LOG(r);

  return state;
}
/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
static az_fsm_state_t az_frw_fsm_handleOnDown(void *ctx, az_fsm_state_t state, az_event_t evt)
{
  az_assert(NULL != evt);
  az_event_id_t evtid = AZ_EVENT_ID(evt);
  az_event_descr_t  *edescr = az_event_descr_get(evt->descr_id);
  az_event_t  sevt = NULL;
  az_r_t r = AZ_SUCCESS;

  az_assert(NULL != edescr);

  az_tlog("%s\n", az_fsm_probe(ctx, 4, evt));
  do {
    switch (evtid) {
      case AZ_FRW_CMD_TINI:
        //r = az_frw_stop(); 
        r = az_frw_deinit();
        r = az_core_deinit(); 
        state = AZ_FRW_STATE_IDLE;
        break;
      default:
        r = AZ_ERR_L(INVALID, 1);
        break;
    }
  } while (0);

  az_event_delete(evt);

  AZ_ERR_LOG(r);

  return state;
}
/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
static az_fsm_state_t az_frw_fsm_handleOnSync(void *ctx, az_fsm_state_t state, az_event_t evt)
{
  az_assert(NULL != evt);
  az_event_id_t evtid = AZ_EVENT_ID(evt);
  az_event_descr_t  *edescr = az_event_descr_get(evt->descr_id);
  az_event_t  sevt = NULL;
  az_r_t r = AZ_SUCCESS;

  az_assert(NULL != edescr);

  az_tlog("%s\n", az_fsm_probe(ctx, 5, evt));
  do {
    switch (evtid) {
      case AZ_FRW_EVT_SYNC:
        break;
      default:
        r = AZ_ERR_L(INVALID, 1);
        break;
    }
  } while (0);

  az_event_delete(evt);

  AZ_ERR_LOG(r);

  return state;
}
/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
static az_fsm_state_t az_frw_fsm_handleOnIerr(void *ctx, az_fsm_state_t state, az_event_t evt)
{
  az_assert(NULL != evt);
  az_event_id_t evtid = AZ_EVENT_ID(evt);
  az_event_descr_t  *edescr = az_event_descr_get(evt->descr_id);
  az_event_t  sevt = NULL;
  az_r_t r = AZ_SUCCESS;

  az_assert(NULL != edescr);

  do {
    switch (evtid) {
      case AZ_FRW_EVT_IERR:
        break;
      default:
        r = AZ_ERR_L(INVALID, 1);
        break;
    }
  } while (0);

  az_event_delete(evt);

  AZ_ERR_LOG(r);

  return state;
}
/* implement global functions */

/**
 * @fn        function name
 * @brief     function-description
 * @param     input-output-parameters
 * @return    return-value
 * @warning   warnings
 * @exception none
 */


/* === end of AZ_FRW_FSM_C === */
