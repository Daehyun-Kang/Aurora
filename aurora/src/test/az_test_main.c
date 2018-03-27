/**
 * @file   az_test_main.c
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
#include "cli/az_cli.h"
#include "frw/az_frw_cmd.h"
#include "az_ver.h"
#include "mon/az_debug.h"
#include "az_test.h"

extern int az_test_xu(int argc, char *argv[]);
extern void az_frw_cmd_init();

/* declare global variables */
az_test_framework_t az_test_frw = {
  .cfg_folder = AZ_TEST_CFG_FOLDER_DFT,
  .log_folder = AZ_TEST_LOG_FOLDER_DFT,
  .frw_cfg_file = AZ_TEST_FRW_CFG_FILE_DFT,
  .frw_ver = &az_version,
  .frw_build_time = NULL
};
az_xcfg_tree_t      az_test_frw_cfg_tree;

az_xu_t az_tp_thread_default = NULL;

az_test_project_t az_testproj;
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
az_r_t az_test_parse_options(int argc, char *argv[])
{
  az_r_t r = AZ_SUCCESS;

  az_kv_t opt_kv[3] = {
    {.key = AZ_TEST_OPT_CFG_FOLDER, .value = NULL},
    {.key = AZ_TEST_OPT_LOG_FOLDER, .value = NULL},
    {.key = AZ_TEST_OPT_FRW_CFG_FILE, .value = NULL},
  };

  az_test_frw.frw_build_time = az_build_time;

  do {
    r = (az_r_t)az_options_parse(argc, argv, opt_kv, 4);
    if (r < 0) break;

    az_kv_t *kv = opt_kv;
    az_test_framework_t  *frw = &az_test_frw;
    if (kv->value != NULL) {
      strncpy(frw->cfg_folder, kv->value, sizeof(frw->cfg_folder)-1); 
    }
    kv++;
    if (kv->value != NULL) {
      strncpy(frw->log_folder, kv->value, sizeof(frw->log_folder)-1); 
    }
    kv++;
    if (kv->value != NULL) {
      strncpy(frw->frw_cfg_file, kv->value, sizeof(frw->frw_cfg_file)-1); 
    }
  } while (0);

  return r;
}
az_r_t az_test_loadTestConfig(char *filename, az_xcfg_tree_t *cfg)
{
  az_r_t r = AZ_SUCCESS;
  char  path[AZ_PATH_MAX];
  az_test_framework_t  *frw = &az_test_frw;

  do {
    az_fs_combinePath(frw->cfg_folder, filename, path);
    r = az_xcfg_load_config(path, cfg); 
    az_rprintf(r, "load test config %s\n", path);
  } while (0);

  return r;
}

az_r_t az_test_frw_init()
{
  az_r_t r = AZ_SUCCESS;
  char  *path[AZ_PATH_MAX];
  az_test_framework_t  *frw = &az_test_frw;

  do {
    r = az_test_loadTestConfig(frw->frw_cfg_file, AZ_TEST_FRW_CFG_TREE()); 
  } while (0);

  return r;
}

az_r_t az_test_loadTestCaseOprs(az_test_case_t *tc) 
{
  az_r_t r = AZ_SUCCESS;
  az_xml_element_t *oprs_xe = NULL;
  az_xml_element_t *xe = NULL;
  
  do {
    oprs_xe = az_xml_element_find_child(tc->xml, AZ_TEST_CFG_KEY_oprs);
    if (oprs_xe == NULL) {
      r = AZ_ERR(NOT_FOUND);
      break;
    }
    xe = az_xml_element_find_child(oprs_xe, AZ_TEST_CFG_KEY_Run);
    if (xe) {
      xe = az_xml_element_find_attr(xe, AZ_TEST_CFG_KEY_name);
      if (xe) {
        r = az_dllib_getSymbol(xe->kv.value, (az_ref_t *)&(tc->oprs.Run));
      } else {
        tc->oprs.Run = NULL;
        r = AZ_ERR(INVALID);
        break;
      }
    } else {
      tc->oprs.Run = NULL;
      r = AZ_ERR(INVALID);
      break;
    }

    xe = az_xml_element_find_child(oprs_xe, AZ_TEST_CFG_KEY_Init);
    if (xe) {
      xe = az_xml_element_find_attr(xe, AZ_TEST_CFG_KEY_name);
      if (xe) {
        r = az_dllib_getSymbol(xe->kv.value, (az_ref_t *)&(tc->oprs.Init));
      } else {
        tc->oprs.Init = NULL;
      }
    } else {
      tc->oprs.Init = NULL;
    }
    xe = az_xml_element_find_child(oprs_xe, AZ_TEST_CFG_KEY_Sync);
    if (xe) {
      xe = az_xml_element_find_attr(xe, AZ_TEST_CFG_KEY_name);
      if (xe) {
        r = az_dllib_getSymbol(xe->kv.value, (az_ref_t *)&(tc->oprs.Sync));
      } else {
        tc->oprs.Sync = NULL;
      }
    } else {
      tc->oprs.Sync = NULL;
    }
    xe = az_xml_element_find_child(oprs_xe, AZ_TEST_CFG_KEY_Prolog);
    if (xe) {
      xe = az_xml_element_find_attr(xe, AZ_TEST_CFG_KEY_name);
      if (xe) {
        r = az_dllib_getSymbol(xe->kv.value, (az_ref_t *)&(tc->oprs.Prolog));
      } else {
        tc->oprs.Prolog = NULL;
      }
    } else {
      tc->oprs.Prolog = NULL;
    }
    xe = az_xml_element_find_child(oprs_xe, AZ_TEST_CFG_KEY_Epilog);
    if (xe) {
      xe = az_xml_element_find_attr(xe, AZ_TEST_CFG_KEY_name);
      if (xe) {
        r = az_dllib_getSymbol(xe->kv.value, (az_ref_t *)&(tc->oprs.Epilog));
      } else {
        tc->oprs.Epilog = NULL;
      }
    } else {
      tc->oprs.Epilog = NULL;
    }
    xe = az_xml_element_find_child(oprs_xe, AZ_TEST_CFG_KEY_Term);
    if (xe) {
      xe = az_xml_element_find_attr(xe, AZ_TEST_CFG_KEY_name);
      if (xe) {
        r = az_dllib_getSymbol(xe->kv.value, (az_ref_t *)&(tc->oprs.Term));
      } else {
        tc->oprs.Epilog = NULL;
      }
    } else {
      tc->oprs.Term = NULL;
    }
  } while (0);

  return r;
}

az_r_t az_test_loadTestIters(az_test_case_t *tc) 
{
  az_assert(NULL != tc);
  az_r_t r = AZ_SUCCESS;
  az_xml_element_t *tv_xe = NULL;
  az_xml_element_t *xe = NULL;
  az_xml_element_t *xe_attr = NULL;
  int test_iter_count = 0;
  az_xml_element_t *s_xml_iters[AZ_ITER_PER_TC_MAX];
  
  do {
    tv_xe = az_xml_element_find_child(tc->xml, AZ_TEST_CFG_KEY_testVector);
    if (tv_xe == NULL) {
      break;
    }
    do {
      xe = az_xml_element_find_child(tv_xe, AZ_TEST_CFG_KEY_row);
      xe = az_xml_element_next_child(tv_xe, xe); 
      if (xe == NULL) break;
      if (strcmp(xe->kv.key, AZ_TEST_CFG_KEY_row)) {
        continue;
      }
      xe_attr = az_xml_element_find_attr(xe, AZ_TEST_CFG_KEY_rowOnoff);
      if (xe_attr) {
        if (strtol(xe_attr->kv.value, NULL, 10) == 0) {
          az_xml_element_remove_child(xe);
          continue;
        }
      }
      s_xml_iters[test_iter_count++] = xe;
      if (test_iter_count == AZ_ITER_PER_TC_MAX) break;
    } while (1);
  } while (0);

  tc->test_iter_count = test_iter_count;
  if (tc->test_iter_count == 0) {
    tc->test_iter.list = NULL;
  } else {
    int j;
    az_test_iter_t *iter = az_malloc(sizeof(az_test_iter_t) * test_iter_count);
    if (iter == NULL) {
      tc->test_iter_count = 0;
      tc->test_iter.list = NULL;
      r = AZ_ERR(MALLOC);
    } else {
      tc->test_iter.list = iter;
      for (j = 0; j < test_iter_count; j++, iter++) {
        iter->index = 0;
        iter->onoff = 1;
        iter->xml = s_xml_iters[j];
        iter->test_vector = NULL;
        iter->result = 0;
        iter->reason[0] = 0;
      }
    }
  }

  return r;
}

void az_test_unloadTestIters(az_test_case_t *tc) 
{
  az_assert(NULL != tc);
  if (tc->test_iter.list) {
    az_free(tc->test_iter.list);
    tc->test_iter.list = NULL;
    tc->test_iter_count = 0;
  }
}

//extern az_r_t az_ut_init_abc(az_test_case_t *pTC);
int az_test_main(int argc, char *argv[])
{
  az_r_t r;
  az_int64_t  tmo_ns = 2000000000;
  az_xu_event_t received;

  az_tp_thread_default = az_xu_self();

  az_version_t *ver = &az_version;

  az_version.major = 1;

  r = az_test_parse_options(argc, argv);
  if (r < 0) {
    exit(r);
  }

  r = az_test_frw_init();
  if (r < 0) {
    exit(r);
  }
  
  az_frw_cmd_init();

  #ifdef CONFIG_AZ_LOG
  r = az_log_start();
  if (r != AZ_SUCCESS) {
    exit(r);
  }
  #endif

  #ifdef CONFIG_AZ_CLI
  az_cli_shell_start(&az_cli_shell_default);
  r = az_cli_svr_start();
  az_sys_printf("default cli shell=%p/%p\n", 
      &az_cli_shell_default, az_cli_shell_default.thread);
  #endif

  az_xml_element_t *xe_testproj = NULL;
  az_xml_element_t *xe_testcase = NULL;
  az_xml_element_t *xe = NULL;
  az_xcfg_find_element(
    AZ_TEST_FRW_CFG_TREE(), NULL, AZ_TEST_CFG_KEY_testProj, &xe_testproj);

  if (NULL == xe_testproj) {
    az_loge("test project config not found on %s\n", az_test_frw.frw_cfg_file);
    exit(r);
  }
  xe = az_xml_element_find_attr(xe_testproj, AZ_TEST_CFG_KEY_name);
  az_logi("test project::%s config found on file::%s\n", 
      xe->kv.value, az_test_frw.frw_cfg_file);

  strncpy(az_testproj.name, xe->kv.value, sizeof(az_testproj.name));

  az_xcfg_tree_t  xcfg_tree_testcase;
  az_testproj.testcase_count = 0;
  az_testproj.testcase_enabled = 0;
  az_testproj.testcase_completed = 0;
  az_testproj.testcase_errored = 0;
  az_testproj.testcaselist = az_malloc(sizeof(az_test_case_t) * AZ_TC_PER_TP_MAX);
  az_test_case_t *tc = az_testproj.testcaselist;
  do {
    az_xcfg_find_element( AZ_TEST_FRW_CFG_TREE(), xe_testproj, AZ_TEST_CFG_KEY_testCase, &xe_testcase);
    if (xe_testcase == NULL) {
      break;
    }
    memset(tc, 0, sizeof(*tc));
    tc->index = az_testproj.testcase_count++;
    tc->oprs.Init = tc->oprs.Sync = tc->oprs.Run = tc->oprs.Term = NULL; 

    xe = az_xml_element_find_attr(xe_testcase, AZ_TEST_CFG_KEY_name);
    if (xe) {
      strncpy(tc->name, xe->kv.value, sizeof(tc->name));
    }

    r = az_xcfg_get_value(xe_testcase, AZ_TEST_CFG_KEY_onoff ".bool", &tc->onoff, sizeof(tc->onoff), NULL); 
    az_logi("test case::%s onoff=%d found on test project::%s\n", 
        tc->name, tc->onoff, az_testproj.name);

    if (tc->onoff == AZ_FALSE) {
      tc++;
      continue;
    }

    az_testproj.testcase_enabled++;
    xe = az_xml_element_find_attr(xe_testcase, AZ_TEST_CFG_KEY_cfgfile);

    r = az_test_loadTestConfig(xe->kv.value, &xcfg_tree_testcase);
    if (r < 0) {
      az_rprintf(r, "loading testcase::%s config file::%s\n", 
          tc->name, xe->kv.value);
      tc++;
      tc->result = r;
      snprintf(tc->reason, sizeof(tc->reason), "config file loading fail") ;
      continue;
    } 
    tc->xcfg_tree = &xcfg_tree_testcase;
    az_logd("%s %s\n", tc->xcfg_tree->xml.kv.key, tc->xcfg_tree->xml.kv.value);
    tc->xml = NULL;
    az_xcfg_find_element(tc->xcfg_tree, NULL, AZ_TEST_CFG_KEY_testCase, &tc->xml);
    if (tc->xml == NULL) {
      tc++;
      tc->result = AZ_ERR(NOT_FOUND);
      snprintf(tc->reason, sizeof(tc->reason), "test case config not found") ;
      break;
    }

    xe = az_xml_element_find_attr(tc->xml, AZ_TEST_CFG_KEY_name);
    if (xe) {
      strncpy(tc->name, xe->kv.value, sizeof(tc->name));
    }
    r = az_test_loadTestCaseOprs(tc);
    if (r < 0) {
      az_rprintf(r, "%d testcase:%s config oprs not missing\n", tc->index, tc->name); 
      tc++;
      tc->result = r; 
      snprintf(tc->reason, sizeof(tc->reason), "test case oprs invalid") ;
      continue;
    }
    r = az_test_loadTestIters(tc); 
    if (r < 0) {
      az_rprintf(r, "%d testcase:%s  test iterations missing\n", tc->index, tc->name); 
      tc++;
      tc->result = r; 
      snprintf(tc->reason, sizeof(tc->reason), "test iterations invalid") ;
      continue;
    }

    az_tc_thread_default = NULL;
    r = az_xu_create("tcRunner", az_tc_thread_proc_default, tc, NULL, &az_tc_thread_default);
    
    while (1) {
      received = 0;
      r = az_xu_recvEvent(0xffff, 0, tmo_ns, &received);  
      if (r < 0) {
        if (r == AZ_ERR(TIMEOUT)) {
        }
        continue;
      }
      if (received & AZ_XU_EVENT_TEST_STARTED) {
        az_logi("test case::%s started\n", tc->name); 
      }
      if (received & AZ_XU_EVENT_TEST_STOPPED) {
        az_logi("test case::%s stopped\n", tc->name); 
        az_testproj.testcase_completed++;
        break;
      }
      if (received & AZ_XU_EVENT_TEST_ERROR) {
        az_logi("test case::%s errored\n", tc->name); 
        az_testproj.testcase_errored++;
        break;
      }
    }

    az_xcfg_free_config(&xcfg_tree_testcase);
    tc->xcfg_tree = NULL; 
    tc++;
  } while (1);

  az_logi("testproj::%s all testcase::total=%d,enabled=%d,completed=%d,errored=%d\n", 
      az_testproj.name,
      az_testproj.testcase_count,
      az_testproj.testcase_enabled,
      az_testproj.testcase_completed,
      az_testproj.testcase_errored);
  // here we need to release all the resouce
  int j;
  tc = az_testproj.testcaselist;
  for (j = 0; j < az_testproj.testcase_count; j++, tc++) {
    az_test_unloadTestIters(tc); 
  }
  az_free(az_testproj.testcaselist);

  return r;
}

/* === end of AZ_TEST_MAIN_C === */
