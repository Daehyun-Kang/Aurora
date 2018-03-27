/**
 * @file   az_test.h
 * @brief  
 * @date   16/07/16
 * @author fware
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
 *   - 2016-07-16 fware initial create
 */

#ifndef AZ_TEST_H
#define AZ_TEST_H

#include "az_xcfg.h"
#include "frw/az_frw.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* constants */
#define AZ_TEST_DESCR_MAX           8
#define AZ_TC_PER_TP_MAX            2048
#define AZ_ITER_PER_TC_MAX          1024

/* basic macros */
#define AZ_TEST_OPT_CFG_FOLDER  AZ_FRW_OPT_CFG_FOLDER
#define AZ_TEST_OPT_LOG_FOLDER  AZ_FRW_OPT_LOG_FOLDER
#define AZ_TEST_OPT_FRW_CFG_FILE  "t" 

#define AZ_TEST_CFG_FOLDER_DFT  AZ_FRW_CFG_FOLDER_DFT
#define AZ_TEST_LOG_FOLDER_DFT  AZ_FRW_LOG_FOLDER_DFT
#define AZ_TEST_FRW_CFG_FILE_DFT  "az_test_frw.xml" 

#define AZ_TEST_CFG_FILE_DEFAULT  "az_test_cfg.xml"

#define AZ_TEST_FRW_CFG_TREE()  (&az_test_frw_cfg_tree)

/* basic types */

/* structures */
typedef az_xcfg_var_t az_test_var_t;

typedef az_xcfg_tree_t az_test_config_t;

typedef struct az_test_descr_t {
  int index;
  az_test_config_t *cfg;
  az_xml_element_t *xml;
  char  *name;
  char  *mode;
  int   onoff;
} az_test_descr_t;

typedef struct az_test_framework {
  char  cfg_folder[AZ_PATH_MAX];
  char  log_folder[AZ_PATH_MAX];

  char  frw_cfg_file[AZ_PATH_MAX];

  az_version_t *frw_ver;
  char  *frw_build_time;
} az_test_framework_t;

struct az_test_case; 

typedef az_r_t (*az_test_case_handler_t)(struct az_test_case *);
typedef az_test_case_handler_t az_testcase_handler_t;

typedef struct az_test_iter {
  int     index;
  az_bool_t   onoff;

  az_xml_element_t *xml;
  void    *test_vector;

  az_r_t  result;
  char    reason[AZ_PATH_MAX];
} az_test_iter_t;

typedef az_test_iter_t az_testiter_t;

typedef struct az_test_case {
  uint16_t   index;
  char  name[AZ_NAME_MAX];
  az_bool_t   onoff;

  az_xcfg_tree_t  *xcfg_tree;
  az_xml_element_t *xml;

  az_r_t      result;
  char        reason[AZ_PATH_MAX];

  void        *test_vector;
  int         test_iter_count;

  struct {
    int count;
    int index;
    int remained;
    int completed;
    int errored;
    az_test_iter_t *list; 
  } test_iter;

  struct {
    az_test_case_handler_t  Init; 
    az_test_case_handler_t  Sync; 
    az_test_case_handler_t  Prolog; 
    az_test_case_handler_t  Run; 
    az_test_case_handler_t  Epilog; 
    az_test_case_handler_t  Term; 
  } oprs;
} az_test_case_t;

#define AZ_TEST_CUR_ITER(tc) \
  (((tc)->test_iter.list == NULL)? NULL:((tc)->test_iter.list[(tc)->test_iter.index]))

typedef az_test_case_t  az_testcase_t;

typedef struct az_test_project {
  char  name[AZ_NAME_MAX];

  int   testcase_count;
  int   testcase_enabled;
  int   testcase_completed;
  int   testcase_errored;

  az_test_case_t  *testcaselist;
} az_test_project_t;

typedef az_test_project_t az_testproj_t;

/* structured types */

/* macros */
#define az_test_curTestCase() ((az_test_case_t *)(az_xu_self()->arg))
// define xu events
#define AZ_XU_EVENT_TEST_STARTED      0x00008000
#define AZ_XU_EVENT_TEST_STOPPED      0x00004000
#define AZ_XU_EVENT_TEST_ERROR        0x00002000

// define test fsm state
#define AZ_TEST_STATE_IDLE        0
#define AZ_TEST_STATE_INIT        1
#define AZ_TEST_STATE_IERR        2
#define AZ_TEST_STATE_SYNC        3
#define AZ_TEST_STATE_TINI        4
#define AZ_TEST_STATE_NORM        5

#define AZ_TEST_SUBSTATE_PRE      0
#define AZ_TEST_SUBSTATE_RUN      1
#define AZ_TEST_SUBSTATE_END      2

// define eventbus events
#ifdef  CONFIG_AZ_TEST_EVT_BASE
#define AZ_TEST_EVT_BASE          AZ_EVENT_ID_SYS_FLAG|CONFIG_AZ_TEST_EVT_BASE
#define AZ_TEST_EVT_MASK          CONFIG_AZ_TEST_EVT_MASK
#else
#define AZ_TEST_EVT_BASE          AZ_EVENT_ID_SYS_FLAG|0x20000000
#define AZ_TEST_EVT_MASK          ((az_event_id_t)0x0fffffff)
#endif
#define AZ_TEST_CMD_INIT          AZ_TEST_EVT_BASE|0x00000001
#define AZ_TEST_EVT_IERR          AZ_TEST_EVT_BASE|0x00000002
#define AZ_TEST_CMD_SYNC          AZ_TEST_EVT_BASE|0x00000003
#define AZ_TEST_CMD_STRT          AZ_TEST_EVT_BASE|0x00000004
#define AZ_TEST_EVT_TERM          AZ_TEST_EVT_BASE|0x00000005
#define AZ_TEST_CMD_STOP          AZ_TEST_EVT_BASE|0x00000006
#define AZ_TEST_CMD_TINI          AZ_TEST_EVT_BASE|0x00000007
#define AZ_TEST_CMD_PLOG          AZ_TEST_EVT_BASE|0x00000008
#define AZ_TEST_CMD_EXEC          AZ_TEST_EVT_BASE|0x00000009
#define AZ_TEST_CMD_ELOG          AZ_TEST_EVT_BASE|0x0000000A

#define AZ_TEST_CFG_KEY_testProj   "testProj"
#define AZ_TEST_CFG_KEY_testCase   "testCase"
#define AZ_TEST_CFG_KEY_testVector "testVector"
#define AZ_TEST_CFG_KEY_name       "name"
#define AZ_TEST_CFG_KEY_cfgfile    "cfgfile"
#define AZ_TEST_CFG_KEY_onoff      "onoff"
#define AZ_TEST_CFG_KEY_prototype  "prototype"
#define AZ_TEST_CFG_KEY_oprs       "oprs"
#define AZ_TEST_CFG_KEY_Init     "Init"
#define AZ_TEST_CFG_KEY_Sync     "Sync"
#define AZ_TEST_CFG_KEY_Prolog   "Prolog"
#define AZ_TEST_CFG_KEY_Run      "Run"
#define AZ_TEST_CFG_KEY_Epilog   "Epilog"
#define AZ_TEST_CFG_KEY_Term     "Term"
#define AZ_TEST_CFG_KEY_row        "row"
#define AZ_TEST_CFG_KEY_rowOnoff   "_onoff"
#define AZ_TEST_CFG_KEY_result     "result"
#define AZ_TEST_CFG_KEY_expected_result     "expected_result"

#define AZ_TEST_CFG_KEY_testDescr  "testDescr"
#define AZ_TEST_CFG_KEY_mode       "mode"
#define AZ_TEST_CFG_KEY_xu         "xu"
#define AZ_TEST_CFG_KEY_event      "event"
#define AZ_TEST_CFG_KEY_eventport      "eventport"
#define AZ_TEST_CFG_KEY_num      "num"
#define AZ_TEST_CFG_KEY_sig      "sig"
#define AZ_TEST_CFG_KEY_acc      "acc"
#define AZ_TEST_CFG_KEY_id      "id"
#define AZ_TEST_CFG_KEY_mask      "mask"

#define AZ_TEST_CFG_KEY_iou      "iou"
#define AZ_TEST_CFG_KEY_ioap      "ioap"
#define AZ_TEST_CFG_KEY_interface      "interface"
#define AZ_TEST_CFG_KEY_transport      "transport"
#define AZ_TEST_CFG_KEY_flags      "flags"
#define AZ_TEST_CFG_KEY_tx      "tx"
#define AZ_TEST_CFG_KEY_rx      "rx"
#define AZ_TEST_CFG_KEY_local      "local"
#define AZ_TEST_CFG_KEY_remote      "remote"
#define AZ_TEST_CFG_KEY_bufferSize      "bufferSize"
#define AZ_TEST_CFG_KEY_descrCount      "descrCount"
#define AZ_TEST_CFG_KEY_frameSize      "frameSize"

#define AZ_TEST_CFG_VAL_eventTest  "eventTest"
#define AZ_TEST_CFG_VAL_event_send  "event_send"
#define AZ_TEST_CFG_VAL_event_recv  "event_recv"

#define AZ_TEST_CFG_VAL_msgqTest  "msgqTest"
#define AZ_TEST_CFG_VAL_msgq_send  "msgq_send"
#define AZ_TEST_CFG_VAL_msgq_recv  "msgq_recv"

/* variabls exposed */
extern az_test_framework_t  az_test_frw;
extern az_xcfg_tree_t       az_test_frw_cfg_tree;

extern az_xu_t          az_tp_thread_default;

extern int              az_tc_thread_state;
extern az_xu_t          az_tc_thread_default;
extern az_fsm_vector_t  az_tc_fsm_table[];
extern az_fsm_t         az_tc_fsm;
/* inline functions */
static inline az_r_t az_test_load_config(char *file, az_test_config_t *cfg)
{
  return az_xcfg_load_config(file, cfg);
}

static inline az_r_t az_test_free_config(az_test_config_t *cfg)
{
  return az_xcfg_free_config(cfg);
}

static inline az_xml_element_t * az_test_find_element(az_test_config_t *cfg, 
    az_xml_element_t *parent, char *key, az_xml_element_t **next)
{
  return az_xcfg_find_element(cfg, parent, key, next);
}
static inline az_size_t az_test_var_toStr(az_test_var_t *pVar, char *bp, int blen)
{
  return az_xcfg_var_toStr(pVar, bp, blen);
}

static inline az_size_t az_test_parse_var(az_xml_element_t *attr, 
    az_test_var_t *pVar, az_size_t index)
{
  return az_xcfg_parse_var(attr, pVar, index); 
}

static inline az_size_t az_test_get_element(az_xml_element_t *cur, az_test_var_t *pVar)
{
  return az_xcfg_get_element(cur, pVar);
}

/* function prototypes exposed */
extern az_r_t az_test_sendEvent(az_event_id_t event_id, az_uint32_t buffer_size, az_ref_t buffer_data);
extern  void *az_tc_thread_proc_default(void *arg);
#ifdef __cplusplus
}
#endif
#endif
