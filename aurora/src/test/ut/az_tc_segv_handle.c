/**
 * @file   az_tc_segv_handle.c
 * @brief  
 * @date   30/03/18
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
 *   - 2018-03-30 dhkang initial create
 */

/* include header files */
#include "az_tc_segv_handle.h"
/* TODO: include function dependant header files */

/* declare global variables */

int az_tc_segv_handle_flags = AZ_TC_FLAG_TRACE;

/* define the reference for test vectors */
struct az_tc_testvector_st_segv_handle *az_tc_testvector_array_segv_handle = NULL;



/* implement global callback functions */

/**
 * @fn function name
 * @brief function-description
 * @param input-output-parameters
 * @return return-value
 * @exception none
 */


/**
 * @fn		az_tc_init_segv_handle
 * @brief	called for init test case
 * @param	test case info
 * @return	az_r_t
 * @exception	none
 */
az_r_t az_tc_init_segv_handle(az_test_case_t *pTC)
{
  az_r_t r = AZ_SUCCESS;

  if (pTC->test_iter_count > 0) {
    az_tc_testvector_array_segv_handle = (struct az_tc_testvector_st_segv_handle *)az_malloc(AZ_TC_TV_SZ(pTC, az_tc_tv_segv_handle_t));
    if (NULL == az_tc_testvector_array_segv_handle) {
      r = AZ_ERR(MALLOC);
    }
    /* here may set the test vector array values from xml configuration file */

    int j;
    az_tc_tv_segv_handle_t *tv = az_tc_testvector_array_segv_handle;
    az_xml_element_t *xe;
    az_test_iter_t *iter = AZ_TC_ITER_LIST(pTC);
    for (j = 0; j < AZ_TC_ITER_COUNT(pTC); j++, iter++, tv++) {
      AZ_TC_ITER_SET_TV(iter, tv);
      xe = az_xml_element_find_attr(iter->xml, "expected_result");
      if (xe) tv->expected_result = strtol(xe->kv.value, NULL, 10);
    }
  }
  AZ_TC_SET_TV(pTC, az_tc_testvector_array_segv_handle);
  if ((az_tc_segv_handle_flags & AZ_TC_FLAG_TRACE) || (az_tc_flags & AZ_TC_FLAG_TRACE)) {
    AZ_TC_PRINT_B(pTC);
  }
  return r;
}

/**
 * @fn		az_tc_sync_segv_handle
 * @brief	called for prepare test case
 * @param	test case info
 * @return	az_r_t
 * @exception	none
 */
az_r_t az_tc_sync_segv_handle(az_test_case_t *pTC)
{
  az_r_t r = AZ_SUCCESS;

  if ((az_tc_segv_handle_flags & AZ_TC_FLAG_TRACE) || (az_tc_flags & AZ_TC_FLAG_TRACE)) {
  }
  return r;
}

/**
 * @fn		az_tc_prolog_segv_handle
 * @brief	called before each test iteration
 * @param	test case info
 * @return	az_r_t
 * @exception	none
 */
az_r_t az_tc_prolog_segv_handle(az_test_case_t *pTC)
{
  az_r_t r = AZ_SUCCESS;
  az_test_iter_t *iter = AZ_TC_CUR_ITER(pTC);
  struct az_tc_testvector_st_segv_handle *tv = AZ_TC_ITER_GET_TV(iter, az_tc_tv_segv_handle_t);

  if ((az_tc_segv_handle_flags & AZ_TC_FLAG_TRACE) || (az_tc_flags & AZ_TC_FLAG_TRACE)) {
    AZ_TC_PRINT_START(pTC, iter);
  }
  return r;
}

/**
 * @fn		az_tc_run_segv_handle
 * @brief	called for each test iteration
 * @param	test case info
 * @return	az_r_t
 * @exception	none
 */
az_r_t az_tc_run_segv_handle(az_test_case_t *pTC)
{
  az_r_t r = AZ_SUCCESS;
  az_test_iter_t *iter = AZ_TC_CUR_ITER(pTC);
  volatile struct az_tc_testvector_st_segv_handle *tv = AZ_TC_ITER_GET_TV(iter, az_tc_tv_segv_handle_t);

  tv->result = !(tv->expected_result);
  if (0 == AZ_SYS_XU_SAVE_CONTEXT()) {
   *(int *)0x12345678 = 1;
   az_sys_xu_remove_context();
  } else {
    tv->result = tv->expected_result;
  }

  if ((az_tc_segv_handle_flags & AZ_TC_FLAG_TRACE) || (az_tc_flags & AZ_TC_FLAG_TRACE)) {
  }
  return r;
}

/**
 * @fn		az_tc_epilog_segv_handle
 * @brief	called after each test iteration
 * @param	test case info
 * @return	az_r_t
 * @exception	none
 */
az_r_t az_tc_epilog_segv_handle(az_test_case_t *pTC)
{
  az_r_t r = AZ_SUCCESS;
  az_test_iter_t *iter = AZ_TC_CUR_ITER(pTC);
  struct az_tc_testvector_st_segv_handle *tv = AZ_TC_ITER_GET_TV(iter, az_tc_tv_segv_handle_t);

  if (tv->result == tv->expected_result) {
    iter->result = AZ_SUCCESS;
    if (AZ_XU_IS_STATE_EXCPT(az_xu_self())) {
      iter->result |= AZ_TC_RESULT_FAILURE;
      //iter->result = AZ_TC_RESULT_SEGV;
      void *ptr = AZ_XU_EXCPT_POINT(az_xu_self());
      az_ilog("excpt: %p\n", ptr); 
      strncpy(iter->reason, az_addr2line(ptr, az_getExeFileName()), sizeof(iter->reason)); 
      az_xu_reset_state_excpt(az_xu_self());
    }
  } else {
    iter->result = AZ_FAIL;
  }

  if ((az_tc_segv_handle_flags & AZ_TC_FLAG_TRACE) || (az_tc_flags & AZ_TC_FLAG_TRACE)) {
    AZ_TC_PRINT_END(pTC, iter);
  }
  return r;
}

/**
 * @fn		az_tc_term_segv_handle
 * @brief	called at test case terminating phase
 * @param	test case info
 * @return	az_r_t
 * @exception	none
 */
az_r_t az_tc_term_segv_handle(az_test_case_t *pTC)
{
  az_r_t r = AZ_SUCCESS;

  /* free the test vector array values from xml configuration file */

  if (NULL != az_tc_testvector_array_segv_handle) {
    az_free(az_tc_testvector_array_segv_handle);
    az_tc_testvector_array_segv_handle = NULL;
    AZ_TC_SET_TV(pTC, NULL);
  }

  if ((az_tc_segv_handle_flags & AZ_TC_FLAG_TRACE) || (az_tc_flags & AZ_TC_FLAG_TRACE)) {
    AZ_TC_PRINT_T(pTC);
  }
  return r;
}

/* === end of AZ_TC_SEGV_HANDLE_C === */
