/**
 * @file   az_main.c
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
#include "frw/az_frw.h"
#include "cli/az_cli.h"
#include "az_printf.h"

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
az_xu_config_t az_xu_main_config = {
  .arg_name[0] = 0,
  .stackSize = 0,
  .coremask = 0x1,
  .policy = SCHED_OTHER,
  .priority = 1,
  .startdelay = -1,
};
az_xu_t az_xu_main = NULL;

struct az_xu_main_arg {
  int argc;
  char **argv;
} az_xu_main_arg;

void *az_xu_main_entry(struct az_xu_main_arg *arg)
{
  az_assert(NULL != arg);

  return az_test_main(arg->argc, arg->argv);
}

//extern void  az_rstdio_begin(void);

az_uint32_t az_ring_ds32[2];
int main(int argc, char *argv[])
{
  az_r_t r;

  //setvbuf(stdout, NULL, _IONBF, 0);
  //setvbuf(stdin, NULL, _IONBF, 0);

  //az_rstdio_begin();

  az_core_init();

  az_log_dup_stdout_start(); 

  az_trace_start_default_thread();

  az_mcheck_init();

  r = az_frw_parse_options(argc, argv);
  if (r < 0) {
    exit(r);
  }

  az_xu_main_arg.argc = argc;
  az_xu_main_arg.argv = argv;

  r = (az_r_t)az_xu_create("main", az_xu_main_entry, &az_xu_main_arg, &az_xu_main_config, &az_xu_main);
  az_printf("create main xu : %p, result=%ld\n", az_xu_main, r);

  #ifdef  CONFIG_AZ_TRACE_START
  az_trace_start();
  #endif

  if (r >= AZ_SUCCESS) {
    return (int)az_xu_entry(az_xu_main);
  } else {
    return (int)r;
  }
}

/* === end of AZ_MAIN_C === */
