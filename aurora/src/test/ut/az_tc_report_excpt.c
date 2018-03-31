/**
 * @file   az_tc_report_excpt.c
 * @brief  
 * @date   01/04/18
 * @author dhkang
 *
 * @copyright 
 * Copyright (c) Fware, 2013-2018 - All Rights Reserved
 * You may use, distribute and modify this code under the 
 * terms of the "Aurora Source Code License".
 * See the file LICENSE for full license details.\n\n
 * 
 * You should have received a copy of  the LICENSE with this file.\n\n
 * 
 * If not, please contact to Fware with the information in the file CONTACTS.
 *
 * History:
 *   - 2018-04-01 dhkang initial create
 */

/* include header files */
#include "az_tc_report_excpt.h"
/* TODO: include function dependant header files */

/* declare global variables */

int az_tc_report_excpt_flags = AZ_TC_FLAG_TRACE;

/* define the reference for test vectors */
struct az_tc_testvector_st_report_excpt *az_tc_testvector_array_report_excpt = NULL;



/* implement global callback functions */

/**
 * @fn function name
 * @brief function-description
 * @param input-output-parameters
 * @return return-value
 * @exception none
 */


/**
 * @fn		az_tc_init_report_excpt
 * @brief	called for init test case
 * @param	test case info
 * @return	az_r_t
 * @exception	none
 */
az_r_t az_tc_init_report_excpt(az_test_case_t *pTC)
{
  az_r_t r = AZ_SUCCESS;

  if (pTC->test_iter_count > 0) {
    az_tc_testvector_array_report_excpt = (struct az_tc_testvector_st_report_excpt *)az_malloc(AZ_TC_TV_SZ(pTC, az_tc_tv_report_excpt_t));
    if (NULL == az_tc_testvector_array_report_excpt) {
      r = AZ_ERR(MALLOC);
    }
    /* here may set the test vector array values from xml configuration file */

    int j;
    az_tc_tv_report_excpt_t *tv = az_tc_testvector_array_report_excpt;
    az_xml_element_t *xe;
    az_test_iter_t *iter = AZ_TC_ITER_LIST(pTC);
    for (j = 0; j < AZ_TC_ITER_COUNT(pTC); j++, iter++, tv++) {
      AZ_TC_ITER_SET_TV(iter, tv);
      xe = az_xml_element_find_attr(iter->xml, "expected_result");
      if (xe) tv->expected_result = strtol(xe->kv.value, NULL, 10);
    }
  }
  AZ_TC_SET_TV(pTC, az_tc_testvector_array_report_excpt);
  if ((az_tc_report_excpt_flags & AZ_TC_FLAG_TRACE) || (az_tc_flags & AZ_TC_FLAG_TRACE)) {
    AZ_TC_PRINT_B(pTC);
  }
  return r;
}

/**
 * @fn		az_tc_sync_report_excpt
 * @brief	called for prepare test case
 * @param	test case info
 * @return	az_r_t
 * @exception	none
 */
az_r_t az_tc_sync_report_excpt(az_test_case_t *pTC)
{
  az_r_t r = AZ_SUCCESS;

  if ((az_tc_report_excpt_flags & AZ_TC_FLAG_TRACE) || (az_tc_flags & AZ_TC_FLAG_TRACE)) {
  }
  return r;
}

/**
 * @fn		az_tc_prolog_report_excpt
 * @brief	called before each test iteration
 * @param	test case info
 * @return	az_r_t
 * @exception	none
 */
az_r_t az_tc_prolog_report_excpt(az_test_case_t *pTC)
{
  az_r_t r = AZ_SUCCESS;
  az_test_iter_t *iter = AZ_TC_CUR_ITER(pTC);
  struct az_tc_testvector_st_report_excpt *tv = AZ_TC_ITER_GET_TV(iter, az_tc_tv_report_excpt_t);

  if ((az_tc_report_excpt_flags & AZ_TC_FLAG_TRACE) || (az_tc_flags & AZ_TC_FLAG_TRACE)) {
    AZ_TC_PRINT_START(pTC, iter);
  }
  return r;
}

/**
 * @fn		az_tc_run_report_excpt
 * @brief	called for each test iteration
 * @param	test case info
 * @return	az_r_t
 * @exception	none
 */

static az_xu_t xu = NULL; 

static az_xu_config_t xu_cfg = {
  .stackSize = 0,
  .policy = SCHED_RR,
  .priority = 50,
  .coremask = 0x0002,
  .startdelay = 0,
};

static void *xu_proc(void *arg)
{
  char *buffer = (char *)arg;

  *buffer = 0;

  return NULL;
}
az_r_t az_tc_run_report_excpt(az_test_case_t *pTC)
{
  az_r_t r = AZ_SUCCESS;
  az_xu_event_t evt;
  az_xu_msg_t msg;
  az_test_iter_t *iter = AZ_TC_CUR_ITER(pTC);
  struct az_tc_testvector_st_report_excpt *tv = AZ_TC_ITER_GET_TV(iter, az_tc_tv_report_excpt_t);

  tv->result = !(tv->expected_result);

  az_ion_id_t xu_id;
  do {
    xu_id = (az_r_t)az_xu_create("excpt", xu_proc, (void *)0x12345678, &xu_cfg, &xu);
    if (xu_id < 0) {
      tv->result = AZ_FAIL;
      break;
    }

    r = az_xu_recvEvent(AZ_XU_EVENT_MSG, AZ_XU_EVENT_OPT_PRESERVE, 1000000000, &evt); 
    if (r >= 0) {
      r = az_xu_recvMsg(&msg);
      az_rprintf(r, "xu msg: %lx from %x\n", msg, xu_id);
      switch(AZ_XU_MSG_CATEGORY(msg)) {
        case AZ_XU_MSG_EXCEPTION:
          if (xu_id == AZ_XU_MSG_GET_ION_ID(msg)) {
            tv->result = tv->expected_result;
          }
          break;
        default:
          tv->result = AZ_ERR(NOT_SUPPORT);
          break;
      }
      break;
    } else {
      tv->result = AZ_ERR(TIMEOUT);
    }
    break;
  } while (0);

  if ((az_tc_report_excpt_flags & AZ_TC_FLAG_TRACE) || (az_tc_flags & AZ_TC_FLAG_TRACE)) {
  }
  return r;
}

/**
 * @fn		az_tc_epilog_report_excpt
 * @brief	called after each test iteration
 * @param	test case info
 * @return	az_r_t
 * @exception	none
 */
az_r_t az_tc_epilog_report_excpt(az_test_case_t *pTC)
{
  az_r_t r = AZ_SUCCESS;
  az_test_iter_t *iter = AZ_TC_CUR_ITER(pTC);
  struct az_tc_testvector_st_report_excpt *tv = AZ_TC_ITER_GET_TV(iter, az_tc_tv_report_excpt_t);

  if (tv->result == tv->expected_result) {
    iter->result = AZ_SUCCESS;
  } else {
    iter->result = AZ_FAIL;
  }
  if ((az_tc_report_excpt_flags & AZ_TC_FLAG_TRACE) || (az_tc_flags & AZ_TC_FLAG_TRACE)) {
    AZ_TC_PRINT_END(pTC, iter);
  }
  return r;
}

/**
 * @fn		az_tc_term_report_excpt
 * @brief	called at test case terminating phase
 * @param	test case info
 * @return	az_r_t
 * @exception	none
 */
az_r_t az_tc_term_report_excpt(az_test_case_t *pTC)
{
  az_r_t r = AZ_SUCCESS;

  /* free the test vector array values from xml configuration file */

  if (NULL != az_tc_testvector_array_report_excpt) {
    az_free(az_tc_testvector_array_report_excpt);
    az_tc_testvector_array_report_excpt = NULL;
    AZ_TC_SET_TV(pTC, NULL);
  }

  if ((az_tc_report_excpt_flags & AZ_TC_FLAG_TRACE) || (az_tc_flags & AZ_TC_FLAG_TRACE)) {
    AZ_TC_PRINT_T(pTC);
  }
  return r;
}

/* === end of AZ_TC_REPORT_EXCPT_C === */
