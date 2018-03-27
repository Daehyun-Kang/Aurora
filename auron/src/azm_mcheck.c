/**
 * @file   azm_mcheck.c
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

/* include header files */
#include "aurora.h"
#include "cli/az_cli_api.h"
#include "auron.h"
#include "azm_mcheck.h"

/* declare global variables */
struct azm_malloc_stats azm_malloc_stats;

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
int azm_mcheck_cli_cmd(int argc, char *argv[])
{
  int r = AZ_CLI_CMD_SUCCESS;
  struct azm_malloc_stats *stats = AZM_MALLOC_STATS();
  int resolve_symbol = 1;
  char  func_name[AZ_NAME_MAX + 1] = {0};
  char  file_name[AZ_PATH_MAX + 1] = {0};

  do {
    if (argc < 2) {
      r = AZ_CLI_CMD_SHOWHELP;
      break;
    }
    if (!strcmp(argv[1], "l")) { 
      int j = 0;
      int k = stats->cAllocPushed;
      az_ref_t addr, callsite;
      uint32_t size;
      az_cli_printf("malloc size counted=%ld reported=%ld, remaining=%u\n", stats->cTotal, stats->nTotal, stats->cAllocPushed);
      for (; j < AZM_MTRACE_LIST_SZ && k != 0; j++) {
          addr = stats->alloc_list[j];
          if (NULL == addr) continue;
          callsite = stats->alloc_call[j];
          size = stats->alloc_size[j];
          k--;
          if (resolve_symbol) {
            libtrace_resolve(callsite, func_name, AZ_NAME_MAX, file_name, AZ_PATH_MAX);
            az_cli_printf("%p %8u %21s %21s\n", addr, size, func_name, file_name); 
          } else {
            az_cli_printf("%p %p %8u\n", addr, callsite, size); 
          }
      }

      break;
    }
    r = AZ_CLI_CMD_SHOWHELP;
  } while (0);

  return r;
}

void az_attr_no_instrument azm_mcheck_init()
{
  struct azm_malloc_stats *stats = AZM_MALLOC_STATS();
  memset(stats, 0, sizeof(struct azm_malloc_stats));

  az_cli_cmd_reg("rmchk", azm_mcheck_cli_cmd, "rmchk [l]\t\t\t; remote mem check", 0);
}


/* === end of AZM_MCHECK_C === */
