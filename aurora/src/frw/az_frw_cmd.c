/**
 * @file   az_frw_cmd.c
 * @brief  
 * @date   28/10/17
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
 *   - 2017-10-28 dhkang initial create
 */

/* include header files */
#include  "aurora.h"
#include  "cli/az_cli_api.h"

/* declare global variables */

/* declare static variables */


/* implement static functions */
char *az_frw_ion_addr(void *pData)
{
  static char addr[32];
  sprintf(addr, "%p",pData);
  return addr;
}

char *az_frw_ion_type2Str(void *pData)
{
  az_ion_type_t type = *(az_ion_type_t *)pData;

  return az_ion_type_name(type);
}


/**
 * @fn        function name
 * @brief     function-description
 * @param     input-output-parameters
 * @return    return-value
 * @warning   warnings
 * @exception none
 */

int32_t az_frw_cmd_show_ion(int argc, char *argv[])
{
  int32_t result =  AZ_CLI_CMD_SUCCESS; 
  char  bp[256];
  int   blen = sizeof(bp);
  az_ion_id_t id = -1;
  az_ion_t  *ion;
  az_size_t ion_count;
  char thwlist[] = { 8, 16, 8, 12 };
  char *thnlist[] = { "id", "addr", "type", "refCount" };
  az_var_value2Str_t convlist[] = { NULL, az_frw_ion_addr, az_frw_ion_type2Str, NULL }; 

  do {
    bp[0] = 0;
    if (argc > 2) {
      id = strtol(argv[2], NULL, 10);
      ion = az_ion(id);
      if (NULL == ion) {
        az_cli_printf("%d out of range" AZ_NL, id);
        break;
      }
      result = az_ion_toStr(ion, "", bp, blen); 
      az_cli_printf("%s" AZ_NL, bp);
      break;
    }
    ion_count = az_ion_count();
    az_cli_printf("# of ions: "AZ_FMT_SIZE(1) AZ_NL, ion_count);
    az_mkTableHeaderStr(4, thwlist, thnlist, bp, blen);
    az_cli_printf("%s" AZ_NL, bp);
    for (id = 0; ion_count > 0; id++) {
      ion = az_ion(id);
      if (NULL == ion) continue;
      ion_count--;
      result = az_ion_valuesOnRow(ion, "", thwlist, convlist, bp, blen);
      az_cli_printf("%s"AZ_NL, bp);
    }

  } while (0);

  return result;
}

struct az_cli_key_cmd az_frw_cmd_show_table[] = {
  {"ion", az_frw_cmd_show_ion},
  {NULL, NULL}
};


/* implement global functions */

/**
 * @fn        function name
 * @brief     function-description 
 * @param     input-output-parameters
 * @return    return-value
 * @warning   warnings
 * @exception none
 */

int32_t az_frw_cmd_show(int argc, char *argv[])
{
  int32_t result =  AZ_CLI_CMD_SHOWHELP; 
  struct az_cli_key_cmd *tbl = az_frw_cmd_show_table;
  do { 
    if (argc < 2) {
      break;
    }

    for (; NULL != tbl->key; tbl++) {
      if (!strcmp(argv[1], tbl->key)) { 
        result = (tbl->cmd)(argc, argv);
        break;
      }
    }

  } while (0);


  return result;
}

void az_frw_cmd_init()
{
  az_cli_cmd_reg("show", az_frw_cmd_show, "show [rec] [index]\t\t\t\t;show records with index", 0); 
}

/* === end of AZ_FRW_CMD_C === */
