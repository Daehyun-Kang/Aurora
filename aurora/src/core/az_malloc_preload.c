/**
 * @file   az_malloc_preload.c
 * @brief  
 * @date   03/03/18
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
 *   - 2018-03-03 dhkang initial create
 */

/* include header files */
#define _GNU_SOURCE
#include "az_def.h"
#include "aurora.h"
#include "az_malloc.h"

#ifdef  CONFIG_AZ_PRELOAD_SO
#include "az_syscall_preload.h"

/* declare global variables */
//void *(*az_real_malloc)(size_t size) = NULL;
//void (*az_real_free)(void *) = NULL;

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
void *malloc(size_t size)
{
  return az_malloc(size);
}

void free(void *ptr)
{
  az_free(ptr);
}
#endif

/* === end of AZ_MALLOC_PRELOAD_C === */
