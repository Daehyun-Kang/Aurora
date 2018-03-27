/**
 * @file   az_frw_cfg.h
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

#ifndef AZ_FRW_CFG_H
#define AZ_FRW_CFG_H

#include "az_xcfg.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* constants */
#define AZ_FRW_CFG_KEY_sys   "frw"

/* basic macros */

/* basic types */

/* structures */

/* structured types */

/* macros */
#define AZ_FRW_CFG_TREE()  (&az_frw_cfg_tree)
#define AZ_APP_CFG_TREE()  (&az_app_cfg_tree)

/* variabls exposed */
extern az_xcfg_tree_t az_frw_cfg_tree;
extern az_xcfg_tree_t az_app_cfg_tree;

/* inline functions */

/* function prototypes exposed */
extern az_r_t  az_frw_loadFrwConfig(char *filename);
extern az_r_t  az_frw_freeFrwConfig();
extern az_r_t  az_frw_loadAppConfig(char *filename);
extern az_r_t  az_frw_freeAppConfig();

#ifdef __cplusplus
}
#endif
#endif
