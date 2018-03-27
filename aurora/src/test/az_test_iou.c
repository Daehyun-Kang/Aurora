/**
 * @file   az_test_iou.c
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
#include "az_iou_api.h"

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
az_r_t az_test_set_iou_config(az_test_descr_t *descr, 
    az_xml_element_t *parent, char *key, az_iou_config_t *iou_cfg, 
    az_r_t (*set_f)(az_iou_config_t *, az_test_var_t *),
    az_xml_element_t **pCur)
{
  az_r_t r = AZ_SUCCESS;
  az_xml_element_t *el;
  az_test_var_t *param, *tv;
  int paramCount, j;
  char temp[16];

  //iou_cfg->flags = 0;

  do {
    //find iou element by key
    if (NULL == parent) parent = descr->xml;
    el = NULL;
    az_test_find_element(descr->cfg, parent, key, &el); 
    if (NULL == el) {
      r = AZ_ERR(NOT_FOUND);
      break;
    }
    if (pCur) *pCur = el;
    param = az_malloc(el->attr_list.count * sizeof(*param));
    paramCount = az_test_get_element(el, param);
    for (j = 0, tv = param; j < paramCount; j++, tv++) {
      temp[0] = 0;
      az_test_var_toStr(tv, temp, sizeof(temp));
      if (NULL == tv->xml) continue;
      az_printf("[%s:%s] %d-the param: type=%d key=%s value=%s\n", 
                  descr->name, el->kv.key, j, tv->var.type, tv->xml->kv.key, temp); 

      if ((r = (set_f)(iou_cfg, tv)) < 0) {
        break;
      }
    }
    az_free(param);
  } while (0); 
  return r;
}

az_r_t az_test_set_iou_element(az_iou_config_t *iou_cfg, az_test_var_t *tv) 
{
  az_r_t r = AZ_SUCCESS;
  char *key;

  do {
      if (strstr(tv->xml->kv.key, (key = AZ_TEST_CFG_KEY_name))) {
        strncpy(iou_cfg->name, tv->var.value.s, sizeof(iou_cfg->name));
        az_printf("%s:%s\n", key, iou_cfg->name); 
        break;
      }
      if (strstr(tv->xml->kv.key, (key = AZ_TEST_CFG_KEY_ioap))) {
        iou_cfg->ioap = tv->var.value.u16;
        az_printf("%s:%u\n", key, iou_cfg->ioap); 
        break;
      }
      if (strstr(tv->xml->kv.key, (key = AZ_TEST_CFG_KEY_interface))) {
        if (!strcmp(tv->var.value.s, "MSGQ")) {
          iou_cfg->interfaceType = AZ_IOU_INTF_MSGQ; 
        }
        az_printf("%s: %s:%d\n", key, tv->var.value.s, iou_cfg->interfaceType);
        break;
      }
      if (strstr(tv->xml->kv.key, (key = AZ_TEST_CFG_KEY_transport))) {
        if (!strcmp(tv->var.value.s, "RAW")) {
          iou_cfg->transportType = AZ_IOU_TRNSP_RAW; 
        }
        az_printf("%s: %s:%d\n", key, tv->var.value.s, iou_cfg->transportType);
        break;
      }
      if (strstr(tv->xml->kv.key, (key = AZ_TEST_CFG_KEY_flags))) {
        if (strstr(tv->var.value.s, "RX")) {
          iou_cfg->flags |= AZ_IOU_FLAGS_RX;
        } 
        if (strstr(tv->var.value.s, "TX")) {
          iou_cfg->flags |= AZ_IOU_FLAGS_TX;
        }
        az_printf("%s: %s:%x\n", key, tv->var.value.s, iou_cfg->flags); 
        break;
      }
  } while (0);
  return r;
}

az_r_t az_test_set_iou_tx_element(az_iou_config_t *iou_cfg, az_test_var_t *tv) 
{
  az_r_t r = AZ_SUCCESS;
  char *key;

  do {
      if (strstr(tv->xml->kv.key, (key = AZ_TEST_CFG_KEY_bufferSize))) {
        iou_cfg->tx.buffer_size = tv->var.value.u64;
        iou_cfg->tx.block_size = 1; 
        az_printf("%s:%lu\n", key, iou_cfg->tx.buffer_size); 
        break;
      }
      if (strstr(tv->xml->kv.key, (key = AZ_TEST_CFG_KEY_descrCount))) {
        iou_cfg->tx.descr_count = tv->var.value.u16;
        iou_cfg->tx.descr_size = 4; 
        az_printf("%s:%u\n", key, iou_cfg->tx.descr_count); 
        break;
      }
      if (strstr(tv->xml->kv.key, (key = AZ_TEST_CFG_KEY_frameSize))) {
        iou_cfg->tx.frame_size = tv->var.value.u32;
        az_printf("%s:%u\n", key, iou_cfg->tx.frame_size); 
        break;
      }
  } while (0);
  return r;
}
az_r_t az_test_set_iou_rx_element(az_iou_config_t *iou_cfg, az_test_var_t *tv) 
{
  az_r_t r = AZ_SUCCESS;
  char *key;

  do {
      if (strstr(tv->xml->kv.key, (key = AZ_TEST_CFG_KEY_bufferSize))) {
        iou_cfg->rx.buffer_size = tv->var.value.u64;
        iou_cfg->rx.block_size = 1; 
        az_printf("%s:%lu\n", key, iou_cfg->rx.buffer_size); 
        break;
      }
      if (strstr(tv->xml->kv.key, (key = AZ_TEST_CFG_KEY_descrCount))) {
        iou_cfg->rx.descr_count = tv->var.value.u16;
        iou_cfg->rx.descr_size = 4; 
        az_printf("%s:%u\n", key, iou_cfg->rx.descr_count); 
        break;
      }
      if (strstr(tv->xml->kv.key, (key = AZ_TEST_CFG_KEY_frameSize))) {
        iou_cfg->rx.frame_size = tv->var.value.u32;
        az_printf("%s:%u\n", key, iou_cfg->rx.frame_size); 
        break;
      }
  } while (0);
  return r;
}
az_r_t az_test_set_iou_remote_element(az_iou_config_t *iou_cfg, az_test_var_t *tv) 
{
  az_r_t r = AZ_SUCCESS;
  char *key;

  do {
      if (strstr(tv->xml->kv.key, (key = AZ_TEST_CFG_KEY_name))) {
        strncpy(iou_cfg->remote_addr.name, tv->var.value.s,
            sizeof(iou_cfg->remote_addr.name)); 
        az_printf("%s:%s\n", key, iou_cfg->remote_addr.name); 
        break;
      }
  } while (0);
  return r;
}
az_r_t az_test_set_iou_local_element(az_iou_config_t *iou_cfg, az_test_var_t *tv) 
{
  az_r_t r = AZ_SUCCESS;
  char *key;

  do {
      if (strstr(tv->xml->kv.key, (key = AZ_TEST_CFG_KEY_name))) {
        strncpy(iou_cfg->local_addr.name, tv->var.value.s,
            sizeof(iou_cfg->local_addr.name)); 
        az_printf("%s:%s\n", key, iou_cfg->local_addr.name); 
        break;
      }
  } while (0);
  return r;
}

az_r_t az_test_load_iou_config(az_test_descr_t *descr, az_iou_config_t *iou_cfg){
  az_r_t r = AZ_SUCCESS;
  az_xml_element_t *el = NULL;
  //char temp[16];
  //char *key;

  iou_cfg->flags = 0;

  do {
    //find iou element
    r = az_test_set_iou_config(descr, NULL, AZ_TEST_CFG_KEY_iou, iou_cfg, 
        az_test_set_iou_element, &el);
    if (r < 0) break;

    // find iou tx element
    r = az_test_set_iou_config(descr, el, AZ_TEST_CFG_KEY_tx, iou_cfg,
        az_test_set_iou_tx_element, NULL);
    if (r == AZ_ERR(NOT_FOUND)) {
      if (iou_cfg->flags & AZ_IOU_FLAGS_TX) {
        az_printf("no tx config found for iou %s\n", iou_cfg->name);
        break;
      }
      r = AZ_SUCCESS;
    }

    // find iou rx element
    r = az_test_set_iou_config(descr, el, AZ_TEST_CFG_KEY_rx, iou_cfg,
        az_test_set_iou_rx_element, NULL);
    if (r == AZ_ERR(NOT_FOUND)) {
      if (iou_cfg->flags & AZ_IOU_FLAGS_RX) {
        az_printf("no rx config found for iou %s\n", iou_cfg->name);
        break;
      }
      r = AZ_SUCCESS;
    }

    // find iou remote element
    r = az_test_set_iou_config(descr, el, AZ_TEST_CFG_KEY_remote, iou_cfg,
        az_test_set_iou_remote_element, NULL);
    if (r == AZ_ERR(NOT_FOUND)) {
      if (iou_cfg->flags & AZ_IOU_FLAGS_TX) {
        az_printf("no remote config found for iou %s\n", iou_cfg->name);
        break;
      }
      r = AZ_SUCCESS;
    }

    // find iou local element
    r = az_test_set_iou_config(descr, el, AZ_TEST_CFG_KEY_local, iou_cfg,
        az_test_set_iou_local_element, NULL);
    if (r == AZ_ERR(NOT_FOUND)) {
      if (iou_cfg->flags & AZ_IOU_FLAGS_RX) {
        az_printf("no local config found for iou %s\n", iou_cfg->name);
        break;
      }
      r = AZ_SUCCESS;
    }
  } while (0); 
  return r;
}

az_r_t az_test_doRecvMsgq(az_test_descr_t *descr)
{
  az_r_t r = AZ_SUCCESS;
  char  *name = descr->name;
  az_iou_config_t iou_config;
  az_iou_id_t iou_id;

  az_printf("[%s] mode: %s\n", name, descr->mode);

  do {
    r = az_test_load_iou_config(descr, &iou_config);
    if (r < 0) {
      az_printf("%s: no iou recv config found\n", name);
      break;
    }

    r = az_iou_create(NULL, &iou_config, &iou_id); 
    if (r < 0) {
      az_printf("%s: iou create error="AZ_FMT_RET(1)":%s\n", name, r, az_err_str(r));
      break;
    }
    r = az_iou_open(iou_id, AZ_IOU_FLAGS_RX);
    if (r < 0) {
      az_printf("%s: iou open error="AZ_FMT_RET(1)":%s\n", name, r, az_err_str(r));
      break;
    }
  } while (0);

  return r; 
}

az_r_t az_test_doSendMsgq(az_test_descr_t *descr)
{
  az_r_t r = AZ_SUCCESS;
  char  *name = descr->name;
  az_iou_config_t iou_config;

  az_printf("[%s] mode: %s\n", name, descr->mode);

  do {
    r = az_test_load_iou_config(descr, &iou_config);
    if (r < 0) {
      az_printf("%s: no iou send config found\n", name);
      break;
    }
  } while (0);

  return r; 
}
#if 0
#endif


/* === end of AZ_TEST_IOU_C === */
