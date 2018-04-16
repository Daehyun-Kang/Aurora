/**
 * @file   aurot_main.c
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

  az_test_main(arg->argc, arg->argv);

  while (1) {
    az_thread_sleep(3 * 1E9);
  }
}

//extern void  az_rstdio_begin(void);

int main(int argc, char *argv[])
{

  return az_main(argc, argv);
}

/* === end of AUROT_MAIN_C === */
