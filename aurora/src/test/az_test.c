/**
 * @file   az_test_xu.c
 * @brief  
 * @date   09/02/16
 * @author Daehyun Kang
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
 *   - 2016-02-09 Daehyun Kang initial create
 */

/* include header files */
#include "aurora.h"
#include "cli/az_cli.h"
#include "frw/az_frw_cmd.h"
#include "az_ver.h"
#include "mon/az_debug.h"
#include "az_test.h"
#include "az_tc.h"

/* declare global variables */
int az_tc_thread_state = 0;
az_thread_t az_tc_thread_default = NULL;

az_event_descr_t    *tc_evt_descr = NULL;
az_event_receiver_t tc_evt_receiver;
az_event_port_t     tc_evt_port = NULL;
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
az_r_t az_test_sendEvent(az_event_id_t event_id, az_uint32_t buffer_size, az_ref_t buffer_data) 
{
  az_r_t r;

  az_event_t sevt = NULL;
  do {
    r = az_event_create(&sevt, tc_evt_descr->index, 0, NULL);
    if (r < 0) {
      az_sys_rprintf(r, "%s" AZ_NL, "event create fail" );
      break;
    }
    sevt->event_id = event_id;
    r = az_event_send(tc_evt_descr->port, sevt);
    if (r < 0) {
      az_sys_rprintf(r, "%s" AZ_NL, "event send" );
      break;
    }
  } while (0);

  return r;
}


void *az_tc_thread_proc_default(void *arg)
{
  volatile az_thread_beam_t received;
  az_thread_beam_t emask;
  volatile az_r_t  r;
  volatile az_int64_t  tmo_ns = 2000000000;
  volatile int tmo_count = 0; 
  volatile az_event_receiver_t *rcvr = &tc_evt_receiver;
  az_event_t  *evts[2];
  az_ring_t   evtq;
  volatile az_testcase_t *tc = (az_testcase_t *)arg;

  az_assert(NULL != tc);
  az_assert(NULL != tc->test_proj);

  az_ion_id_t tp_xu_id = (tc->test_proj)->xu_id;

  az_sys_xu_register_exception_handler();

  AZ_TC_SET_CUR_TC(tc);

  do {
    if (tc_evt_descr == NULL) {
      r = az_event_descr_create(&tc_evt_descr, AZ_TEST_EVT_BASE, 0, 2);
      if (r < 0) {
        break;
      }
    }
    if (tc_evt_port == NULL) {
      r = az_event_port_create(AZ_TEST_EVT_BASE, AZ_TEST_EVT_MASK, 
          AZ_EVENT_PORT_TYPE_TX|AZ_EVENT_PORT_TYPE_RX, &tc_evt_port);
      if (r < 0) {
        break;
      }
      r = az_event_port_attach(tc_evt_port);
      if (r < 0) {
        break;
      }
      tc_evt_descr->port = tc_evt_port;
    }
    az_ring_init(&evtq, AZ_RING_TYPE_DSREF, 2, evts);
    az_event_receiver_init(rcvr, NULL, NULL, &evtq);
    az_dlog("rcvr=%p port=%p\n", rcvr, tc_evt_port);
    r = az_event_port_add_receiver(tc_evt_port, rcvr);
  } while (0);

  if (AZ_SUCCESS == r) {
    az_tc_thread_state = 1;
    az_ilog("%s start..." AZ_NL, __FUNCTION__);

    volatile az_event_t  revt = NULL;
    volatile uint64_t  tmo_count = 0;

    az_fsm_activate(&az_tc_fsm, az_thread_self());

    r = az_test_sendEvent(AZ_TEST_CMD_INIT, 0, NULL);
    if (r < 0) {
      az_tc_thread_state = 0;
      az_thread_send_beam(tp_xu_id, AZ_THREAD_BEAM_TEST_ERROR);
    } else {
      az_thread_send_beam(tp_xu_id, AZ_THREAD_BEAM_TEST_STARTED);
    }
    while (az_tc_thread_state) {
      received = 0;
      az_dlog("received:%p\n", &received); 
      r = az_thread_recv_beam(AZ_THREAD_BEAM_THR_STOP|AZ_THREAD_BEAM_EVTBUS, 0, tmo_ns, &received);  
      if (r >= 0) {
        if (received & AZ_THREAD_BEAM_EVTBUS) {
          r = az_event_recv(rcvr, &revt);
          if (r >= 0) {
            //az_event_toStr(az_thread_prtbuf, sizeof(az_thread_prtbuf), NULL, revt);
            //az_rprintf(r, "xu recvent %p: %s\n", revt, az_thread_prtbuf);
            if (0 == AZ_SYS_XU_SAVE_CONTEXT()) { 
              az_fsm_run(&az_tc_fsm, revt);
              az_sys_xu_remove_context();
              //tmo_ns = 2000000000;
            } else {
              tmo_ns = 2000000000;
              tc = AZ_TC_GET_CUR_TC();
              az_sys_eprintf("%s", "xu state set\n");
              if (AZ_THREAD_IS_STATE_EXCPT(az_thread_self())) {
                az_test_iter_t *iter = tc->test_iter.list + tc->test_iter.index; 
                void *ptr = AZ_THREAD_EXCPT_POINT(az_thread_self());
                strncpy(iter->reason, az_addr2line(ptr, az_getExeFileName()), sizeof(iter->reason));
                az_thread_reset_state_excpt(az_thread_self());
                az_ilog("testcase %s : exception during iter%d\n", tc->name, iter->index);
                tc->report.fail++;
                tc->test_iter.errored++;
                iter->result = AZ_TC_RESULT_SEGV;
                tc->test_iter.index++;
                tc->test_iter.remained--;
                if (tc->test_iter.remained > 0) {
                  // continue to next iteration
                  az_tc_fsm.substate = AZ_TEST_SUBSTATE_PRE; 
                  r = az_test_sendEvent(AZ_TEST_CMD_PLOG, 0, NULL);
                } else {
                  r = az_test_sendEvent(AZ_TEST_CMD_STOP, 0, NULL);
                }
              } else {
                az_eprintf("%s", "xu state not set\n");
              } //if (AZ_THREAD_IS_STATE_EXCPT(az_thread_self())) 
               
            } //if (0 == az_sys_xu_save_context())
          }
        }
        if (received & AZ_THREAD_BEAM_THR_STOP) {
          az_tc_thread_state = 0;
        }
        if (tc->timeout > 0) tc->timeout++;
        continue;
      }
      if (r < 0) {
        if (r == AZ_ERR(TIMEOUT)) {
          if (tmo_count++ == 5) {
            r = az_test_sendEvent(AZ_TEST_CMD_STOP, 0, NULL);
          }
          if (tc->timeout > 0) tc->timeout++;
        }
        continue;
      }
    }
    r = az_event_port_del_receiver(tc_evt_port, rcvr);
    az_thread_send_beam(tp_xu_id, AZ_THREAD_BEAM_TEST_STOPPED);
  } else {
    az_tc_thread_state = 0;
    az_rprintf(r, "%s error..." AZ_NL, __FUNCTION__);
    az_thread_send_beam(tp_xu_id, AZ_THREAD_BEAM_TEST_ERROR);
  }

  AZ_TC_SET_CUR_TC(NULL);

  return NULL;
}

/* === end of AZ_TEST_XU_C === */
