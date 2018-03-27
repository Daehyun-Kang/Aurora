/**
 * @file   az_atomic_gnuc.h
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

#ifndef AZ_ATOMIC_GNUC_H
#define AZ_ATOMIC_GNUC_H

#ifdef __cplusplus
extern "C"
{
#endif

/* constants */

/* basic macros */
#if defined(__GNUC__)

#define az_atomic_synchronize() __sync_synchronize()

#define az_atomic_compare_and_swap8(ptr, oldval, newval) \
  __sync_val_compare_and_swap(ptr, oldval, newval)

#define az_atomic_compare_and_swap16(ptr, oldval, newval) \
  __sync_val_compare_and_swap(ptr, oldval, newval)

#define az_atomic_compare_and_swap32(ptr, oldval, newval) \
  __sync_val_compare_and_swap(ptr, oldval, newval)

#define az_atomic_compare_and_swap64(ptr, oldval, newval) \
  __sync_val_compare_and_swap(ptr, oldval, newval)
          
#define az_atomic_fetch_and_add8(ptr, val) \
  __sync_fetch_and_add(ptr, val)
#define az_atomic_fetch_and_add16(ptr, val) \
  __sync_fetch_and_add(ptr, val)
#define az_atomic_fetch_and_add32(ptr, val) \
  __sync_fetch_and_add(ptr, val)
#define az_atomic_fetch_and_add64(ptr, val) \
  __sync_fetch_and_add(ptr, val)
#define az_atomic_fetch_and_sub8(ptr, val) \
  __sync_fetch_and_sub(ptr, val)
#define az_atomic_fetch_and_sub16(ptr, val) \
  __sync_fetch_and_sub(ptr, val)
#define az_atomic_fetch_and_sub32(ptr, val) \
  __sync_fetch_and_sub(ptr, val)
#define az_atomic_fetch_and_sub64(ptr, val) \
  __sync_fetch_and_sub(ptr, val)
#define az_atomic_fetch_and_or8(ptr, val) \
  __sync_fetch_and_or(ptr, val)
#define az_atomic_fetch_and_or16(ptr, val) \
  __sync_fetch_and_or(ptr, val)
#define az_atomic_fetch_and_or32(ptr, val) \
  __sync_fetch_and_or(ptr, val)
#define az_atomic_fetch_and_or64(ptr, val) \
  __sync_fetch_and_or(ptr, val)
#define az_atomic_fetch_and_and8(ptr, val) \
  __sync_fetch_and_and(ptr, val)
#define az_atomic_fetch_and_and16(ptr, val) \
  __sync_fetch_and_and(ptr, val)
#define az_atomic_fetch_and_and32(ptr, val) \
  __sync_fetch_and_and(ptr, val)
#define az_atomic_fetch_and_and64(ptr, val) \
  __sync_fetch_and_and(ptr, val)
#define az_atomic_fetch_and_xor8(ptr, val) \
  __sync_fetch_and_xor(ptr, val)
#define az_atomic_fetch_and_xor16(ptr, val) \
  __sync_fetch_and_xor(ptr, val)
#define az_atomic_fetch_and_xor32(ptr, val) \
  __sync_fetch_and_xor(ptr, val)
#define az_atomic_fetch_and_xor64(ptr, val) \
  __sync_fetch_and_xor(ptr, val)
#define az_atomic_fetch_and_nand8(ptr, val) \
  __sync_fetch_and_nand(ptr, val)
#define az_atomic_fetch_and_nand16(ptr, val) \
  __sync_fetch_and_nand(ptr, val)
#define az_atomic_fetch_and_nand32(ptr, val) \
  __sync_fetch_and_nand(ptr, val)
#define az_atomic_fetch_and_nand64(ptr, val) \
  __sync_fetch_and_nand(ptr, val)

#define az_atomic_add_and_fetch8(ptr, val) \
  __sync_add_and_fetch(ptr, val)
#define az_atomic_add_and_fetch16(ptr, val) \
  __sync_add_and_fetch(ptr, val)
#define az_atomic_add_and_fetch32(ptr, val) \
  __sync_add_and_fetch(ptr, val)
#define az_atomic_add_and_fetch64(ptr, val) \
  __sync_add_and_fetch(ptr, val)
#define az_atomic_sub_and_fetch8(ptr, val) \
  __sync_sub_and_fetch(ptr, val)
#define az_atomic_sub_and_fetch16(ptr, val) \
  __sync_sub_and_fetch(ptr, val)
#define az_atomic_sub_and_fetch32(ptr, val) \
  __sync_sub_and_fetch(ptr, val)
#define az_atomic_sub_and_fetch64(ptr, val) \
  __sync_sub_and_fetch(ptr, val)
#define az_atomic_or_and_fetch8(ptr, val) \
  __sync_or_and_fetch(ptr, val)
#define az_atomic_or_and_fetch16(ptr, val) \
  __sync_or_and_fetch(ptr, val)
#define az_atomic_or_and_fetch32(ptr, val) \
  __sync_or_and_fetch(ptr, val)
#define az_atomic_or_and_fetch64(ptr, val) \
  __sync_or_and_fetch(ptr, val)
#define az_atomic_and_and_fetch8(ptr, val) \
  __sync_and_and_fetch(ptr, val)
#define az_atomic_and_and_fetch16(ptr, val) \
  __sync_and_and_fetch(ptr, val)
#define az_atomic_and_and_fetch32(ptr, val) \
  __sync_and_and_fetch(ptr, val)
#define az_atomic_and_and_fetch64(ptr, val) \
  __sync_and_and_fetch(ptr, val)
#define az_atomic_xor_and_fetch8(ptr, val) \
  __sync_xor_and_fetch(ptr, val)
#define az_atomic_xor_and_fetch16(ptr, val) \
  __sync_xor_and_fetch(ptr, val)
#define az_atomic_xor_and_fetch32(ptr, val) \
  __sync_xor_and_fetch(ptr, val)
#define az_atomic_xor_and_fetch64(ptr, val) \
  __sync_xor_and_fetch(ptr, val)
#define az_atomic_nand_and_fetch8(ptr, val) \
  __sync_nand_and_fetch(ptr, val)
#define az_atomic_nand_and_fetch16(ptr, val) \
  __sync_nand_and_fetch(ptr, val)
#define az_atomic_nand_and_fetch32(ptr, val) \
  __sync_nand_and_fetch(ptr, val)
#define az_atomic_nand_and_fetch64(ptr, val) \
  __sync_nand_and_fetch(ptr, val)

#define az_atomic_inc32(ptr)  az_atomic_add_and_fetch32(ptr, 1)
#define az_atomic_dec32(ptr)  az_atomic_sub_and_fetch32(ptr, 1)
#define az_atomic_inc64(ptr)  az_atomic_add_and_fetch64(ptr, 1)
#define az_atomic_dec64(ptr)  az_atomic_sub_and_fetch64(ptr, 1)

#define az_atomic_exchange32(ptr, val) _az_atomic_exchange32(ptr, val)
#define az_atomic_exchange64(ptr, val) _az_atomic_exchange64(ptr, val)

/* basic types */

/* structures */

/* structured types */

/* macros */

/* variabls exposed */

/* inline functions */
static inline uint32_t _az_atomic_exchange32(uint32_t *ptr, uint32_t val)
{
  uint32_t cur = *ptr;
  do {
    if (cur == __sync_val_compare_and_swap(ptr, cur, val)) {
      break;
    }
    cur = *ptr;
  } while (1);
  return cur;
}
static inline uint64_t _az_atomic_exchange64(uint64_t *ptr, uint64_t val)
{
  uint64_t cur = *ptr;
  do {
    if (cur == __sync_val_compare_and_swap(ptr, cur, val)) {
      break;
    }
    cur = *ptr;
  } while (1);
  return cur;
}
#endif

/* function prototypes exposed */

#ifdef __cplusplus
}
#endif
#endif
