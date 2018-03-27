/**
 * @file   az_malloc.c
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
#include "aurora.h"
#include "az_malloc.h"

/* declare global variables */
#ifdef  CONFIG_AZ_PRELOAD_SO_ALONE

struct az_malloc_stats az_malloc_stats = {0};

az_size_t az_sys_malloc_count;
az_size_t az_sys_free_count;

#else
struct az_malloc_stats az_malloc_stats = {0};
#endif


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
void az_malloc_show_status()
{
  struct az_malloc_stats *stats = AZ_MALLOC_STATS();
  printf("alloc=%ld free=%ld fail=%ld\n", stats->cAlloc, stats->cFree, stats->cFail);
  printf("errNull=%ld errInvd=%ld nTotal=%ld\n", stats->cErrNull, stats->cErrInvd, stats->nTotal);
}

/* === end of AZ_MALLOC_C === */
