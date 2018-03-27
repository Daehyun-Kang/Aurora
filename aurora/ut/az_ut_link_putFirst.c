/**
 * @file az_ut_link_putFirst.c
 * @brief 
 * @date 30/04/16
 * @author mercury
 *
 * @copyright
 * Copyright (c) Fware, 2013-2016 - All Rights Reserved
 * You may use, distribute and modify this code under the 
 * terms of the "Aurora Source Code License".
 * See the file LICENSE for full license details.\n\n
 *
 * You should have received a copy of  the LICENSE with this file.\n\n
 *
 * If not, please contact to Fware with the information in the file CONTACTS.
 *
 * History:
 *   - 2016-04-30 mercury initial create
 */
/* include header files */
#include "aurora.h"
/* TODO: include function dependant header files */

#define AZ_UT_FUNC_VOID  1
#undef AZ_UT_FUNC_HAS_RETURN  
#include "az_unitTest.h"

#include "az_link.h"
/* declare global variables */

/* this is unit test target function */
extern void az_link_putFirst(az_link_list_t *,az_link_t *);

/* this is prolog for each test vector */
extern AZ_UnitTest_cb_t az_ut_prolog_az_link_putFirst(void *ctx);

/* this is epilog for each test vector */
extern AZ_UnitTest_cb_t az_ut_epilog_az_link_putFirst(void *ctx);

/* this is to define test vector structure */
AZ_UT_TESTVECTOR_ST(az_link_putFirst,az_link_list_t *,az_link_t *);

/* define a number of test vectors */
AZ_UT_TESTVECTOR_ARRAY(testvector_array_az_link_putFirst, 2, az_link_putFirst,
{0, az_ut_prolog_az_link_putFirst, az_ut_epilog_az_link_putFirst,FILE_NULL, 0, REASON_NULL, NULL, NULL},
{1, az_ut_prolog_az_link_putFirst, az_ut_epilog_az_link_putFirst,FILE_NULL, 0, REASON_NULL, NULL, NULL},
);

/* this is to define test run context */
AZ_UT_TESTRUNCTX_ST(az_link_putFirst);

AZ_UT_DECL_TESTVECTOR_CUR(az_link_putFirst);

/* declare static variables */


/* implement static functions */

/**
 * @fn		az_ut_set_reason_az_link_putFirst
 * @brief	called from macro AZ_UT_SET_REASON 
 * @param	file, line, reason
 * @return	void
 * @exception	none
 */
void az_ut_set_reason_az_link_putFirst(char *file, int line, char *reason)
{
	extern AZ_UT_DECL_TESTVECTOR_CUR(az_link_putFirst);
	 AZ_UT_TESTVECTOR_CUR(az_link_putFirst) -> _reason = reason;
	 AZ_UT_TESTVECTOR_CUR(az_link_putFirst) -> _file = file;
	 AZ_UT_TESTVECTOR_CUR(az_link_putFirst) -> _line = line;
}

/**
 * @fn		az_ut_prolog_az_link_putFirst
 * @brief	run before exec target function for each test vector
 * @param	test run context
 * @return	void
 * @exception	none
 */
static az_link_list_t az_list;
static az_link_t az_link, az_link1;
static print_buf[512];
AZ_UnitTest_cb_t az_ut_prolog_az_link_putFirst(void *pInCtx)
{
	AZ_UT_INIT_TESTRUNCTX_PTR(az_link_putFirst, pCtx, pInCtx); //declare test run context: pCtx
	AZ_UT_INIT_TESTVECTOR_PTR(az_link_putFirst, pInput, pCtx->_input); //declare test vector: pInput

	/* TODO: allocate any resource and setup the parameters of the test vector */

	AZ_UT_PRINT_START(pCtx, pInput);

  az_link_list_toStr(&az_list, NULL, print_buf, sizeof(print_buf));
  printf("\tprep list(%p): %s\n", &az_list, print_buf);

  switch (pInput->_index) {
    case 0:
      pInput->arg0 = &az_list;
      pInput->arg1 = &az_link;
      break;
    case 1:
      pInput->arg0 = &az_list;
      pInput->arg1 = &az_link1;
      break;
  }

	 return 0;
}

/**
 * @fn		az_ut_epilog_az_link_putFirst
 * @brief	run after exec target function for each test vector
 * @param	test run context
 * @return	void
 * @exception	none
 */
AZ_UnitTest_cb_t az_ut_epilog_az_link_putFirst(void *pInCtx)
{
	AZ_UT_INIT_TESTRUNCTX_PTR(az_link_putFirst, pCtx, pInCtx); //declare test run context: pCtx
	AZ_UT_INIT_TESTVECTOR_PTR(az_link_putFirst, pInput, pCtx->_input); //declare test vector: pInput

	int testFailed = 0;
  az_link_list_t *list = pInput->arg0;
  az_link_t *link = pInput->arg1;

	/* TODO: inspect the result and determine test pass/fail */

	if (pCtx->_result != AZ_UT_RESULT_SEGV)
	{
    if (list->next != link) {
      testFailed = 1;
    }
	}
	else
	{
		testFailed = AZ_UT_RESULT_SEGV;
	}
	if (testFailed)
	{
		AZ_UT_PRINT_END_FAIL(pCtx, pInput, testFailed);
	}
	else
	{
		AZ_UT_PRINT_END_PASS(pCtx, pInput, testFailed);
	}

  az_link_toStr(link, NULL, print_buf, sizeof(print_buf));
  printf("\tlink info(%p): %s\n", link, print_buf);

  az_link_list_toStr(list, NULL, print_buf, sizeof(print_buf));
  printf("\tpost list(%p): %s\n", list, print_buf);

	/* TODO: release any resource allocated */
	return testFailed;
}

/**
 * @fn function name
 * @brief function-description
 * @param input-output-parameters
 * @return return-value
 * @exception none
 */


/* implement global functions */

/**
 * @fn		az_ut_prolog_ctx_az_link_putFirst
 * @brief	run before test run 
 * @param	test run context
 * @return	void
 * @exception	none
 */
AZ_UnitTest_cb_t az_ut_prolog_ctx_az_link_putFirst(void *pInCtx)
{
	AZ_UT_INIT_TESTRUNCTX_PTR(az_link_putFirst, pCtx, pInCtx); //test run context: pCtx
	AZ_UT_INIT_TESTVECTOR_PTR(az_link_putFirst, pInput, pCtx->_input); //test vector: pInput

	/* TODO: may replace test vector array */

	AZ_UT_PRINT_BEGIN(pCtx);

  az_link_initList(&az_list);

	return 0;
}

/**
 * @fn		az_ut_epilog_ctx_az_link_putFirst
 * @brief	run after test run 
 * @param	test run context
 * @return	void
 * @exception	none
 */
AZ_UnitTest_cb_t az_ut_epilog_ctx_az_link_putFirst(void *pInCtx)
{
	AZ_UT_INIT_TESTRUNCTX_PTR(az_link_putFirst, pCtx, pInCtx); //test run context: pCtx
	AZ_UT_INIT_TESTVECTOR_PTR(az_link_putFirst, pInput, pCtx->_input); //test vector: pInput

	AZ_UT_PRINT_TERM(pCtx);

	/* TODO: free any resource allocated in test run context wide */


	return 0;
}

/**
 * @fn		az_ut_main_az_link_putFirst
 * @brief	test run main
 * @param	 
 * @return	0
 * @exception	none
 */
AZ_UT_DEF_TEST_MAIN("UT_az_link_putFirst", az_ut_prolog_ctx_az_link_putFirst, az_ut_epilog_ctx_az_link_putFirst, testvector_array_az_link_putFirst, 
az_link_putFirst,az_link_list_t *,az_link_t *)

/*
int main(int argc, char *argv[])
{
	return AZ_UT_TEST_MAIN(az_link_putFirst, argc, argv);
}
*/
/* === end of AZ_UT_LINK_PUTFIRST_C === */
