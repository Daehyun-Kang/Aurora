/**
 * @file   az_cirbuffer.h
 * @brief  
 * @date   26/09/17
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
 *   - 2017-09-26 dhkang initial create
 */

#ifndef AZ_CIRBUFFER_H
#define AZ_CIRBUFFER_H

#include "az_def.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* constants */

/* basic macros */

/* basic types */
typedef int (*az_cirbuffer_pushdata_t)(void *priv, int push, uint8_t *src, int len);
typedef int (*az_cirbuffer_popdata_t)(void *priv, int pop, uint8_t *dst, int len);

typedef struct az_cirbuffer {
  az_pos_t  push;
  az_pos_t  pop;
  az_size_t count;
  az_size_t size;

  az_cirbuffer_pushdata_t pushdata;
  az_cirbuffer_popdata_t popdata;

  az_sys_lw_lock_t  *lock;
  az_sys_lw_lock_t  _lock;

  void  *priv;
} az_cirbuffer_t;

/* structures */

/* structured types */

/* macros */
#define az_cirbuffer_lock(b) if ((b)->lock) { az_sys_lw_lock((b)->lock); }
#define az_cirbuffer_unlock(b) if ((b)->lock) { az_sys_lw_unlock((b)->lock); }

#define az_cirbuffer_size(b)  ((b)->size)
#define az_cirbuffer_space(b)  ((b)->size - (b)->count)
/* variabls exposed */

/* inline functions */
static inline int az_cirbuffer_pushdata_default(void *priv, int push, uint8_t *src, int len)
{
  return len;
}
static inline int az_cirbuffer_popdata_default(void *priv, int pop, uint8_t *dst, int len)
{
  return len;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
static inline void az_cirbuffer_init(az_cirbuffer_t *buffer, az_size_t size)
{
  az_assert(NULL != buffer);
  buffer->push = buffer->pop = 0;
  buffer->count = 0;
  buffer->size = size;
  buffer->pushdata = az_cirbuffer_pushdata_default;
  buffer->popdata = az_cirbuffer_popdata_default;
  buffer->lock = NULL; 
  buffer->priv = NULL;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
static inline void az_cirbuffer_init_s(az_cirbuffer_t *buffer, az_size_t size,
    az_sys_lw_lock_t *lock)
{
  az_cirbuffer_init(buffer, size);
  if (lock) {
    buffer->lock = lock; 
  } else {
    buffer->lock = &buffer->_lock; 
  }
  az_sys_lw_lock_init(buffer->lock);
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
static inline void az_cirbuffer_deinit(az_cirbuffer_t *buffer)
{
  az_assert(NULL != buffer);
  if (buffer->lock) {
    az_sys_lw_lock_deinit(buffer->lock);
    buffer->lock = NULL;
  }
  buffer->push = buffer->pop = 0;
  buffer->count = 0;
  buffer->size = 0; 
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
static inline void az_cirbuffer_setcallbacks(az_cirbuffer_t *buffer, 
    void *priv, az_cirbuffer_pushdata_t pushdata, az_cirbuffer_popdata_t popdata)
{
  az_assert(NULL != buffer);
  if (pushdata) buffer->pushdata = pushdata;
  if (popdata) buffer->popdata = popdata;
  buffer->priv = priv;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
static inline int az_cirbuffer_push(az_cirbuffer_t *buffer, uint8_t *inData, int len)
{
  int result;
  az_pos_t  push;

  /*
  az_sys_printf("buffer=%p size=%ld count=%ld push=%ld pop=%ld priv=%p <= inData=%p len=%d \n", 
      buffer, buffer->size, buffer->count,
      buffer->push, buffer->pop,
      buffer->priv,
      inData, len);
  */
  
  do {
    az_cirbuffer_lock(buffer);
    if ((buffer->size - buffer->count) < len) {
      result = AZ_ERR(OVERFLOW);
      az_cirbuffer_unlock(buffer);
      break;
    }
    push = buffer->push;
    if (push + len < buffer->size) {
      result = (buffer->pushdata)(buffer->priv, push, inData, len);
      az_assert(result == len);

      buffer->count += result;
      buffer->push += result;
    } else {
      int sublen = buffer->size - push;
      result = (buffer->pushdata)(buffer->priv, push, inData, sublen); 
      result += (buffer->pushdata)(buffer->priv, 0, inData+result, len - sublen); 
      az_assert(result == len);

      buffer->count += result;
      buffer->push = len - sublen; 
    }
    az_cirbuffer_unlock(buffer);
  } while (0);

  return result;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */

static inline int az_cirbuffer_pop(az_cirbuffer_t *buffer, uint8_t *outData, int len)
{
  int result;
  az_pos_t  pop;

  do {
    az_cirbuffer_lock(buffer);
    if (buffer->count < len) len = buffer->count;
    if (len <= 0) {
      result = 0; 
      az_cirbuffer_unlock(buffer);
    }
    pop = buffer->pop;
    if (pop + len < buffer->size) {
      result = (buffer->popdata)(buffer->priv, pop, outData, len);
      az_assert(result == len);

      buffer->count -= result;
      buffer->pop += result;
    } else {
      int sublen = buffer->size - pop;
      result = (buffer->popdata)(buffer->priv, pop, outData, sublen); 
      result += (buffer->popdata)(buffer->priv, 0, outData, len - sublen); 
      az_assert(result == len);

      buffer->count -= result;
      buffer->pop = len - sublen; 
    }
    az_cirbuffer_unlock(buffer);
  } while (0);

  return result;
}
#if 0
#endif
/* function prototypes exposed */

#ifdef __cplusplus
}
#endif
#endif
