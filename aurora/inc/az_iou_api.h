/**
 * @file   az_iou_api.h
 * @brief  
 * @date   08/08/16
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
 *   - 2016-08-08 fware initial create
 */

#ifndef AZ_IOU_API_H
#define AZ_IOU_API_H
#include "az_iou.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* constants */

/* basic macros */

/* basic types */

/* structures */

/* structured types */

/* macros */

/* variabls exposed */

/* inline functions */

/* function prototypes exposed */
extern az_r_t  az_iou_create(az_iou_t *iou, az_iou_config_t *cfg, az_iou_id_t *pId);
extern az_r_t  az_iou_delete(az_iou_id_t iou_id);
extern az_r_t  az_iou_open(az_iou_id_t iou_id, int flags);
extern az_r_t  az_iou_close(az_iou_id_t iou_id, int flags);
extern az_r_t  az_iou_setup(az_iou_id_t iou_id, int options);
extern az_r_t  az_iou_release(az_iou_id_t iou_id, int options);
extern az_r_t  az_iou_control(az_iou_id_t iou_id, int cmd, void *arg1, void *arg2);
extern az_size_t  az_iou_send(az_iou_id_t iou_id, void *bp, az_size_t size);
extern az_size_t  az_iou_recv(az_iou_id_t iou_id, void *bp, az_size_t size);

#ifdef __cplusplus
}
#endif
#endif
