/**
 * @file   az_index.h
 * @brief  
 * @date   04/07/16
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
 *   - 2016-07-04 fware initial create
 */

#ifndef AZ_INDEX_H
#define AZ_INDEX_H

#include "az_def.h"
#include "az_atomic.h"
#include "az_malloc.h"
#include "az_ring.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* constants */

/* basic macros */

/* basic types */
#ifdef  AZ_INDEX_POOL_USE_BITS_LIST

typedef uint8_t az_index_list_unit_t; 
typedef struct az_index_pool {
  az_size_t size;
  az_size_t count;
  az_pos_t q, r;
  az_pos_t pop;
  az_index_list_unit_t   *list;
  az_index_list_unit_t   *_list;
} az_index_pool_t;
#define AZ_INDEX_POOL_LIST_UNIT_SZ  sizeof(*(((az_index_pool_t *)0)->list))
#define AZ_INDEX_POOL_LIST_SZ(size)  \
  (((size + AZ_INDEX_POOL_LIST_UNIT_SZ - 1)/AZ_INDEX_POOL_LIST_UNIT_SZ)*AZ_INDEX_POOL_LIST_SZ)

#define AZ_INDEX_POOL_TEST_AND_SET(ptr, cur_val, new_val) \
          az_atomic_compare_and_swap8(ptr, cur_val, new_val)

#define AZ_INDEX_POOL_SIZE(pool)  ((pool)->size)
#define AZ_INDEX_POOL_COUNT(pool)  ((pool)-count)
/* structures */

/* structured types */

/* macros */

/* variabls exposed */

/* inline functions */
static inline void az_index_pool_init(az_index_pool_t *pool, az_size_t size, uint8_t *list)
{
  az_assert(NULL != pool);
  az_size_t list_size = AZ_INDEX_POOL_LIST_SZ(size); 
  pool->size = size;
  pool->count = 0;
  pool->pop = 0;
  if (NULL == list) {
    list = pool->_list = az_malloc(list_size);
  } else {
    pool->_list = NULL;
  }
  pool->list = list;
  az_size_t j;
  for (j = 0; j < list_size; j++) {
    list[j] = 0;
  }
  az_pos_t q, r;
  q = pool->size / AZ_INDEX_POOL_LIST_UNIT_SZ; 
  r = pool->size % AZ_INDEX_POOL_LIST_UNIT_SZ;
  if (r > 0) {
    for (j = r; j < AZ_INDEX_POOL_LIST_UNIT_SZ; j++) {
      list[q] |= (1 << (AZ_INDEX_POOL_LIST_UNIT_SZ - j - 1));
    }
  }
  pool->q = q;
  pool->r = r;
}
static inline void az_index_pool_deinit(az_index_pool_t *pool)
{
  az_assert(NULL != pool);
  if (pool->_list) {
    az_free(pool->_list);
    pool->_list = NULL;
  }
  pool->list = NULL;
}
static inline az_pos_t az_index_pool_get(az_index_pool_t *pool)
{
  az_assert(NULL != pool);
  az_assert(NULL != pool->list);
  az_pos_t pop = pool->pop;
  az_pos_t q, r;
  az_index_list_unit_t   test;
  az_index_list_unit_t   cur;

  q = pop / AZ_INDEX_POOL_LIST_UNIT_SZ; 
  r = pop % AZ_INDEX_POOL_LIST_UNIT_SZ;

  do {
    if (pool->count >= pool->size) {
      pop = AZ_ERR(EMPTY);
      break;
    }
    test = (1 << (AZ_INDEX_POOL_LIST_UNIT_SZ - r - 1));
    cur = pool->list[q];
    if (!(cur & test)) {
      test = cur | test;
      if (cur == AZ_INDEX_POOL_TEST_AND_SET(&(pool>list[q]), cur, test)) {
#ifndef  AZ_INDEX_POOL_USE_SIMPLE
        pop = q *AZ_INDEX_POOL_LIST_UNIT_SZ + r; 
#endif
        pool->pop = ((pop+1) == pool->size)? 0:(pop+1); 
        az_atomic_inc_size(&(pool->count));
        break;
      }
    }
#ifdef  AZ_INDEX_POOL_USE_SIMPLE
    if (++pop == pool->size) pop = 0;
    q = pop / AZ_INDEX_POOL_LIST_UNIT_SZ; 
    r = pop % AZ_INDEX_POOL_LIST_UNIT_SZ;
#else
    if (q == pool->q) {
      if (++r == pool->r) { 
        q = 0; 
        r = 0;
      }
    } else {
      if (++r == AZ_INDEX_POOL_LIST_UNIT_SZ) {
        r = 0;
        q++;
      }
    }
#endif
  } while (1);

  return pop;
}
static inline void az_index_pool_release(az_index_pool_t *pool, az_pos_t index)
{
  az_assert(NULL != pool);
  az_assert(NULL != pool->list);
  az_pos_t q, r;
  az_index_list_unit_t   test;
  az_index_list_unit_t   cur;

  az_assert(index >= 0 && index <= pool->size);
  q = index / AZ_INDEX_POOL_LIST_UNIT_SZ; 
  r = index % AZ_INDEX_POOL_LIST_UNIT_SZ;
  test = (1 << (AZ_INDEX_POOL_LIST_UNIT_SZ - r - 1));
  do {
    cur = pool->list[q];
    if (!(cur & test)) break; 
    test = cur & ~test;
    if (cur == AZ_INDEX_POOL_TEST_AND_SET(&(pool>list[q]), cur, test)) {
      az_atomic_dec_size(&(pool->count));
      break;
    } else {
      az_sys_eprintf("bit %d update fail for %x\n", index, cur); 
    }
  } while (0);
}

#else
typedef struct az_index_pool {
  uint8_t   index_unit_size;
  uint8_t   ring_type;
  az_ring_t ring;
  uint8_t *list;
  uint8_t *_list;

  struct {
    az_pos_t (*Get)(struct az_index_pool *);
    void (*Release)(struct az_index_pool *, az_pos_t);
  } oprs;
} az_index_pool_t;

#define AZ_INDEX_POOL_SIZE(pool)  ((pool)->ring.size)
#define AZ_INDEX_POOL_COUNT(pool)  ((pool)->ring.count)

static inline az_pos_t az_index_pool_get8(az_index_pool_t *pool);
static inline az_pos_t az_index_pool_get16(az_index_pool_t *pool);
static inline az_pos_t az_index_pool_get32(az_index_pool_t *pool);
static inline az_pos_t az_index_pool_get64(az_index_pool_t *pool);
static inline void az_index_pool_release8(az_index_pool_t *pool, az_pos_t index);
static inline void az_index_pool_release16(az_index_pool_t *pool, az_pos_t index);
static inline void az_index_pool_release32(az_index_pool_t *pool, az_pos_t index);
static inline void az_index_pool_release64(az_index_pool_t *pool, az_pos_t index);

static inline void az_index_pool_init(az_index_pool_t *pool, az_size_t size, uint8_t *list)
{
  az_assert(NULL != pool);
  az_assert(size > 0);

  if (size < 256) {
    pool->index_unit_size = sizeof(uint8_t);
    pool->ring_type = AZ_RING_TYPE_DS8;
    pool->oprs.Get = az_index_pool_get8;
    pool->oprs.Release = az_index_pool_release8;
  }
  else if (size < 65536) {
    pool->index_unit_size = sizeof(uint16_t);
    pool->ring_type = AZ_RING_TYPE_DS16;
    pool->oprs.Get = az_index_pool_get16;
    pool->oprs.Release = az_index_pool_release16;
  }
  else if (size <= 0xffffffff) {
    pool->index_unit_size = sizeof(uint32_t);
    pool->ring_type = AZ_RING_TYPE_DS32;
    pool->oprs.Get = az_index_pool_get32;
    pool->oprs.Release = az_index_pool_release32;
  }
  else {
    pool->index_unit_size = sizeof(uint64_t);
    pool->ring_type = AZ_RING_TYPE_DS64;
    pool->oprs.Get = az_index_pool_get64;
    pool->oprs.Release = az_index_pool_release64;
  }

  if (NULL == list) {
    list = pool->_list = az_malloc(size * pool->index_unit_size);
  } else {
    pool->_list = NULL;
  }
  pool->list = list;
  az_memset(list, 0, size*pool->index_unit_size);
  az_ring_init(&(pool->ring), pool->ring_type, size, list);  
  switch (pool->ring_type) {
    case AZ_RING_TYPE_DS8:
      {
        uint8_t index = 1;
        for (; index <= size; index++) {
          az_ring_push(&(pool->ring), &index);
        }
      } break;
    case AZ_RING_TYPE_DS16:
      {
        uint16_t index = 1;
        for (; index <= size; index++) {
          az_ring_push(&(pool->ring), &index);
        }
      } break;
    case AZ_RING_TYPE_DS32:
      {
        uint32_t index = 1;
        for (; index <= size; index++) {
          az_ring_push(&(pool->ring), &index);
        }
      } break;
    case AZ_RING_TYPE_DS64:
      {
        uint64_t index = 1;
        for (; index <= size; index++) {
          az_ring_push(&(pool->ring), &index);
        }
      } break;
  }
}

static inline void az_index_pool_deinit(az_index_pool_t *pool)
{
  az_assert(NULL != pool);
  az_ring_deinit(&(pool->ring));
  if (pool->_list) {
    az_free(pool->_list);
    pool->_list = NULL;
  }
  pool->list = NULL;
}
static inline az_pos_t az_index_pool_get8(az_index_pool_t *pool)
{
  az_assert(pool->ring_type == AZ_RING_TYPE_DS8);
  az_pos_t pop;
  uint8_t index;

  do {
    pop = az_ring_pop(&(pool->ring), &index);
    if (pop < 0) break;
    pop = (az_pos_t)index - 1;
  } while (0);
  return pop;
}
static inline az_pos_t az_index_pool_get16(az_index_pool_t *pool)
{
  az_assert(pool->ring_type == AZ_RING_TYPE_DS16);
  az_pos_t pop;
  uint16_t index;

  do {
    pop = az_ring_pop(&(pool->ring), &index);
    if (pop < 0) break;
    pop = (az_pos_t)index - 1;
  } while (0);
  return pop;
}
static inline az_pos_t az_index_pool_get32(az_index_pool_t *pool)
{
  az_assert(pool->ring_type == AZ_RING_TYPE_DS32);
  az_pos_t pop;
  uint32_t index;

  do {
    pop = az_ring_pop(&(pool->ring), &index);
    if (pop < 0) break;
    pop = (az_pos_t)index - 1;
  } while (0);
  return pop;
}
static inline az_pos_t az_index_pool_get64(az_index_pool_t *pool)
{
  az_assert(pool->ring_type == AZ_RING_TYPE_DS64);
  az_pos_t pop;
  uint64_t index;

  do {
    pop = az_ring_pop(&(pool->ring), &index);
    if (pop < 0) break;
    pop = (az_pos_t)index - 1;
  } while (0);
  return pop;
}
static inline az_pos_t az_index_pool_get(az_index_pool_t *pool)
{
  az_assert(NULL != pool);
  az_assert(NULL != pool->list);

  return pool->oprs.Get(pool);
}
static inline void az_index_pool_release8(az_index_pool_t *pool, az_pos_t index)
{
  az_assert(pool->ring_type == AZ_RING_TYPE_DS8);
  uint8_t push = index + 1;
  az_ring_push(&(pool->ring), &push);
}
static inline void az_index_pool_release16(az_index_pool_t *pool, az_pos_t index)
{
  az_assert(pool->ring_type == AZ_RING_TYPE_DS16);
  uint16_t push = index + 1;
  az_ring_push(&(pool->ring), &push);
}
static inline void az_index_pool_release32(az_index_pool_t *pool, az_pos_t index)
{
  az_assert(pool->ring_type == AZ_RING_TYPE_DS32);
  uint32_t push = index + 1;
  az_ring_push(&(pool->ring), &push);
}
static inline void az_index_pool_release64(az_index_pool_t *pool, az_pos_t index)
{
  az_assert(pool->ring_type == AZ_RING_TYPE_DS64);
  uint64_t push = index + 1;
  az_ring_push(&(pool->ring), &push);
}

static inline void az_index_pool_release(az_index_pool_t *pool, az_pos_t index)
{
  az_assert(NULL != pool);
  az_assert(NULL != pool->list);
  az_assert(index < pool->ring.size && index >= 0);
  pool->oprs.Release(pool, index);
}
#endif


/* function prototypes exposed */

#ifdef __cplusplus
}
#endif
#endif
