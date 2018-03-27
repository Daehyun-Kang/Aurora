/**
 * @file   az_bits.h
 * @brief  
 * @date   31/12/17
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
 *   - 2017-12-31 dhkang initial create
 */

#ifndef AZ_BITS_H
#define AZ_BITS_H

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
static inline int az_bitmask_rfilled_size4(uint8_t n)
{
                               //0, 1, 2, 3, 4, 5, 6, 7, 8, 9, a, b, c, d, e, f
  static int bitmask_count4[] = {0, 1,-1, 2,-1,-1,-1, 3,-1,-1,-1,-1,-1,-1,-1,4};

  return bitmask_count4[n & 0x0f];
}
static inline int _az_bitmask_rfilled_size8(uint8_t n)
{
  int sz = az_bitmask_rfilled_size4(n);
  if (sz != 4) {
    return sz;
  }
  return (sz + az_bitmask_rfilled_size4(n >> 4));
}
static inline int az_bitmask_rfilled_size8(uint8_t n)
{
  if (n) {
    return _az_bitmask_rfilled_size8(n);
  } else {
    return n;
  }
}
static inline int _az_bitmask_rfilled_size16(uint16_t n)
{
  int sz = _az_bitmask_rfilled_size8((uint8_t)n);
  if (sz != 8) {
    return sz;
  }
  return (sz + _az_bitmask_rfilled_size8((uint8_t)(n >> 8)));
}
static inline int az_bitmask_rfilled_size16(uint16_t n)
{
  if (n) {
    return _az_bitmask_rfilled_size16(n);
  } else {
    return n;
  }
}

static inline int _az_bitmask_rfilled_size32(uint32_t n)
{
  int sz = _az_bitmask_rfilled_size16((uint16_t)n);
  if (sz != 16) {
    return sz;
  }
  return (sz + _az_bitmask_rfilled_size16((uint16_t)(n >> 16)));
}
static inline int az_bitmask_rfilled_size32(uint32_t n)
{
  if (n) {
    return _az_bitmask_rfilled_size32(n);
  } else {
    return n;
  }
}

static inline int _az_bitmask_rfilled_size64(uint64_t n)
{
  int sz = _az_bitmask_rfilled_size32((uint32_t)n);
  if (sz != 32) {
    return  sz;
  }
  return (sz + _az_bitmask_rfilled_size32((uint32_t)(n >> 32)));
}
static inline int az_bitmask_rfilled_size64(uint32_t n)
{
  if (n) {
    return _az_bitmask_rfilled_size64(n);
  } else {
    return n;
  }
}

/* function prototypes exposed */

#ifdef __cplusplus
}
#endif
#endif
