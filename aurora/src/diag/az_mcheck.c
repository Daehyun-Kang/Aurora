/**
 * @file   az_mcheck.c
 * @brief  
 * @date   08/03/18
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
 *   - 2018-03-08 dhkang initial create
 */

/* include header files */
#include "az_def.h"
#include "az_malloc.h"
#include "mon/az_mcheck.h"
#include "mon/az_trace.h"
#include "cli/az_cli_api.h"

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

int az_mcheck_printMallocStats(char *bp, int blen)
{
  extern az_size_t az_sys_malloc_count;
  extern az_size_t az_sys_free_count;
#ifdef  CONFIG_AZ_MALLOC
  struct az_malloc_stats *stats = AZ_MALLOC_STATS();

  int nlen = snprintf(bp, blen,
      "alloc=%ld/%ld free=%ld/%ld fail=%ld errnull=%ld errinvd=%ld total=%ld", 
      stats->cAlloc, az_sys_malloc_count,
      stats->cFree, az_sys_free_count, 
      stats->cFail,
      stats->cErrNull, stats->cErrInvd, stats->nTotal);
#ifdef  CONFIG_AZ_MTRACE
  nlen += snprintf(&bp[nlen], blen-nlen,
      AZ_NL "allocPoped=%ld freePoped=%ld",
      stats->cAllocPoped, stats->cFreePoped);
#endif
#else
  int nlen = snprintf(bp, blen,
      "alloc=%ld free=%ld", 
      az_sys_malloc_count,
      az_sys_free_count);
#endif

  return nlen;
}

#ifdef  CONFIG_AZ_MTRACE
void az_mcheck_dispMallocList()
{
  struct az_malloc_stats *stats = AZ_MALLOC_STATS();
  int j, k, i;
  struct az_malloc_sign *ptr; 

  az_cli_printf("alloc list\n");
  for (i = 0, j = 0, k = stats->cAlloc; j < CONFIG_AZ_MTRACE_LIST_SZ; j++) {
    if (k == 0) break;
    ptr = stats->alloc_list[j];
    if (NULL == ptr) continue;
    k--;
    i++;
    az_cli_printf("%d %p %p %u\n", i, ptr, stats->alloc_call[j], stats->alloc_size[j]);
  }
  az_cli_printf("free list\n");
  for (i = 0, j = 0, k = stats->cFree; j < CONFIG_AZ_MTRACE_LIST_SZ; j++) {
    if (k == 0) break;
    ptr = stats->free_list[j];
    if (NULL == ptr) continue;
    k--;
    i++;
    az_cli_printf("%d %p %p %u\n", i, ptr, stats->free_call[j], stats->free_size[j]);
  }
}

int az_mcheck_mkMallocRpt(az_mchk_info_t *info)
{
  int j, k, i, l;
  unsigned long count;
  az_ref_t addr, callsite;
  uint32_t size;
  struct az_malloc_stats *stats = AZ_MALLOC_STATS();

  count = stats->cAlloc - stats->cAllocPoped;
  l = array_size(info->list);
  for (j = 0, k = 0, i = 0; j < CONFIG_AZ_MTRACE_LIST_SZ && k < count; j++) {
    AZ_MTRACE_ALLOC_POP(addr, callsite, size, stats);
    if (addr == NULL) continue;
    else k++;
    info->list[i].addr = addr;
    info->list[i].call = callsite;
    info->list[i].size = size;
    i++;
    if (i >= l) break;
  }
  info->nTotal = stats->nTotal;
  info->code = AZ_TRACE_CODE_RPT_MALLOC;
  info->length = AZ_MCK_INFO_LEN(i); 

  return i; 
}

int az_mcheck_mkFreeRpt(az_mchk_info_t *info)
{
  int j, k, i, l;
  unsigned long count;
  az_ref_t addr, callsite;
  uint32_t size;
  struct az_malloc_stats *stats = AZ_MALLOC_STATS();

  count = stats->cFree - stats->cFreePoped;
  l = array_size(info->list);
  for (j = 0, k = 0, i = 0; j < CONFIG_AZ_MTRACE_LIST_SZ && k < count; j++) {
    AZ_MTRACE_FREE_POP(addr, callsite, size, stats);
    if (addr == NULL) continue;
    else k++;
    info->list[i].addr = addr;
    info->list[i].call = callsite;
    info->list[i].size = size;
    i++;
    if (i >= l) break;
  }
  info->nTotal = stats->nTotal;
  info->code = AZ_TRACE_CODE_RPT_FREE;
  info->length = AZ_MCK_INFO_LEN(i); 

  return i; 
}

void az_mcheck_disableMtrace()
{
  struct az_malloc_stats *stats = AZ_MALLOC_STATS();
  stats->mtrace_state = 0;
}
#endif

int az_mcheck_cli_cmd(int argc, char *argv[])
{
  int r = AZ_CLI_CMD_SUCCESS;
  char buf[128];

  do {
    if (argc < 1) {
      r = AZ_CLI_CMD_SHOWHELP;
      break;
    }
    if (argc > 1) {
      if (!strcmp(argv[1], "l")) {
      #ifdef  CONFIG_AZ_MTRACE
        az_mcheck_dispMallocList();
      #else
        az_cli_printf("AZ_MTRACE not enabled\n");
      #endif
      }
      break;
    }
    az_mcheck_printMallocStats(buf, sizeof(buf));
    az_cli_printf("malloc status: %s" AZ_NL, buf);
  } while (0);

  return r;
}

static int az_mcheck_state = 0;
az_r_t az_mcheck_init()
{
  az_r_t r = AZ_SUCCESS;
  if (az_mcheck_state == 0) {
    az_mcheck_state = 1;
    r = az_cli_cmd_reg("mchk", az_mcheck_cli_cmd, "mchk [l]\t\t\t\t\t;memory usage status", 0);
  }

  return r;
}

az_r_t az_mcheck_deinit()
{
  az_r_t r = AZ_SUCCESS;
  if (az_mcheck_state == 1) {
    az_mcheck_state = 0;
  }
  return r;
}

/* === end of AZ_MCHECK_C === */
