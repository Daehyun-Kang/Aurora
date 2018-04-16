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

void *az_thread_main_entry(struct az_thread_main_arg *arg)
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
  return az_main(argc, argv);
}

/* === end of AZS_MAIN_C === */
