/**
 * @file   az_iou_util.c
 * @brief  
 * @date   02/08/16
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
 *   - 2016-08-02 fware initial create
 */

/* include header files */
#include "az_core.h"
#include "az_sys_base.h"
#include "az_printf.h"
#include "az_bio.h"
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

/**
 * @fn        function name
 * @brief     function-description
 * @param     input-output-parameters
 * @return    return-value
 * @warning   warnings
 * @exception none
 */
int az_iou_comparebyname(char *name, az_iou_t *iou)
{
  int r = 0;
  do {
    if (NULL == name) { r = -1; break; } 
    if (NULL == iou) { r = -2; break; } 
    if (iou->ion.type != AZ_ION_TYPE_IOU) {
      r = 1; break;
    }
    r = strcmp(name, iou->config.name);
  } while (0);
  return r;
}
az_iou_t *az_iou_find(char *name)
{
  return (az_iou_t *)az_ion_find(name, (az_ion_compare_t)az_iou_comparebyname); 
}


/* === end of AZ_IOU_UTIL_C === */
