/**
 * @file az_ut_fs_file2mem.c
 * @brief 
 * @date 07/07/16
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
 *   - 2016-07-07 fware initial create
 */
/* include header files */
#include "aurora.h"
/* TODO: include function dependant header files */
#include "az_fs.h"

#define AZ_UT_FUNC_HAS_RETURN  1
#undef AZ_UT_FUNC_VOID  

#include "az_unitTest.h"

/* declare global variables */

/* this is unit test target function */
//extern az_size_t az_fs_file2mem(char *,az_ref_t *);

/* this is prolog for each test vector */
extern AZ_UnitTest_cb_t az_ut_prolog_az_fs_file2mem(void *ctx);

/* this is epilog for each test vector */
extern AZ_UnitTest_cb_t az_ut_epilog_az_fs_file2mem(void *ctx);

/* this is to define test vector structure */
AZ_UT_TESTVECTOR_ST(az_fs_file2mem,az_size_t,char *,az_ref_t *, az_size_t);

/* define a number of test vectors */
AZ_UT_TESTVECTOR_ARRAY(testvector_array_az_fs_file2mem, 2, az_fs_file2mem,
/*{0, az_ut_prolog_az_fs_file2mem, az_ut_epilog_az_fs_file2mem,FILE_NULL, 0, REASON_NULL, [az_size_t],[char *],[az_ref_t *]},*/
{0, az_ut_prolog_az_fs_file2mem, az_ut_epilog_az_fs_file2mem,FILE_NULL, 0, REASON_NULL, 0, "CDF.xml",NULL, 345},
{1, az_ut_prolog_az_fs_file2mem, az_ut_epilog_az_fs_file2mem,FILE_NULL, 0, REASON_NULL, 0, "abc.xml",NULL, -1},
);

/* this is to define test run context */
AZ_UT_TESTRUNCTX_ST(az_fs_file2mem);

AZ_UT_DECL_TESTVECTOR_CUR(az_fs_file2mem);

/* declare static variables */


/* implement static functions */

/**
 * @fn		az_ut_set_reason_az_fs_file2mem
 * @brief	called from macro AZ_UT_SET_REASON 
 * @param	file, line, reason
 * @return	void
 * @exception	none
 */
void az_ut_set_reason_az_fs_file2mem(char *file, int line, char *reason)
{
	extern AZ_UT_DECL_TESTVECTOR_CUR(az_fs_file2mem);
	 AZ_UT_TESTVECTOR_CUR(az_fs_file2mem) -> _reason = reason;
	 AZ_UT_TESTVECTOR_CUR(az_fs_file2mem) -> _file = file;
	 AZ_UT_TESTVECTOR_CUR(az_fs_file2mem) -> _line = line;
}

/**
 * @fn		az_ut_prolog_az_fs_file2mem
 * @brief	run before exec target function for each test vector
 * @param	test run context
 * @return	void
 * @exception	none
 */
az_ref_t mem = NULL;
AZ_UnitTest_cb_t az_ut_prolog_az_fs_file2mem(void *pInCtx)
{
	AZ_UT_INIT_TESTRUNCTX_PTR(az_fs_file2mem, pCtx, pInCtx); //declare test run context: pCtx
	AZ_UT_INIT_TESTVECTOR_PTR(az_fs_file2mem, pInput, pCtx->_input); //declare test vector: pInput

	/* TODO: allocate any resource and setup the parameters of the test vector */
  mem = NULL;
  pInput->arg2 = &mem;

	AZ_UT_PRINT_START(pCtx, pInput);

	 return 0;
}

/**
 * @fn		az_ut_epilog_az_fs_file2mem
 * @brief	run after exec target function for each test vector
 * @param	test run context
 * @return	void
 * @exception	none
 */
AZ_UnitTest_cb_t az_ut_epilog_az_fs_file2mem(void *pInCtx)
{
	AZ_UT_INIT_TESTRUNCTX_PTR(az_fs_file2mem, pCtx, pInCtx); //declare test run context: pCtx
	AZ_UT_INIT_TESTVECTOR_PTR(az_fs_file2mem, pInput, pCtx->_input); //declare test vector: pInput

	int testFailed = 0;
  int success = 0;

	/* TODO: inspect the result and determine test pass/fail */
  az_size_t size = pInput->arg0;
  if (size == pInput->arg3) {
    pCtx->_result = AZ_SUCCESS;
    if (size > 0) success = 1;
  } else {
    if (size < 0) {
      pCtx->_result = AZ_SUCCESS;
    }
  }

  printf("size %d\n", size); 

	if (pCtx->_result != AZ_UT_RESULT_SEGV)
	{
		testFailed = pCtx->_result;
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
		AZ_UT_PRINT_END_PASS(pCtx, pInput, success);
	}

  if (size > 0) {
    az_sys_free(mem);
    mem = NULL;
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
 * @fn		az_ut_prolog_ctx_az_fs_file2mem
 * @brief	run before test run 
 * @param	test run context
 * @return	void
 * @exception	none
 */
AZ_UnitTest_cb_t az_ut_prolog_ctx_az_fs_file2mem(void *pInCtx)
{
	AZ_UT_INIT_TESTRUNCTX_PTR(az_fs_file2mem, pCtx, pInCtx); //test run context: pCtx
	AZ_UT_INIT_TESTVECTOR_PTR(az_fs_file2mem, pInput, pCtx->_input); //test vector: pInput

	/* TODO: may replace test vector array */

	AZ_UT_PRINT_BEGIN(pCtx);

	return 0;
}

/**
 * @fn		az_ut_epilog_ctx_az_fs_file2mem
 * @brief	run after test run 
 * @param	test run context
 * @return	void
 * @exception	none
 */
AZ_UnitTest_cb_t az_ut_epilog_ctx_az_fs_file2mem(void *pInCtx)
{
	AZ_UT_INIT_TESTRUNCTX_PTR(az_fs_file2mem, pCtx, pInCtx); //test run context: pCtx
	AZ_UT_INIT_TESTVECTOR_PTR(az_fs_file2mem, pInput, pCtx->_input); //test vector: pInput

	AZ_UT_PRINT_TERM(pCtx);

	/* TODO: free any resource allocated in test run context wide */

	return 0;
}

/**
 * @fn		az_ut_main_az_fs_file2mem
 * @brief	test run main
 * @param	 
 * @return	0
 * @exception	none
 */
AZ_UT_DEF_TEST_MAIN("UT_az_fs_file2mem", az_ut_prolog_ctx_az_fs_file2mem, az_ut_epilog_ctx_az_fs_file2mem, testvector_array_az_fs_file2mem, 
az_fs_file2mem,az_size_t,char *,az_ref_t *)

/*
int main(int argc, char *argv[])
{
	return AZ_UT_TEST_MAIN(az_fs_file2mem, argc, argv);
}
*/
/* === end of AZ_UT_FS_FILE2MEM_C === */
