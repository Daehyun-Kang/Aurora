/**
 * @file   az_xu_cfg.c
 * @brief  
 * @date   11/11/17
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
 *   - 2017-11-11 dhkang initial create
 */

/* include header files */
#include  "az_xu_cfg.h"

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

/**
 * @fn        function name
 * @brief     function-description
 * @param     input-output-parameters
 * @return    return-value
 * @warning   warnings
 * @exception none
 */

az_xml_element_t *az_xu_cfg_getlist(int cfg_cat)
{
  az_xml_element_t *list;
  az_xml_element_t *next = NULL;
  char  *key = AZ_XU_CFG_KEY_xulist;
  az_xcfg_tree_t  *tree = AZ_XU_CFG_TREE(cfg_cat);

  list = az_xcfg_find_element(tree, NULL, key, &next);

  return list;
}

az_xml_element_t *az_xu_cfg_getfirst(int cfg_cat)
{
  az_xml_element_t *parent = NULL;
  az_xml_element_t *next = NULL;
  az_xml_element_t *cfg = NULL;
  char  *key = AZ_XU_CFG_KEY_xu;
  az_xcfg_tree_t  *tree = AZ_XU_CFG_TREE(cfg_cat);

  do {
    parent = az_xu_cfg_getlist(cfg_cat);
    if (NULL == parent) break;
    cfg = az_xcfg_find_element(tree, parent, key, &next);
  } while (0);

  return cfg;
}

az_xml_element_t *az_xu_cfg_getnext(int cfg_cat, az_xml_element_t *cur)
{
  az_xml_element_t *parent = NULL;
  az_xml_element_t *next = cur;
  az_xml_element_t *cfg = NULL;
  char  *key = AZ_XU_CFG_KEY_xu;
  az_xcfg_tree_t  *tree = AZ_XU_CFG_TREE(cfg_cat);

  do {
    if (NULL == next) break;
    parent = next->parent;
    cfg = az_xcfg_find_element(tree, parent, key, &next);
  } while (0);

  return cfg;
}
az_r_t az_xu_cfg_convPolicy(az_xml_element_t *attr, void *pVar, int varlen)
{
  az_assert(NULL != attr);
  az_assert(NULL != pVar);
  az_r_t r = AZ_SUCCESS;
  az_assert(varlen == sizeof(az_xu_policy_t));
  if (!strcmp(attr->kv.value, AZ_XU_CFG_VAL_policy_SCHED_RR)) {
    *(az_xu_policy_t *)pVar = SCHED_RR;
  } else
  if (!strcmp(attr->kv.value, AZ_XU_CFG_VAL_policy_SCHED_FIFO)) {
    *(az_xu_policy_t *)pVar = SCHED_FIFO;
  } else {
    *(az_xu_policy_t *)pVar = SCHED_OTHER;
  }
  return r;
}

az_r_t az_xu_cfg_xml2cfg(az_xml_element_t *elem, az_xu_config_t *cfg)
{
  az_xml_element_t *child;
  az_r_t  r;

  az_assert(NULL != cfg);
  do {
    r = az_xcfg_get_value(elem, AZ_XU_CFG_KEY_name, cfg->name, sizeof(cfg->name), NULL);
    if (r < 0) break;
    r = az_xcfg_get_value(elem, AZ_XU_CFG_KEY_entry_name, cfg->entry_name, sizeof(cfg->entry_name), NULL);
    if (r < 0) break;
    r = az_xcfg_get_value(elem, AZ_XU_CFG_KEY_arg_name, cfg->arg_name, sizeof(cfg->arg_name), NULL);
    if (r < 0) break;
    r = az_xcfg_get_value(elem, AZ_XU_CFG_KEY_stackSize, &cfg->stackSize, sizeof(cfg->stackSize), NULL);
    if (r < 0) break;

    child = az_xml_element_find_child(elem, AZ_XU_CFG_KEY_schedule);
    if (NULL == child) {
      r = AZ_ERR(NOT_FOUND);
      break;
    }
    r = az_xcfg_get_value(child, AZ_XU_CFG_KEY_coremask, &cfg->coremask, sizeof(cfg->coremask), NULL);
    if (r < 0) break;
    r = az_xcfg_get_value(child, AZ_XU_CFG_KEY_policy, &cfg->policy, sizeof(cfg->policy), az_xu_cfg_convPolicy);
    if (r < 0) break;
    r = az_xcfg_get_value(child, AZ_XU_CFG_KEY_priority, &cfg->priority, sizeof(cfg->priority), NULL);
    if (r < 0) break;
  } while (0);

  return r;
}

/* === end of AZ_XU_CFG_C === */
