/**
 * @file   az_tc_timer_start.c
 * @brief  
 * @date   13/04/18
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
 *   - 2018-04-13 dhkang initial create
 */

/* include header files */
#include "az_tc_timer_start.h"
/* TODO: include function dependant header files */

/* declare global variables */

int az_tc_timer_start_flags = AZ_TC_FLAG_TRACE;

/* define the reference for test vectors */
struct az_tc_testvector_st_timer_start *az_tc_testvector_array_timer_start = NULL;



/* implement global callback functions */

/**
 * @fn function name
 * @brief function-description
 * @param input-output-parameters
 * @return return-value
 * @exception none
 */

static az_timer_t timer = NULL;
static az_sys_timespec_t  ststamp, etstamp;

static void az_tc_timer_handler(void *arg)
{
  az_test_case_t *pTC = (az_test_case_t *)arg;
  az_test_iter_t *iter = AZ_TC_CUR_ITER(pTC);
  struct az_tc_testvector_st_timer_start *tv = AZ_TC_ITER_GET_TV(iter, az_tc_tv_timer_start_t);

  struct itimerspec cur_itime;

  timerfd_gettime(tv->id, &cur_itime);

  clock_gettime(CLOCK_REALTIME, &etstamp);
  /*
  uint64_t tdiff = az_prof_tdiff(etstamp,ststamp); 
  az_printf("iter%d: timeout %d tdiff=%ld\n", 
      iter->index, timer->sys_timer->repeat, tdiff);
  */

  timer->sys_timer->repeat--;

}

/**
 * @fn		az_tc_init_timer_start
 * @brief	called for init test case
 * @param	test case info
 * @return	az_r_t
 * @exception	none
 */
az_r_t az_tc_init_timer_start(az_test_case_t *pTC)
{
  az_r_t r = AZ_SUCCESS;

  if (pTC->test_iter_count > 0) {
    az_tc_testvector_array_timer_start = (struct az_tc_testvector_st_timer_start *)az_malloc(AZ_TC_TV_SZ(pTC, az_tc_tv_timer_start_t));
    if (NULL == az_tc_testvector_array_timer_start) {
      r = AZ_ERR(MALLOC);
    }
    /* here may set the test vector array values from xml configuration file */

    int j;
    az_tc_tv_timer_start_t *tv = az_tc_testvector_array_timer_start;
    az_xml_element_t *xe;
    az_test_iter_t *iter = AZ_TC_ITER_LIST(pTC);
    for (j = 0; j < AZ_TC_ITER_COUNT(pTC); j++, iter++, tv++) {
      AZ_TC_ITER_SET_TV(iter, tv);
      xe = az_xml_element_find_attr(iter->xml, "id");
      if (xe) tv->id = strtol(xe->kv.value, NULL, 10);
      xe = az_xml_element_find_attr(iter->xml, "interval");
      if (xe) tv->interval = strtol(xe->kv.value, NULL, 10);
      xe = az_xml_element_find_attr(iter->xml, "repeat");
      if (xe) tv->repeat = strtol(xe->kv.value, NULL, 10);
      xe = az_xml_element_find_attr(iter->xml, "expected_result");
      if (xe) tv->expected_result = strtol(xe->kv.value, NULL, 10);

      az_printf("tv%d id:%d interval:%ld repeat:%d\n", j, tv->id, tv->interval, tv->repeat); 
    }
  }
  AZ_TC_SET_TV(pTC, az_tc_testvector_array_timer_start);
  if ((az_tc_timer_start_flags & AZ_TC_FLAG_TRACE) || (az_tc_flags & AZ_TC_FLAG_TRACE)) {
    AZ_TC_PRINT_B(pTC);
  }
  return r;
}

/**
 * @fn		az_tc_sync_timer_start
 * @brief	called for prepare test case
 * @param	test case info
 * @return	az_r_t
 * @exception	none
 */
az_r_t az_tc_sync_timer_start(az_test_case_t *pTC)
{
  az_r_t r = AZ_SUCCESS;

  if ((az_tc_timer_start_flags & AZ_TC_FLAG_TRACE) || (az_tc_flags & AZ_TC_FLAG_TRACE)) {
  }
  return r;
}

/**
 * @fn		az_tc_prolog_timer_start
 * @brief	called before each test iteration
 * @param	test case info
 * @return	az_r_t
 * @exception	none
 */
az_r_t az_tc_prolog_timer_start(az_test_case_t *pTC)
{
  az_r_t r = AZ_SUCCESS;
  az_test_iter_t *iter = AZ_TC_CUR_ITER(pTC);
  struct az_tc_testvector_st_timer_start *tv = AZ_TC_ITER_GET_TV(iter, az_tc_tv_timer_start_t);
  timer = NULL;
  tv->id  = az_timer_create("az_timer", tv->interval, tv->repeat, az_tc_timer_handler, pTC, &timer); 

  if (tv->id < 0) {
    az_rprintf(r, "%s", "timer create fail\n");
    r = tv->id;
  } else {
    az_printf("timer id %d create ok\n", tv->id);
  }

  if ((az_tc_timer_start_flags & AZ_TC_FLAG_TRACE) || (az_tc_flags & AZ_TC_FLAG_TRACE)) {
    AZ_TC_PRINT_START(pTC, iter);
  }
  return r;
}

/**
 * @fn		az_tc_run_timer_start
 * @brief	called for each test iteration
 * @param	test case info
 * @return	az_r_t
 * @exception	none
 */
az_r_t az_tc_run_timer_start(az_test_case_t *pTC)
{
  az_r_t r = AZ_SUCCESS;
  az_test_iter_t *iter = AZ_TC_CUR_ITER(pTC);
  struct az_tc_testvector_st_timer_start *tv = AZ_TC_ITER_GET_TV(iter, az_tc_tv_timer_start_t);

  AZ_TC_PRINT_RUN(pTC, iter);

  az_sys_io_event_t ioevt;

  az_sys_xu_open_iomux();


  //az_sys_xu_iomux_add(tv->id, AZ_SYS_IO_IN|AZ_SYS_IO_ET);
  clock_gettime(CLOCK_REALTIME, &ststamp);
  r = az_timer_start(tv->id);
  if (r < 0) {
    az_rprintf(r, "%s", "timer start fail\n");
  } else {
    az_printf("timer %d start ok\n", tv->id);
    do {
      int nevt = az_sys_xu_wait_iomux(&ioevt, 1, -1);
      if (nevt > 0) {
        az_tc_timer_handler(pTC);
      }
    } while (timer->sys_timer->repeat > 0);
    az_timer_stop(tv->id);
    //az_sys_xu_iomux_del(tv->id);
    uint64_t tdiff = az_prof_tdiff(etstamp,ststamp); 
    az_printf("iter%d: timeout %d tdiff=%ld\n", 
        iter->index, timer->sys_timer->repeat, tdiff);
  }

  if ((az_tc_timer_start_flags & AZ_TC_FLAG_TRACE) || (az_tc_flags & AZ_TC_FLAG_TRACE)) {
  }
  return r;
}

/**
 * @fn		az_tc_epilog_timer_start
 * @brief	called after each test iteration
 * @param	test case info
 * @return	az_r_t
 * @exception	none
 */
az_r_t az_tc_epilog_timer_start(az_test_case_t *pTC)
{
  az_r_t r = AZ_SUCCESS;
  az_test_iter_t *iter = AZ_TC_CUR_ITER(pTC);
  struct az_tc_testvector_st_timer_start *tv = AZ_TC_ITER_GET_TV(iter, az_tc_tv_timer_start_t);

  timer = NULL;
  r = az_timer_delete(tv->id);
  if (r < 0) {
    az_rprintf(r, "%s", "timer delete fail\n");
  }
  if ((az_tc_timer_start_flags & AZ_TC_FLAG_TRACE) || (az_tc_flags & AZ_TC_FLAG_TRACE)) {
    AZ_TC_PRINT_END(pTC, iter);
  }
  return r;
}

/**
 * @fn		az_tc_term_timer_start
 * @brief	called at test case terminating phase
 * @param	test case info
 * @return	az_r_t
 * @exception	none
 */
az_r_t az_tc_term_timer_start(az_test_case_t *pTC)
{
  az_r_t r = AZ_SUCCESS;

  /* free the test vector array values from xml configuration file */

  if (NULL != az_tc_testvector_array_timer_start) {
    az_free(az_tc_testvector_array_timer_start);
    az_tc_testvector_array_timer_start = NULL;
    AZ_TC_SET_TV(pTC, NULL);
  }

  if ((az_tc_timer_start_flags & AZ_TC_FLAG_TRACE) || (az_tc_flags & AZ_TC_FLAG_TRACE)) {
    AZ_TC_PRINT_T(pTC);
  }
  return r;
}
#if 0
#endif
/* === end of AZ_TC_TIMER_START_C === */
