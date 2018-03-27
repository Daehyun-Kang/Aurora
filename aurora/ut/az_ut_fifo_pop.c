/**
 * @file az_fifo_pop.c
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
extern az_pos_t az_fifo_pop(az_fifo_t *);

/* this is prolog for each test vector */
extern AZ_UnitTest_cb_t az_ut_prolog_az_fifo_pop(void *ctx);

/* this is epilog for each test vector */
extern AZ_UnitTest_cb_t az_ut_epilog_az_fifo_pop(void *ctx);

/* this is to define test vector structure */
AZ_UT_TESTVECTOR_ST(az_fifo_pop,az_pos_t,az_fifo_t *);

/* define a number of test vectors */
AZ_UT_TESTVECTOR_ARRAY(testvector_array_az_fifo_pop, 3, az_fifo_pop,
{0, az_ut_prolog_az_fifo_pop, az_ut_epilog_az_fifo_pop,FILE_NULL, 0, REASON_NULL, 0, NULL},
{1, az_ut_prolog_az_fifo_pop, az_ut_epilog_az_fifo_pop,FILE_NULL, 0, REASON_NULL, 1, NULL},
{2, az_ut_prolog_az_fifo_pop, az_ut_epilog_az_fifo_pop,FILE_NULL, 0, REASON_NULL, 2, NULL},
);

/* this is to define test run context */
AZ_UT_TESTRUNCTX_ST(az_fifo_pop);

AZ_UT_DECL_TESTVECTOR_CUR(az_fifo_pop);

/* declare static variables */


/* implement static functions */

/**
 * @fn		az_ut_set_reason_az_fifo_pop
 * @brief	called from macro AZ_UT_SET_REASON 
 * @param	file, line, reason
 * @return	void
 * @exception	none
 */
void az_ut_set_reason_az_fifo_pop(char *file, int line, char *reason)
{
	extern AZ_UT_DECL_TESTVECTOR_CUR(az_fifo_pop);
	 AZ_UT_TESTVECTOR_CUR(az_fifo_pop) -> _reason = reason;
	 AZ_UT_TESTVECTOR_CUR(az_fifo_pop) -> _file = file;
	 AZ_UT_TESTVECTOR_CUR(az_fifo_pop) -> _line = line;
}
#define AZ_FIFO_SIZE  3
static az_fifo_t az_fifo;
static char print_buf[512];

/**
 * @fn		az_ut_prolog_az_fifo_pop
 * @brief	run before exec target function for each test vector
 * @param	test run context
 * @return	void
 * @exception	none
 */
AZ_UnitTest_cb_t az_ut_prolog_az_fifo_pop(void *pInCtx)
{
	AZ_UT_INIT_TESTRUNCTX_PTR(az_fifo_pop, pCtx, pInCtx); //declare test run context: pCtx
	AZ_UT_INIT_TESTVECTOR_PTR(az_fifo_pop, pInput, pCtx->_input); //declare test vector: pInput

	/* TODO: allocate any resource and setup the parameters of the test vector */

	AZ_UT_PRINT_START(pCtx, pInput);
  az_fifo_toStr(&az_fifo, NULL, print_buf, sizeof(print_buf));
  printf("\tprep fifo(%p): %s\n", &az_fifo, print_buf);

  pInput->arg1 = &az_fifo;

	 return 0;
}

/**
 * @fn		az_ut_epilog_az_fifo_pop
 * @brief	run after exec target function for each test vector
 * @param	test run context
 * @return	void
 * @exception	none
 */
AZ_UnitTest_cb_t az_ut_epilog_az_fifo_pop(void *pInCtx)
{
	AZ_UT_INIT_TESTRUNCTX_PTR(az_fifo_pop, pCtx, pInCtx); //declare test run context: pCtx
	AZ_UT_INIT_TESTVECTOR_PTR(az_fifo_pop, pInput, pCtx->_input); //declare test vector: pInput

	int testFailed = 0;
  az_fifo_t *fifo = pInput->arg1;

	/* TODO: inspect the result and determine test pass/fail */

	if (pCtx->_result != AZ_UT_RESULT_SEGV)
	{
    if (pInput->_index < fifo->size) {
      if (pInput->arg0 == AZ_FIFO_EMPTY) { 
        testFailed = 1;
      }
    } else {
      if (pInput->arg0 != AZ_FIFO_EMPTY) { 
        testFailed = 2;
      }
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

  az_fifo_toStr(fifo, NULL, print_buf, sizeof(print_buf));
  printf("\tpost fifo(%p): %s\n", fifo, print_buf);
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
 * @fn		az_ut_prolog_ctx_az_fifo_pop
 * @brief	run before test run 
 * @param	test run context
 * @return	void
 * @exception	none
 */
AZ_UnitTest_cb_t az_ut_prolog_ctx_az_fifo_pop(void *pInCtx)
{
	AZ_UT_INIT_TESTRUNCTX_PTR(az_fifo_pop, pCtx, pInCtx); //test run context: pCtx
	AZ_UT_INIT_TESTVECTOR_PTR(az_fifo_pop, pInput, pCtx->_input); //test vector: pInput

	/* TODO: may replace test vector array */

	AZ_UT_PRINT_BEGIN(pCtx);
  az_fifo_init(&az_fifo, 2);
  az_fifo_push(&az_fifo);
  az_fifo_push(&az_fifo);

	return 0;
}

/**
 * @fn		az_ut_epilog_ctx_az_fifo_pop
 * @brief	run after test run 
 * @param	test run context
 * @return	void
 * @exception	none
 */
AZ_UnitTest_cb_t az_ut_epilog_ctx_az_fifo_pop(void *pInCtx)
{
	AZ_UT_INIT_TESTRUNCTX_PTR(az_fifo_pop, pCtx, pInCtx); //test run context: pCtx
	AZ_UT_INIT_TESTVECTOR_PTR(az_fifo_pop, pInput, pCtx->_input); //test vector: pInput

	AZ_UT_PRINT_TERM(pCtx);

	/* TODO: free any resource allocated in test run context wide */

	return 0;
}

/**
 * @fn		az_ut_main_az_fifo_pop
 * @brief	test run main
 * @param	 
 * @return	0
 * @exception	none
 */
AZ_UT_DEF_TEST_MAIN("UT_az_fifo_pop", az_ut_prolog_ctx_az_fifo_pop, az_ut_epilog_ctx_az_fifo_pop, testvector_array_az_fifo_pop, 
az_fifo_pop,az_pos_t,az_fifo_t *)

/*
int main(int argc, char *argv[])
{
	return AZ_UT_TEST_MAIN(az_fifo_pop, argc, argv);
}
*/
/* === end of AZ_UT_FIFO_POP_C === */
