/**
 * @file   az_ut_main.c
 * @brief  
 * @date   09/04/16
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
#include "az_unitTest.h"

/* declare global variables */

/* declare static variables */


/* implement static functions */

/**
 * @fn        function name
 * @brief     function-description
 * @param     input-output-parameters
 * @return    return-value
 * @warning   warnings
 * @exception none
 */


/* implement global functions */

/**
 * @fn        function name
 * @brief     function-description
 * @param     input-output-parameters
 * @return    return-value
 * @warning   warnings
 * @exception none
 */

extern AZ_UT_TEST_MAIN(az_ion_register, argc, argv);
extern AZ_UT_TEST_MAIN(az_link_putLast, argc, argv);
extern AZ_UT_TEST_MAIN(az_link_putFirst, argc, argv);
extern AZ_UT_TEST_MAIN(az_link_remove, argc, argv);
extern AZ_UT_TEST_MAIN(az_link_getFirst, argc, argv);
extern AZ_UT_TEST_MAIN(az_double_link_add, argc, argv);
extern AZ_UT_TEST_MAIN(az_double_link_del, argc, argv);
	extern  AZ_UT_TEST_MAIN(az_fifo_push, argc, argv);
	extern AZ_UT_TEST_MAIN(az_fifo_pop, argc, argv);
	extern AZ_UT_TEST_MAIN(az_buffer_reserve, argc, argv);
	extern AZ_UT_TEST_MAIN(az_xu_create, argc, argv);

	extern AZ_UT_TEST_MAIN(az_fs_file2mem, argc, argv);
	extern  AZ_UT_TEST_MAIN(az_xml_parser_run, argc, argv);
	extern  AZ_UT_TEST_MAIN(az_xml_tree_parse, argc, argv);
az_ut_result_t az_ut_result;
az_ut_testProjResult_t az_ut_testProjRest_primitives;
int main(int argc, char *argv[])
{
  az_ut_result_init(&az_ut_result); 
  az_ut_testProjResult_init(&az_ut_testProjRest_primitives, "primitives"); 
  az_ut_add_testProjResult(&az_ut_result, &az_ut_testProjRest_primitives); 

  //AZ_UT_TEST_MAIN(az_ion_register, argc, argv);
  //AZ_UT_TEST_MAIN(az_link_putLast, argc, argv);
  //AZ_UT_TEST_MAIN(az_link_putFirst, argc, argv);
  //AZ_UT_TEST_MAIN(az_link_remove, argc, argv);
  //AZ_UT_TEST_MAIN(az_link_getFirst, argc, argv);
  //AZ_UT_TEST_MAIN(az_double_link_add, argc, argv);
  //AZ_UT_TEST_MAIN(az_double_link_del, argc, argv);
	//AZ_UT_TEST_MAIN(az_fifo_push, argc, argv);
	//AZ_UT_TEST_MAIN(az_fifo_pop, argc, argv);
	//AZ_UT_TEST_MAIN(az_buffer_reserve, argc, argv);
	//AZ_UT_TEST_MAIN(az_xu_create, argc, argv);

	//AZ_UT_TEST_MAIN(az_fs_file2mem, argc, argv);
  
	AZ_UT_TEST_MAIN(az_xml_parser_run, argc, argv);
	AZ_UT_TEST_MAIN(az_xml_tree_parse, argc, argv);

  az_ut_testProjResult_t *curTestProj = AZ_UT_CurTestProjResult();
  az_ut_testCaseResult_t *curTestCase;
  char *hline = "---------------------------------------------------------------------";
  char *lfmt = " %s\n";
  char *sfmt = " %3s %20s %10s %10s %10s %10s\n";
  char *ifmt = " %3d %20s %10d %10d %10d %10d\n";

  az_sys_printf("\n\r\n\r");
  az_sys_printf(lfmt, "======= AURORA FRAMEWORK UNIT TEST RESULT =======\n");
  //az_sys_printf(lfmt, hline);
  az_sys_printf("   Test Project: %s(%d)\n", curTestProj->name, curTestProj->testCaseCount);
  az_sys_printf(lfmt, hline);
  az_sys_printf(sfmt, "No", "Test Case Name", 
                  "PASS", "FAIL", "SUCCESS", "FAILURE");
  az_sys_printf(lfmt, hline);
  int j;
  
  curTestCase = curTestProj->tcHead;
  for (j = 0; j < curTestProj->testCaseCount; j++) {
    az_sys_printf(ifmt, curTestCase->index, curTestCase->name, 
                  curTestCase->pass, curTestCase->fail,
                  curTestCase->success, curTestCase->failure);
    curTestCase = curTestCase->next;
    if (curTestCase == NULL) break;
  }
  az_sys_printf(lfmt, hline);
  az_sys_printf(ifmt, curTestProj->testCaseCount, "TOTAL",
                  curTestProj->pass, curTestProj->fail,
                  curTestProj->success, curTestProj->failure);
  az_sys_printf(lfmt, hline);
  
  az_sys_printf("\n\r");
}

/* === end of AZ_UT_MAIN_C === */
