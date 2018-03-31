/**
 * @file   az_tc_report_excpt.h
 * @brief  
 * @date   01/04/18
 * @author dhkang
 *
 * @copyright 
 * Copyright (c) Fware, 2013-2018 - All Rights Reserved
 * You may use, distribute and modify this code under the 
 * terms of the "Aurora Source Code License".
 * See the file LICENSE for full license details.\n\n
 * 
 * You should have received a copy of  the LICENSE with this file.\n\n
 * 
 * If not, please contact to Fware with the information in the file CONTACTS.
 *
 * History:
 *   - 2018-04-01 dhkang initial create
 */

#ifndef AZ_TC_REPORT_EXCPT_H
#define AZ_TC_REPORT_EXCPT_H

#include "aurora.h"
#include "az_test.h"
/* TODO: include function dependant header files */

#define AZ_TC_FUNC_HAS_RETURN  1
#undef AZ_TC_FUNC_VOID  
#include "az_tc.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* constants */

/* basic macros */

/* basic types */

/* structures */

/* this is to define test vector structure */
struct az_tc_testvector_st_report_excpt {
  int  expected_result;
  int  result;
};

typedef struct az_tc_testvector_st_report_excpt az_tc_tv_report_excpt_t;
/* structured types */

/* macros */

/* variabls exposed */

extern int az_tc_report_excpt_flags;

/* declare current test case pointer */
extern AZ_SYS_TLS az_test_case_t *az_tc_cur;

/* declare the reference for test vectors */
extern struct az_tc_testvector_st_report_excpt *az_tc_testvector_array_report_excpt;

/* inline functions */

/* function prototypes exposed */

/* this is unit test target function */
extern int report_excpt();

/* declare global callback functions */

/* this is init handler for each test case */
extern az_r_t az_tc_init_report_excpt(az_test_case_t *pTC);

/* this is sync handler for each test case */
extern az_r_t az_tc_sync_report_excpt(az_test_case_t *pTC);

/* this is prolog handler for each test iteration */
extern az_r_t az_tc_prolog_report_excpt(az_test_case_t *pTC);

/* this is run handler for each test iteration */
extern az_r_t az_tc_run_report_excpt(az_test_case_t *pTC);

/* this is epilog handler for each test iteration */
extern az_r_t az_tc_epilog_report_excpt(az_test_case_t *pTC);

/* this is term handler for each test case */
extern az_r_t az_tc_term_report_excpt(az_test_case_t *pTC);

#ifdef __cplusplus
}
#endif
#endif

/* === end of AZ_TC_REPORT_EXCPT_H === */
