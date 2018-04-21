/**
 * @file   az_frw.c
 * @brief  
 * @date   24/03/18
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
 *   - 2018-03-24 dhkang initial create
 */

/* include header files */
#include "aurora.h"
#include "az_ver.h"
#include "az_thread.h"
#include "az_options.h"
#include "frw/az_frw.h"
#include "cli/az_cli.h"

/* declare global variables */
az_framework_t  az_frw = {
  .cfg_folder = AZ_FRW_CFG_FOLDER_DFT, 
  .log_folder = AZ_FRW_LOG_FOLDER_DFT, 
  .frw_cfg_file = AZ_FRW_CFG_FILE_DFT, 
  .app_cfg_file = AZ_FRW_APP_FILE_DFT, 
  .frw_ver = &az_version,
  .app_ver = NULL, 
  .frw_build_time = NULL, //az_build_time, 
  .app_build_time = NULL, 
};

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
az_r_t az_frw_parse_options(int argc, char *argv[])
{
  az_r_t r = AZ_SUCCESS;

  az_kv_t opt_kv[4] = {
    {.key = AZ_FRW_OPT_CFG_FOLDER, .value = NULL},
    {.key = AZ_FRW_OPT_LOG_FOLDER, .value = NULL},
    {.key = AZ_FRW_OPT_FRW_CFG_FILE, .value = NULL},
    {.key = AZ_FRW_OPT_APP_CFG_FILE, .value = NULL},
  };

  az_frw.frw_build_time = az_build_time;

  do {
    r = (az_r_t)az_options_parse(argc, argv, opt_kv, 4);
    if (r < 0) break;

    az_kv_t *kv = opt_kv;
    az_framework_t  *frw = &az_frw;
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
    kv++;
    if (kv->value != NULL) {
      strncpy(frw->app_cfg_file, kv->value, sizeof(frw->app_cfg_file)-1); 
    }
  } while (0);

  return r;
}

az_r_t az_frw_init()
{
  az_r_t r = AZ_SUCCESS;
  char  *path[AZ_PATH_MAX];
  az_framework_t  *frw = &az_frw;

  do {
    az_fs_combinePath(frw->cfg_folder, frw->frw_cfg_file, path);
    r = az_frw_loadFrwConfig(path);
    az_sys_rprintf(r, "load framework config %s\n", path);
    if (r < 0) {
      break;
    }
    az_fs_combinePath(frw->cfg_folder, frw->app_cfg_file, path);
    r = az_frw_loadAppConfig(path);
    az_sys_rprintf(r, "load app config %s\n", path);
    if (r < 0) {
      //az_sys_eprintf("load app config %s err %d:%s\n", path, r, az_err_str(r));
      r = AZ_ERR(NO_APP_CFG);
    } 
  } while (0);

  return r;
}

void az_frw_setAppVer(az_version_t *pVer)
{
  az_frw.app_ver = pVer;
}

void az_frw_setAppBuildTime(char *pBuildTime)
{
  az_frw.app_build_time = pBuildTime;
}

az_r_t az_frw_deinit()
{
  az_r_t r = AZ_SUCCESS;

  do {
    r = az_frw_freeAppConfig();
    if (r < 0) {
      az_sys_eprintf("err %d:%s\n", r, az_err_str(r));
    }
    r = az_frw_freeFrwConfig();
    if (r < 0) {
      az_sys_eprintf("err %d:%s\n", r, az_err_str(r));
      break;
    }
  } while (0);

  return r;
}

az_event_descr_t *frw_evt_descr = NULL;
az_event_port_t frw_evt_tx_port = NULL;
az_event_port_t frw_evt_rx_port = NULL;
az_event_receiver_t frw_evt_receiver;
void *az_frw_main()
{
  az_r_t r;
  az_event_receiver_t *rcvr = &frw_evt_receiver;
  az_ring_t evtq;
  az_event_t  *evts[2];

  do {
    r = az_event_descr_create(&frw_evt_descr, AZ_FRW_EVT_BASE, 0, 2);  
    if (r < 0) {
      az_sys_rprintf(r, "event descr create fail for id=" AZ_FMT_EVENT_ID(10) AZ_NL,
        AZ_FRW_EVT_BASE);
      break;
    }
    az_sys_printf("mask=%x size=%d\n", AZ_FRW_EVT_MASK, AZ_EVENT_MASK_SIZE(AZ_FRW_EVT_MASK));
    r = az_event_port_create(AZ_FRW_EVT_BASE, AZ_FRW_EVT_MASK, AZ_EVENT_PORT_TYPE_TX, &frw_evt_tx_port);
    if (r < 0) {
      az_sys_rprintf(r, "event port create fail for id=" AZ_FMT_EVENT_ID(10) " mask=" AZ_FMT_EVENT_ID(10) " port=%p" AZ_NL, AZ_FRW_EVT_BASE, AZ_FRW_EVT_MASK, frw_evt_tx_port);
      break;
    }
    r = az_event_port_attach(frw_evt_tx_port);
    if (r < 0) {
      az_sys_rprintf(r, "%s" AZ_NL, "tx event port attach fail" );
      break;
    }
    frw_evt_descr->port = frw_evt_tx_port;

    r = az_event_port_create(AZ_FRW_EVT_BASE, AZ_FRW_EVT_MASK, AZ_EVENT_PORT_TYPE_RX, &frw_evt_rx_port);
    if (r < 0) {
      az_sys_rprintf(r, "event port create fail for id=" AZ_FMT_EVENT_ID(10) " mask=" AZ_FMT_EVENT_ID(10) " port=%p" AZ_NL, AZ_FRW_EVT_BASE, AZ_FRW_EVT_MASK, frw_evt_rx_port);
      break;
    }
    r = az_event_port_attach(frw_evt_rx_port);
    if (r < 0) {
      az_sys_rprintf(r, "%s" AZ_NL, "rx event port attach fail" );
      break;
    }
    az_ring_init(&evtq, AZ_RING_TYPE_DSREF, 2, evts);
    az_event_receiver_init(rcvr, NULL, NULL, &evtq);
    r = az_event_port_add_receiver(frw_evt_rx_port, rcvr);
    if (r < 0) {
      az_sys_rprintf(r, "%s" AZ_NL, "event port attach fail" );
      break;
    }
    az_thread_beam_t received;
    //az_thread_beam_t emask;
    az_int64_t  tmo_ns = 1000000000;

    extern az_fsm_t  az_frw_fsm; 
    az_fsm_activate(&az_frw_fsm, az_thread_self());

    az_event_t revt = NULL;
    r = az_frw_sendEvent(AZ_FRW_CMD_INIT, 0, NULL); 
    if (r < 0) {
      az_sys_rprintf(r, "%s" AZ_NL, "event send" );
      break;
    }
    uint32_t tmo_count = 0;
    az_sys_timespec_t ts;
    while (1) {
      received = 0;
      r = az_thread_recv_beam(AZ_THREAD_BEAM_EVTBUS, 0, tmo_ns, &received);  
      int j;
      for ( j = 0; j < 10000000; j++) {
        clock_gettime(CLOCK_REALTIME, &ts);
      }
      if (r >= 0 && (received & AZ_THREAD_BEAM_EVTBUS)) {
        r = az_event_recv(rcvr, &revt);
        if (r >= 0) {
          az_event_toStr(az_thread_prtbuf, sizeof(az_thread_prtbuf), revt);
          az_dprintf("xu recvent %p: %s\n", revt, az_thread_prtbuf); 
          az_fsm_run(&az_frw_fsm, revt);
        } else {
          az_rprintf(r, "xu recvent: %s\n", NULL); 
        }
        continue;
      }
      if (r == AZ_ERR(TIMEOUT)) {
        if ((++tmo_count % 30) == 0) {
          az_rprintf(r, "%s" AZ_NL, "xu recvevent" );
        }
        continue;
      }
      az_rprintf(r, "%s" AZ_NL, "xu recvevent" );
    }
  } while (0);


  return (void *)r;
}

az_r_t az_frw_sendEvent(az_event_id_t event_id, az_uint32_t buffer_size, az_ref_t buffer_data) {
  az_r_t r;

  az_event_t sevt = NULL;
  do {
    r = az_event_create(&sevt, frw_evt_descr->index, 0, NULL);
    if (r < 0) {
      az_sys_rprintf(r, "%s" AZ_NL, "event create fail" );
      break;
    }
    sevt->event_id = event_id;
    r = az_event_send(frw_evt_descr->port, sevt);
    if (r < 0) {
      az_sys_rprintf(r, "%s" AZ_NL, "event send" );
      break;
    }
  } while (0);

  return r;
}

az_r_t az_frw_start_sync()
{
  az_r_t r;
  do {
    r = az_frw_sendEvent(AZ_FRW_EVT_SYNC, 0, NULL); 
  } while (0);

  return r;
}

/* === end of AZ_FRW_C === */
