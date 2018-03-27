/**
 * @file   az_atomic_msvc.h
 * @brief  
 * @date   09/07/16
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
 *   - 2016-07-09 fware initial create
 */

#ifndef AZ_ATOMIC_MSVC_H
#define AZ_ATOMIC_MSVC_H

#ifdef __cplusplus
extern "C"
{
#endif

/* constants */

/* basic macros */
#if defined(_MSC_VER)
/* MS VS */
#define az_atomic_compare_and_swap8(ptr, comparand, exchange) \
    _InterlockedCompareExchange8(ptr, exchange, comparand)

#define az_atomic_compare_and_swap16(ptr, comparand, exchange) \
    _InterlockedCompareExchange16(ptr, exchange, comparand)

#define az_atomic_compare_and_swap32(ptr, comparand, exchange) \
    _InterlockedCompareExchange(ptr, exchange, comparand)

#define az_atomic_compare_and_swap64(ptr, comparand, exchange) \
    _InterlockedCompareExchange64(ptr, exchange, comparand)

#define az_atomic_synchronize() _ReadWriteBarrier()

#define az_atomic_exchange32(ptr, value)  _InterlockedExchange32(ptr, value)
#define az_atomic_exchange64(ptr, value)  _InterlockedExchange64(ptr, value)

#define az_atomic_fetch_and_add8(ptr, value) _InterlockedExchangeAdd8(ptr, value)
#define az_atomic_fetch_and_add16(ptr, value) _InterlockedExchangeAdd16(ptr, value)
#define az_atomic_fetch_and_add32(ptr, value) _InterlockedExchangeAdd(ptr, value)
#define az_atomic_fetch_and_add64(ptr, value) _InterlockedExchangeAdd64(ptr, value)

#define az_atomic_fetch_and_sub8(ptr, value) _InterlockedExchangeAdd8(ptr, -value)
#define az_atomic_fetch_and_sub16(ptr, value) _InterlockedExchangeAdd16(ptr, -value)
#define az_atomic_fetch_and_sub32(ptr, value) _InterlockedExchangeAdd(ptr, -value)
#define az_atomic_fetch_and_sub64(ptr, value) _InterlockedExchangeSub64(ptr, -value)

#define az_atomic_fetch_and_or8(ptr, value) _InterlockedOr8(ptr, value)
#define az_atomic_fetch_and_or16(ptr, value) _InterlockedOr16(ptr, value)
#define az_atomic_fetch_and_or32(ptr, value) _InterlockedOr(ptr, value)
#define az_atomic_fetch_and_or64(ptr, value) _InterlockedOr64(ptr, value)

#define az_atomic_fetch_and_and8(ptr, value) _InterlockedAnd8(ptr, value)
#define az_atomic_fetch_and_and16(ptr, value) _InterlockedAnd16(ptr, value)
#define az_atomic_fetch_and_and32(ptr, value) _InterlockedAnd(ptr, value)
#define az_atomic_fetch_and_and64(ptr, value) _InterlockedAnd64(ptr, value)

#define az_atomic_fetch_and_xor8(ptr, value) _InterlockedXor8(ptr, value)
#define az_atomic_fetch_and_xor16(ptr, value) _InterlockedXor16(ptr, value)
#define az_atomic_fetch_and_xor32(ptr, value) _InterlockedXor(ptr, value)
#define az_atomic_fetch_and_xor64(ptr, value) _InterlockedXor32(ptr, value)

#define az_atomic_fetch_and_nand8(ptr, value) _az_atomic_fetch_and_nand8(ptr, value)
#define az_atomic_fetch_and_nand16(ptr, value) _az_atomic_fetch_and_nand16(ptr, value)
#define az_atomic_fetch_and_nand32(ptr, value) _az_atomic_fetch_and_nand32(ptr, value)
#define az_atomic_fetch_and_nand64(ptr, value) _az_atomic_fetch_and_nand64(ptr, value)

static inline uint8_t _az_atomic_fetch_and_nand8(uint8_t *ptr, uint8_t value)
{
  uint8_t ret;
  do {
    ret = *ptr;
    if (ret == az_atomic_compare_and_swap8(ptr, ret, ~(value & ret))) break;
  } while (1);
  return ret;
}
static inline uint16_t _az_atomic_fetch_and_nand16(uint16_t *ptr, uint16_t value)
{
  uint16_t ret;
  do {
    ret = *ptr;
    if (ret == az_atomic_compare_and_swap16(ptr, ret, ~(value & ret))) break;
  } while (1);
  return ret;
}
static inline uint32_t _az_atomic_fetch_and_nand32(uint32_t *ptr, uint32_t value)
{
  uint32_t ret;
  do {
    ret = *ptr;
    if (ret == az_atomic_compare_and_swap32(ptr, ret, ~(value & ret))) break;
  } while (1);
  return ret;
}
static inline uint64_t _az_atomic_fetch_and_nand64(uint64_t *ptr, uint64_t value)
{
  uint64_t ret;
  do {
    ret = *ptr;
    if (ret == az_atomic_compare_and_swap64(ptr, ret, ~(value & ret))) break;
  } while (1);
  return ret;
}

#define az_atomic_add_and_fetch32(ptr, value) _InterlockedAdd(ptr, value)
#define az_atomic_add_and_fetch64(ptr, value) _InterlockedAdd64(ptr, value)

#define az_atomic_sub_and_fetch32(ptr, value) _InterlockedAdd(ptr, -1*value)
#define az_atomic_sub_and_fetch64(ptr, value) _InterlockedAdd(ptr, -1*value)

#define az_atomic_inc32(ptr)  az_atomic_add_and_fetch32(ptr, 1)
#define az_atomic_dec32(ptr)  az_atomic_sub_and_fetch32(ptr, 1)
#define az_atomic_inc64(ptr)  az_atomic_add_and_fetch64(ptr, 1)
#define az_atomic_dec64(ptr)  az_atomic_sub_and_fetch64(ptr, 1)
#endif
/* basic types */

/* structures */

/* structured types */

/* macros */

/* variabls exposed */

/* inline functions */

/* function prototypes exposed */

#ifdef __cplusplus
}
#endif
#endif
