/**
 * @file   azs_main.c
 * @brief  
 * @date   06/03/18
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
 *   - 2018-03-06 dhkang initial create
 */

/* include header files */
#include "aurora.h"
#include "az_thread.h"
#include "cli/az_cli.h"

#include "auros.h"
#include "azs_rstdio.h"

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

az_thread_config_t azs_xu_main_config = {
  .arg_name[0] = 0,
  .stackSize = 0,
  .coremask = 0x1,
  .policy = SCHED_OTHER,
  .priority = 1,
  .startdelay = -1,
};
az_thread_t azs_xu_main = NULL;

struct azs_xu_main_arg {
  int argc;
  char **argv;
} azs_xu_main_arg;

void *azs_xu_main_entry(struct azs_xu_main_arg *arg)
{
  az_assert(NULL != arg);

#ifdef CONFIG_AZ_LOG
  az_log_start();
#endif // CONFIG_AZ_LOG

  azs_rstdio_init();

  while (1) {
    az_thread_sleep(1000000000L * 3);
  }
  return NULL;
}

int main(int argc, char *argv[])
{
  az_r_t r;

  //setvbuf(stdout, NULL, _IONBF, 0);
  //setvbuf(stdin, NULL, _IONBF, 0);

  az_printf("%s start...\n", argv[0]);

  az_core_init();

  az_log_dup_stdout_start(); 

  az_trace_start_default_thread();

  azs_xu_main_arg.argc = argc;
  azs_xu_main_arg.argv = argv;

  r = az_thread_create("main", azs_xu_main_entry, &azs_xu_main_arg, &azs_xu_main_config, &azs_xu_main);
  az_printf("create main xu : %p, result=%ld\n", azs_xu_main, r);

  if (r >= AZ_SUCCESS) {
    return (int)az_thread_entry(azs_xu_main);
  } else {
    return (int)r;
  }
}

/* === end of AZS_MAIN_C === */
