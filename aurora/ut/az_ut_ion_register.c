/**
 * @file az_ut_ion_register.c
 * @brief 
 * @date 09/04/16
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
 *   - 2016-04-09 mercury initial create
 */
/* include header files */
#include "aurora.h"
/* TODO: include function dependant header files */

#define AZ_UT_FUNC_HAS_RETURN  1
#undef AZ_UT_FUNC_VOID  
#include "az_unitTest.h"
#include "az_ion_types.h"
#include "az_ion.h"
/* declare global variables */

/* this is unit test target function */
extern az_r_t az_ion_register(az_ion_t *, az_ion_type_t type);

/* this is prolog for each test vector */
extern AZ_UnitTest_cb_t az_ut_prolog_az_ion_register(void *ctx);

/* this is epilog for each test vector */
extern AZ_UnitTest_cb_t az_ut_epilog_az_ion_register(void *ctx);

/* this is to define test vector structure */
AZ_UT_TESTVECTOR_ST(az_ion_register,az_r_t,az_ion_t *, az_ion_type_t);

/* define a number of test vectors */
AZ_UT_TESTVECTOR_ARRAY(testvector_array_az_ion_register, 1, az_ion_register,
{0, az_ut_prolog_az_ion_register, az_ut_epilog_az_ion_register,FILE_NULL, 0, REASON_NULL, 0, NULL, 0},
/*{0, az_ut_prolog_az_ion_register, az_ut_epilog_az_ion_register,FILE_NULL, 0, REASON_NULL, [az_r_t],[az_ion_t *]},*/
);

/* this is to define test run context */
AZ_UT_TESTRUNCTX_ST(az_ion_register);

AZ_UT_DECL_TESTVECTOR_CUR(az_ion_register);

/* declare static variables */


/* implement static functions */

/**
 * @fn		az_ut_set_reason_az_ion_register
 * @brief	called from macro AZ_UT_SET_REASON 
 * @param	file, line, reason
 * @return	void
 * @exception	none
 */
void az_ut_set_reason_az_ion_register(char *file, int line, char *reason)
{
	extern AZ_UT_DECL_TESTVECTOR_CUR(az_ion_register);
	 AZ_UT_TESTVECTOR_CUR(az_ion_register) -> _reason = reason;
	 AZ_UT_TESTVECTOR_CUR(az_ion_register) -> _file = file;
	 AZ_UT_TESTVECTOR_CUR(az_ion_register) -> _line = line;
}

static az_ion_t az_ion;
static az_array_t az_array = {
  .size = 10,
  .list = NULL,
  .id_list = NULL
};
/**
 * @fn		az_ut_prolog_az_ion_register
 * @brief	run before exec target function for each test vector
 * @param	test run context
 * @return	void
 * @exception	none
 */
AZ_UnitTest_cb_t az_ut_prolog_az_ion_register(void *pInCtx)
{
	AZ_UT_INIT_TESTRUNCTX_PTR(az_ion_register, pCtx, pInCtx); //declare test run context: pCtx
	AZ_UT_INIT_TESTVECTOR_PTR(az_ion_register, pInput, pCtx->_input); //declare test vector: pInput

	/* TODO: allocate any resource and setup the parameters of the test vector */
  az_ion_init(&az_array);
  pInput->arg1 = &az_ion;

	AZ_UT_PRINT_START(pCtx, pInput);

	 return 0;
}

/**
 * @fn		az_ut_epilog_az_ion_register
 * @brief	run after exec target function for each test vector
 * @param	test run context
 * @return	void
 * @exception	none
 */
AZ_UnitTest_cb_t az_ut_epilog_az_ion_register(void *pInCtx)
{
	AZ_UT_INIT_TESTRUNCTX_PTR(az_ion_register, pCtx, pInCtx); //declare test run context: pCtx
	AZ_UT_INIT_TESTVECTOR_PTR(az_ion_register, pInput, pCtx->_input); //declare test vector: pInput

	int testFailed = 0;

	/* TODO: inspect the result and determine test pass/fail */

	if (pCtx->_result != AZ_UT_RESULT_SEGV)
	{
		testFailed = ((pInput->arg0 == AZ_SUCCESS)? 0:1);
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
    az_printf("id=%d\n", pInput->arg0);
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
 * @fn		az_ut_prolog_ctx_az_ion_register
 * @brief	run before test run 
 * @param	test run context
 * @return	void
 * @exception	none
 */
AZ_UnitTest_cb_t az_ut_prolog_ctx_az_ion_register(void *pInCtx)
{
	AZ_UT_INIT_TESTRUNCTX_PTR(az_ion_register, pCtx, pInCtx); //test run context: pCtx
	AZ_UT_INIT_TESTVECTOR_PTR(az_ion_register, pInput, pCtx->_input); //test vector: pInput

	/* TODO: may replace test vector array */

	AZ_UT_PRINT_BEGIN(pCtx);

	return 0;
}

/**
 * @fn		az_ut_epilog_ctx_az_ion_register
 * @brief	run after test run 
 * @param	test run context
 * @return	void
 * @exception	none
 */
AZ_UnitTest_cb_t az_ut_epilog_ctx_az_ion_register(void *pInCtx)
{
	AZ_UT_INIT_TESTRUNCTX_PTR(az_ion_register, pCtx, pInCtx); //test run context: pCtx
	AZ_UT_INIT_TESTVECTOR_PTR(az_ion_register, pInput, pCtx->_input); //test vector: pInput

	AZ_UT_PRINT_TERM(pCtx);

	/* TODO: free any resource allocated in test run context wide */

	return 0;
}

/**
 * @fn		az_ut_main_az_ion_register
 * @brief	test run main
 * @param	 
 * @return	0
 * @exception	none
 */
AZ_UT_DEF_TEST_MAIN("UT_az_ion_register", az_ut_prolog_ctx_az_ion_register, az_ut_epilog_ctx_az_ion_register, testvector_array_az_ion_register, 
az_ion_register,az_r_t,az_ion_t *, az_ion_type_t)

/*
int main(int argc, char *argv[])
{
	return AZ_UT_TEST_MAIN(az_ion_register, argc, argv);
}
*/
/* === end of AZ_UT_ION_REGISTER_C === */
