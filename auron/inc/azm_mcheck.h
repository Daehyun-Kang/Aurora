/**
 * @file   azm_mcheck.h
 * @brief  
 * @date   11/03/18
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
 *   - 2018-03-11 dhkang initial create
 */

#ifndef AZM_MCHECK_H
#define AZM_MCHECK_H

#include "az_malloc.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* constants */

/* basic macros */
#define AZM_MTRACE_LIST_SZ  (CONFIG_AZ_MTRACE_LIST_SZ*8)

/* basic types */
struct azm_malloc_stats {
  //unsigned long  cAlloc;
  //unsigned long  cFree;
  //unsigned long  cFail;
  //unsigned long  cErrNull;
  //unsigned long  cErrInvd;
  unsigned long  nTotal;
  unsigned long  cTotal;
  unsigned long  cAllocPushed;
  uint32_t  pos;
  az_ref_t  alloc_list[AZM_MTRACE_LIST_SZ];
  az_ref_t  alloc_call[AZM_MTRACE_LIST_SZ];
  uint32_t  alloc_size[AZM_MTRACE_LIST_SZ];
};

/* structures */

/* structured types */

/* macros */
#define AZM_MALLOC_STATS() ((struct azm_malloc_stats *)(&azm_malloc_stats))

#define AZM_MTRACE_ALLOC_PUSH(ptr, callsite, sz, stats) \
{\
  int j; \
  for (j = 0; j < AZM_MTRACE_LIST_SZ ; j++) {\
  if (NULL == az_atomic_compare_and_swap_ptr(&stats->alloc_list[j], NULL, ptr)){\
    stats->alloc_call[j] = callsite;\
    stats->alloc_size[j] = sz;\
    stats->cTotal += sz;\
    stats->pos = j;\
    stats->cAllocPushed++;\
    break;\
  }\
  }\
}
#define AZM_MTRACE_ALLOC_POP(ptr, callsite, sz, stats) \
{ \
  int j, k, l = stats->pos; \
  az_ref_t _ptr = ptr;\
  for (j = 0, k = l; j < AZM_MTRACE_LIST_SZ; j++) {\
    if (_ptr == az_atomic_compare_and_swap_ptr(&stats->alloc_list[k], _ptr, NULL)){\
    stats->cTotal -= sz;\
    stats->alloc_size[k] = 0;\
    stats->cAllocPushed--;\
    break;\
    }\
    if (--k == 0) k = l;\
    else k++;\
  }\
}

/* variabls exposed */
extern struct azm_malloc_stats azm_malloc_stats;

/* inline functions */

/* function prototypes exposed */
extern void az_attr_no_instrument azm_mcheck_init();

#ifdef __cplusplus
}
#endif
#endif
