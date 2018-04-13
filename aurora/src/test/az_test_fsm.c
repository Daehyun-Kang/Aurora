/**
 * @file   az_test_fsm.c
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
#include "az_ver.h"
#include "az_test.h"

static az_fsm_state_t az_test_fsm_handleOnIdle(void *, az_fsm_state_t, az_event_t);
static az_fsm_state_t az_test_fsm_handleOnInit(void *, az_fsm_state_t, az_event_t);
static az_fsm_state_t az_test_fsm_handleOnSync(void *, az_fsm_state_t, az_event_t);
static az_fsm_state_t az_test_fsm_handleOnIerr(void *, az_fsm_state_t, az_event_t);
az_fsm_state_t az_test_fsm_handleOnNorm(void *, az_fsm_state_t, az_event_t);
static az_fsm_state_t az_test_fsm_handleOnTini(void *, az_fsm_state_t, az_event_t);

/* declare global variables */
#define AZ_TEST_FSM_VECTOR(state, handle) {AZ_TEST_STATE_##state, az_test_fsm_##handle}
// order should be equal to the order of state
az_fsm_vector_t az_tc_fsm_table[] = {
  AZ_TEST_FSM_VECTOR(IDLE, handleOnIdle), //IDLE=0
  AZ_TEST_FSM_VECTOR(INIT, handleOnInit), //INIT=1
  AZ_TEST_FSM_VECTOR(IERR, handleOnIerr), //IERR=2
  AZ_TEST_FSM_VECTOR(SYNC, handleOnSync), //SYNC=3
  AZ_TEST_FSM_VECTOR(TINI, handleOnTini), //TINI=4
  AZ_TEST_FSM_VECTOR(NORM, handleOnNorm), //NORM=5
};

az_fsm_t  az_tc_fsm = 
  AZ_FSM_INIT_SEQ("tc_fsm", array_size(az_tc_fsm_table), az_tc_fsm_table, &az_tc_fsm_table[AZ_TEST_STATE_IDLE], NULL, NULL);

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
static az_fsm_state_t az_test_fsm_handleOnIdle(void *ctx, az_fsm_state_t state, az_event_t evt)
{
  az_assert(NULL != evt);
  az_event_id_t evtid = AZ_EVENT_ID(evt);
  az_event_descr_t  *edescr = az_event_descr_get(evt->descr_id);
  az_r_t r = AZ_SUCCESS;

  az_assert(NULL != edescr);

  az_test_case_t *tc = az_test_curTestCase();
  az_tlog("testcase:%s %s\n", tc->name, az_fsm_prtstate(ctx, 0, evt));
  do {
    switch (evtid) {
      case AZ_TEST_CMD_INIT:
        state = AZ_TEST_STATE_INIT;
        tc->test_iter.count = tc->test_iter_count;
        tc->test_iter.index = 0; 
        tc->test_iter.remained = tc->test_iter_count; 
        tc->test_iter.completed = 0; 
        tc->test_iter.errored = 0; 
        if (tc->oprs.Init) {
          r = (tc->oprs.Init)(tc);
        }
        if (r < 0) {
          tc->result = r;
          state = AZ_TEST_STATE_IERR;
          r = az_test_sendEvent(AZ_TEST_EVT_IERR, 0, NULL);
        } else {
          r = az_test_sendEvent(AZ_TEST_CMD_SYNC, 0, NULL);
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
static az_fsm_state_t az_test_fsm_handleOnInit(void *ctx, az_fsm_state_t state, az_event_t evt)
{
  az_assert(NULL != evt);
  az_event_id_t evtid = AZ_EVENT_ID(evt);
  az_event_descr_t  *edescr = az_event_descr_get(evt->descr_id);
  az_r_t r = AZ_SUCCESS;

  az_assert(NULL != edescr);

  az_test_case_t *tc = az_test_curTestCase();
  az_tlog("testcase:%s %s\n", tc->name, az_fsm_prtstate(ctx, 1, evt));
  do {
    switch (evtid) {
      case AZ_TEST_CMD_SYNC:
          state = AZ_TEST_STATE_SYNC;
          if (tc->oprs.Sync) {
            r = (tc->oprs.Sync)(tc);
          }
          if (r < 0) {
            tc->result = r;
            state = AZ_TEST_STATE_IERR;
            r = az_test_sendEvent(AZ_TEST_EVT_IERR, 0, NULL);
          } else {
            r = az_test_sendEvent(AZ_TEST_CMD_STRT, 0, NULL);
          }
          if (r < 0) {
            state = AZ_TEST_STATE_IERR;
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
static az_fsm_state_t az_test_fsm_handleOnSync(void *ctx, az_fsm_state_t state, az_event_t evt)
{
  az_assert(NULL != evt);
  az_event_id_t evtid = AZ_EVENT_ID(evt);
  az_event_descr_t  *edescr = az_event_descr_get(evt->descr_id);
  az_r_t r = AZ_SUCCESS;
  az_fsm_t *fsm = (az_fsm_t *)ctx;

  az_assert(NULL != edescr);

  az_test_case_t *tc = az_test_curTestCase();
  az_tlog("testcase:%s %s\n", tc->name, az_fsm_prtstate(ctx, 2, evt));
  do {
    switch (evtid) {
      case AZ_TEST_CMD_STRT:
        state = AZ_TEST_STATE_NORM;
        /*
        if (tc->oprs.Run) {
          r = (tc->oprs.Run)(tc);
        }
        if (r < 0) {
          tc->result = r;
          state = AZ_TEST_STATE_IERR;
          r = az_test_sendEvent(AZ_TEST_EVT_IERR, 0, NULL);
        } else {
          r = az_test_sendEvent(AZ_TEST_CMD_STOP, 0, NULL);
        }
        */
        if (tc->test_iter.remained > 0) {
          fsm->substate = AZ_TEST_SUBSTATE_PRE; 
          r = az_test_sendEvent(AZ_TEST_CMD_PLOG, 0, NULL);
        } else {
          r = az_test_sendEvent(AZ_TEST_CMD_STOP, 0, NULL);
        }
        if (r < 0) {
          tc->result = r;
          state = AZ_TEST_STATE_IERR;
          r = az_test_sendEvent(AZ_TEST_EVT_IERR, 0, NULL);
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
az_fsm_state_t az_test_fsm_handleOnNorm(void *ctx, az_fsm_state_t state, az_event_t evt)
{
  az_assert(NULL != evt);
  az_event_id_t evtid = AZ_EVENT_ID(evt);
  az_event_descr_t  *edescr = az_event_descr_get(evt->descr_id);
  az_event_t  sevt = NULL;
  az_r_t r = AZ_SUCCESS;
  az_fsm_t *fsm = (az_fsm_t *)ctx;
  az_test_iter_t *iter;

  az_assert(NULL != edescr);

  az_test_case_t *tc = az_test_curTestCase();
  az_tlog("testcase%d:%s %d %d iter=%d/%d %s\n", 
      tc->index, tc->name, 
      tc->report.pass, tc->report.fail,
      tc->test_iter.index, 
      tc->test_iter.remained, az_fsm_prtstate(ctx, 3, evt));

  if (tc->test_iter.index < tc->test_iter.count) {
    iter = tc->test_iter.list + tc->test_iter.index;
    az_tlog("iter %d result:%d:%s\n", iter->index, iter->result, az_err_str(iter->result)); 
  } else {
    iter = NULL; 
  }
  do {
    switch (evtid) {
      case AZ_TEST_CMD_PLOG:
        if (tc->oprs.Prolog) {
          r = (tc->oprs.Prolog)(tc);
        }
        if (r < 0) {
          tc->report.fail++;
          tc->test_iter.index++;
          tc->test_iter.remained--;
          tc->test_iter.errored++;
          if (tc->test_iter.remained > 0) {
            fsm->substate = AZ_TEST_SUBSTATE_PRE; 
            r = az_test_sendEvent(AZ_TEST_CMD_PLOG, 0, NULL);
          } else {
            r = az_test_sendEvent(AZ_TEST_CMD_STOP, 0, NULL);
          }
        } else {
          fsm->substate = AZ_TEST_SUBSTATE_RUN; 
          r = az_test_sendEvent(AZ_TEST_CMD_EXEC, 0, NULL);
        }
        break;
      case AZ_TEST_CMD_EXEC:
        if (tc->oprs.Run) {
          r = (tc->oprs.Run)(tc);
        }
        if (r < 0) {
          tc->report.fail++;
          tc->test_iter.index++;
          tc->test_iter.remained--;
          tc->test_iter.errored++;
          if (tc->test_iter.remained > 0) {
            fsm->substate = AZ_TEST_SUBSTATE_PRE; 
            r = az_test_sendEvent(AZ_TEST_CMD_PLOG, 0, NULL);
          } else {
            fsm->substate = AZ_TEST_SUBSTATE_END; 
            r = az_test_sendEvent(AZ_TEST_CMD_STOP, 0, NULL);
          }
        } else {
          fsm->substate = AZ_TEST_SUBSTATE_END; 
          r = az_test_sendEvent(AZ_TEST_CMD_ELOG, 0, NULL);
        }
        break;
      case AZ_TEST_CMD_ELOG:
        if (tc->oprs.Epilog) {
          r = (tc->oprs.Epilog)(tc);
        }
        if (r < 0) {
          tc->report.fail++;
          tc->test_iter.errored++;
        } else {
          tc->test_iter.completed++;
          if (iter->result < 0) {
            tc->report.fail++;
          } else {
            tc->report.pass++;
            if (iter->result == 0) {
              tc->report.success++;
            } else {
              tc->report.failure++;
            }
          }
        }
        tc->test_iter.index++;
        tc->test_iter.remained--;
        if (tc->test_iter.remained > 0) {
          fsm->substate = AZ_TEST_SUBSTATE_PRE; 
          r = az_test_sendEvent(AZ_TEST_CMD_PLOG, 0, NULL);
        } else {
          r = az_test_sendEvent(AZ_TEST_CMD_STOP, 0, NULL);
        }
        break;
      case AZ_TEST_CMD_STOP:
        state = AZ_TEST_STATE_TINI;
        if (tc->oprs.Term) {
          r = (tc->oprs.Term)(tc);
        }
        if (r < 0) {
          tc->result = r;
          state = AZ_TEST_STATE_IERR;
          r = az_test_sendEvent(AZ_TEST_EVT_IERR, 0, NULL);
        } else {
          r = az_test_sendEvent(AZ_TEST_CMD_TINI, 0, NULL);
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
static az_fsm_state_t az_test_fsm_handleOnTini(void *ctx, az_fsm_state_t state, az_event_t evt)
{
  az_assert(NULL != evt);
  az_event_id_t evtid = AZ_EVENT_ID(evt);
  az_event_descr_t  *edescr = az_event_descr_get(evt->descr_id);
  az_event_t  sevt = NULL;
  az_r_t r = AZ_SUCCESS;

  az_assert(NULL != edescr);

  az_test_case_t *tc = az_test_curTestCase();
  az_tlog("testcase:%s %s\n", tc->name, az_fsm_prtstate(ctx, 4, evt));
  do {
    switch (evtid) {
      case AZ_TEST_CMD_TINI:
        az_xu_sendEvent(az_xu_self()->ion.id, AZ_XU_EVENT_THR_STOP);
        state = AZ_TEST_STATE_IDLE;
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
static az_fsm_state_t az_test_fsm_handleOnIerr(void *ctx, az_fsm_state_t state, az_event_t evt)
{
  az_assert(NULL != evt);
  az_event_id_t evtid = AZ_EVENT_ID(evt);
  az_event_descr_t  *edescr = az_event_descr_get(evt->descr_id);
  az_event_t  sevt = NULL;
  az_r_t r = AZ_SUCCESS;

  az_assert(NULL != edescr);

  az_test_case_t *tc = az_test_curTestCase();
  az_tlog("testcase:%s %s\n", tc->name, az_fsm_prtstate(ctx, 5, evt));
  do {
    switch (evtid) {
      case AZ_TEST_EVT_IERR:
        az_xu_sendEvent(az_xu_self()->ion.id, AZ_XU_EVENT_THR_STOP);
        state = AZ_TEST_STATE_IDLE;
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


/* === end of AZ_TEST_FSM_C === */
