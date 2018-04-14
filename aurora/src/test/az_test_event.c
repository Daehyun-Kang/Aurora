/**
 * @file   az_test_event.c
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

#if 0
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

az_r_t az_test_get_event_id(az_test_descr_t *descr, az_event_id_t *p)
{
  az_r_t r = AZ_SUCCESS;
  az_int64_t eid = 0;
  az_event_id_st_t *eid_str = &eid;
  az_xml_element_t *el = NULL;
  az_test_var_t *eventParam, *tv;
  int eventParamCount, j;
  char temp[16];

  az_test_find_element(descr->cfg, descr->xml, AZ_TEST_CFG_KEY_event, &el); 
  if (NULL != el) {
    eventParam = az_malloc(el->attr_list.count * sizeof(*eventParam));
    eventParamCount = az_test_get_element(el, eventParam);
    for (j = 0, tv = eventParam; j < eventParamCount; j++, tv++) {
      temp[0] = 0;
      az_test_var_toStr(tv, temp, sizeof(temp));
      if (NULL == tv->xml) continue;
      az_printf("[%s] %d-the eventParam: type=%d key=%s value=%s\n", 
                  descr->name, j, tv->var.type, tv->xml->kv.key, temp); 
      if (strstr(tv->xml->kv.key, AZ_TEST_CFG_KEY_num)) {
        //eid = tv->var.value.b64;
        eid_str->num = tv->var.value.b64;
        //eid_str->num = 0x12345678; 
        az_printf("%lu %lu\n", eid, tv->var.value.b64);
        continue;
      }
      if (strstr(tv->xml->kv.key, AZ_TEST_CFG_KEY_sig)) {
        eid_str->sig = tv->var.value.b64;
        //eid_str->sig = 0xa; 
        az_printf("%lx %lx\n", eid, tv->var.value.b64);
        continue;
      }
      if (strstr(tv->xml->kv.key, AZ_TEST_CFG_KEY_acc)) {
        eid_str->acc = tv->var.value.b64;
        //eid_str->acc = 0; 
        az_printf("%lx %lx\n", eid, tv->var.value.b64);
        continue;
      }
    }
    az_free(eventParam);
  } else {
    r = AZ_ERR(NO_DATA);
  }
  az_printf("event_id:%0lx %ld %ld %ld\n", eid, eid_str->num, eid_str->sig, eid_str->acc);
  az_printf("event_id:%0lx %ld %ld %ld\n", eid, eid & 0xffffffffff, (eid >> 48) & 0x3f, (eid >> 55) & 0x1, (eid >> 56) & 0x1f); 
  *p = eid;
  return r;
}

az_r_t az_test_get_event_port_info(az_test_descr_t *descr, az_event_id_t *pId, az_event_id_t *pMask)
{
  az_r_t r = AZ_SUCCESS;
  az_int64_t eid = 0;
  az_int64_t emask = 0;
  az_xml_element_t *el = NULL;
  az_test_var_t *eventParam, *tv;
  int eventParamCount, j;
  char temp[32];
  int found = 0;

  az_test_find_element(descr->cfg, descr->xml, AZ_TEST_CFG_KEY_eventport, &el); 
  if (NULL != el) {
    eventParam = az_malloc(el->attr_list.count * sizeof(*eventParam));
    eventParamCount = az_test_get_element(el, eventParam);
    for (j = 0, tv = eventParam; j < eventParamCount; j++, tv++) {
      temp[0] = 0;
      az_test_var_toStr(tv, temp, sizeof(temp));
      if (NULL == tv->xml) continue;
      az_printf("[%s] %d-the eventParam: type=%d key=%s value=%s\n", 
                  descr->name, j, tv->var.type, tv->xml->kv.key, temp); 
      if (strstr(tv->xml->kv.key, AZ_TEST_CFG_KEY_id)) {
        eid = tv->var.value.b64;
        az_printf("eid:%lx\n", eid); 
        found |= 1;
        continue;
      }
      if (strstr(tv->xml->kv.key, AZ_TEST_CFG_KEY_mask)) {
        emask = tv->var.value.b64;
        az_printf("emask:%lx\n", emask); 
        found |= 2;
        continue;
      }
    }
    az_free(eventParam);
  } else {
    r = AZ_ERR(NO_DATA);
  }
  *pId = eid;
  *pMask = emask;
  return r;
}

az_r_t az_test_doSendEvent(az_test_descr_t *descr)
{
  az_r_t r = AZ_SUCCESS;
  char  *name = descr->name;
  az_event_id_t event_id;
  az_event_id_t eport_id, eport_mask;
  az_event_port_t *eport = NULL;

  az_printf("[%s] mode: %s\n", name, descr->mode);

  r = az_test_get_event_id(descr, &event_id);
  r = az_test_get_event_port_info(descr, &eport_id, &eport_mask);

  az_printf("eid=%lx\n", event_id);
  az_printf("eport id=%lx mask=%lx\n", eport_id, eport_mask);

  do {
    if (AZ_EVENT_ID_ACC(event_id)) {
      r = az_event_port_create(eport_id, eport_mask, AZ_EVENT_PORT_TYPE1, &eport);
    } else {
      r = az_event_port_create(eport_id, eport_mask, AZ_EVENT_PORT_TYPE0, &eport);
    }

    if (r == AZ_SUCCESS) {
      r = az_event_port_attach(eport);
      if (r) {
        az_printf("eport %d attach fail %s:%d\n", eport->no, az_err_str(r), r);
        break;
      }
      az_printf("eport %d create ok\n", eport->no);
    } else if (r == AZ_ERR(AGAIN)) {
      az_printf("eport %d already created\n", eport->no);
      r = AZ_SUCCESS;
    } else {
      az_printf("eport %d create fail %s:%d\n", eport->no, az_err_str(r), r);
      break;
    }

    az_event_t *event;
    az_event_sleep(2000000000);
    srand(NULL);
    int count = 0;
    int acc = 1;
    do {
      if (acc) {
        r = az_event_port_prepare(eport, event_id, 4, &event); 
      } else {
        r = az_event_port_prepare(eport, event_id+count, 4, &event); 
      }
      if (r < 0) {
        az_printf("eport %d prepare fail %s:%d\n", eport->no, az_err_str(r), r);
        break;
      }
      event->id = event->id + count;
      r = az_event_port_post(eport, event);
      if (r < 0) {
        az_printf("eport %d post fail %s:%d\n", eport->no, az_err_str(r), r);
        //break;
      }
      az_printf("%s: eport %d post %d %p(%x) ok\n", descr->name, eport->no, 
          count, event, event->id); 
      az_event_sleep(((rand()%100000) + 100000)*1000);
      count++;
      //break;
    } while (1);
  } while (0);

  return r; 
}

az_r_t az_test_doRecvEvent(az_test_descr_t *descr)
{
  az_r_t r = AZ_SUCCESS;
  char  *name = descr->name;
  az_event_id_t event_id;
  az_event_id_t eport_id, eport_mask;
  az_event_port_t *eport = NULL;

  az_printf("[%s] mode: %s\n", name, descr->mode);

  r = az_test_get_event_id(descr, &event_id);
  r = az_test_get_event_port_info(descr, &eport_id, &eport_mask);

  az_printf("eid=%lx\n", event_id);
  az_printf("eport id=%lx mask=%lx\n", eport_id, eport_mask);

  do {
    if (AZ_EVENT_ID_ACC(event_id)) {
      r = az_event_port_create(eport_id, eport_mask, AZ_EVENT_PORT_TYPE1, &eport);
    } else {
      r = az_event_port_create(eport_id, eport_mask, AZ_EVENT_PORT_TYPE0, &eport);
    }

    if (r == AZ_SUCCESS) {
      r = az_event_port_attach(eport);
      if (r) {
        az_printf("eport %d attach fail %s:%d\n", eport->no, az_err_str(r), r);
        break;
      }
      az_printf("eport %d create ok\n", eport->no);
    } else if (r == AZ_ERR(AGAIN)) {
      az_printf("eport %d already created\n", eport->no);
      r = AZ_SUCCESS;
    } else {
      az_printf("eport %d create fail %s:%d\n", eport->no, az_err_str(r), r);
      break;
    }
    az_event_port_mux_t rcv_port_mux;
    az_event_port_no_t  rcv_port_list[8];
    int                 rcv_port_status[8];

    az_event_receiver_t rcvr;
    az_event_ref_t	  acc_event_list[8];
    int				  acc_state_list[8];
    az_event_acc_t  event_acc;
    az_ring_t rxq;
    az_ref_t  elist[16];

    az_ring_init(&rxq, AZ_RING_TYPE_DSREF, 16, elist);
    az_event_acc_init(&event_acc, 8, acc_event_list, acc_state_list);
    az_event_port_mux_init(&rcv_port_mux, 8, rcv_port_list, rcv_port_status);

#ifdef  CONFIG_LINUX
    rcvr.sys_port = az_sys_xu_eport(); 
#endif
    rcvr.port_mux = &rcv_port_mux; 
    rcvr.event_acc = &event_acc;
    rcvr.event_q = &rxq;

    r = az_event_port_subscribe(eport, &rcvr);
    if (r != AZ_SUCCESS) {
      az_printf("eport %d subscribe fail %s:%d\n", eport->no, az_err_str(r), r);
      break;
    }
    az_printf("eport %d subscribe ok \n", eport->no);
      
    az_event_id_st_t *eid_str = &event_id;
    az_int64_t tmo_ns = 2000000000;
    az_event_event_t revent = 0; 
    int count = 0;
    az_event_t *event;
    int j;

    do {
      //r = az_event_recvEvent((1 << eid_str->sig), 0, tmo_ns, &revent); 
      //r = az_event_recvEvent(0xffffffff, 0, tmo_ns, &revent); 
      r = az_event_recv_wait(&rcvr, tmo_ns);
      if (r == AZ_ERR(TIMEOUT)) {
        az_printf("eport %d timeout %d \n", eport->no, count++);
        continue;
      }
      if (r < 0) {
       az_printf("eport %d recv fail %s:%d\n", eport->no, az_err_str(r), r);
        break;
      }
      for (j = 0; j < rcv_port_mux.size; j++) {
        if (rcv_port_mux.state_list[j] == 0) continue;
        rcv_port_mux.state_list[j]--;
        eport = az_event_port_get(rcv_port_mux.port_list[j]);
        r = az_event_port_fetch(eport, &rcvr, &event);
        if (r < 0) {
          az_printf("eport %d fetch fail %s:%d\n", eport->no, az_err_str(r), r);
          continue;
        }
        az_printf("%s: eport %d recv event %x %x(%x)\n", descr->name, eport->no, revent, event, event->id); 
        r = az_event_port_fetchComplete(eport, &rcvr, event);
        if (r < 0) {
          az_printf("eport %d fetch complete fail %s:%d\n", eport->no, az_err_str(r), r);
          continue;
        }
      }
    } while (1);
  } while (0);

  return r; 
}
#endif
az_r_t az_test_doSendEvent(az_test_descr_t *descr)
{
  az_r_t r = AZ_SUCCESS;

  return r;
}
az_r_t az_test_doRecvEvent(az_test_descr_t *descr)
{
  az_r_t r = AZ_SUCCESS;

  return r;
}

/* === end of AZ_TEST_EVENT_C === */
