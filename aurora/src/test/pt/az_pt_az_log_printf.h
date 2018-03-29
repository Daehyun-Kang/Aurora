/**
 * @file   az_pt_az_log_printf.h
 * @brief  
 * @date   29/03/18
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
 *   - 2018-03-29 dhkang initial create
 */

#ifndef AZ_PT_AZ_LOG_PRINTF_H
#define AZ_PT_AZ_LOG_PRINTF_H

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
struct az_pt_testvector_st_az_log_printf {
  int samples;
  uint64_t result;
};
typedef struct az_pt_testvector_st_az_log_printf az_pt_tv_az_log_printf_t;
/* structured types */

/* macros */

/* variabls exposed */

extern int az_pt_az_log_printf_flags;

/* declare current test case pointer */
extern AZ_SYS_TLS az_test_case_t *az_tc_cur;

/* declare the reference for test vectors */
extern struct az_pt_testvector_st_az_log_printf *az_pt_testvector_array_az_log_printf;

/* inline functions */

/* function prototypes exposed */

/* this is unit test target function */
//extern int az_log_printf();

/* declare global callback functions */

/* this is init handler for each test case */
extern az_r_t az_pt_init_az_log_printf(az_test_case_t *pTC);

/* this is sync handler for each test case */
extern az_r_t az_pt_sync_az_log_printf(az_test_case_t *pTC);

/* this is prolog handler for each test iteration */
extern az_r_t az_pt_prolog_az_log_printf(az_test_case_t *pTC);

/* this is run handler for each test iteration */
extern az_r_t az_pt_run_az_log_printf(az_test_case_t *pTC);

/* this is epilog handler for each test iteration */
extern az_r_t az_pt_epilog_az_log_printf(az_test_case_t *pTC);

/* this is term handler for each test case */
extern az_r_t az_pt_term_az_log_printf(az_test_case_t *pTC);

#ifdef __cplusplus
}
#endif
#endif

/* === end of AZ_PT_AZ_LOG_PRINTF_H === */
