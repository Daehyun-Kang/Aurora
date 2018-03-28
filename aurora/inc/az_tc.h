/**
 * @file   az_tc.h
 * @brief  
 * @date   01/02/16
 * @author Daehyun Kang
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
 *   - 2016-02-01 Daehyun Kang initial create
 */

#ifndef AZ_TC_H
#define AZ_TC_H

#include "az_args.h"
#include "az_test.h"

#define AZ_TC_FLAG_TRACE       1


#ifdef __cplusplus
extern "C"
{
#endif

#ifdef  CONFIG_TEST_CASE

/* define constants */

/* define basic macros */

/* define basic types */
#define AZ_TC_SET_CUR_TC(tc) az_tc_cur = tc 
#define AZ_TC_GET_CUR_TC() az_tc_cur 
#define AZ_TC_TV_SZ(pTC, type) (sizeof(type) * (pTC->test_iter_count))  

#define AZ_TC_SET_TV(pTC, tv) pTC->test_vector = (void *)tv 
#define AZ_TC_GET_TV(pTC, type) ((type *)(pTC->test_vector))

#define AZ_TC_ITER_LIST(pTC) pTC->test_iter.list
#define AZ_TC_ITER_COUNT(pTC) pTC->test_iter.count

#define AZ_TC_CUR_ITER(pTC) (pTC->test_iter.list + pTC->test_iter.index)
#define AZ_TC_ITER_SET_TV(iter, tv) iter->test_vector = (void *)tv 
#define AZ_TC_ITER_GET_TV(iter, type) ((type *)(iter->test_vector))

/* define structures */

/* define structured types */

/* define macros */
#define FILE_NULL     NULL
#define REASON_NULL   NULL

#define AZ_TC_RESULT_SUCCESS  0
#define AZ_TC_RESULT_ERROR    -1
#define AZ_TC_RESULT_SEGV     -11

#define AZ_TC_PRINT_B(pCtx) \
    do { \
    printf("\n\n[B] [%s] test run [%d]\n",pCtx->_title, pCtx->_tv_count);\
    } while (0);

#define AZ_TC_PRINT_BEGIN(pCtx) AZ_TC_PRINT_B(pCtx)

#define AZ_TC_PRINT_T(pCtx) \
  do {\
    printf("[T] [%s] test run [%d] pass [%d] fail [%d]\n",pCtx->_title, pCtx->_tv_count, pCtx->_pass, pCtx->_fail); \
  } while (0);

#define AZ_TC_PRINT_TERM(pCtx) AZ_TC_PRINT_T(pCtx)

#define AZ_TC_PRINT_START(pCtx, pInput) \
  do { \
    printf("[S] [%s] test run [%d]\n",pCtx->_title, pInput->_index); \
  } while (0); 

#define AZ_TC_PRINT_END_FAIL(pCtx, pInput, err) \
  do {\
    printf("[E] [%s] test run [%d] [%s] [%d] [%s:%d] [%s]\n",\
            pCtx->_title, pInput->_index,\
          (!err)? "PASS":"FAIL", err,\
            pInput->_file, pInput->_line, pInput->_reason);\
  } while (0);

#define AZ_TC_PRINT_END_PASS(pCtx, pInput, err) \
  do {\
    printf("[E] [%s] test run [%d] [%s] [%d] \n",\
            pCtx->_title, pInput->_index,\
          (!err)? "PASS:success":"PASS:failure", err); \
    if (!err) pCtx->_success++;\
    else pCtx->_failure++; \
  } while (0);

/* variabls exposed */
extern  int az_tc_flags;
extern AZ_SYS_TLS az_testcase_t  *az_tc_cur;

/* inline functions */

/* function prototypes exposed*/

#else
#define AZ_TC_SET_REASON(func, ...) 
#define AZ_TC_SET_OK(func, ...) 

#endif

#ifdef __cplusplus
}
#endif

#endif
