/**
 * @file   az_ring.h
 * @brief  
 * @date   20/05/16
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
 *   - 2016-05-20 fware initial create
 */

#ifndef AZ_RING_H
#define AZ_RING_H

#include "az_atomic.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* constants */

/* basic macros */
typedef enum {
  AZ_RING_TYPE_NULL,
  AZ_RING_TYPE_DS8 = 1,    // data size 8bit
  AZ_RING_TYPE_DS16 = 2,   // data size 16bit
  AZ_RING_TYPE_DS32 = 4,   // data size 32bit
  AZ_RING_TYPE_DS64 = 8,   // data size 64bit
  AZ_RING_TYPE_DSREF = 9,  // data size address
} az_ring_type_t;

typedef union {
  az_uint8_t   b8;
  az_uint16_t  b16;
  az_uint32_t  b32;
  az_uint64_t  b64;
  az_ref_t     ref;
} az_ring_value_t;

/* basic types */

typedef struct az_ring {
  az_pos_t  push, pop;
  az_uint32_t size;
  az_uint32_t count;
  az_ring_type_t type;
  az_ref_t  data;

  az_uint32_t pushfails;
  az_uint32_t popfails;

  struct {
    az_pos_t (*push)(struct az_ring *, az_ref_t);
    az_pos_t (*pop)(struct az_ring *, az_ref_t); 
    az_pos_t (*first)(struct az_ring *, az_ref_t); 
    az_r_t   (*setflag)(struct az_ring *, az_pos_t , az_ref_t);
  } oprs;
} az_ring_t;

/* structures */

/* structured types */

/* macros */

/* variabls exposed */

/* inline functions */
static inline az_pos_t az_ring_push8(az_ring_t *ring, az_ref_t pValue); 
static inline az_pos_t az_ring_pop8(az_ring_t *ring, az_ref_t pValue); 
static inline az_r_t az_ring_setflag8(az_ring_t *ring, az_pos_t pos, az_ref_t pFlag);
static inline az_pos_t az_ring_first8(az_ring_t *ring, az_ref_t pValue);

static inline az_pos_t az_ring_push16(az_ring_t *ring, az_ref_t pValue); 
static inline az_pos_t az_ring_pop16(az_ring_t *ring, az_ref_t pValue); 
static inline az_r_t az_ring_setflag16(az_ring_t *ring, az_pos_t pos, az_ref_t pFlag);
static inline az_pos_t az_ring_first16(az_ring_t *ring, az_ref_t pValue); 

static inline az_pos_t az_ring_push32(az_ring_t *ring, az_ref_t pValue); 
static inline az_pos_t az_ring_pop32(az_ring_t *ring, az_ref_t pValue); 
static inline az_r_t az_ring_setflag32(az_ring_t *ring, az_pos_t pos, az_ref_t pFlag);
static inline az_pos_t az_ring_first32(az_ring_t *ring, az_ref_t pValue); 

static inline az_pos_t az_ring_push64(az_ring_t *ring, az_ref_t pValue); 
static inline az_pos_t az_ring_pop64(az_ring_t *ring, az_ref_t pValue); 
static inline az_r_t az_ring_setflag64(az_ring_t *ring, az_pos_t pos, az_ref_t pFlag);
static inline az_pos_t az_ring_first64(az_ring_t *ring, az_ref_t pValue); 

static inline void az_ring_init(az_ring_t *ring, az_ring_type_t type, az_size_t size, az_ref_t data)
{
  az_pos_t j;

  az_assert(NULL != ring);
  ring->push = ring->pop = 0;
  ring->size = (az_uint32_t)size;
  ring->count = 0;
  ring->type = type;
  ring->data = data;
  ring->pushfails = 0;
  ring->popfails = 0;
  switch (ring->type) {
    case AZ_RING_TYPE_DS8: {
          az_uint8_t *dp = (az_uint8_t *)data;
          for (j = 0; j < size; j++) *dp++ = 0;
          ring->oprs.push = az_ring_push8; 
          ring->oprs.pop = az_ring_pop8; 
          ring->oprs.first = az_ring_push8; 
          ring->oprs.setflag = az_ring_setflag8; 
        } break;
    case AZ_RING_TYPE_DS16: {
          az_uint16_t *dp = (az_uint16_t *)data;
          for (j = 0; j < size; j++) *dp++ = 0;
          ring->oprs.push = az_ring_push16; 
          ring->oprs.pop = az_ring_pop16; 
          ring->oprs.first = az_ring_push16; 
          ring->oprs.setflag = az_ring_setflag16; 
        } break;
    case AZ_RING_TYPE_DS64: {
          az_uint64_t *dp = (az_uint64_t *)data;
          for (j = 0; j < size; j++) *dp++ = 0;
          ring->oprs.push = az_ring_push64; 
          ring->oprs.pop = az_ring_pop64; 
          ring->oprs.first = az_ring_push64; 
          ring->oprs.setflag = az_ring_setflag64; 
        } break;
    case AZ_RING_TYPE_DSREF: {
          az_ref_t *dp = (az_ref_t *)data;
          for (j = 0; j < size; j++) *dp++ = NULL;
          #ifdef  CONFIG_M64
          ring->oprs.push = az_ring_push64; 
          ring->oprs.pop = az_ring_pop64; 
          ring->oprs.first = az_ring_push64; 
          ring->oprs.setflag = az_ring_setflag64; 
          #else
          ring->oprs.push = az_ring_push32; 
          ring->oprs.pop = az_ring_pop32; 
          ring->oprs.first = az_ring_push32; 
          ring->oprs.setflag = az_ring_setflag32; 
          #endif
        } break;
    case AZ_RING_TYPE_DS32: 
    default: {
          az_uint32_t *dp = (az_uint32_t *)data;
          for (j = 0; j < size; j++) *dp++ = 0;
          ring->oprs.push = az_ring_push32; 
          ring->oprs.pop = az_ring_pop32; 
          ring->oprs.first = az_ring_push32; 
          ring->oprs.setflag = az_ring_setflag32; 
        } break;
  }
}
static inline void az_ring_deinit(az_ring_t *ring)
{
  az_assert(NULL != ring);
  ring->size = 0;
  ring->data = NULL;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
static inline az_pos_t az_ring_push8(az_ring_t *ring, az_ref_t pValue) 
{
  az_assert(NULL != ring && NULL != pValue);
  az_pos_t push = ring->push;
  az_pos_t npush = push + 1;
  if (npush == ring->size) npush = 0; 
  az_uint8_t *dp = (az_uint8_t *)(ring->data) + push;
  az_uint8_t value = *(az_uint8_t *)pValue;

  if (0 == az_atomic_compare_and_swap8(dp, 0, value)) {
    ring->push = npush;
    ring->count++;
  } else {
    push = AZ_ERR(FULL);
    ring->pushfails++;
  }

  return push;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
static inline az_pos_t az_ring_pop8(az_ring_t *ring, az_ref_t pValue) 
{
  az_assert(NULL != ring && NULL != pValue);
  az_pos_t pop = ring->pop;
  az_pos_t npop = pop + 1;
  if (npop == ring->size) npop = 0;
  az_uint8_t *dp = (az_uint8_t *)(ring->data) + pop;
  az_uint8_t curval, oldval;

  curval = *dp; 
  if (0 == curval) {
    pop = AZ_ERR(EMPTY);
    ring->popfails++;
  } else {
    oldval = az_atomic_compare_and_swap8(dp, curval, 0);
    if (oldval == curval) {
      *(az_uint8_t *)pValue = oldval;
      ring->pop = npop;
      ring->count--;
    } else {
      pop = AZ_ERR(EMPTY);
      ring->popfails++;
    }
  }

  return pop;
}
static inline az_pos_t az_ring_first8(az_ring_t *ring, az_ref_t pValue) 
{
  az_assert(NULL != ring && NULL != pValue);
  az_pos_t pop = ring->pop;
  *(az_uint8_t *)pValue = *((az_uint8_t *)(ring->data) + pop);

  return pop;
}
static inline az_r_t az_ring_setflag8(az_ring_t *ring, az_pos_t pos, az_ref_t pFlag)
{
  az_assert(NULL != ring && NULL != pFlag);
  az_r_t r = AZ_SUCCESS;
  az_uint8_t *pD8 = (az_uint8_t *)(ring->data) + pos;

  *pD8 = (*pD8) | *(az_uint8_t *)pFlag;

  return r;
}
/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
static inline az_pos_t az_ring_push16(az_ring_t *ring, az_ref_t pValue) 
{
  az_assert(NULL != ring && NULL != pValue);
  az_pos_t push = ring->push;
  az_pos_t npush = push + 1;
  if (npush == ring->size) npush = 0;
  az_uint16_t *dp = (az_uint16_t *)(ring->data) + push;
  az_uint16_t value = *(az_uint16_t *)pValue;

  if (0 == az_atomic_compare_and_swap16(dp, 0, value)) {
    ring->push = npush;
    ring->count++;
  } else {
    push = AZ_ERR(FULL);
    ring->pushfails++;
  } 

  return push;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
static inline az_pos_t az_ring_pop16(az_ring_t *ring, az_ref_t pValue) 
{
  az_assert(NULL != ring && NULL != pValue);
  az_pos_t pop = ring->pop;
  az_pos_t npop = pop + 1;
  if (npop == ring->size) npop = 0 ; 
  az_uint16_t *dp = (az_uint16_t *)(ring->data) + pop;
  az_uint16_t curval, oldval;

  curval = *dp;
  if (0 == curval) { 
    pop = AZ_ERR(EMPTY);
    ring->popfails++;
  } else {
    oldval = az_atomic_compare_and_swap16(dp, curval, 0);
    if (oldval == curval) {
      *(az_uint16_t *)pValue = oldval;
      ring->pop = npop;
      ring->count--;
    } else {
      pop = AZ_ERR(EMPTY);
      ring->popfails++;
    }
  }

  return pop;
}
static inline az_pos_t az_ring_first16(az_ring_t *ring, az_ref_t pValue) 
{
  az_assert(NULL != ring && NULL != pValue);
  az_pos_t pop = ring->pop;
  *(az_uint16_t *)pValue = *((az_uint16_t *)(ring->data) + pop);

  return pop;
}
static inline az_r_t az_ring_setflag16(az_ring_t *ring, az_pos_t pos, az_ref_t pFlag)
{
  az_r_t r = AZ_SUCCESS;
  az_assert(NULL != ring && NULL != pFlag);
  az_uint16_t *pD16 = (az_uint16_t *)(ring->data) + pos;

  *pD16 = (*pD16) | *(az_uint16_t *)pFlag;

  return r;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
static inline az_pos_t az_ring_push32(az_ring_t *ring, az_ref_t pValue) 
{
  az_assert(NULL != ring && NULL != pValue);
  az_pos_t push = ring->push;
  az_pos_t npush = push + 1;
  if (npush == ring->size) npush = 0;
  az_uint32_t *dp = (az_uint32_t *)(ring->data) + push;
  az_uint32_t value = *(az_uint32_t *)pValue;

  //az_uint32_t curval = az_atomic_compare_and_swap32(dp, 0, d32);
  //printf("%s: curval = %d\n", __FUNCTION__, curval);
  //if (0 == curval) { 
  if (0 == az_atomic_compare_and_swap32(dp, 0, value)) {
    ring->push = npush;
    ring->count++;
  } else {
    push = AZ_ERR(FULL);
    ring->pushfails++;
  } 

  return push;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
static inline az_pos_t az_ring_pop32(az_ring_t *ring, az_ref_t pValue) 
{
  az_assert(NULL != ring && NULL != pValue);
  az_pos_t pop = ring->pop;
  az_pos_t npop = pop + 1;
  if (npop == ring->size) npop = 0;
  az_uint32_t *dp = (az_uint32_t *)(ring->data) + pop;
  az_uint32_t curval, oldval;

  curval = *dp;
  if (0 == curval) { 
    pop = AZ_ERR(EMPTY);
    ring->popfails++;
  } else {
    oldval = az_atomic_compare_and_swap32(dp, curval, 0);
    if (oldval == curval) {
      *(az_uint32_t *)pValue = oldval;
      ring->pop = npop;
      ring->count--;
    } else {
      pop = AZ_ERR(EMPTY);
      ring->popfails++;
    }
  }

  return pop;
}
static inline az_pos_t az_ring_first32(az_ring_t *ring, az_ref_t pValue) 
{
  az_assert(NULL != ring && NULL != pValue);
  az_pos_t pop = ring->pop;
  *(az_uint32_t *)pValue = *((az_uint32_t *)(ring->data) + pop);

  return pop;
}
static inline az_r_t az_ring_setflag32(az_ring_t *ring, az_pos_t pos, az_ref_t pFlag)
{
  az_r_t r = AZ_SUCCESS;
  az_assert(NULL != ring && NULL != pFlag);
  az_uint32_t *pD32 = (az_uint32_t *)(ring->data) + pos;

  *pD32 = (*pD32) | *(az_uint32_t *)pFlag;

  return r;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
static inline az_pos_t az_ring_push64(az_ring_t *ring, az_ref_t pValue) 
{
  az_assert(NULL != ring && NULL != pValue);
  az_pos_t push = ring->push;
  az_pos_t npush = push + 1;
  if (npush == ring->size) npush = 0;
  az_uint64_t *dp = (az_uint64_t *)(ring->data) + push;
  az_uint64_t value = *(az_uint64_t *)pValue;

  if (0 == az_atomic_compare_and_swap64(dp, 0, value)) {
    //az_sys_eprintf("push:%ld %lx %lx\n", push, value, *dp);
    ring->push = npush;
    ring->count++;
  } else {
    push = AZ_ERR(FULL);
    ring->pushfails++;
  } 

  return push;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
static inline az_pos_t az_ring_pop64(az_ring_t *ring, az_ref_t pValue) 
{
  az_assert(NULL != ring && NULL != pValue);
  az_pos_t pop = ring->pop;
  az_pos_t npop = pop + 1;
  if (npop == ring->size) npop = 0;
  az_uint64_t *dp = (az_uint64_t *)(ring->data) + pop;
  az_uint64_t curval, oldval;

  curval = *dp;
  if (0 == curval) { 
    pop = AZ_ERR(EMPTY);
    ring->popfails++;
  } else {
    oldval = az_atomic_compare_and_swap64(dp, curval, 0);
    if (oldval == curval) {
      *(az_uint64_t *)pValue = oldval;
      //az_sys_eprintf("pop:%ld %lx\n", pop, oldval);
      ring->pop = npop;
      ring->count--;
    } else{
      pop = AZ_ERR(EMPTY);
      ring->popfails++;
    }
  }

  return pop;
}
static inline az_pos_t az_ring_first64(az_ring_t *ring, az_ref_t pValue) 
{
  az_assert(NULL != ring && NULL != pValue);
  az_pos_t pop = ring->pop;
  *(az_uint64_t *)pValue = *((az_uint64_t *)(ring->data) + pop);

  return pop;
}
static inline az_r_t az_ring_setflag64(az_ring_t *ring, az_pos_t pos, az_ref_t pFlag)
{
  az_r_t r = AZ_SUCCESS;
  az_uint64_t *pD64 = (az_uint64_t *)(ring->data) + pos;

  *pD64 = (*pD64) | *(az_uint64_t *)pFlag;

  return r;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
static inline az_pos_t az_ring_push(az_ring_t *ring, az_ref_t pValue) 
{
  az_assert(NULL != ring && NULL != pValue);
  return ring->oprs.push(ring, pValue);
}
/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
static inline az_pos_t az_ring_pop(az_ring_t *ring, az_ref_t pValue) 
{
  az_assert(NULL != ring && NULL != pValue);
  return ring->oprs.pop(ring, pValue);
}

/* function prototypes exposed */

extern int az_ring_toStr(char *bp, int blen, az_ring_t *ring);

#ifdef __cplusplus
}
#endif
#endif
