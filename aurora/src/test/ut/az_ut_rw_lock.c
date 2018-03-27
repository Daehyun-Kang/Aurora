/**
 * @file   az_ut_rw_lock.c
 * @brief  
 * @date   27/03/18
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
 *   - 2018-03-27 dhkang initial create
 */

/* include header files */
#include "aurora.h"
#include "az_test.h"
/* TODO: include function dependant header files */

#define AZ_UT_FUNC_HAS_RETURN  1
#undef AZ_UT_FUNC_VOID  
#include "az_unitTest.h"

/* declare global variables */

int az_ut_az_rw_lock_flags = AZ_UT_FLAG_TRACE;

/* this is unit test target function */
//extern az_r_t az_rw_lock;

/* this is to define test vector structure */
struct az_ut_testvector_st_az_rw_lock {
  char *  order;
  char *  expected_result;
  char *  result;
};

/* define the reference for test vectors */
struct az_ut_testvector_st_az_rw_lock *az_ut_testvector_array_az_rw_lock = NULL;

/* declare global callback functions */

/* this is init handler for each test case */
extern az_r_t az_ut_init_az_rw_lock(az_test_case_t *pTC);

/* this is sync handler for each test case */
extern az_r_t az_ut_sync_az_rw_lock(az_test_case_t *pTC);

/* this is prolog handler for each test iteration */
extern az_r_t az_ut_prolog_az_rw_lock(az_test_case_t *pTC);

/* this is run handler for each test iteration */
extern az_r_t az_ut_run_az_rw_lock(az_test_case_t *pTC);

/* this is epilog handler for each test iteration */
extern az_r_t az_ut_epilog_az_rw_lock(az_test_case_t *pTC);

/* this is term handler for each test case */
extern az_r_t az_ut_term_az_rw_lock(az_test_case_t *pTC);



/* implement global callback functions */

/**
 * @fn function name
 * @brief function-description
 * @param input-output-parameters
 * @return return-value
 * @exception none
 */


/**
 * @fn		az_ut_init_az_rw_lock
 * @brief	called for init test case
 * @param	test case info
 * @return	az_r_t
 * @exception	none
 */
static az_rw_lock_t  rw_lock;
static int exec_order;

az_r_t az_ut_init_az_rw_lock(az_test_case_t *pTC)
{
  az_r_t r = AZ_SUCCESS;

  if (pTC->test_iter_count > 0) {
    az_ut_testvector_array_az_rw_lock = (struct az_ut_testvector_st_az_rw_lock *)az_malloc(sizeof(struct az_ut_testvector_st_az_rw_lock)*(pTC->test_iter_count));
    if (NULL == az_ut_testvector_array_az_rw_lock) {
      r = AZ_ERR(MALLOC);
    }
    /* here you may want make up the test vector array values from xml configuration file */
    struct az_ut_testvector_st_az_rw_lock *tv =  az_ut_testvector_array_az_rw_lock;
    az_xml_element_t *xe;
    az_test_iter_t *iter = pTC->test_iter.list; 
    int j;
    for (j = 0; j < pTC->test_iter.count; j++, iter++, tv++) {
      iter-> test_vector = tv;

      xe = az_xml_element_find_attr(iter->xml, "order");
      if (xe) tv->order = xe->kv.value;
      xe = az_xml_element_find_attr(iter->xml, "expected_result");
      if (xe) tv->expected_result = xe->kv.value;
      xe = az_xml_element_find_attr(iter->xml, "result");
      if (xe) tv->result = xe->kv.value;
      az_printf("e=%s r=%s\n", tv->expected_result, tv->result);
    }


  }
  r = az_rw_lock_init(&rw_lock);

  pTC->test_vector = (void *)az_ut_testvector_array_az_rw_lock;
  if ((az_ut_az_rw_lock_flags & AZ_UT_FLAG_TRACE) || (az_ut_flags & AZ_UT_FLAG_TRACE)) {
    az_rprintf(r, "%s" AZ_NL, "...");
  }
  return r;
}

/**
 * @fn		az_ut_sync_az_rw_lock
 * @brief	called for prepare test case
 * @param	test case info
 * @return	az_r_t
 * @exception	none
 */
static az_xu_t xus[3] = {0};

void *xu_reader_proc(void *arg)
{
  char *buffer = (char *)arg;

  az_rw_lock_read(&rw_lock);
  buffer[exec_order] = 'r';
  exec_order++;
  az_xu_sleep(100000000);
  az_rw_unlock_read(&rw_lock);

  return NULL;
}
void *xu_writer_proc(void *arg)
{
  char *buffer = (char *)arg;

  az_rw_lock_write(&rw_lock);
  buffer[exec_order] = 'w';
  exec_order++;
  az_xu_sleep(100000000);
  az_rw_unlock_write(&rw_lock);

  return NULL;
}

az_r_t az_ut_sync_az_rw_lock(az_test_case_t *pTC)
{
  az_r_t r = AZ_SUCCESS;

  exec_order = 0;

  if ((az_ut_az_rw_lock_flags & AZ_UT_FLAG_TRACE) || (az_ut_flags & AZ_UT_FLAG_TRACE)) {
    az_rprintf(r, "%s" AZ_NL, "...");
  }
  return r;
}

/**
 * @fn		az_ut_prolog_az_rw_lock
 * @brief	called before each test iteration
 * @param	test case info
 * @return	az_r_t
 * @exception	none
 */
az_r_t az_ut_prolog_az_rw_lock(az_test_case_t *pTC)
{
  az_r_t r = AZ_SUCCESS;

  exec_order = 0;
  xus[0] = xus[1] = xus[2] = NULL; 
  if ((az_ut_az_rw_lock_flags & AZ_UT_FLAG_TRACE) || (az_ut_flags & AZ_UT_FLAG_TRACE)) {
    az_rprintf(r, "%s" AZ_NL, "...");
  }
  return r;
}

/**
 * @fn		az_ut_run_az_rw_lock
 * @brief	called for each test iteration
 * @param	test case info
 * @return	az_r_t
 * @exception	none
 */
static az_xu_config_t xu_cfg = {
  .stackSize = 0,
  .policy = SCHED_RR,
  .priority = 50,
  .coremask = 0x0002,
  .startdelay = 0,
};
az_r_t az_ut_run_az_rw_lock(az_test_case_t *pTC)
{
  az_r_t r = AZ_SUCCESS;
  az_test_iter_t *iter = pTC->test_iter.list + pTC->test_iter.index; 
  struct az_ut_testvector_st_az_rw_lock *tv =  az_ut_testvector_array_az_rw_lock + pTC->test_iter.index;
  char name[] = "r0"; 

  int j = 0;
  for(; j < 3; j++) {
    name[1] + j;
    if (tv->order[j] == 'r') {
      if (xu_cfg.policy == SCHED_FIFO) xu_cfg.priority = 1;
      az_xu_create(name, xu_reader_proc, tv->result, &xu_cfg, &xus[j]);
    } else
    if (tv->order[j] == 'w') {
      if (xu_cfg.policy == SCHED_FIFO) xu_cfg.priority = 99;
      az_xu_create(name, xu_writer_proc, tv->result, &xu_cfg, &xus[j]);
    }
    az_xu_sleep(1000);
  }

  if ((az_ut_az_rw_lock_flags & AZ_UT_FLAG_TRACE) || (az_ut_flags & AZ_UT_FLAG_TRACE)) {
    az_rprintf(r, "%s" AZ_NL, "...");
  }
  return r;
}

/**
 * @fn		az_ut_epilog_az_rw_lock
 * @brief	called after each test iteration
 * @param	test case info
 * @return	az_r_t
 * @exception	none
 */
az_r_t az_ut_epilog_az_rw_lock(az_test_case_t *pTC)
{
  az_r_t r = AZ_SUCCESS;
  az_test_iter_t *iter = pTC->test_iter.list + pTC->test_iter.index; 
  struct az_ut_testvector_st_az_rw_lock *tv =  az_ut_testvector_array_az_rw_lock + pTC->test_iter.index;

  char name[] = "r0";
  while (exec_order < 3) {
    az_xu_sleep(1000000000);
  }

  if (!strncmp(tv->expected_result, tv->result, 3)) {
    iter->result = AZ_SUCCESS;
  } else {
    iter->result = AZ_FAIL;
  }

  az_printf("e=%s r=%s %d\n", tv->expected_result, tv->result, iter->result); 

  if ((az_ut_az_rw_lock_flags & AZ_UT_FLAG_TRACE) || (az_ut_flags & AZ_UT_FLAG_TRACE)) {
    az_rprintf(r, "%s" AZ_NL, "...");
  }
  return r;
}

/**
 * @fn		az_ut_term_az_rw_lock
 * @brief	called at test case terminating phase
 * @param	test case info
 * @return	az_r_t
 * @exception	none
 */
az_r_t az_ut_term_az_rw_lock(az_test_case_t *pTC)
{
  az_r_t r = AZ_SUCCESS;

  r = az_rw_lock_deinit(&rw_lock);
  /* free the test vector array values from xml configuration file */

  if (NULL != az_ut_testvector_array_az_rw_lock) {
    az_free(az_ut_testvector_array_az_rw_lock);
    az_ut_testvector_array_az_rw_lock = NULL;
    pTC->test_vector = NULL;
  }

  if ((az_ut_az_rw_lock_flags & AZ_UT_FLAG_TRACE) || (az_ut_flags & AZ_UT_FLAG_TRACE)) {
    az_rprintf(r, "%s" AZ_NL, "...");
  }
  return r;
}

/* === end of AZ_UT_RW_LOCK_C === */
