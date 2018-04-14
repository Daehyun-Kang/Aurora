/**
 * @file   az_test_watchpoint.c
 * @brief  
 * @date   09/02/16
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
 *   - 2016-02-09 Daehyun Kang initial create
 */

/* include header files */
#include "aurora.h"
#include "cli/az_cli.h"
#include "frw/az_frw_cmd.h"
#include "az_ver.h"
#include "mon/az_debug.h"

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

uint64_t az_g_var64 = 0;
uint32_t az_g_var32 = 0;
uint16_t az_g_var16 = 0;
uint8_t az_g_var8 = 0;

int az_test_watchpoint()
{
  int r;

  az_debug_init_watchpoint();

  r = az_debug_set_watchpoint(0, &az_g_var64, sizeof(az_g_var64));
  az_printf("r = %d\n", r);
  r = az_debug_set_watchpoint(1, &az_g_var32, sizeof(az_g_var32));
  az_printf("r = %d\n", r);
  r = az_debug_set_watchpoint(2, &az_g_var16, sizeof(az_g_var16));
  az_printf("r = %d\n", r);
  r = az_debug_set_watchpoint(3, &az_g_var8, sizeof(az_g_var8));
  az_printf("r = %d\n", r);

#ifdef CONFIG_AZ_SEGV_TEST
  *(int *)0x12345678 = 0;
#endif

  int count = 0;
  while (count < 64) {
    az_thread_sleep(1000000000 * 1);
    count++;
   if (count == 8) {
     az_g_var8 = count;
   } 
   if (count == 16) {
     az_g_var16 = count;
   }
   if (count == 32) {
     az_g_var32 = count;
   }
   if (count == 64) {
     az_g_var64 = count;
   }
  }
}

/* === end of AZ_TEST_WATCHPOINT_C === */
