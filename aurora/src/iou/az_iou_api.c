/**
 * @file   az_iou_api.c
 * @brief  
 * @date   08/08/16
 * @author fware
 *
 * @copyright
 * Copyright (c) Fware, 2013-2016 - All Rights Reserved
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

/* include header files */
#include "az_iou_api.h"
#include "az_iou_msgq.h"

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
az_r_t  az_iou_create(az_iou_t *iou, az_iou_config_t *cfg, az_iou_id_t *pId)
{
  az_assert(NULL != cfg);
  az_r_t r;

  switch (cfg->interfaceType) {
    case AZ_IOU_INTF_MSGQ:
      r = az_iou_msgq_create(iou, cfg, pId);
      break;
    default:
      r = AZ_ERR(NOT_SUPPORT);
      break;
  }
  return r;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
az_r_t  az_iou_delete(az_iou_id_t iou_id)
{
  az_iou_t  *iou = (az_iou_t *)az_ion(iou_id);
  az_r_t r = AZ_ERR(ENTITY_NULL);

  do {
    if (NULL == iou) break;
    az_assert(iou->ion.type == AZ_ION_TYPE_IOU);
    r = (iou->oprs->Delete)(iou); 
  } while (0);

  return r;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
az_r_t  az_iou_open(az_iou_id_t iou_id, int flags)
{
  az_iou_t  *iou = (az_iou_t *)az_ion(iou_id);
  az_r_t r = AZ_ERR(ENTITY_NULL);

  do {
    if (NULL == iou) break;
    az_assert(iou->ion.type == AZ_ION_TYPE_IOU);
    r = (iou->oprs->Open)(iou, flags); 
  } while (0);

  return r;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
az_r_t  az_iou_close(az_iou_id_t iou_id, int flags)
{
  az_iou_t  *iou = (az_iou_t *)az_ion(iou_id);
  az_r_t r = AZ_ERR(ENTITY_NULL);

  do {
    if (NULL == iou) break;
    az_assert(iou->ion.type == AZ_ION_TYPE_IOU);
    r = (iou->oprs->Close)(iou, flags); 
  } while (0);

  return r;
}
/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
az_r_t  az_iou_setup(az_iou_id_t iou_id, int options)
{
  az_iou_t  *iou = (az_iou_t *)az_ion(iou_id);
  az_r_t r = AZ_ERR(ENTITY_NULL);

  do {
    if (NULL == iou) break;
    az_assert(iou->ion.type == AZ_ION_TYPE_IOU);
    r = (iou->oprs->Setup)(iou, options); 
  } while (0);

  return r;
}
/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
az_r_t  az_iou_release(az_iou_id_t iou_id, int options)
{
  az_iou_t  *iou = (az_iou_t *)az_ion(iou_id);
  az_r_t r = AZ_ERR(ENTITY_NULL);

  do {
    if (NULL == iou) break;
    az_assert(iou->ion.type == AZ_ION_TYPE_IOU);
    r = (iou->oprs->Release)(iou, options); 
  } while (0);

  return r;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
az_size_t  az_iou_send(az_iou_id_t iou_id, void *bp, az_size_t size) 
{
  az_iou_t  *iou = (az_iou_t *)az_ion(iou_id);
  az_size_t r = AZ_ERR(ENTITY_NULL);

  do {
    if (NULL == iou) break;
    az_assert(iou->ion.type == AZ_ION_TYPE_IOU);
    r = (iou->oprs->Send)(iou, bp, size); 
  } while (0);

  return r;
}
/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
az_size_t  az_iou_recv(az_iou_id_t iou_id, void *bp, az_size_t size) 
{
  az_iou_t  *iou = (az_iou_t *)az_ion(iou_id);
  az_size_t r = AZ_ERR(ENTITY_NULL);

  do {
    if (NULL == iou) break;
    az_assert(iou->ion.type == AZ_ION_TYPE_IOU);
    r = (iou->oprs->Recv)(iou, bp, size); 
  } while (0);

  return r;
}
/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
az_r_t  az_iou_control(az_iou_id_t iou_id, int cmd, void *arg1, void *arg2)
{
  az_iou_t  *iou = (az_iou_t *)az_ion(iou_id);
  az_r_t r = AZ_ERR(ENTITY_NULL);

  do {
    if (NULL == iou) break;
    az_assert(iou->ion.type == AZ_ION_TYPE_IOU);
    r = (iou->oprs->Control)(iou, cmd, arg1, arg2); 
  } while (0);

  return r;
}

/* === end of AZ_IOU_API_C === */
