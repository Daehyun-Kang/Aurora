/**
 * @file   az_obuffer.h
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
typedef struct az_obuffer {
  az_pos_t  inp;
  az_pos_t  outp;
  az_size_t count;
  az_size_t size;
} az_obuffer_t;

/* structures */

/* structured types */

/* macros */
#define AZ_BUFFER_NO_SPACE  (-1)

/* variabls exposed */

/* inline functions */

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
static inline void az_obuffer_init(az_obuffer_t *buffer, az_size_t size)
{
  az_assert(NULL != buffer);
  buffer->inp = buffer->outp = 0;
  buffer->count = 0;
  buffer->size = size;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
static inline az_pos_t az_obuffer_reserve(az_obuffer_t *buffer, az_size_t len)
{
  az_assert(NULL != buffer);
  az_pos_t inp = buffer->inp;
  az_pos_t np = buffer->inp + len;
  if (buffer->inp > buffer->outp) {
    az_assert(buffer->count == buffer->inp - buffer->outp);
    if (np > buffer->size) {
      inp = 0;
      np = len;
      if (np <= buffer->outp) {
        buffer->count += (len + buffer->size - buffer->inp);
        buffer->inp = np;
      } else {
        inp = AZ_BUFFER_NO_SPACE;
      }
    } else {
      buffer->inp = ((np == buffer->size)? 0:np);
      buffer->count += len;
    }
  } else if (buffer->inp == buffer->outp) {
    if (buffer->count == 0) {
      if (np > buffer->size) {
        inp = 0;
        np = len;
        if (np <= buffer->outp) {
          buffer->count += (len + buffer->size - buffer->inp);
          buffer->inp = np;
        } else {
          inp = AZ_BUFFER_NO_SPACE;
        }
      } else {
        buffer->inp = ((np == buffer->size)? 0:np);
        buffer->count += len;
      }
    } else {
      az_assert(buffer->count == buffer->size);
      inp = AZ_BUFFER_NO_SPACE;
    }
  } else {
    az_assert(buffer->count == buffer->size - (buffer->outp - buffer->inp));
    if (np <= buffer->outp) {
      buffer->inp = np;
      buffer->count += len;
    } else {
      inp = AZ_BUFFER_NO_SPACE;
    }
  }

  return inp;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
static inline az_pos_t az_obuffer_restore(az_obuffer_t *buffer, az_pos_t inp, az_size_t len)
{
  az_assert(NULL != buffer);
  az_pos_t pp = buffer->inp - len;
  az_assert(pp >= 0);
  if (pp == inp) {
    buffer->count -= len;
    buffer->inp = pp;
  } else {
    pp = buffer->inp = inp;
    buffer->count -= (len + buffer->size - inp);
  }
  az_assert(buffer->count >= 0);

  return pp;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
static inline az_pos_t az_obuffer_advance(az_obuffer_t *buffer, az_pos_t outp, az_size_t len)
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

/* function prototypes exposed */

#ifdef __cplusplus
}
#endif
#endif
