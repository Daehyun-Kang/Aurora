/**
 * @file az_ut_xml_parser_run
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
#include "az_xml.h"

#define AZ_UT_FUNC_VOID  1
#undef AZ_UT_FUNC_HAS_RETURN  
#include "az_unitTest.h"

/* declare global variables */

/* this is unit test target function */
extern void az_xml_parser_run(az_xml_parser_t *);

/* this is prolog for each test vector */
extern AZ_UnitTest_cb_t az_ut_prolog_az_xml_parser_run(void *ctx);

/* this is epilog for each test vector */
extern AZ_UnitTest_cb_t az_ut_epilog_az_xml_parser_run(void *ctx);

/* this is to define test vector structure */
AZ_UT_TESTVECTOR_ST(az_xml_parser_run,az_xml_parser_t *);

/* define a number of test vectors */
AZ_UT_TESTVECTOR_ARRAY(testvector_array_az_xml_parser_run, 1, az_xml_parser_run,
{0, az_ut_prolog_az_xml_parser_run, az_ut_epilog_az_xml_parser_run,FILE_NULL, 0, REASON_NULL, NULL},
);

/* this is to define test run context */
AZ_UT_TESTRUNCTX_ST(az_xml_parser_run);

AZ_UT_DECL_TESTVECTOR_CUR(az_xml_parser_run);

/* declare static variables */


/* implement static functions */

/**
 * @fn		az_ut_set_reason_az_xml_parser_run
 * @brief	called from macro AZ_UT_SET_REASON 
 * @param	file, line, reason
 * @return	void
 * @exception	none
 */
void az_ut_set_reason_az_xml_parser_run(char *file, int line, char *reason)
{
	extern AZ_UT_DECL_TESTVECTOR_CUR(az_xml_parser_run);
	 AZ_UT_TESTVECTOR_CUR(az_xml_parser_run) -> _reason = reason;
	 AZ_UT_TESTVECTOR_CUR(az_xml_parser_run) -> _file = file;
	 AZ_UT_TESTVECTOR_CUR(az_xml_parser_run) -> _line = line;
}

/**
 * @fn		az_ut_prolog_az_xml_parser_run
 * @brief	run before exec target function for each test vector
 * @param	test run context
 * @return	void
 * @exception	none
 */
static char *mem;
static az_xml_parser_t xmlp;
AZ_UnitTest_cb_t az_ut_prolog_az_xml_parser_run(void *pInCtx)
{
	AZ_UT_INIT_TESTRUNCTX_PTR(az_xml_parser_run, pCtx, pInCtx); //declare test run context: pCtx
	AZ_UT_INIT_TESTVECTOR_PTR(az_xml_parser_run, pInput, pCtx->_input); //declare test vector: pInput

  az_size_t size = az_fs_file2mem("CDF.xml", &mem);
	/* TODO: allocate any resource and setup the parameters of the test vector */
  if (size > 0) {
    az_xml_parser_init(&xmlp, mem, mem + size, az_xml_parser_default_callback); 
    pInput->arg0 = &xmlp;
  }

	AZ_UT_PRINT_START(pCtx, pInput);

	 return 0;
}

/**
 * @fn		az_ut_epilog_az_xml_parser_run
 * @brief	run after exec target function for each test vector
 * @param	test run context
 * @return	void
 * @exception	none
 */
AZ_UnitTest_cb_t az_ut_epilog_az_xml_parser_run(void *pInCtx)
{
	AZ_UT_INIT_TESTRUNCTX_PTR(az_xml_parser_run, pCtx, pInCtx); //declare test run context: pCtx
	AZ_UT_INIT_TESTVECTOR_PTR(az_xml_parser_run, pInput, pCtx->_input); //declare test vector: pInput

	int testFailed = 0;

	/* TODO: inspect the result and determine test pass/fail */

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
 * @fn		az_ut_prolog_ctx_az_xml_parser_run
 * @brief	run before test run 
 * @param	test run context
 * @return	void
 * @exception	none
 */
AZ_UnitTest_cb_t az_ut_prolog_ctx_az_xml_parser_run(void *pInCtx)
{
	AZ_UT_INIT_TESTRUNCTX_PTR(az_xml_parser_run, pCtx, pInCtx); //test run context: pCtx
	AZ_UT_INIT_TESTVECTOR_PTR(az_xml_parser_run, pInput, pCtx->_input); //test vector: pInput

	/* TODO: may replace test vector array */

	AZ_UT_PRINT_BEGIN(pCtx);

	return 0;
}

/**
 * @fn		az_ut_epilog_ctx_az_xml_parser_run
 * @brief	run after test run 
 * @param	test run context
 * @return	void
 * @exception	none
 */
AZ_UnitTest_cb_t az_ut_epilog_ctx_az_xml_parser_run(void *pInCtx)
{
	AZ_UT_INIT_TESTRUNCTX_PTR(az_xml_parser_run, pCtx, pInCtx); //test run context: pCtx
	AZ_UT_INIT_TESTVECTOR_PTR(az_xml_parser_run, pInput, pCtx->_input); //test vector: pInput

	AZ_UT_PRINT_TERM(pCtx);

	/* TODO: free any resource allocated in test run context wide */

	return 0;
}

/**
 * @fn		az_ut_main_az_xml_parser_run
 * @brief	test run main
 * @param	 
 * @return	0
 * @exception	none
 */
AZ_UT_DEF_TEST_MAIN("UT_az_xml_parser_run", az_ut_prolog_ctx_az_xml_parser_run, az_ut_epilog_ctx_az_xml_parser_run, testvector_array_az_xml_parser_run, 
az_xml_parser_run,az_xml_parser_t *)

/*
int main(int argc, char *argv[])
{
	return AZ_UT_TEST_MAIN(az_xml_parser_run, argc, argv);
}
*/
/* === end of AZ_UT_XML_PARSER_RUN === */
