/**
 * @file   az_ver.c
 * @brief  
 * @date   05/11/17
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
 *   - 2017-11-05 dhkang initial create
 */

/* include header files */
#include "az_ver.h"

/* declare global variables */
az_version_t  az_version = {
  .major = 0,
  .minor = 1,
  .stage = 0,
  .build = 0,
};

char  *az_build_time = __DATE__ " " __TIME__;

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

/* === end of AZ_VER_C === */
