/**
 * @file   az_dllib.c
 * @brief  
 * @date   12/11/17
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
 *   - 2017-11-12 dhkang initial create
 */

/* include header files */
#include "az_dllib.h"

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
az_r_t az_dllib_getSymbol(char *name, az_ref_t *pAddr)
{
  az_r_t r;

  do {
    r = az_sys_dllib_getStaticSymbol(name, pAddr);
  } while (0);

  return r;
}

/**
 * @fn        function name
 * @brief     function-description
 * @param     input-output-parameters
 * @return    return-value
 * @warning   warnings
 * @exception none
 */


/* === end of AZ_DLLIB_C === */
