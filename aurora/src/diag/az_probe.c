/**
 * @file   az_probe.c
 * @brief  
 * @date   19/03/18
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
 *   - 2018-03-19 dhkang initial create
 */

/* include header files */
#include "aurora.h"
#include "mon/az_probe.h"

/* declare global variables */
az_sys_timespec_t  az_probe_tstamp_base;
az_ring_t az_probe_samples;
#ifdef  CONFIG_AZ_PROBE_VAR_SAMPLE_BUFFER
az_probe_sample_t *az_probe_sample_buffer = NULL;
uint32_t  az_probe_samples_size = AZ_PROBE_SAMPLES_MAX;
#else
az_probe_sample_t az_probe_sample_buffer[AZ_PROBE_SAMPLES_MAX];
const uint32_t  az_probe_samples_size = AZ_PROBE_SAMPLES_MAX;
#endif

az_probe_ctrl_t az_probe_ctrl = {
  .state = AZ_PROBE_STATE_IDLE,
  .ctrl_sock = NULL,
  .data_sock = NULL,
  .last_sample = 0,
  .svrIpStr = "127.0.0.1",
  .svrPort = CONFIG_AZ_PROBE_SVR_PORT, 
};

/* declare static variables */
AZ_SYS_TLS uint8_t az_probe_level = 0;


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
void  az_probe_init()
{
  int r;
  az_probe_ctrl_t *ctrl = &az_probe_ctrl; 
  do {
    clock_gettime(AZ_PROBE_CLOCK_ID, &az_probe_tstamp_base);
    #ifdef  CONFIG_AZ_PROBE_VAR_SAMPLE_BUFFER
    az_probe_sample_buffer = (az_probe_sample_t *)az_malloc(az_probe_samples_size);
    #endif
    if (!az_probe_sample_buffer) break;
    az_ring_init(&az_probe_samples, AZ_RING_TYPE_DS64, az_probe_samples_size,
        az_probe_sample_buffer);

    ctrl->state = AZ_PROBE_STATE_READY|AZ_PROBE_STATE_DSND;
    ctrl->ctrl_sock = &ctrl->_ctrl_sock;
    AZ_SOCKET_INIT_STATIC(ctrl->ctrl_sock);

    r = az_inet_openTcpClient(ctrl->svrIpStr, ctrl->svrPort, NULL, 0,
        &ctrl->ctrl_sock);
    if (r != AZ_SUCCESS) {
      az_sys_eprintf("connect ctrl to probe server %s:%u error = %d" AZ_NL,
          ctrl->svrIpStr, ctrl->svrPort, r);
      break;
    }

    ctrl->data_sock = &ctrl->_data_sock;
    AZ_SOCKET_INIT_STATIC(ctrl->data_sock);
    r = az_inet_openTcpClient(ctrl->svrIpStr, ctrl->svrPort, NULL, 0,
        &ctrl->data_sock);
    if (r != AZ_SUCCESS) {
      az_sys_eprintf("connect data to probe server %s:%u error = %d" AZ_NL,
          ctrl->svrIpStr, ctrl->svrPort, r);
      break;
    }
    az_sys_eprintf("connect to probe server %s:%u OK, ctrl=%d, data=%d" AZ_NL,
          ctrl->svrIpStr, ctrl->svrPort, ctrl->ctrl_sock->sys_socket, ctrl->data_sock->sys_socket);

  } while (0);
}
void  az_probe_deinit()
{
  az_probe_ctrl_t *ctrl = &az_probe_ctrl; 
  do {
    if (!(ctrl->state & AZ_PROBE_STATE_INIT)) {
      break;
    }
    ctrl->state &= ~AZ_PROBE_STATE_INIT;
    if (ctrl->data_sock->sys_socket != AZ_SOCK_INVALID) {
      az_socket_delete(ctrl->data_sock->sys_socket);
      ctrl->data_sock->sys_socket = AZ_SOCK_INVALID;
    }
    if (ctrl->ctrl_sock->sys_socket != AZ_SOCK_INVALID) {
      az_socket_delete(ctrl->ctrl_sock->sys_socket);
      ctrl->ctrl_sock->sys_socket = AZ_SOCK_INVALID;
    }
    az_ring_deinit(&az_probe_samples);
    #ifdef  CONFIG_AZ_PROBE_VAR_SAMPLE_BUFFER
    if (az_probe_sample_buffer) {
      az_free(az_probe_sample_buffer);
      az_probe_sample_buffer = NULL;
    }
    #endif
    ctrl->state = AZ_PROBE_STATE_IDLE; 
  } while (0);

  az_sys_eprintf0("end\n");
  fflush(stdout);
}

static az_probe_sample_t az_probe_sendbuffer[sizeof(az_probe_sample_t) * AZ_PROBE_SAMPLE_SEND_MAX];

void az_probe_send()
{
  int j;
  ssize_t sent = 0;
  for (j = 0; j < AZ_PROBE_SAMPLE_SEND_MAX; j++) {
    if (AZ_PROBE_GET_SAMPLE(&az_probe_sendbuffer[j]) < 0) {
      break;
    }
  }
  if (j > 0 && az_probe_ctrl.data_sock->sys_socket != AZ_SOCK_INVALID) {
    sent = send(az_probe_ctrl.data_sock->sys_socket, az_probe_sendbuffer, j * sizeof(az_probe_sample_t), 0); 
  }
  if (sent <= 0) {
    az_sys_eprintf("sent %d = %d\n", j*sizeof(az_probe_sample_t), sent); 
  } else {
    //az_memdisp(STDOUT_FILENO, az_probe_sendbuffer, sent, sizeof(az_probe_sample_t), 0, STDIN_FILENO);
  }
}

void az_probe_proc()
{
  az_probe_ctrl_t *ctrl = &az_probe_ctrl; 
  az_sys_timespec_t tmo = {
    .tv_sec = 0,
    .tv_nsec = 1E8, // 100ms
  };
  ctrl->state &= ~AZ_PROBE_STATE_DSND;
  while (ctrl->state & AZ_PROBE_STATE_INIT) {
    AZ_PROBE_WAIT_SAMPLE(&tmo);
    do {
      az_probe_send();
    } while (az_probe_samples.count > 0);
  }
}

/* === end of AZ_PROBE_C === */
