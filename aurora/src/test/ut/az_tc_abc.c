/**
 * @file   az_tc_abc.c
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
#include "az_tc_abc.h"
/* TODO: include function dependant header files */

/* declare global variables */

int az_tc_abc_flags = AZ_TC_FLAG_TRACE;

/* define the reference for test vectors */
AZ_SYS_TLS struct az_tc_testvector_st_abc *az_tc_testvector_array_abc = NULL;



/* implement global callback functions */

/**
 * @fn function name
 * @brief function-description
 * @param input-output-parameters
 * @return return-value
 * @exception none
 */


/**
 * @fn		az_tc_init_abc
 * @brief	called for init test case
 * @param	test case info
 * @return	az_r_t
 * @exception	none
 */
az_r_t az_tc_init_abc(az_test_case_t *pTC)
{
  az_r_t r = AZ_SUCCESS;

  if (pTC->test_iter_count > 0) {
    az_tc_testvector_array_abc = (struct az_tc_testvector_st_abc *)az_malloc(AZ_TC_TV_SZ(pTC, az_tc_tv_abc_t));
    if (NULL == az_tc_testvector_array_abc) {
      r = AZ_ERR(MALLOC);
    }
    /* here may set the test vector array values from xml configuration file */

    int j;
    az_tc_tv_abc_t *tv = az_tc_testvector_array_abc;
    az_xml_element_t *xe;
    az_test_iter_t *iter = AZ_TC_ITER_LIST(pTC); 
    for (j = 0; j < AZ_TC_ITER_COUNT(pTC); j++, iter++, tv++) {
      AZ_TC_ITER_SET_TV(iter, tv);

      xe = az_xml_element_find_attr(iter->xml, "a");
      if (xe) tv->a = strtol(xe->kv.value, NULL, 10);

      xe = az_xml_element_find_attr(iter->xml, "b");
      if (xe) tv->b = strtol(xe->kv.value, NULL, 10);

      xe = az_xml_element_find_attr(iter->xml, "expected_result");
      if (xe) tv->expected_result = strtol(xe->kv.value, NULL, 10);

      az_printf("a=%d b=%d r=%d\n", tv->a, tv->b, tv->expected_result);
    }
  }
  AZ_TC_SET_TV(pTC, az_tc_testvector_array_abc);
  if ((az_tc_abc_flags & AZ_TC_FLAG_TRACE) || (az_tc_flags & AZ_TC_FLAG_TRACE)) {
    az_rprintf(r, "%s" AZ_NL, "...");
  }
  return r;
}

/**
 * @fn		az_tc_sync_abc
 * @brief	called for prepare test case
 * @param	test case info
 * @return	az_r_t
 * @exception	none
 */
az_r_t az_tc_sync_abc(az_test_case_t *pTC)
{
  az_r_t r = AZ_SUCCESS;

  if ((az_tc_abc_flags & AZ_TC_FLAG_TRACE) || (az_tc_flags & AZ_TC_FLAG_TRACE)) {
    az_rprintf(r, "%s" AZ_NL, "...");
  }
  return r;
}

/**
 * @fn		az_tc_prolog_abc
 * @brief	called before each test iteration
 * @param	test case info
 * @return	az_r_t
 * @exception	none
 */
az_r_t az_tc_prolog_abc(az_test_case_t *pTC)
{
  az_r_t r = AZ_SUCCESS;
  az_test_iter_t *iter = AZ_TC_CUR_ITER(pTC);
  struct az_tc_testvector_st_abc *tv = AZ_TC_ITER_GET_TV(iter, az_tc_tv_abc_t);


  if ((az_tc_abc_flags & AZ_TC_FLAG_TRACE) || (az_tc_flags & AZ_TC_FLAG_TRACE)) {
    az_rprintf(r, "%s" AZ_NL, "...");
  }
  return r;
}

/**
 * @fn		az_tc_run_abc
 * @brief	called for each test iteration
 * @param	test case info
 * @return	az_r_t
 * @exception	none
 */
az_r_t az_tc_run_abc(az_test_case_t *pTC)
{
  az_r_t r = AZ_SUCCESS;
  az_test_iter_t *iter = AZ_TC_CUR_ITER(pTC);
  struct az_tc_testvector_st_abc *tv = AZ_TC_ITER_GET_TV(iter, az_tc_tv_abc_t);

  az_printf("iter%d: a=%d b=%d r=%d\n", iter->index, tv->a, tv->b, tv->expected_result);

  if (tv->a >= 0 && tv->b >= 0) {
    tv->result = tv->a + tv->b;
  } else {
    tv->result = tv->expected_result; 
  }

  if ((az_tc_abc_flags & AZ_TC_FLAG_TRACE) || (az_tc_flags & AZ_TC_FLAG_TRACE)) {
    az_rprintf(r, "%s" AZ_NL, "...");
  }
  return r;
}

/**
 * @fn		az_tc_epilog_abc
 * @brief	called after each test iteration
 * @param	test case info
 * @return	az_r_t
 * @exception	none
 */
az_r_t az_tc_epilog_abc(az_test_case_t *pTC)
{
  az_r_t r = AZ_SUCCESS;
  az_test_iter_t *iter = AZ_TC_CUR_ITER(pTC);
  struct az_tc_testvector_st_abc *tv = AZ_TC_ITER_GET_TV(iter, az_tc_tv_abc_t);

  if (tv->result != tv->expected_result) {
    iter->result = -1;
  } else if (tv->result < 0) {
    iter->result = 1;
  } else {
    iter->result = 0;
  }

  if ((az_tc_abc_flags & AZ_TC_FLAG_TRACE) || (az_tc_flags & AZ_TC_FLAG_TRACE)) {
    az_rprintf(r, "%s" AZ_NL, "...");
  }
  return r;
}

/**
 * @fn		az_tc_term_abc
 * @brief	called at test case terminating phase
 * @param	test case info
 * @return	az_r_t
 * @exception	none
 */
az_r_t az_tc_term_abc(az_test_case_t *pTC)
{
  az_r_t r = AZ_SUCCESS;

  /* free the test vector array values from xml configuration file */

  if (NULL != az_tc_testvector_array_abc) {
    az_free(az_tc_testvector_array_abc);
    az_tc_testvector_array_abc = NULL;
    AZ_TC_SET_TV(pTC, NULL);
  }

  if ((az_tc_abc_flags & AZ_TC_FLAG_TRACE) || (az_tc_flags & AZ_TC_FLAG_TRACE)) {
    az_rprintf(r, "%s" AZ_NL, "...");
  }
  return r;
}

/* === end of AZ_TC_ABC_C === */
