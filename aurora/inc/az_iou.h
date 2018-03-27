/**
 * @file   az_iou.h
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

#ifndef AZ_IOU_H
#define AZ_IOU_H
#include  "az_def.h"
#include  "az_err.h"
#include  "az_macros.h"
#include  "az_malloc.h"
#include  "az_ion.h"
#include  "az_event.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* constants */
#define AZ_IOU_NAME_LEN         CONFIG_AZ_NAME_MAX
#define AZ_IOU_ADDR_NAME_LEN    24

/* define IOU interface type */
#define AZ_IOU_INTF_MSGQ         0
#define AZ_IOU_INTF_INET         1
#define AZ_IOU_INTF_SERIAL       2

/* define IOU transport type */
#define AZ_IOU_TRNSP_RAW          0
#define AZ_IOU_TRNSP_MSG          1
#define AZ_IOU_TRNSP_UDP          2
#define AZ_IOU_TRNSP_TCP          3

/* define IOU flags */ 
#define AZ_IOU_FLAGS_TX           0x0001
#define AZ_IOU_FLAGS_RX           0x0002
#define AZ_IOU_FLAGS_BUF_TX       0x0004
#define AZ_IOU_FLAGS_BUF_RX       0x0008
#define AZ_IOU_FLAGS_FRAMED       0x0010
#define AZ_IOU_FLAGS_CRC          0x0020

#define AZ_IOU_FLAGS_TRX  (AZ_IOU_FLAGS_TX|AZ_IOU_FLAGS_RX)         

#define AZ_IOU_FLAGS_DUMP         0x4000
#define AZ_IOU_FLAGS_TRACE        0x8000

/* define IOU state */
#define AZ_IOU_STATE_ASSIGNED     0x1000
#define AZ_IOU_STATE_CRFAILED     0x8000
#define AZ_IOU_STATE_TX_UP        0x0001
#define AZ_IOU_STATE_RX_UP        0x0002
#define AZ_IOU_STATE_TX_OPEN      0x0004
#define AZ_IOU_STATE_RX_OPEN      0x0008
#define AZ_IOU_STATE_TX_BLK       0x0010
#define AZ_IOU_STATE_RX_BLK       0x0020
#define AZ_IOU_STATE_SND_BLK      0x0040
#define AZ_IOU_STATE_RCV_BLK      0x0080
#define AZ_IOU_STATE_TX_PEND      0x0100
#define AZ_IOU_STATE_RX_FULL      0x0200

#define AZ_IOU_STATE_OPEN (AZ_IOU_STATE_TX_OPEN|AZ_IOU_STATE_RX_OPEN)
#define AZ_IOU_STATE_UP (AZ_IOU_STATE_TX_UP|AZ_IOU_STATE_RX_UP)
#define AZ_IOU_STATE_BUSY (AZ_IOU_STATE_TX_UP|AZ_IOU_STATE_RX_UP|AZ_IOU_STATE_TX_OPEN|AZ_IOU_STATE_RX_OPEN)

/* basic macros */
struct az_iou;
typedef az_ion_id_t az_iou_id_t;
typedef az_int32_t  az_ioap_t;

typedef union {
    struct {
      char  ip[AZ_IOU_ADDR_NAME_LEN-sizeof(int)];
      int   port;
    } inet;
    struct {
      char  devname[AZ_IOU_ADDR_NAME_LEN];
    } serial;
    char  name[AZ_IOU_ADDR_NAME_LEN];
} az_iou_addr_config_t;

typedef struct az_iou_config {
  char          name[AZ_IOU_NAME_LEN];
  az_uint16_t   interfaceType;
  az_uint16_t   transportType;
  az_uint32_t   flags;
  az_iou_addr_config_t local_addr;
  az_iou_addr_config_t remote_addr;
  struct {
    az_size_t buffer_size;
    az_int8_t block_size;
    az_int8_t descr_size;
    az_int16_t descr_count;
    az_int32_t frame_size;

    az_event_id_t event_id;
    az_event_id_t event_mask;
  } tx;
  struct {
    az_size_t buffer_size;
    az_int8_t block_size;
    az_int8_t descr_size;
    az_int16_t descr_count;
    az_int32_t frame_size;

    az_event_id_t event_id;
    az_event_id_t event_mask;
  } rx;

  az_ioap_t ioap;
} az_iou_config_t;

typedef struct az_iou_status {
  az_int32_t  state;

  az_usize_t  rxCount;
  az_usize_t  rxBytes;
  az_usize_t  recvCount;
  az_usize_t  recvBytes;
  az_usize_t  rxRate;

  az_usize_t  txCount;
  az_usize_t  txBytes;
  az_usize_t  sendCount;
  az_usize_t  sendBytes;
  az_usize_t  txRate;

} az_iou_status_t;

typedef az_r_t (*az_iou_create_t)(struct az_iou *, az_iou_config_t *cfg, az_iou_id_t *);
typedef az_r_t (*az_iou_delete_t)(struct az_iou *);
typedef az_r_t (*az_iou_open_t)(struct az_iou *, int flags);
typedef az_r_t (*az_iou_close_t)(struct az_iou *, int flags);
typedef az_r_t (*az_iou_setup_t)(struct az_iou *, int options); 
typedef az_r_t (*az_iou_release_t)(struct az_iou *, int options);
typedef az_r_t (*az_iou_control_t)(struct az_iou *, int cmd, void *arg1, void *arg2);
typedef az_size_t (*az_iou_recv_t)(struct az_iou *, void *bp, az_size_t size); 
typedef az_size_t (*az_iou_send_t)(struct az_iou *, void *bp, az_size_t size); 

struct az_iou_oprs {
  az_iou_create_t   Create;
  az_iou_delete_t   Delete;
  az_iou_open_t     Open;
  az_iou_close_t    Close;
  az_iou_setup_t    Setup;
  az_iou_release_t  Release;
  az_iou_control_t  Control;
  az_iou_recv_t     Recv;
  az_iou_send_t     Send;
};
typedef struct az_iou {
  az_ion_t          ion;
  az_iou_config_t   config;
  az_iou_status_t   status;

  #ifdef  CONFIG_AZ_IOU_USE_EVENT_PORT
  struct {
    az_event_descr_t *event_descr;
    az_event_port_t   event_port;
    az_event_t        event;
  } tx;

  struct {
    az_event_descr_t  *event_descr;
    az_event_port_t   event_port;
    az_event_t        event;
  } rx;
  #endif  

  struct az_iou_oprs       *oprs;

  void  *priv;
} az_iou_t;

/* basic types */

/* structures */

/* structured types */

/* macros */

/* variabls exposed */

/* inline functions */

/* function prototypes exposed */
extern int az_iou_comparebyname(char *name, az_iou_t *iou);
extern az_iou_t *az_iou_find(char *name);

#ifdef __cplusplus
}
#endif
#endif
