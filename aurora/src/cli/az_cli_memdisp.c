/**
 * @file   az_cli_memdisp.c
 * @brief  
 * @date   14/03/18
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
 *   - 2018-03-14 dhkang initial create
 */

/* include header files */
#include "az_def.h"
#include "az_string.h"
#include  "cli/az_cli.h"
#include  "cli/az_cli_cmd.h"

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
char *az_cli_cmd_memdisp_help = "md [-s/-i/-l] [addr] [len]\t\t\t;memory display";  
int az_cli_cmd_memdisp(int argc, char *argv[])
{
  char *addr;
  int j, k;
  int r = AZ_CLI_CMD_SHOWHELP; 
  int mode = 1;
  int size = 0x10;

  do {
    if (argc < 2) {
      break;
    }
    switch (argc) {
      case 2:
        addr = (char *)strtol(argv[1], NULL, 16); 
      break;
      default:
        if (!strcmp(argv[1], "-s")) {
          mode = 2;
          addr = (char *)strtol(argv[2], NULL, 16); 
          if (argc == 4) size = (char *)strtol(argv[3], NULL, 0); 
          break;
        } 
        if (!strcmp(argv[1], "-i")) {
          mode = 4;
          addr = (char *)strtol(argv[2], NULL, 16); 
          if (argc == 4) size = (char *)strtol(argv[3], NULL, 0); 
          break;
        }
        if (!strcmp(argv[1], "-l")) {
          mode = 8;
          addr = (char *)strtol(argv[2], NULL, 16); 
          if (argc == 4) size = (char *)strtol(argv[3], NULL, 0); 
          break;
        }
        addr = (char *)strtol(argv[1], NULL, 16); 
        size = (char *)strtol(argv[2], NULL, 0); 
      break;
    }
    az_cli_shell_t *pSh = (az_cli_shell_t *)az_xu_getarg();
    if (NULL != pSh) {
      az_memdisp(pSh->txport, addr, size, mode, 1, pSh->rxport);
      r = AZ_CLI_CMD_SUCCESS; 
    }

  } while (0);

  return r;
}


/* === end of AZ_CLI_MEMDISP_C === */
