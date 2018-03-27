/**
 * @file az_ut_buffer_reserve.c
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

#define AZ_UT_FUNC_HAS_RETURN  1
#undef AZ_UT_FUNC_VOID  

#include "az_unitTest.h"

/* declare global variables */

/* this is unit test target function */
extern az_pos_t az_buffer_reserve(az_buffer_t *,az_size_t);

/* this is prolog for each test vector */
extern AZ_UnitTest_cb_t az_ut_prolog_az_buffer_reserve(void *ctx);

/* this is epilog for each test vector */
extern AZ_UnitTest_cb_t az_ut_epilog_az_buffer_reserve(void *ctx);

/* this is to define test vector structure */
AZ_UT_TESTVECTOR_ST(az_buffer_reserve,az_pos_t,az_buffer_t *,az_size_t);

/* define a number of test vectors */
AZ_UT_TESTVECTOR_ARRAY(testvector_array_az_buffer_reserve, 3, az_buffer_reserve,
{0, az_ut_prolog_az_buffer_reserve, az_ut_epilog_az_buffer_reserve,FILE_NULL, 0, REASON_NULL, 0, NULL, 0},
{1, az_ut_prolog_az_buffer_reserve, az_ut_epilog_az_buffer_reserve,FILE_NULL, 0, REASON_NULL, 0, NULL, 0},
{2, az_ut_prolog_az_buffer_reserve, az_ut_epilog_az_buffer_reserve,FILE_NULL, 0, REASON_NULL, 0, NULL, 0},
);

/* this is to define test run context */
AZ_UT_TESTRUNCTX_ST(az_buffer_reserve);

AZ_UT_DECL_TESTVECTOR_CUR(az_buffer_reserve);

/* declare static variables */


/* implement static functions */

/**
 * @fn		az_ut_set_reason_az_buffer_reserve
 * @brief	called from macro AZ_UT_SET_REASON 
 * @param	file, line, reason
 * @return	void
 * @exception	none
 */
void az_ut_set_reason_az_buffer_reserve(char *file, int line, char *reason)
{
	extern AZ_UT_DECL_TESTVECTOR_CUR(az_buffer_reserve);
	 AZ_UT_TESTVECTOR_CUR(az_buffer_reserve) -> _reason = reason;
	 AZ_UT_TESTVECTOR_CUR(az_buffer_reserve) -> _file = file;
	 AZ_UT_TESTVECTOR_CUR(az_buffer_reserve) -> _line = line;
}
#define AZ_BUFFER_SIZE 100 
static az_buffer_t az_buffer;
static char print_buf[512];

/**
 * @fn		az_ut_prolog_az_buffer_reserve
 * @brief	run before exec target function for each test vector
 * @param	test run context
 * @return	void
 * @exception	none
 */
AZ_UnitTest_cb_t az_ut_prolog_az_buffer_reserve(void *pInCtx)
{
	AZ_UT_INIT_TESTRUNCTX_PTR(az_buffer_reserve, pCtx, pInCtx); //declare test run context: pCtx
	AZ_UT_INIT_TESTVECTOR_PTR(az_buffer_reserve, pInput, pCtx->_input); //declare test vector: pInput

	/* TODO: allocate any resource and setup the parameters of the test vector */

	AZ_UT_PRINT_START(pCtx, pInput);
  az_buffer_toStr(&az_buffer, NULL, print_buf, sizeof(print_buf));
  printf("\tprep fifo(%p): %s\n", &az_buffer, print_buf);

  switch (pInput->_index) {
    case 0:
      pInput->arg1 = &az_buffer;
      pInput->arg2 = AZ_BUFFER_SIZE/2;  
      break;
    case 1:
      pInput->arg1 = &az_buffer;
      pInput->arg2 = AZ_BUFFER_SIZE/3;  
      break;
    case 2:
      pInput->arg1 = &az_buffer;
      pInput->arg2 = AZ_BUFFER_SIZE/2;  
      break;
  }
	 return 0;
}

/**
 * @fn		az_ut_epilog_az_buffer_reserve
 * @brief	run after exec target function for each test vector
 * @param	test run context
 * @return	void
 * @exception	none
 */
AZ_UnitTest_cb_t az_ut_epilog_az_buffer_reserve(void *pInCtx)
{
	AZ_UT_INIT_TESTRUNCTX_PTR(az_buffer_reserve, pCtx, pInCtx); //declare test run context: pCtx
	AZ_UT_INIT_TESTVECTOR_PTR(az_buffer_reserve, pInput, pCtx->_input); //declare test vector: pInput

	int testFailed = 0;
  az_buffer_t *buffer = &az_buffer;

	/* TODO: inspect the result and determine test pass/fail */

	if (pCtx->_result != AZ_UT_RESULT_SEGV)
	{
      if (pInput->arg0 >= 0 && pInput->arg0 < buffer->size) { 
        testFailed = 0;
      } else {
        testFailed = 2; 
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

  az_buffer_toStr(buffer, NULL, print_buf, sizeof(print_buf));
  printf("\tpost fifo(%p): %s\n", buffer, print_buf);

	/* TODO: release any resource allocated */

  az_buffer_advance(&az_buffer, pInput->arg0, pInput->arg2);

  az_buffer_toStr(buffer, NULL, print_buf, sizeof(print_buf));
  printf("\tpost fifo(%p): %s\n", buffer, print_buf);
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
 * @fn		az_ut_prolog_ctx_az_buffer_reserve
 * @brief	run before test run 
 * @param	test run context
 * @return	void
 * @exception	none
 */
AZ_UnitTest_cb_t az_ut_prolog_ctx_az_buffer_reserve(void *pInCtx)
{
	AZ_UT_INIT_TESTRUNCTX_PTR(az_buffer_reserve, pCtx, pInCtx); //test run context: pCtx
	AZ_UT_INIT_TESTVECTOR_PTR(az_buffer_reserve, pInput, pCtx->_input); //test vector: pInput

	/* TODO: may replace test vector array */

	AZ_UT_PRINT_BEGIN(pCtx);

  az_buffer_t *buffer = &az_buffer;

  az_buffer_init(&az_buffer, AZ_BUFFER_SIZE);
  az_buffer_toStr(buffer, NULL, print_buf, sizeof(print_buf));
  printf("\tprep fifo(%p): %s\n", buffer, print_buf);

  az_buffer_reserve(&az_buffer, AZ_BUFFER_SIZE/2);
  az_buffer_toStr(buffer, NULL, print_buf, sizeof(print_buf));
  printf("\trsv fifo(%p): %s\n", buffer, print_buf);

  az_buffer_advance(&az_buffer, az_buffer.outp, AZ_BUFFER_SIZE/2);
  az_buffer_toStr(buffer, NULL, print_buf, sizeof(print_buf));
  printf("\tadv fifo(%p): %s\n", buffer, print_buf);

	return 0;
}

/**
 * @fn		az_ut_epilog_ctx_az_buffer_reserve
 * @brief	run after test run 
 * @param	test run context
 * @return	void
 * @exception	none
 */
AZ_UnitTest_cb_t az_ut_epilog_ctx_az_buffer_reserve(void *pInCtx)
{
	AZ_UT_INIT_TESTRUNCTX_PTR(az_buffer_reserve, pCtx, pInCtx); //test run context: pCtx
	AZ_UT_INIT_TESTVECTOR_PTR(az_buffer_reserve, pInput, pCtx->_input); //test vector: pInput

	AZ_UT_PRINT_TERM(pCtx);

	/* TODO: free any resource allocated in test run context wide */

	return 0;
}

/**
 * @fn		az_ut_main_az_buffer_reserve
 * @brief	test run main
 * @param	 
 * @return	0
 * @exception	none
 */
AZ_UT_DEF_TEST_MAIN("UT_az_buffer_reserve", az_ut_prolog_ctx_az_buffer_reserve, az_ut_epilog_ctx_az_buffer_reserve, testvector_array_az_buffer_reserve, 
az_buffer_reserve,az_pos_t,az_buffer_t *,az_size_t)

/*
int main(int argc, char *argv[])
{
	return AZ_UT_TEST_MAIN(az_buffer_reserve, argc, argv);
}
*/
/* === end of AZ_UT_BUFFER_RESERVE_C === */
