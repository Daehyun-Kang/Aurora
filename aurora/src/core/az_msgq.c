/**
 * @file   az_msgq.c
 * @brief  
 * @date   05/08/16
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
 *   - 2016-08-05 fware initial create
 */

/* include header files */
#include "az_msgq.h"

/* declare global variables */
struct az_msgq_oprs az_msgq_oprs32 = {
  .Send = az_msgq_send32,
  .Recv = az_msgq_recv32,
};
struct az_msgq_oprs az_msgq_oprs64 = {
  .Send = az_msgq_send64,
  .Recv = az_msgq_recv64,
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
int az_msgq_comparebyname(char *name, az_msgq_t *msgq)
{
  int r = 0;
  do {
    if (NULL == name) { r = -1; break; } 
    if (NULL == msgq) { r = -2; break; } 
    if (msgq->ion.type != AZ_ION_TYPE_MSGQ) {
      r = 1; break;
    }
    r = strcmp(name, msgq->name);
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
az_size_t az_msgq_send32(struct az_msgq *msgq, void *src, az_size_t len)
{
  az_size_t r;
  az_pos_t index;
  az_ref_t dest;
  do {
    r = (az_size_t)az_bio_beginDataIn32(&(msgq->bio), len, &dest, &index);
    if (r < 0) {
      break;
    }
    az_memcpy(dest, src, len);
    az_bio_endDataIn32(&(msgq->bio), index);
    r = len;
  } while (0);

  return r; 
}

az_size_t az_msgq_recv32(struct az_msgq *msgq, void *dest, az_size_t size)
{
  az_size_t r;
  az_pos_t index;
  az_ref_t src;
  az_size_t len;
  do {
    r = (az_size_t)az_bio_beginDataOut32(&(msgq->bio), &len, &src, &index); 
    if (r < 0) {
      break;
    }
    if (len < size) {
      az_memcpy(dest, src, len);
    } else {
      az_memcpy(dest, src, size);
    }
    az_bio_endDataOut32(&(msgq->bio), index, len);
    r = len;
  } while (0);

  return r; 
}

az_size_t az_msgq_send64(struct az_msgq *msgq, void *src, az_size_t len)
{
  az_size_t r;
  az_pos_t index;
  az_ref_t dest;
  do {
    r = (az_size_t)az_bio_beginDataIn64(&(msgq->bio), len, &dest, &index);
    if (r < 0) {
      break;
    }
    az_memcpy(dest, src, len);
    az_bio_endDataIn64(&(msgq->bio), index);
    r = len;
  } while (0);

  return r; 
}
az_size_t az_msgq_recv64(struct az_msgq *msgq, void *dest, az_size_t size)
{
  az_size_t r;
  az_pos_t index;
  az_ref_t src;
  az_size_t len;
  do {
    r = (az_size_t)az_bio_beginDataOut64(&(msgq->bio), &len, &src, &index); 
    if (r < 0) {
      break;
    }
    if (len < size) {
      az_memcpy(dest, src, len);
    } else {
      az_memcpy(dest, src, size);
    }
    az_bio_endDataOut64(&(msgq->bio), index, len);
    r = len;
  } while (0);

  return r; 
}


/* === end of AZ_MSGQ_C === */
