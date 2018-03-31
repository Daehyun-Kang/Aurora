/**
 * @file   az_xu_cfg.h
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

#ifndef AZ_XU_CFG_H
#define AZ_XU_CFG_H

#include "az_def.h"
#include "az_xcfg.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* constants */
#define AZ_XU_CFG_NAME_LEN         32

#define AZ_XU_CFG_KEY_xulist   "xulist"
#define AZ_XU_CFG_KEY_xu       "xu"
#define AZ_XU_CFG_KEY_name         "name"
#define AZ_XU_CFG_KEY_entry_name   "entry"
#define AZ_XU_CFG_KEY_arg_name     "arg"
#define AZ_XU_CFG_KEY_stackSize    "stackSize.u32"
#define AZ_XU_CFG_KEY_schedule     "schedule"
#define AZ_XU_CFG_KEY_coremask     "coremask.h64"
#define AZ_XU_CFG_KEY_policy       "policy"
#define AZ_XU_CFG_KEY_priority     "priority.u32"

#define AZ_XU_CFG_VAL_policy_SCHED_RR       "SCHED_RR"
#define AZ_XU_CFG_VAL_policy_SCHED_FIFO     "SCHED_FIFO"

#define AZ_XU_CFG_CAT_SYS          0
#define AZ_XU_CFG_CAT_APP          1

/* basic macros */
#define AZ_XU_CFG_TREE(cat) \
  ((cat == AZ_XU_CFG_CAT_SYS)? AZ_FRW_CFG_TREE():AZ_APP_CFG_TREE())

/* basic types */
typedef int az_xu_policy_t;

typedef struct az_xu_config {
  char            name[AZ_XU_CFG_NAME_LEN];
  char            entry_name[AZ_XU_CFG_NAME_LEN];
  char            arg_name[AZ_XU_CFG_NAME_LEN];
  uint32_t        stackSize;
  uint64_t        coremask;
  az_xu_policy_t  policy;
  uint32_t        priority;

  int             startdelay;
} az_xu_config_t;

/* structures */

/* structured types */

/* macros */

/* variabls exposed */

/* inline functions */

/* function prototypes exposed */

#ifdef __cplusplus
}
#endif
#endif
