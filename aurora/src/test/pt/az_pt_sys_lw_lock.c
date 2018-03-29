/**
 * @file   az_pt_sys_lw_lock.c
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
#include "az_pt_sys_lw_lock.h"
/* TODO: include function dependant header files */

/* declare global variables */

int az_pt_sys_lw_lock_flags = AZ_TC_FLAG_TRACE;

/* define the reference for test vectors */
struct az_pt_testvector_st_sys_lw_lock *az_pt_testvector_array_sys_lw_lock = NULL;


static az_sys_lw_lock_t  sys_lw_lock;

static az_xu_t xu = NULL;

static az_xu_config_t xu_cfg = {
  .stackSize = 0,
  .policy = SCHED_FIFO,
  .priority = 50,
  .coremask = 0x0002,
  .startdelay = 0,
};

static void *xu_measure_proc(void *arg)
{
  az_test_case_t *pTC = (az_test_case_t *)arg; 
  az_test_iter_t *iter = AZ_TC_CUR_ITER(pTC);
  struct az_pt_testvector_st_sys_lw_lock *tv = AZ_TC_ITER_GET_TV(iter, az_pt_tv_sys_lw_lock_t);

  uint64_t stime, mtime, etime; 
  int j;
  assert(tv->samples != 0);
  int samples = tv->samples;
  
  if (strstr(pTC->name, "sys_lw_lock")) {
    stime = az_timestamp();
    for (j = 0; j < samples; j++) {
      az_sys_lw_lock(&sys_lw_lock);
      mtime = az_timestamp();
      az_sys_lw_unlock(&sys_lw_lock );
    }
    etime = az_timestamp();
  } else 
  if (strstr(pTC->name, "pthread_mutex")) {
    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);

    stime = az_timestamp();
    for (j = 0; j < samples; j++) {
      pthread_mutex_lock(&mutex);
      mtime = az_timestamp();
      pthread_mutex_unlock(&mutex );
    }
    etime = az_timestamp();

    pthread_mutex_destroy(&mutex);
  }
  tv->result = etime - stime;

  xu = NULL;
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
 * @fn		az_pt_init_sys_lw_lock
 * @brief	called for init test case
 * @param	test case info
 * @return	az_r_t
 * @exception	none
 */
az_r_t az_pt_init_sys_lw_lock(az_test_case_t *pTC)
{
  az_r_t r = AZ_SUCCESS;

  if (pTC->test_iter_count > 0) {
    az_pt_testvector_array_sys_lw_lock = (struct az_pt_testvector_st_sys_lw_lock *)az_malloc(AZ_TC_TV_SZ(pTC, az_pt_tv_sys_lw_lock_t));
    if (NULL == az_pt_testvector_array_sys_lw_lock) {
      r = AZ_ERR(MALLOC);
    }
    /* here may set the test vector array values from xml configuration file */

    int j;
    az_pt_tv_sys_lw_lock_t *tv = az_pt_testvector_array_sys_lw_lock;
    az_xml_element_t *xe;
    az_test_iter_t *iter = AZ_TC_ITER_LIST(pTC);
    for (j = 0; j < AZ_TC_ITER_COUNT(pTC); j++, iter++, tv++) {
      AZ_TC_ITER_SET_TV(iter, tv);

      xe = az_xml_element_find_attr(iter->xml, "samples");
      if (xe) tv->samples = strtol(xe->kv.value, NULL, 10);
      xe = az_xml_element_find_attr(iter->xml, "result");
      if (xe) tv->result = xe->kv.value;
      az_dlog("samples=%d r=%ld\n", tv->samples, tv->result);
    }
  }
  AZ_TC_SET_TV(pTC, az_pt_testvector_array_sys_lw_lock);


  if ((az_pt_sys_lw_lock_flags & AZ_TC_FLAG_TRACE) || (az_tc_flags & AZ_TC_FLAG_TRACE)) {
    AZ_TC_PRINT_B(pTC);
  }
  return r;
}

/**
 * @fn		az_pt_sync_sys_lw_lock
 * @brief	called for prepare test case
 * @param	test case info
 * @return	az_r_t
 * @exception	none
 */
az_r_t az_pt_sync_sys_lw_lock(az_test_case_t *pTC)
{
  az_r_t r = AZ_SUCCESS;

  r = az_sys_lw_lock_init(&sys_lw_lock);

  if ((az_pt_sys_lw_lock_flags & AZ_TC_FLAG_TRACE) || (az_tc_flags & AZ_TC_FLAG_TRACE)) {
    az_rprintf(r, "%s" AZ_NL, "...");
  }
  return r;
}

/**
 * @fn		az_pt_prolog_sys_lw_lock
 * @brief	called before each test iteration
 * @param	test case info
 * @return	az_r_t
 * @exception	none
 */
az_r_t az_pt_prolog_sys_lw_lock(az_test_case_t *pTC)
{
  az_r_t r = AZ_SUCCESS;
  az_test_iter_t *iter = AZ_TC_CUR_ITER(pTC);
  struct az_pt_testvector_st_sys_lw_lock *tv = AZ_TC_ITER_GET_TV(iter, az_pt_tv_sys_lw_lock_t);

  xu = NULL;

  if ((az_pt_sys_lw_lock_flags & AZ_TC_FLAG_TRACE) || (az_tc_flags & AZ_TC_FLAG_TRACE)) {
    AZ_TC_PRINT_START(pTC, iter);
  }
  return r;
}

/**
 * @fn		az_pt_run_sys_lw_lock
 * @brief	called for each test iteration
 * @param	test case info
 * @return	az_r_t
 * @exception	none
 */
az_r_t az_pt_run_sys_lw_lock(az_test_case_t *pTC)
{
  az_r_t r = AZ_SUCCESS;
  az_test_iter_t *iter = AZ_TC_CUR_ITER(pTC);
  struct az_pt_testvector_st_sys_lw_lock *tv = AZ_TC_ITER_GET_TV(iter, az_pt_tv_sys_lw_lock_t);

  char name[] = "measureRwLock"; 

  r = az_xu_create(name, xu_measure_proc, pTC, &xu_cfg, &xu);
  if (r) az_rprintf(r, "thread %s create\n", name);


  if ((az_pt_sys_lw_lock_flags & AZ_TC_FLAG_TRACE) || (az_tc_flags & AZ_TC_FLAG_TRACE)) {
    az_rprintf(r, "%s" AZ_NL, "...");
  }
  return r;
}

/**
 * @fn		az_pt_epilog_sys_lw_lock
 * @brief	called after each test iteration
 * @param	test case info
 * @return	az_r_t
 * @exception	none
 */
az_r_t az_pt_epilog_sys_lw_lock(az_test_case_t *pTC)
{
  az_r_t r = AZ_SUCCESS;
  az_test_iter_t *iter = AZ_TC_CUR_ITER(pTC);
  struct az_pt_testvector_st_sys_lw_lock *tv = AZ_TC_ITER_GET_TV(iter, az_pt_tv_sys_lw_lock_t);


  do {
    az_xu_sleep(1000);
  } while (xu);

  iter->samples = tv->samples;
  iter->result = tv->result/tv->samples;
  az_ilog("%s:%d cycles=%ld\n", pTC->name, iter->index, tv->result);

  if ((az_pt_sys_lw_lock_flags & AZ_TC_FLAG_TRACE) || (az_tc_flags & AZ_TC_FLAG_TRACE)) {
    AZ_TC_PRINT_END(pTC, iter);
  }
  return r;
}

/**
 * @fn		az_pt_term_sys_lw_lock
 * @brief	called at test case terminating phase
 * @param	test case info
 * @return	az_r_t
 * @exception	none
 */
az_r_t az_pt_term_sys_lw_lock(az_test_case_t *pTC)
{
  az_r_t r = AZ_SUCCESS;

  r = az_sys_lw_lock_deinit(&sys_lw_lock);
  /* free the test vector array values from xml configuration file */

  if (NULL != az_pt_testvector_array_sys_lw_lock) {
    az_free(az_pt_testvector_array_sys_lw_lock);
    az_pt_testvector_array_sys_lw_lock = NULL;
    AZ_TC_SET_TV(pTC, NULL);
  }

  if ((az_pt_sys_lw_lock_flags & AZ_TC_FLAG_TRACE) || (az_tc_flags & AZ_TC_FLAG_TRACE)) {
    AZ_TC_PRINT_T(pTC);
  }
  return r;
}
#if 0
#endif

/* === end of AZ_PC_RW_LOCK_C === */
