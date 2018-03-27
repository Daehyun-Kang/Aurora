/**
 * @file   abc.c
 * @brief  
 * @date   26/03/18
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
 *   - 2018-03-26 dhkang initial create
 */

/* include header files */
#include "aurora.h"
#include "az_test.h"
/* TODO: include function dependant header files */

#define AZ_UT_FUNC_VOID  1
#undef AZ_UT_FUNC_HAS_RETURN  
#include "az_unitTest.h"

/* declare global variables */

int az_ut_abc_flags = AZ_UT_FLAG_TRACE;

/* this is unit test target function */
extern int abc(int a, int b);

/* this is to define test vector structure */
struct az_ut_testvector_st_abc {
  int a;
  int b;
  int expected_result;
  int result;
};

/* define the reference for test vectors */
struct az_ut_testvector_st_abc *az_ut_testvector_array_abc = NULL;

/* declare global callback functions */

/* this is init handler for each test case */
extern az_r_t az_ut_init_abc(az_test_case_t *pTC);

/* this is sync handler for each test case */
extern az_r_t az_ut_sync_abc(az_test_case_t *pTC);

/* this is prolog handler for each test iteration */
extern az_r_t az_ut_prolog_abc(az_test_case_t *pTC);

/* this is run handler for each test iteration */
extern az_r_t az_ut_run_abc(az_test_case_t *pTC);

/* this is epilog handler for each test iteration */
extern az_r_t az_ut_epilog_abc(az_test_case_t *pTC);

/* this is term handler for each test case */
extern az_r_t az_ut_term_abc(az_test_case_t *pTC);



/* implement global callback functions */

/**
 * @fn function name
 * @brief function-description
 * @param input-output-parameters
 * @return return-value
 * @exception none
 */


/**
 * @fn		az_ut_init_abc
 * @brief	called for init test case
 * @param	test case info
 * @return	az_r_t
 * @exception	none
 */
az_r_t az_ut_init_abc(az_test_case_t *pTC)
{
  az_r_t r = AZ_SUCCESS;

  if (pTC->test_iter_count > 0) {
    az_ut_testvector_array_abc = (struct az_ut_testvector_st_abc *)az_malloc(sizeof(struct az_ut_testvector_st_abc) * pTC->test_iter_count);
    if (NULL == az_ut_testvector_array_abc) {
      r = AZ_ERR(MALLOC);
    }
    /* here you may want make up the test vector array values from xml configuration file */
  int j;

    struct az_ut_testvector_st_abc *tv =  az_ut_testvector_array_abc;
    az_xml_element_t *xe;
    az_test_iter_t *iter = pTC->test_iter.list; 
    for (j = 0; j < pTC->test_iter.count; j++, iter++, tv++) {
      iter-> test_vector = tv;

      xe = az_xml_element_find_attr(iter->xml, "a");
      if (xe) tv->a = strtol(xe->kv.value, NULL, 10);

      xe = az_xml_element_find_attr(iter->xml, "b");
      if (xe) tv->b = strtol(xe->kv.value, NULL, 10);

      xe = az_xml_element_find_attr(iter->xml, "expected_result");
      if (xe) tv->expected_result = strtol(xe->kv.value, NULL, 10);
      az_printf("a=%d b=%d r=%d\n", tv->a, tv->b, tv->expected_result);
    }
  } else {
      az_printf("test case: %s test iteration = %d\n", pTC->name, pTC->test_iter_count); 
  }
  pTC->test_vector = (void *)az_ut_testvector_array_abc;

  if ((az_ut_abc_flags & AZ_UT_FLAG_TRACE) || (az_ut_flags & AZ_UT_FLAG_TRACE)) {
    az_rprintf(r, "%s" AZ_NL, "...");
  }

  return r;
}

/**
 * @fn		az_ut_sync_abc
 * @brief	called for prepare test case
 * @param	test case info
 * @return	az_r_t
 * @exception	none
 */
az_r_t az_ut_sync_abc(az_test_case_t *pTC)
{
  az_r_t r = AZ_SUCCESS;

  if ((az_ut_abc_flags & AZ_UT_FLAG_TRACE) || (az_ut_flags & AZ_UT_FLAG_TRACE)) {
    az_rprintf(r, "%s" AZ_NL, "...");
  }
  return r;
}

/**
 * @fn		az_ut_prolog_abc
 * @brief	called before each test iteration
 * @param	test case info
 * @return	az_r_t
 * @exception	none
 */
az_r_t az_ut_prolog_abc(az_test_case_t *pTC)
{
  az_r_t r = AZ_SUCCESS;

  if ((az_ut_abc_flags & AZ_UT_FLAG_TRACE) || (az_ut_flags & AZ_UT_FLAG_TRACE)) {
    az_rprintf(r, "%s" AZ_NL, "...");
  }
  return r;
}

/**
 * @fn		az_ut_run_abc
 * @brief	called for each test iteration
 * @param	test case info
 * @return	az_r_t
 * @exception	none
 */
az_r_t az_ut_run_abc(az_test_case_t *pTC)
{
  az_r_t r = AZ_SUCCESS;
  az_test_iter_t *iter = pTC->test_iter.list + pTC->test_iter.index; 
  struct az_ut_testvector_st_abc *tv = (struct az_ut_testvector_st_abc *)iter->test_vector; 
  
  az_printf("iter%d: a=%d b=%d r=%d\n", iter->index, tv->a, tv->b, tv->expected_result);

  if (tv->a >= 0 && tv->b >= 0) {
    tv->result = tv->a + tv->b;
  } else {
    tv->result = tv->expected_result; 
  }

  if ((az_ut_abc_flags & AZ_UT_FLAG_TRACE) || (az_ut_flags & AZ_UT_FLAG_TRACE)) {
    az_rprintf(r, "%s" AZ_NL, "...");
  }
  return r;
}

/**
 * @fn		az_ut_epilog_abc
 * @brief	called after each test iteration
 * @param	test case info
 * @return	az_r_t
 * @exception	none
 */
az_r_t az_ut_epilog_abc(az_test_case_t *pTC)
{
  az_r_t r = AZ_SUCCESS;
  az_test_iter_t *iter = pTC->test_iter.list + pTC->test_iter.index; 
  struct az_ut_testvector_st_abc *tv = (struct az_ut_testvector_st_abc *)iter->test_vector; 

  if (tv->result != tv->expected_result) {
    iter->result = -1;
  } else if (tv->result < 0) {
    iter->result = 1;
  } else {
    iter->result = 0;
  }
  if ((az_ut_abc_flags & AZ_UT_FLAG_TRACE) || (az_ut_flags & AZ_UT_FLAG_TRACE)) {
    az_rprintf(r, "%s" AZ_NL, "...");
  }
  return r;
}

/**
 * @fn		az_ut_term_abc
 * @brief	called at test case terminating phase
 * @param	test case info
 * @return	az_r_t
 * @exception	none
 */
az_r_t az_ut_term_abc(az_test_case_t *pTC)
{
  az_r_t r = AZ_SUCCESS;

  /* free the test vector array values from xml configuration file */

  if (NULL != az_ut_testvector_array_abc) {
    az_free(az_ut_testvector_array_abc);
    az_ut_testvector_array_abc = NULL;
    pTC->test_vector = NULL;
  }

  if ((az_ut_abc_flags & AZ_UT_FLAG_TRACE) || (az_ut_flags & AZ_UT_FLAG_TRACE)) {
    az_rprintf(r, "%s" AZ_NL, "...");
  }
  return r;
}

/* === end of ABC_C === */
