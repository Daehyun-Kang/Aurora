/**
 * @file   az_pt_az_log_printf.c
 * @brief  
 * @date   29/03/18
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
 *   - 2018-03-29 dhkang initial create
 */

/* include header files */
#include "az_pt_az_log_printf.h"
/* TODO: include function dependant header files */

/* declare global variables */

int az_pt_az_log_printf_flags = AZ_TC_FLAG_TRACE;

/* define the reference for test vectors */
struct az_pt_testvector_st_az_log_printf *az_pt_testvector_array_az_log_printf = NULL;



/* implement global callback functions */

/**
 * @fn function name
 * @brief function-description
 * @param input-output-parameters
 * @return return-value
 * @exception none
 */


/**
 * @fn		az_pt_init_az_log_printf
 * @brief	called for init test case
 * @param	test case info
 * @return	az_r_t
 * @exception	none
 */
az_r_t az_pt_init_az_log_printf(az_test_case_t *pTC)
{
  az_r_t r = AZ_SUCCESS;

  if (pTC->test_iter_count > 0) {
    az_pt_testvector_array_az_log_printf = (struct az_pt_testvector_st_az_log_printf *)az_malloc(AZ_TC_TV_SZ(pTC, az_pt_tv_az_log_printf_t));
    if (NULL == az_pt_testvector_array_az_log_printf) {
      r = AZ_ERR(MALLOC);
    }
    /* here may set the test vector array values from xml configuration file */

    int j;
    az_pt_tv_az_log_printf_t *tv = az_pt_testvector_array_az_log_printf;
    az_xml_element_t *xe;
    az_test_iter_t *iter = AZ_TC_ITER_LIST(pTC);
    for (j = 0; j < AZ_TC_ITER_COUNT(pTC); j++, iter++, tv++) {
      AZ_TC_ITER_SET_TV(iter, tv);
      xe = az_xml_element_find_attr(iter->xml, "samples");
      if (xe) tv->samples = strtol(xe->kv.value, NULL, 10);
    }
  }
  AZ_TC_SET_TV(pTC, az_pt_testvector_array_az_log_printf);
  if ((az_pt_az_log_printf_flags & AZ_TC_FLAG_TRACE) || (az_tc_flags & AZ_TC_FLAG_TRACE)) {
    AZ_TC_PRINT_B(pTC);
  }
  return r;
}

/**
 * @fn		az_pt_sync_az_log_printf
 * @brief	called for prepare test case
 * @param	test case info
 * @return	az_r_t
 * @exception	none
 */
az_r_t az_pt_sync_az_log_printf(az_test_case_t *pTC)
{
  az_r_t r = AZ_SUCCESS;

  if ((az_pt_az_log_printf_flags & AZ_TC_FLAG_TRACE) || (az_tc_flags & AZ_TC_FLAG_TRACE)) {
  }
  return r;
}

/**
 * @fn		az_pt_prolog_az_log_printf
 * @brief	called before each test iteration
 * @param	test case info
 * @return	az_r_t
 * @exception	none
 */
az_r_t az_pt_prolog_az_log_printf(az_test_case_t *pTC)
{
  az_r_t r = AZ_SUCCESS;
  az_test_iter_t *iter = AZ_TC_CUR_ITER(pTC);
  struct az_pt_testvector_st_az_log_printf *tv = AZ_TC_ITER_GET_TV(iter, az_pt_tv_az_log_printf_t);

  if ((az_pt_az_log_printf_flags & AZ_TC_FLAG_TRACE) || (az_tc_flags & AZ_TC_FLAG_TRACE)) {
    AZ_TC_PRINT_START(pTC, iter);
  }
  return r;
}

/**
 * @fn		az_pt_run_az_log_printf
 * @brief	called for each test iteration
 * @param	test case info
 * @return	az_r_t
 * @exception	none
 */
az_r_t az_pt_run_az_log_printf(az_test_case_t *pTC)
{
  az_r_t r = AZ_SUCCESS;
  az_test_iter_t *iter = AZ_TC_CUR_ITER(pTC);
  struct az_pt_testvector_st_az_log_printf *tv = AZ_TC_ITER_GET_TV(iter, az_pt_tv_az_log_printf_t);

  if ((az_pt_az_log_printf_flags & AZ_TC_FLAG_TRACE) || (az_tc_flags & AZ_TC_FLAG_TRACE)) {
  }
  return r;
}

/**
 * @fn		az_pt_epilog_az_log_printf
 * @brief	called after each test iteration
 * @param	test case info
 * @return	az_r_t
 * @exception	none
 */
az_r_t az_pt_epilog_az_log_printf(az_test_case_t *pTC)
{
  az_r_t r = AZ_SUCCESS;
  az_test_iter_t *iter = AZ_TC_CUR_ITER(pTC);
  struct az_pt_testvector_st_az_log_printf *tv = AZ_TC_ITER_GET_TV(iter, az_pt_tv_az_log_printf_t);

  if ((az_pt_az_log_printf_flags & AZ_TC_FLAG_TRACE) || (az_tc_flags & AZ_TC_FLAG_TRACE)) {
    AZ_TC_PRINT_END(pTC, iter);
  }
  return r;
}

/**
 * @fn		az_pt_term_az_log_printf
 * @brief	called at test case terminating phase
 * @param	test case info
 * @return	az_r_t
 * @exception	none
 */
az_r_t az_pt_term_az_log_printf(az_test_case_t *pTC)
{
  az_r_t r = AZ_SUCCESS;

  /* free the test vector array values from xml configuration file */

  if (NULL != az_pt_testvector_array_az_log_printf) {
    az_free(az_pt_testvector_array_az_log_printf);
    az_pt_testvector_array_az_log_printf = NULL;
    AZ_TC_SET_TV(pTC, NULL);
  }

  if ((az_pt_az_log_printf_flags & AZ_TC_FLAG_TRACE) || (az_tc_flags & AZ_TC_FLAG_TRACE)) {
    AZ_TC_PRINT_T(pTC);
  }
  return r;
}

/* === end of AZ_PT_AZ_LOG_PRINTF_C === */
