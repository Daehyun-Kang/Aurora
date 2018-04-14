/**
 * @file   az_tc_rw_lock.c
 * @brief  
 * @date   28/03/18
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
 *   - 2018-03-28 dhkang initial create
 */

/* include header files */
#include "az_tc_rw_lock.h"
/* TODO: include function dependant header files */

/* declare global variables */

int az_tc_rw_lock_flags = AZ_TC_FLAG_TRACE;

/* define the reference for test vectors */
struct az_tc_testvector_st_rw_lock *az_tc_testvector_array_rw_lock = NULL;


static az_rw_lock_t  rw_lock;
static int exec_order;

static az_thread_t xus[3] = {0};

static az_thread_config_t xu_cfg = {
  .stackSize = 0,
  .policy = SCHED_RR,
  .priority = 50,
  .coremask = 0x0002,
  .startdelay = 0,
};

static void *xu_reader_proc(void *arg)
{
  char *buffer = (char *)arg;

  az_rw_lock_read(&rw_lock);
  buffer[exec_order] = 'r';
  exec_order++;
  az_thread_sleep(100000000);
  az_rw_unlock_read(&rw_lock);

  return NULL;
}
static void *xu_writer_proc(void *arg)
{
  char *buffer = (char *)arg;

  az_rw_lock_write(&rw_lock);
  buffer[exec_order] = 'w';
  exec_order++;
  az_thread_sleep(100000000);
  az_rw_unlock_write(&rw_lock);

  return NULL;
}

/* implement global callback functions */

/**
 * @fn function name
 * @brief function-description
 * @param input-output-parameters
 * @return return-value
 * @exception none
 */


/**
 * @fn		az_tc_init_rw_lock
 * @brief	called for init test case
 * @param	test case info
 * @return	az_r_t
 * @exception	none
 */
az_r_t az_tc_init_rw_lock(az_test_case_t *pTC)
{
  az_r_t r = AZ_SUCCESS;

  if (pTC->test_iter_count > 0) {
    az_tc_testvector_array_rw_lock = (struct az_tc_testvector_st_rw_lock *)az_malloc(AZ_TC_TV_SZ(pTC, az_tc_tv_rw_lock_t));
    if (NULL == az_tc_testvector_array_rw_lock) {
      r = AZ_ERR(MALLOC);
    }
    /* here may set the test vector array values from xml configuration file */

    int j;
    az_tc_tv_rw_lock_t *tv = az_tc_testvector_array_rw_lock;
    az_xml_element_t *xe;
    az_test_iter_t *iter = AZ_TC_ITER_LIST(pTC);
    for (j = 0; j < AZ_TC_ITER_COUNT(pTC); j++, iter++, tv++) {
      AZ_TC_ITER_SET_TV(iter, tv);

      xe = az_xml_element_find_attr(iter->xml, "order");
      if (xe) tv->order = xe->kv.value;
      xe = az_xml_element_find_attr(iter->xml, "expected_result");
      if (xe) tv->expected_result = xe->kv.value;
      xe = az_xml_element_find_attr(iter->xml, "result");
      if (xe) tv->result = xe->kv.value;
      az_dlog("e=%s r=%s\n", tv->expected_result, tv->result);
    }
  }
  AZ_TC_SET_TV(pTC, az_tc_testvector_array_rw_lock);


  if ((az_tc_rw_lock_flags & AZ_TC_FLAG_TRACE) || (az_tc_flags & AZ_TC_FLAG_TRACE)) {
    AZ_TC_PRINT_B(pTC);
  }
  return r;
}

/**
 * @fn		az_tc_sync_rw_lock
 * @brief	called for prepare test case
 * @param	test case info
 * @return	az_r_t
 * @exception	none
 */
az_r_t az_tc_sync_rw_lock(az_test_case_t *pTC)
{
  az_r_t r = AZ_SUCCESS;

  exec_order = 0;
  r = az_rw_lock_init(&rw_lock);

  if ((az_tc_rw_lock_flags & AZ_TC_FLAG_TRACE) || (az_tc_flags & AZ_TC_FLAG_TRACE)) {
    az_rprintf(r, "%s" AZ_NL, "...");
  }
  return r;
}

/**
 * @fn		az_tc_prolog_rw_lock
 * @brief	called before each test iteration
 * @param	test case info
 * @return	az_r_t
 * @exception	none
 */
az_r_t az_tc_prolog_rw_lock(az_test_case_t *pTC)
{
  az_r_t r = AZ_SUCCESS;
  az_test_iter_t *iter = AZ_TC_CUR_ITER(pTC);
  struct az_tc_testvector_st_rw_lock *tv = AZ_TC_ITER_GET_TV(iter, az_tc_tv_rw_lock_t);

  exec_order = 0;
  xus[0] = xus[1] = xus[2] = NULL; 

  if ((az_tc_rw_lock_flags & AZ_TC_FLAG_TRACE) || (az_tc_flags & AZ_TC_FLAG_TRACE)) {
    AZ_TC_PRINT_START(pTC, iter);
  }
  return r;
}

/**
 * @fn		az_tc_run_rw_lock
 * @brief	called for each test iteration
 * @param	test case info
 * @return	az_r_t
 * @exception	none
 */
az_r_t az_tc_run_rw_lock(az_test_case_t *pTC)
{
  az_r_t r = AZ_SUCCESS;
  az_test_iter_t *iter = AZ_TC_CUR_ITER(pTC);
  struct az_tc_testvector_st_rw_lock *tv = AZ_TC_ITER_GET_TV(iter, az_tc_tv_rw_lock_t);

  char name[] = "r0"; 

  int j = 0;
  for(; j < 3; j++) {
    name[1] += j;
    if (tv->order[j] == 'r') {
      if (xu_cfg.policy == SCHED_FIFO) xu_cfg.priority = 1;
      r = (az_r_t)az_thread_create(name, xu_reader_proc, tv->result, &xu_cfg, &xus[j]);
    } else
    if (tv->order[j] == 'w') {
      if (xu_cfg.policy == SCHED_FIFO) xu_cfg.priority = 99;
      r = (az_r_t)az_thread_create(name, xu_writer_proc, tv->result, &xu_cfg, &xus[j]);
    }
    if (r < AZ_SUCCESS) az_rprintf(r, "thread %s create\n", name);
    az_thread_sleep(1000);
  }

  if ((az_tc_rw_lock_flags & AZ_TC_FLAG_TRACE) || (az_tc_flags & AZ_TC_FLAG_TRACE)) {
    az_rprintf(r, "%s" AZ_NL, "...");
  }
  return r;
}

/**
 * @fn		az_tc_epilog_rw_lock
 * @brief	called after each test iteration
 * @param	test case info
 * @return	az_r_t
 * @exception	none
 */
az_r_t az_tc_epilog_rw_lock(az_test_case_t *pTC)
{
  az_r_t r = AZ_SUCCESS;
  az_test_iter_t *iter = AZ_TC_CUR_ITER(pTC);
  struct az_tc_testvector_st_rw_lock *tv = AZ_TC_ITER_GET_TV(iter, az_tc_tv_rw_lock_t);

  char name[] = "r0";
  while (exec_order < 3) {
    az_thread_sleep(1000000000);
  }

  if (!strncmp(tv->expected_result, tv->result, 3)) {
    iter->result = AZ_SUCCESS;
  } else {
    iter->result = AZ_FAIL;
  }

  az_ilog("%s:%d e=%s r=%s [%s]\n", 
      pTC->name, iter->index,
      tv->expected_result, tv->result, 
      az_err_str(iter->result)); 

  if ((az_tc_rw_lock_flags & AZ_TC_FLAG_TRACE) || (az_tc_flags & AZ_TC_FLAG_TRACE)) {
    AZ_TC_PRINT_END(pTC, iter);
  }
  return r;
}

/**
 * @fn		az_tc_term_rw_lock
 * @brief	called at test case terminating phase
 * @param	test case info
 * @return	az_r_t
 * @exception	none
 */
az_r_t az_tc_term_rw_lock(az_test_case_t *pTC)
{
  az_r_t r = AZ_SUCCESS;

  r = az_rw_lock_deinit(&rw_lock);
  /* free the test vector array values from xml configuration file */

  if (NULL != az_tc_testvector_array_rw_lock) {
    az_free(az_tc_testvector_array_rw_lock);
    az_tc_testvector_array_rw_lock = NULL;
    AZ_TC_SET_TV(pTC, NULL);
  }

  if ((az_tc_rw_lock_flags & AZ_TC_FLAG_TRACE) || (az_tc_flags & AZ_TC_FLAG_TRACE)) {
    AZ_TC_PRINT_T(pTC);
  }
  return r;
}

/* === end of AZ_TC_RW_LOCK_C === */
