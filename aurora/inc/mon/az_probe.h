/**
 * @file   az_probe.h
 * @brief  
 * @date   29/07/16
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
 *   - 2016-07-29 fware initial create
 */

#ifndef AZ_PROBE_H
#define AZ_PROBE_H

#include "az_def.h"
#include "az_ring.h"
#include "az_prof.h"
#include "sys/az_xu.h"
#include "sys/az_inet.h"


#ifdef __cplusplus
extern "C"
{
#endif

/* constants */

/* basic macros */
#ifdef  CONFIG_AZ_PROBE_SAMPLES_MAX
#define AZ_PROBE_SAMPLES_MAX CONFIG_AZ_PROBE_SAMPLES_MAX
#else
#define AZ_PROBE_SAMPLES_MAX 409600 
#endif

#ifdef  CONFIG_AZ_PROBE_SAMPLE_SEND_MAX
#define  AZ_PROBE_SAMPLE_SEND_MAX CONFIG_AZ_PROBE_SAMPLE_SEND_MAX
#else
#define  AZ_PROBE_SAMPLE_SEND_MAX 180
#endif

#define AZ_PROBE_STATE_IDLE        0x00
#define AZ_PROBE_STATE_INIT        0x01
#define AZ_PROBE_STATE_ENAB        0x02
#define AZ_PROBE_STATE_DSND        0x04
#define AZ_PROBE_STATE_FULL        0x80

#define AZ_PROBE_STATE_READY       (AZ_PROBE_STATE_INIT|AZ_PROBE_STATE_ENAB) 
/* probe value structure src:level:tstamp = 16:4:44 */
#define AZ_PROBE_MK_SAMPLE(src, level, tstamp) \
  (((uint64_t)(src) << 48) + ((uint64_t)(level & 0x0f) << 44) + (uint64_t)(tstamp))
#define AZ_PROBE_SRC(val) ((val) >> 48) 
#define AZ_PROBE_LEVEL(val) (((val) >> 44) & 0x0f) 
#define AZ_PROBE_TSTAMP(val) ((val) & 0xfffffffffff) 

#define AZ_PROBE_CLOCK_ID CLOCK_REALTIME
/* basic types */
typedef uint64_t   az_probe_sample_t;

/* structures */
typedef struct az_probe_ctrl {
  uint8_t   state;
  az_sock_t ctrl_sock;
  az_sock_t data_sock;

  az_probe_sample_t last_sample;

  char  svrIpStr[CONFIG_AZ_NAME_MAX];
  uint16_t  svrPort;
} az_probe_ctrl_t;

/* structured types */

/* macros */

/* variabls exposed */
extern AZ_SYS_TLS uint8_t az_probe_level;
extern az_sys_timespec_t  az_probe_tstamp_base;
extern az_ring_t az_probe_samples;

#ifdef  CONFIG_AZ_PROBE_VAR_SAMPLE_BUFFER
extern uint32_t az_probe_samples_size;
#else
extern const uint32_t az_probe_samples_size;
#endif

extern az_probe_ctrl_t  az_probe_ctrl;

/* inline functions */
#ifdef  CONFIG_AZ_PROBE_ENABLE
#define AZ_PROBE_SET(level) \
  do {\
    az_xu_t _xu = az_xu_self();  \
    az_probe_sample_t _sample;\
    az_sys_timespec_t  _cur;\
    if ((AZ_PROBE_STATE_READY & az_probe_ctrl.state) != AZ_PROBE_STATE_READY) {\
      break;\
    }\
    clock_gettime(AZ_PROBE_CLOCK_ID, &_cur);\
    if (_xu) {\
      _sample = AZ_PROBE_MK_SAMPLE(_xu->ion.id, level, \
          az_prof_tdiff(_cur, az_probe_tstamp_base));\
    } else {\
      _sample = AZ_PROBE_MK_SAMPLE(0, level, \
          az_prof_tdiff(_cur, az_probe_tstamp_base));\
    }\
    if (az_probe_ctrl.state & AZ_PROBE_STATE_DSND){\
      if (az_probe_samples.count >= AZ_PROBE_SAMPLE_SEND_MAX) { \
          az_probe_send();\
          az_probe_ctrl.last_sample = _sample;\
      } else if ((AZ_PROBE_TSTAMP(_sample) - AZ_PROBE_TSTAMP(az_probe_ctrl.last_sample)) > 1E8) {\
        if (az_probe_samples.count > 0) { \
          az_probe_send();\
          az_probe_ctrl.last_sample = _sample;\
        }\
      }\
      az_ring_push64(&az_probe_samples, &_sample);\
      break;\
    }\
    if (az_ring_push64(&az_probe_samples, &_sample) < 0) {\
      az_probe_ctrl.state |= AZ_PROBE_STATE_FULL;\
    } else if (az_probe_samples.count == 1) {\
      futex((int *)&az_probe_samples.count, FUTEX_WAKE, 1, NULL, NULL, 0);\
    }\
  } while (0);
#else
#define AZ_PROBE_SET(level)
#endif

#define AZ_PROBE_INC()    AZ_PROBE_SET(++az_probe_level)
#define AZ_PROBE_DEC()    AZ_PROBE_SET(--az_probe_level)

#define AZ_PROBE_GET_SAMPLE(_pSample) \
          az_ring_pop64(&az_probe_samples, _pSample)

#define AZ_PROBE_WAIT_SAMPLE(_pTMO) \
          futex((int *)&az_probe_samples.count, FUTEX_WAIT, 0, _pTMO, NULL, 0)
          

/* function prototypes exposed */
extern void az_probe_init();
extern void az_probe_send();
extern void az_probe_proc();

#ifdef __cplusplus
}
#endif
#endif
