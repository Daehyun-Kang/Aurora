/**
 * @file   az_buffer.h
 * @brief  
 * @date   30/04/16
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
 *   - 2016-04-30 fware initial create
 */

#ifndef AZ_BUFFER_H
#define AZ_BUFFER_H

#ifdef __cplusplus
extern "C"
{
#endif

/* constants */

/* basic macros */

/* basic types */
typedef struct az_buffer {
  az_pos_t  inp;
  az_pos_t  outp;
  az_size_t count;
  az_size_t size;

  az_pos_t prev_inp;
  az_pos_t prev_count;

  az_sys_lw_lock_t  *lock;
  az_sys_lw_lock_t  _lock;
} az_buffer_t;

/* structures */

/* structured types */

/* macros */
#define AZ_BUFFER_NO_SPACE        (-1)
#define AZ_BUFFER_BUSY            (-2)
#define AZ_BUFFER_EMPTY           (-3)
#define AZ_BUFFER_OUTP_OOR        (-4)
#define AZ_BUFFER_OUT_TOO_LONG1   (-5)
#define AZ_BUFFER_OUT_TOO_LONG2   (-6)
#define AZ_BUFFER_OUT_TOO_LONG3   (-7)

#define az_buffer_lock(b) if ((b)->lock) { az_sys_lw_lock((b)->lock); }
#define az_buffer_unlock(b) if ((b)->lock) { az_sys_lw_unlock((b)->lock); }

/* variabls exposed */

/* inline functions */

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
static inline void az_buffer_init(az_buffer_t *buffer, az_size_t size)
{
  az_assert(NULL != buffer);
  buffer->inp = buffer->outp = 0;
  buffer->count = 0;
  buffer->size = size;
  buffer->prev_inp = 0;
  buffer->prev_count = 0;
  buffer->lock = NULL; 
}
static inline void az_buffer_init_s(az_buffer_t *buffer, az_size_t size,
    az_sys_lw_lock_t *lock)
{
  az_buffer_init(buffer, size);
  if (lock) {
    buffer->lock = lock; 
  } else {
    buffer->lock = &buffer->_lock; 
  }
  az_sys_lw_lock_init(buffer->lock);
}
static inline void az_buffer_deinit(az_buffer_t *buffer)
{
  az_assert(NULL != buffer);
  if (buffer->lock) {
    az_sys_lw_lock_deinit(buffer->lock);
    buffer->lock = NULL;
  }
  buffer->inp = buffer->outp = 0;
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
static inline az_pos_t az_buffer_reserve0(az_buffer_t *buffer, az_size_t len)
{
  az_assert(NULL != buffer);
  az_pos_t inp = buffer->inp;
  az_pos_t np = buffer->inp + len;
  do {
    if ((buffer->count + len) > buffer->size) {
      inp = AZ_BUFFER_NO_SPACE;
      break;
    }
    if (inp < buffer->outp) {
      az_assert(buffer->count == buffer->size-buffer->outp+buffer->inp);
      if (np <= buffer->outp) {
        buffer->prev_inp = inp;
        buffer->prev_count = buffer->count;
        buffer->inp = np;
        buffer->count += len;
        break;
      }
      inp = AZ_BUFFER_NO_SPACE;
      break;
    } 
    if (np < buffer->size) {
      buffer->prev_inp = inp;
      buffer->prev_count = buffer->count;
      buffer->inp = np;
      buffer->count += len; 
      break;
    } 
    if (np == buffer->size) {
      buffer->prev_inp = inp;
      buffer->prev_count = buffer->count;
      buffer->inp = 0;
      buffer->count += len;
      break;
    } 
    if (len > buffer->outp) {
      inp = AZ_BUFFER_NO_SPACE;
      break;
    }
    buffer->prev_inp = inp;
    buffer->prev_count = buffer->count;
    buffer->count += (buffer->size - inp) + len;
    buffer->inp = len;
    inp = 0;
    break;
  } while (0);

  return inp;
}
static inline az_pos_t az_buffer_reserve(az_buffer_t *buffer, az_size_t len)
{
  az_assert(NULL != buffer);
  az_pos_t inp;
  az_buffer_lock(buffer);
  inp = az_buffer_reserve0(buffer, len);
  az_buffer_unlock(buffer);

  return inp;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
static inline az_pos_t az_buffer_restore0(az_buffer_t *buffer, az_pos_t inp, az_size_t len)
{
  az_assert(NULL != buffer);
  az_assert(inp == buffer->inp);

  buffer->inp = buffer->prev_inp;
  buffer->count = buffer->prev_count;

  return buffer->inp;
}
static inline az_pos_t az_buffer_restore(az_buffer_t *buffer, az_pos_t inp, az_size_t len)
{
  az_assert(NULL != buffer);
  az_assert(inp == buffer->inp);

  az_buffer_lock(buffer);
  inp = az_buffer_restore0(buffer, inp, len);
  az_buffer_unlock(buffer);

  return inp;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
/*
static inline az_pos_t az_buffer_advance0(az_buffer_t *buffer, az_pos_t outp, az_size_t len)
{
  az_assert(NULL != buffer);
  az_size_t np = outp + len;
  az_assert(np <= buffer->size);

  if (outp >= buffer->outp) {
    if (np == buffer->size) np = 0;
    az_assert(np <= buffer->inp);
    buffer->count -= len + (outp - buffer->outp);
    buffer->outp = np;
  } else {
    az_assert(np <= buffer->inp);
    az_assert(np <= buffer->outp);
    buffer->count -= len + (buffer->size - buffer->outp + outp);
    buffer->outp = np;
  }
  az_assert(buffer->count >= 0);

  return np;
}
*/
static inline az_pos_t az_buffer_advance0(az_buffer_t *buffer, az_pos_t outp, az_size_t len)
{
  az_size_t np;
  az_assert(NULL != buffer);
  az_assert(len > 0);

  np = outp + len;
  do {
    if (buffer->outp < buffer->inp) {
      az_assert(buffer->count == (buffer->inp - buffer->outp));
      if (outp < buffer->outp) {
        np = AZ_BUFFER_OUTP_OOR; 
        break;
      }
      if (np > buffer->inp) {
        np = AZ_BUFFER_OUT_TOO_LONG1; 
        break;
      }
      buffer->count += (np - buffer->outp);
      buffer->outp = np; 
      break;
    }
    if (buffer->count == 0) {
        np = AZ_BUFFER_EMPTY;
        break;
    }
    az_assert(buffer->count == (buffer->size - (buffer->outp - buffer->inp)));
    if (outp >= buffer->outp) {
      if (np > buffer->size) {
        np = AZ_BUFFER_OUT_TOO_LONG2; 
        break;
      }
      buffer->count -= (np - buffer->outp);
      buffer->outp = np; 
      break;
    }
    if (np > buffer->inp) {
        np = AZ_BUFFER_OUT_TOO_LONG3; 
        break;
    }
    buffer->count -= (buffer->size - buffer->outp + np);
    buffer->outp = np;
  } while (0);

  return np;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
static inline az_pos_t az_buffer_advance(az_buffer_t *buffer, az_pos_t outp, az_size_t len)
{
  az_assert(NULL != buffer);

  az_buffer_lock(buffer);
  outp = az_buffer_advance0(buffer, outp, len);
  az_buffer_unlock(buffer);

  return outp;
}

/* function prototypes exposed */

#ifdef __cplusplus
}
#endif
#endif
