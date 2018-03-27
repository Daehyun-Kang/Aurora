/**
 * @file az_ut_xu_create.c
 * @brief 
 * @date 01/05/16
 * @author fware
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
 *   - 2016-05-01 fware initial create
 */
/* include header files */
#include "aurora.h"
/* TODO: include function dependant header files */

#include "sys/az_xu.h"

#define AZ_UT_FUNC_HAS_RETURN  1
#undef AZ_UT_FUNC_VOID  
#include "az_unitTest.h"

/* declare global variables */

/* this is unit test target function */
extern az_r_t az_xu_create(const char *,az_xu_entry_t,az_xu_arg_t,void *,az_xu_t *);

/* this is prolog for each test vector */
extern AZ_UnitTest_cb_t az_ut_prolog_az_xu_create(void *ctx);

/* this is epilog for each test vector */
extern AZ_UnitTest_cb_t az_ut_epilog_az_xu_create(void *ctx);

/* this is to define test vector structure */
AZ_UT_TESTVECTOR_ST(az_xu_create,az_r_t,char *,az_xu_entry_t,az_xu_arg_t,void *,az_xu_t *);

/* define a number of test vectors */
AZ_UT_TESTVECTOR_ARRAY(testvector_array_az_xu_create, 1, az_xu_create,
{0, az_ut_prolog_az_xu_create, az_ut_epilog_az_xu_create,FILE_NULL, 0, REASON_NULL, AZ_SUCCESS,NULL,NULL,NULL,NULL,NULL},
);

/* this is to define test run context */
AZ_UT_TESTRUNCTX_ST(az_xu_create);

AZ_UT_DECL_TESTVECTOR_CUR(az_xu_create);

/* declare static variables */


/* implement static functions */

/**
 * @fn		az_ut_set_reason_az_xu_create
 * @brief	called from macro AZ_UT_SET_REASON 
 * @param	file, line, reason
 * @return	void
 * @exception	none
 */
void az_ut_set_reason_az_xu_create(char *file, int line, char *reason)
{
	extern AZ_UT_DECL_TESTVECTOR_CUR(az_xu_create);
	 AZ_UT_TESTVECTOR_CUR(az_xu_create) -> _reason = reason;
	 AZ_UT_TESTVECTOR_CUR(az_xu_create) -> _file = file;
	 AZ_UT_TESTVECTOR_CUR(az_xu_create) -> _line = line;
}
static az_xu_t az_xu;
static void *az_xu_entry(az_xu_arg_t arg)
{
  printf("%s:%d....\n", __FUNCTION__, __LINE__);
  printf("%s %p\n", __FUNCTION__, az_xu_self());
}

/**
 * @fn		az_ut_prolog_az_xu_create
 * @brief	run before exec target function for each test vector
 * @param	test run context
 * @return	void
 * @exception	none
 */
AZ_UnitTest_cb_t az_ut_prolog_az_xu_create(void *pInCtx)
{
	AZ_UT_INIT_TESTRUNCTX_PTR(az_xu_create, pCtx, pInCtx); //declare test run context: pCtx
	AZ_UT_INIT_TESTVECTOR_PTR(az_xu_create, pInput, pCtx->_input); //declare test vector: pInput

	/* TODO: allocate any resource and setup the parameters of the test vector */

	AZ_UT_PRINT_START(pCtx, pInput);
  pInput->arg1 = "az_xu";
  pInput->arg2 = az_xu_entry;
  pInput->arg3 = NULL;
  pInput->arg4 = NULL;
  pInput->arg5 = &az_xu;

	 return 0;
}

/**
 * @fn		az_ut_epilog_az_xu_create
 * @brief	run after exec target function for each test vector
 * @param	test run context
 * @return	void
 * @exception	none
 */
AZ_UnitTest_cb_t az_ut_epilog_az_xu_create(void *pInCtx)
{
	AZ_UT_INIT_TESTRUNCTX_PTR(az_xu_create, pCtx, pInCtx); //declare test run context: pCtx
	AZ_UT_INIT_TESTVECTOR_PTR(az_xu_create, pInput, pCtx->_input); //declare test vector: pInput

	int testFailed = 0;

	/* TODO: inspect the result and determine test pass/fail */

	if (pCtx->_result != AZ_UT_RESULT_SEGV)
	{
    if (pInput->arg0 == AZ_SUCCESS) {
      testFailed = 0;
      sleep(1);
      printf("%s %p\n", __FUNCTION__, az_xu);
      az_xu_delete(az_xu);
    } else {
		  testFailed = pInput->arg0; 
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
 * @fn		az_ut_prolog_ctx_az_xu_create
 * @brief	run before test run 
 * @param	test run context
 * @return	void
 * @exception	none
 */
AZ_UnitTest_cb_t az_ut_prolog_ctx_az_xu_create(void *pInCtx)
{
	AZ_UT_INIT_TESTRUNCTX_PTR(az_xu_create, pCtx, pInCtx); //test run context: pCtx
	AZ_UT_INIT_TESTVECTOR_PTR(az_xu_create, pInput, pCtx->_input); //test vector: pInput

	/* TODO: may replace test vector array */

	AZ_UT_PRINT_BEGIN(pCtx);
  az_core_init();

	return 0;
}

/**
 * @fn		az_ut_epilog_ctx_az_xu_create
 * @brief	run after test run 
 * @param	test run context
 * @return	void
 * @exception	none
 */
AZ_UnitTest_cb_t az_ut_epilog_ctx_az_xu_create(void *pInCtx)
{
	AZ_UT_INIT_TESTRUNCTX_PTR(az_xu_create, pCtx, pInCtx); //test run context: pCtx
	AZ_UT_INIT_TESTVECTOR_PTR(az_xu_create, pInput, pCtx->_input); //test vector: pInput

	AZ_UT_PRINT_TERM(pCtx);
  //az_core_deinit();

	/* TODO: free any resource allocated in test run context wide */

	return 0;
}

/**
 * @fn		az_ut_main_az_xu_create
 * @brief	test run main
 * @param	 
 * @return	0
 * @exception	none
 */
AZ_UT_DEF_TEST_MAIN("UT_az_xu_create", az_ut_prolog_ctx_az_xu_create, az_ut_epilog_ctx_az_xu_create, testvector_array_az_xu_create, 
az_xu_create,az_r_t,char *,az_xu_entry_t,az_xu_arg_t,void *,az_xu_t *)

/*
int main(int argc, char *argv[])
{
	return AZ_UT_TEST_MAIN(az_xu_create, argc, argv);
}
*/
/* === end of AZ_UT_XU_CREATE_C === */
