/**
 * @file   az_frw.h
 * @brief  
 * @date   16/11/17
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
 *   - 2017-11-16 dhkang initial create
 */

#ifndef AZ_FRW_H
#define AZ_FRW_H
#include "az_core.h"
#include "az_fsm.h"
#include "az_ver.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* constants */
#define   AZ_FRW_STATE_IDLE     0
#define   AZ_FRW_STATE_INIT     1
#define   AZ_FRW_STATE_CNFG     2
#define   AZ_FRW_STATE_SYNC     3
#define   AZ_FRW_STATE_IERR     4
#define   AZ_FRW_STATE_NORM     5
#define   AZ_FRW_STATE_DOWN     6

/* define framework events */
#ifdef    CONFIG_AZ_FRW_EVT_BASE
#define   AZ_FRW_EVT_BASE       AZ_EVENT_ID_SYS_FLAG|CONFIG_AZ_FRW_EVT_BASE
#define   AZ_FRW_EVT_MASK       CONFIG_AZ_FRW_EVT_MASK 
#else
#define   AZ_FRW_EVT_BASE       AZ_EVENT_ID_SYS_FLAG|0x10000000 
#define   AZ_FRW_EVT_MASK       ((az_event_id_t)0x0fffffff) 
#endif
#define   AZ_FRW_CMD_INIT       AZ_FRW_EVT_BASE|0x00000001
#define   AZ_FRW_CMD_CONF       AZ_FRW_EVT_BASE|0x00000002
#define   AZ_FRW_CMD_SYNC       AZ_FRW_EVT_BASE|0x00000003
#define   AZ_FRW_EVT_TOUT       AZ_FRW_EVT_BASE|0x00000004
#define   AZ_FRW_EVT_SYNC       AZ_FRW_EVT_BASE|0x00000005
#define   AZ_FRW_CMD_SAPP       AZ_FRW_EVT_BASE|0x00000006
#define   AZ_FRW_CMD_TAPP       AZ_FRW_EVT_BASE|0x00000007
#define   AZ_FRW_CMD_DOWN       AZ_FRW_EVT_BASE|0x00000008
#define   AZ_FRW_CMD_TINI       AZ_FRW_EVT_BASE|0x00000009
#define   AZ_FRW_EVT_IERR       AZ_FRW_EVT_BASE|0x0000000a

/* basic macros */
#define AZ_FRW_OPT_CFG_FOLDER       "C"
#define AZ_FRW_OPT_LOG_FOLDER       "L"
#define AZ_FRW_OPT_FRW_CFG_FILE     "f"
#define AZ_FRW_OPT_APP_CFG_FILE     "a"

#ifdef  CONFIG_AZ_DFT_FRW_CFG_FOLDER  
#define  AZ_FRW_CFG_FOLDER_DFT  CONFIG_AZ_DFT_FRW_CFG_FOLDER
#else
#define  AZ_FRW_CFG_FOLDER_DFT  "cfg"
#endif

#ifdef  CONFIG_AZ_DFT_FRW_LOG_FOLDER
#define  AZ_FRW_LOG_FOLDER_DFT  CONFIG_AZ_DFT_FRW_LOG_FOLDER 
#else
#define  AZ_FRW_LOG_FOLDER_DFT  "log"
#endif

#ifdef  CONFIG_AZ_DFT_FRW_CFG_FILE
#define  AZ_FRW_CFG_FILE_DFT CONFIG_AZ_DFT_FRW_CFG_FILE 
#else
#define  AZ_FRW_CFG_FILE_DFT    "az_frw_cfg.xml"
#endif

#ifdef  CONFIG_AZ_DFT_FRW_APP_FILE
#define  AZ_FRW_APP_FILE_DFT   CONFIG_AZ_DFT_FRW_APP_FILE 
#else
#define  AZ_FRW_APP_FILE_DFT    "az_app_cfg.xml"
#endif
/* basic types */

/* structures */

/* structured types */
typedef struct az_framework {
  char  cfg_folder[AZ_PATH_MAX];
  char  log_folder[AZ_PATH_MAX];
  char  frw_cfg_file[AZ_NAME_MAX];
  char  app_cfg_file[AZ_NAME_MAX];

  az_version_t  *frw_ver;  
  az_version_t  *app_ver;  
  char          *frw_build_time;
  char          *app_build_time;
} az_framework_t;

/* macros */

/* variabls exposed */
extern az_fsm_vector_t  az_frw_fsm_table[];
extern az_fsm_t az_frw_fsm;

/* inline functions */

/* function prototypes exposed */
extern az_r_t az_frw_parse_options(int argc, char *argv[]);
extern az_r_t az_frw_init();
extern az_r_t az_frw_deinit();
extern az_r_t az_frw_sendEvent(az_event_id_t event_id, az_uint32_t buffer_size, az_ref_t buffer_data); 
extern az_r_t az_frw_start_sync(); 

#ifdef __cplusplus
}
#endif
#endif
