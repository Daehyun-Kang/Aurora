/**
 * @file   az_tc_timer_start.h
 * @brief  
 * @date   13/04/18
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
 *   - 2018-04-13 dhkang initial create
 */

#ifndef AZ_TC_TIMER_START_H
#define AZ_TC_TIMER_START_H

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
struct az_tc_testvector_st_timer_start {
  az_ion_id_t id;
  uint64_t interval;
  int repeat;
  az_r_t  expected_result;
  az_r_t  result;
};

typedef struct az_tc_testvector_st_timer_start az_tc_tv_timer_start_t;
/* structured types */

/* macros */

/* variabls exposed */

extern int az_tc_timer_start_flags;

/* declare current test case pointer */
extern AZ_SYS_TLS az_test_case_t *az_tc_cur;

/* declare the reference for test vectors */
extern struct az_tc_testvector_st_timer_start *az_tc_testvector_array_timer_start;

/* inline functions */

/* function prototypes exposed */

/* this is unit test target function */
//extern az_r_t timer_start(az_ion_id_t id, uint64_t interval, int repeat);

/* declare global callback functions */

/* this is init handler for each test case */
extern az_r_t az_tc_init_timer_start(az_test_case_t *pTC);

/* this is sync handler for each test case */
extern az_r_t az_tc_sync_timer_start(az_test_case_t *pTC);

/* this is prolog handler for each test iteration */
extern az_r_t az_tc_prolog_timer_start(az_test_case_t *pTC);

/* this is run handler for each test iteration */
extern az_r_t az_tc_run_timer_start(az_test_case_t *pTC);

/* this is epilog handler for each test iteration */
extern az_r_t az_tc_epilog_timer_start(az_test_case_t *pTC);

/* this is term handler for each test case */
extern az_r_t az_tc_term_timer_start(az_test_case_t *pTC);

#ifdef __cplusplus
}
#endif
#endif

/* === end of AZ_TC_TIMER_START_H === */
