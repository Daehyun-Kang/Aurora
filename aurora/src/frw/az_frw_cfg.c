/**
 * @file   az_frw_cfg.c
 * @brief  
 * @date   10/11/17
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
 *   - 2017-11-10 dhkang initial create
 */

/* include header files */
#include "frw/az_frw_cfg.h"

/* declare global variables */
az_xcfg_tree_t az_frw_cfg_tree;
az_xcfg_tree_t az_app_cfg_tree;

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
az_r_t  az_frw_loadFrwConfig(char *filename)
{
  az_r_t r = az_xcfg_load_config(filename, AZ_FRW_CFG_TREE());

  return r;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
az_r_t  az_frw_freeFrwConfig()
{
  az_r_t r = az_xcfg_free_config(AZ_FRW_CFG_TREE());

  return r;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
az_r_t  az_frw_loadAppConfig(char *filename)
{
  az_r_t r = az_xcfg_load_config(filename, AZ_APP_CFG_TREE());

  return r;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
az_r_t  az_frw_freeAppConfig()
{
  az_r_t r = az_xcfg_free_config(AZ_APP_CFG_TREE());

  return r;
}


/* === end of AZ_FRW_CFG_C === */
