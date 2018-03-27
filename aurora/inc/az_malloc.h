/**
 * @file   az_malloc.h
 * @brief  
 * @date   12/11/17
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
 *   - 2017-11-12 dhkang initial create
 */

#ifndef AZ_MALLOC_H
#define AZ_MALLOC_H

#include "az_config.h"
#include "az_assert.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* constants */
#define AZ_MALLOC_MAGIC    0x5a5a5a5a

/* basic macros */

/* basic types */
typedef struct az_malloc_sign {
  int sign;
  int size;
#ifdef  CONFIG_AZ_MTRACE
  void  *func;
  int   line;
#endif
} az_malloc_sign_t;

/* structures */
struct az_malloc_stats {
  unsigned long  cAlloc;
  unsigned long  cFree;
  unsigned long  cFail;
  unsigned long  cErrNull;
  unsigned long  cErrInvd;
  unsigned long  nTotal;
#ifdef  CONFIG_AZ_MTRACE
  long  mtrace_state;
  unsigned long  cAllocPoped;
  unsigned long  cFreePoped;
  az_ref_t  alloc_list[CONFIG_AZ_MTRACE_LIST_SZ];
  az_ref_t  alloc_call[CONFIG_AZ_MTRACE_LIST_SZ];
  az_ref_t  free_list[CONFIG_AZ_MTRACE_LIST_SZ];
  az_ref_t  free_call[CONFIG_AZ_MTRACE_LIST_SZ];
  uint32_t  alloc_size[CONFIG_AZ_MTRACE_LIST_SZ];
  uint32_t  free_size[CONFIG_AZ_MTRACE_LIST_SZ];
#endif
};

#ifdef  CONFIG_AZ_MTRACE
#define AZ_MTRACE_ALLOC_PUSH(ptr, callsite, sz, stats) \
{\
  if (stats->mtrace_state) {\
  int j; \
  for (j = 0; j < CONFIG_AZ_MTRACE_LIST_SZ; j++) {\
  if (NULL == az_atomic_compare_and_swap_ptr(&stats->alloc_list[j], NULL, ptr)){\
    stats->alloc_call[j] = callsite;\
    stats->alloc_size[j] = sz;\
    break;\
  }\
  }\
  }\
}
#define AZ_MTRACE_ALLOC_POP(ptr, callsite, sz, stats) \
{ \
  if (stats->mtrace_state) {\
  int j; \
  az_ref_t _ptr;\
  ptr = NULL;\
  for (j = 0; j < CONFIG_AZ_MTRACE_LIST_SZ; j++) {\
    _ptr = stats->alloc_list[j];\
    if (_ptr == NULL) continue;\
    if (_ptr == az_atomic_compare_and_swap_ptr(&stats->alloc_list[j], _ptr, NULL)){\
   ptr = _ptr;\
   callsite = stats->alloc_call[j];\
   sz = stats->alloc_size[j];\
      stats->cAllocPoped++;\
    break;\
  }\
  }\
  }\
}
#define AZ_MTRACE_FREE_PUSH(ptr, callsite, sz, stats) \
{\
  if (stats->mtrace_state) {\
  int j; \
  for (j = 0; j < CONFIG_AZ_MTRACE_LIST_SZ; j++) {\
  if (NULL == az_atomic_compare_and_swap_ptr(&stats->free_list[j], NULL, ptr)){\
    stats->free_call[j] = callsite;\
    stats->free_size[j] = sz;\
    break;\
  }\
  }\
  }\
}
#define AZ_MTRACE_FREE_POP(ptr, callsite, sz, stats) \
{ \
  if (stats->mtrace_state) {\
  int j; \
  az_ref_t _ptr;\
  ptr = NULL;\
  for (j = 0; j < CONFIG_AZ_MTRACE_LIST_SZ; j++) {\
    _ptr = stats->free_list[j];\
    if (_ptr == NULL) continue;\
    if (_ptr == az_atomic_compare_and_swap_ptr(&stats->free_list[j], _ptr, NULL)){\
      ptr = _ptr;\
   callsite = stats->free_call[j];\
   sz = stats->free_size[j];\
      stats->cFreePoped++;\
    break;\
  }\
  }\
  }\
}
#endif

/* structured types */

/* macros */
#define AZ_MALLOC_SIGN(ptr, sz) \
  ((az_malloc_sign_t *)ptr)->sign = (int)sz ^ AZ_MALLOC_MAGIC;\
  ((az_malloc_sign_t *)ptr)->size = (int)sz;

#define AZ_MALLOC_INVALID(ptr) \
  ((((az_malloc_sign_t *)ptr)->size ^ AZ_MALLOC_MAGIC) != ((az_malloc_sign_t *)ptr)->sign)

#define AZ_MALLOC_CLEAR(ptr) \
  ((az_malloc_sign_t *)ptr)->sign = 0;\
  ((az_malloc_sign_t *)ptr)->size = 0; 

#define AZ_MALLOC_STATS() ((struct az_malloc_stats *)(&az_malloc_stats))
/* variabls exposed */
extern struct az_malloc_stats az_malloc_stats;

/* inline functions */
#ifdef  CONFIG_AZ_MALLOC
static inline void az_attr_no_instrument az_malloc_init()
{
  struct az_malloc_stats *stats = AZ_MALLOC_STATS();
  memset(stats, 0, sizeof(struct az_malloc_stats));

  #ifdef  CONFIG_AZ_MTRACE
  stats->mtrace_state = 1;
  #endif

  /*
  if (az_real_malloc == NULL) {
    az_real_malloc = dlsym(RTLD_NEXT, "malloc");
  }
  if (az_real_free == NULL) {
    az_real_free = dlsym(RTLD_NEXT, "free");
  }
  */
}
#ifdef  _CONFIG_AZ_MTRACE
#define az_malloc(size) _az_malloc(size, __FUNCTION__, __LINE__)
static inline void az_attr_no_instrument *_az_malloc(size_t  size, char *funcname, int linenum)
#else
static inline void az_attr_no_instrument *az_malloc(size_t  size)
#endif
{
  az_malloc_sign_t  *ptr = NULL;
  struct az_malloc_stats *stats = AZ_MALLOC_STATS();
  ptr = (az_malloc_sign_t *)az_sys_malloc(sizeof(az_malloc_sign_t) + size); 
  if (ptr) {
    AZ_MALLOC_SIGN(ptr, size);
    stats->cAlloc++; 
    stats->nTotal += size;
    #ifdef  CONFIG_AZ_MTRACE
    az_ref_t retaddr = __builtin_return_address(0); 
    ((az_malloc_sign_t *)ptr)->func = retaddr; 
    //((az_malloc_sign_t *)ptr)->func = funcname; 
    //((az_malloc_sign_t *)ptr)->line = linenum; 
    AZ_MTRACE_ALLOC_PUSH(ptr, retaddr, size, stats);
    #endif

    ptr++;

  } else {
    stats->cFail++;
  }

  return (void *)ptr;
}

static inline void az_attr_no_instrument az_free(void *in)
{
  az_malloc_sign_t  *ptr = (az_malloc_sign_t  *)in;
  struct az_malloc_stats *stats = AZ_MALLOC_STATS();
  do {
    if (NULL == in) {
      stats->cErrNull++;
      break;
    }

    ptr--;

    if (AZ_MALLOC_INVALID(ptr)) {
      stats->cErrInvd++;
      break;
    }

    #ifdef  CONFIG_AZ_MTRACE
    az_ref_t retaddr = __builtin_return_address(0); 
    AZ_MTRACE_FREE_PUSH(ptr, retaddr, ptr->size, stats);
    #endif

    stats->cFree++; 
    stats->nTotal -= ptr->size;
    AZ_MALLOC_CLEAR(ptr);
    az_sys_free(ptr);
  } while (0);
}
#else
static inline void az_attr_no_instrument az_malloc_init()
{
}
static inline void az_attr_no_instrument *az_malloc(size_t  size)
{
  return az_sys_malloc(size);
}
static inline void az_attr_no_instrument az_free(void *in)
{
  az_sys_free(in);
}

#endif

/* function prototypes exposed */

#ifdef __cplusplus
}
#endif
#endif
