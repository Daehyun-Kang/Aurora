/**
 * @file   az_test_xu.c
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
#include "az_test.h"
#include "az_iou.h"

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
az_thread_t test_xu[AZ_TEST_DESCR_MAX];

/**
 * @fn        function name
 * @brief     function-description
 * @param     input-output-parameters
 * @return    return-value
 * @warning   warnings
 * @exception none
 */
#if 0
#endif

az_r_t az_test_run(az_test_descr_t *descr)
{
  az_r_t r = AZ_SUCCESS;

  do {
    if (!strcmp(descr->mode, AZ_TEST_CFG_VAL_event_recv)) {
      r = az_test_doRecvEvent(descr);
    } else
    if (!strcmp(descr->mode, AZ_TEST_CFG_VAL_event_send)) {
      r = az_test_doSendEvent(descr);
    } else
    if (!strcmp(descr->mode, AZ_TEST_CFG_VAL_msgq_send)) {
      r = az_test_doSendMsgq(descr);
    } else
    if (!strcmp(descr->mode, AZ_TEST_CFG_VAL_msgq_recv)) {
      r = az_test_doRecvMsgq(descr);
    }

  } while (0);
  return r; 
}

void *az_test_xu_entry(void *arg)
{
  //int count = 0;
  az_test_descr_t  *descr = (az_test_descr_t *)arg; 

  az_printf("%s %s start...\n", __FUNCTION__, descr->name);

  az_test_run(descr);

  return NULL;
}

az_test_config_t  az_test_cfg;

az_test_descr_t *az_test_loadTestDescr(az_test_config_t *pCfg, az_xml_element_t *pTestCase, int index)
{
  az_xml_element_t *parent = pTestCase, *child, *attr;
  az_test_descr_t *descr = NULL;
  do {
      az_test_find_element(pCfg, parent, AZ_TEST_CFG_KEY_testDescr, &child); 
      if (NULL == child) break;

      descr = az_malloc(sizeof(*descr));
      if (NULL == descr) break;
      descr->cfg = pCfg; 
      descr->xml = child;
      attr = az_xml_element_find_attr(child, AZ_TEST_CFG_KEY_name);
      if (NULL == attr) {
        descr->name = "name";
      } else {
        descr->name = attr->kv.value;
      }
      attr = az_xml_element_find_attr(child, AZ_TEST_CFG_KEY_mode);
      if (NULL == attr) {
        descr->mode = "default";
      } else {
        descr->mode = attr->kv.value; 
      }
      attr = az_xml_element_find_attr(child, AZ_TEST_CFG_KEY_onoff);
      if (NULL == attr) {
        descr->onoff = 1;
      } else {
        descr->onoff = atoi(attr->kv.value); 
      }
      descr->index = index;
  } while (0);

  return descr;
}

int az_test_doTestCase(az_test_config_t *pCfg, az_xml_element_t *pTestCase, char *name)
{
  az_r_t r = AZ_ERR(NOT_FOUND);
  az_test_descr_t *descr;
  int index;

  do {
      descr = az_test_loadTestDescr(pCfg, pTestCase, index);
      if (NULL == descr) break;
      if (!descr->onoff) {
        az_printf("test descr %s off\n", descr->name); 
        az_free(descr);
        continue;
      }
      r = az_thread_create(descr->name, az_test_xu_entry, descr, NULL, &test_xu[index]);
      index++;
  } while (index < AZ_TEST_DESCR_MAX);

  return (int)r;
}

int az_test_xu(int argc, char *argv[])
{
  az_r_t r;
  az_test_config_t  *pCfg = &az_test_cfg;
  az_xml_element_t *parent, *child, *attr;
  //az_test_descr_t *descr;
  //int index;

  do {
    if (argc < 2) {
      az_printf("%s", "testCase name missing\n");
      r =  AZ_ERR(INVALID_ARG);
      break;
    }
    r = az_test_load_config(AZ_TEST_CFG_FILE_DEFAULT, pCfg);
    if (AZ_SUCCESS != r) {
      az_printf("cfg load error=%s\n", az_err_str(r));
      break;
    }
    parent = child = NULL;
    az_test_find_element(pCfg, parent, AZ_TEST_CFG_KEY_testProj, &child);  
    if (NULL == child) {
      az_printf("%s", "no test proj defined\n");
      break;
    }
    attr = az_xml_element_find_attr(child, AZ_TEST_CFG_KEY_name);
    if (NULL == attr) {
      az_printf("%s", "test proj name not defined\n");
      break;
    }
    az_printf("test proj %s found\n", attr->kv.value);
    parent = child;
    child = NULL;
    do {
      az_test_find_element(pCfg, parent, AZ_TEST_CFG_KEY_testCase, &child);  
      if (NULL == child) {
        break;
      }
      attr = az_xml_element_find_attr(child, AZ_TEST_CFG_KEY_name);
      if (NULL == attr) {
        continue;
      }
      if (!strcmp(argv[1], attr->kv.value)) {
        az_printf("test case %s found\n", attr->kv.value);
        break;
      }
    } while (1);
    if (NULL == child) {
      r = AZ_ERR(EMPTY);
      break;
    }

    r = az_test_doTestCase(pCfg, child, attr->kv.value);

  } while (0);

  return (int)r;
}

/* === end of AZ_TEST_XU_C === */
