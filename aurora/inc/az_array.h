/**
 * @file   az_array.h
 * @brief  
 * @date   12/03/16
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
 *   - 2016-03-12 fware initial create
 */

#ifndef AZ_ARRAY_H
#define AZ_ARRAY_H

#include "az_def.h"
#include "az_err.h"
#include "az_malloc.h"
#include "az_atomic.h"
#include "az_rw_lock.h"
#include "az_string.h"
#include "az_ring.h"
#include "az_index.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* constants */

/* basic macros */

/* basic types */
typedef az_ref_t    az_array_element_ref_t;
typedef az_pos_t    az_array_id_t;

typedef struct az_array {
  az_size_t           size;
  az_size_t           count;
  az_array_element_ref_t  *list;
  az_size_t           last;
  az_size_t           element_size;

  az_index_pool_t     id_list; 

  az_rw_lock_t        *lock;

  az_array_element_ref_t  _list;
  az_rw_lock_t        _lock;

  az_refcount_t refCount;
} az_array_t;

/* structures */

/* structured types */

/* macros */
#define az_array_lock_write(ar)   if((ar)->lock) { az_rw_lock_write((ar)->lock); }
#define az_array_unlock_write(ar) if((ar)->lock) { az_rw_unlock_write((ar)->lock); }
#define az_array_lock_read(ar)    if((ar)->lock) { az_rw_lock_read((ar)->lock); }
#define az_array_unlock_read(ar)  if((ar)->lock) { az_rw_unlock_read((ar)->lock); }

#define az_array_element_ref_at(arr, i)   (az_array_element_ref_t)((uint8_t *)((arr)->list) + (i)*((arr)->element_size))
#define az_array_get_element8_at(arr, i)   *(uint8_t *)((uint8_t *)((arr)->list) + i)
#define az_array_get_element16_at(arr, i)   *(uint16_t *)((uint16_t *)((arr)->list) + i)
#define az_array_get_element32_at(arr, i)   *(uint32_t *)((uint32_t *)((arr)->list) + i)
#define az_array_get_element64_at(arr, i)   *(uint64_t *)((uint64_t *)((arr)->list) + i)
#define az_array_get_element_ptr_at(arr, i)   *(az_ref_t *)((az_ref_t *)((arr)->list) + i)
#define az_array_set_element8_at(arr, i, e)   *(uint8_t *)((uint8_t *)((arr)->list) + i) = (uint8_t)e
#define az_array_set_element16_at(arr, i, e)   *(uint16_t *)((uint16_t *)((arr)->list) + i) = (uint16_t)e
#define az_array_set_element32_at(arr, i, e)   *(uint32_t *)((uint32_t *)((arr)->list) + i) = (uint32_t)e
#define az_array_set_element64_at(arr, i, e)   *(uint64_t *)((uint64_t *)((arr)->list) + i) = (uint64_t)e
#define az_array_set_element_ptr_at(arr, i, e)   *(az_ref_t *)((az_ref_t *)((arr)->list) + i) = (az_ref_t)e

#ifdef  CONFIG_M64
static inline int az_array_element_null(az_array_element_ref_t e, az_size_t element_size) 
{
  uint64_t  *i64 = (uint64_t *)e;
  az_size_t l64;
  l64 = element_size/sizeof(uint64_t);
  for (;l64 > 0; l64--) {
    if (*i64++) return 0;
  }

  element_size = element_size%sizeof(uint64_t);

  uint32_t  *i32 = (uint32_t *)i64;
  int l32 = element_size/sizeof(uint32_t);
  for (; l32 > 0; l32--) {
    if (*i32++) return 0;
  }

  uint8_t   *i8;
  int l8 =  element_size%sizeof(uint32_t);
  for (i8 = (uint8_t *)i32; l8 > 0; l8--) {
    if (*i8++) return 0;
  }
  return 1;
} 
static inline void az_array_element_clear(az_array_element_ref_t e, az_size_t element_size) 
{
  uint64_t  *i64 = (uint64_t *)e;
  az_size_t l64;
  l64 = element_size/sizeof(uint64_t);
  for (;l64 > 0; l64--) {
    *i64++ = 0;
  }

  element_size = element_size%sizeof(uint64_t);

  uint32_t  *i32 = (uint32_t *)i64;
  int l32 = element_size/sizeof(uint32_t);
  for (; l32 > 0; l32--) {
    *i32++ = 0;
  }

  uint8_t   *i8;
  int l8 =  element_size%sizeof(uint32_t);
  for (i8 = (uint8_t *)i32; l8 > 0; l8--) {
    *i8++ = 0;
  }
} 
#else
static inline int az_array_element_null(az_array_element_ref_t e, az_size_t element_size) 
{
  uint32_t  *i32 = (uint32_t *)e;
  int l32;
  l32 = element_size/sizeof(uint32_t);
  for (; l32 > 0; l32--) {
    if (*i32++) return 0;
  }
  uint8_t   *i8;
  int l8 =  element_size%sizeof(uint32_t);
  for (i8 = (uint8_t *)i32; l8 > 0; l8--) {
    if (*i8++) return 0;
  }
  return 1;
}
static inline void az_array_element_clear(az_array_element_t *e, az_size_t element_size) 
{
  uint32_t  *i32 = (uint32_t *)e;
  int l32;
  l32 = element_size/sizeof(uint32_t);
  for (; l32 > 0; l32--) {
    *i32++ = 0;
  }
  uint8_t   *i8;
  int l8 =  element_size%sizeof(uint32_t);
  for (i8 = (uint8_t *)i32; l8 > 0; l8--) {
    *i8++ = 0;
  }
}
#endif

/* variabls exposed */

/* inline functions */

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
static inline az_r_t az_array_init(az_array_t *pArray, az_size_t size,
    az_array_element_ref_t list, az_ref_t ids, az_size_t element_size) 
{
  az_r_t err = AZ_ERR_L(ENTITY_NULL, 0);
  if (NULL == pArray) {
    pArray = az_malloc(sizeof(az_array_t));
    if (pArray) {
      az_refcount_init_dynamic(&pArray->refCount);
    }
  }
  do {
    if (NULL == pArray) {
      break;
    }
    pArray->lock = NULL; 
    az_refcount_atomic_inc(&pArray->refCount);
    pArray->element_size = element_size;
    if (NULL == list) {
      list = az_malloc(size * element_size);
      if (NULL == list) break;
      pArray->_list = list;
    } else {
      pArray->_list = NULL;
    }
    err = AZ_SUCCESS;
    pArray->list = list;
    pArray->size = size;
    memset(list, 0, size * element_size);

    az_index_pool_init(&(pArray->id_list), size, ids);

    pArray->count = 0;
    pArray->last = 0;

    pArray->lock = &pArray->_lock;
    az_rw_lock_init(pArray->lock);

  } while (0);

  if (err) {
    if (pArray) {
      if (NULL != pArray->lock) {
        az_rw_lock_deinit(pArray->lock);
        pArray->lock = NULL;
      }
      az_index_pool_deinit(&(pArray->id_list));
      if (pArray->_list) {
        az_free(pArray->_list);
        pArray->_list = NULL;
      }
      az_refcount_atomic_dec(&pArray->refCount);
      if (AZ_REFCOUNT_IS_ZERO(&pArray->refCount)) {
        az_free(pArray);
      }
    }
  }

  return err;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
static inline void az_array_deinit(az_array_t *pArray)
{
  if (pArray) {
    if (NULL != pArray->lock) {
      az_rw_lock_deinit(pArray->lock);
      pArray->lock = NULL;
    }
    az_index_pool_deinit(&(pArray->id_list));
    if (pArray->_list) {
      az_free(pArray->_list);
      pArray->_list = NULL;
    }
    az_refcount_atomic_dec(&pArray->refCount);
    if (AZ_REFCOUNT_IS_ZERO(&pArray->refCount)) {
      az_free(pArray);
    }
  }
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
static inline az_array_id_t az_array_add(az_array_t *pArray, az_array_element_ref_t e)
{
  az_array_id_t id;

  az_array_lock_write(pArray);
  id = az_index_pool_get(&pArray->id_list);
  if (id < pArray->size) {
    if (NULL != e) {
    az_memcpy(az_array_element_ref_at(pArray, id), e, pArray->element_size);
    }
    az_atomic_inc_size(&pArray->count);
    if (pArray->last <= id) pArray->last = id + 1;
  } else {
    id = AZ_ERR(OVERFLOW); 
  }
  az_array_unlock_write(pArray);

  return id;
}
static inline az_array_id_t az_array_add8(az_array_t *pArray, uint8_t e)
{
  az_array_id_t id;

  az_array_lock_write(pArray);
  id = az_index_pool_get(&pArray->id_list);
  if (id < pArray->size) {
    az_array_set_element8_at(pArray, id, e);
    az_atomic_inc_size(&pArray->count);
    if (pArray->last <= id) pArray->last = id + 1;
  } else {
    id = AZ_ERR(OVERFLOW); 
  }
  az_array_unlock_write(pArray);

  return id;
}
static inline az_array_id_t az_array_add16(az_array_t *pArray, uint16_t e)
{
  az_array_id_t id;

  az_array_lock_write(pArray);
  id = az_index_pool_get(&pArray->id_list);
  if (id < pArray->size) {
    az_array_set_element16_at(pArray, id, e);
    az_atomic_inc_size(&pArray->count);
    if (pArray->last <= id) pArray->last = id + 1;
  } else {
    id = AZ_ERR(OVERFLOW); 
  }
  az_array_unlock_write(pArray);

  return id;
}
static inline az_array_id_t az_array_add32(az_array_t *pArray, uint32_t e)
{
  az_array_id_t id;

  az_array_lock_write(pArray);
  id = az_index_pool_get(&pArray->id_list);
  if (id < pArray->size) {
    az_array_est_element32_at(pArray, id, e);
    az_atomic_inc_size(&pArray->count);
    if (pArray->last <= id) pArray->last = id + 1;
  } else {
    id = AZ_ERR(OVERFLOW); 
  }
  az_array_unlock_write(pArray);

  return id;
}
static inline az_array_id_t az_array_add64(az_array_t *pArray, uint64_t e)
{
  az_array_id_t id;

  az_array_lock_write(pArray);
  id = az_index_pool_get(&pArray->id_list);
  if (id < pArray->size) {
    az_array_set_element64_at(pArray, id, e);
    az_atomic_inc_size(&pArray->count);
    if (pArray->last <= id) pArray->last = id + 1;
  } else {
    id = AZ_ERR(OVERFLOW); 
  }
  az_array_unlock_write(pArray);

  return id;
}
static inline az_array_id_t az_array_addptr(az_array_t *pArray, az_ref_t e)
{
  az_array_id_t id;

  az_array_lock_write(pArray);
  id = az_index_pool_get(&pArray->id_list);
  if (id < pArray->size) {
    az_array_set_element_ptr_at(pArray, id, e);
    az_atomic_inc_size(&pArray->count);
    if (pArray->last <= id) pArray->last = id + 1;
  } else {
    id = AZ_ERR(OVERFLOW); 
  }
  az_array_unlock_write(pArray);

  return id;
}
/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
static inline az_array_id_t az_array_fill(az_array_t *pArray, 
    az_array_element_ref_t e, 
    az_array_element_ref_t (*fill_f)(az_array_element_ref_t, az_array_element_ref_t, az_size_t))
{
  az_array_id_t id;
  az_assert(NULL != pArray && NULL != e);
  az_assert(NULL != fill_f);

  az_array_lock_write(pArray);
  id = az_index_pool_get(&pArray->id_list);
  if (id < pArray->size) {
    (fill_f)(az_array_element_ref_at(pArray,id), e, pArray->element_size); 
    az_atomic_inc_size(&pArray->count);
    if (pArray->last <= id) pArray->last = id + 1;
  } else {
    id = AZ_ERR(OVERFLOW); 
  }
  az_array_unlock_write(pArray);

  return id;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */

static inline az_r_t az_array_del(az_array_t *pArray, az_array_id_t id)
{
  az_array_element_ref_t e;
  az_r_t result = AZ_ERR(ENTITY_NULL);

  az_array_lock_write(pArray);
  do {
    if (id >= pArray->size) {
      break;
    }
    e = (az_array_element_ref_t)az_array_element_ref_at(pArray, id);
    if (az_array_element_null(e, pArray->element_size)) break;
    result = AZ_SUCCESS;
    memset(e, 0, pArray->element_size);
    az_atomic_dec_size(&pArray->count);
    az_index_pool_release(&pArray->id_list, id);
    do {
      if (--id == 0) break;
      e = (az_array_element_ref_t)az_array_element_ref_at(pArray, id);
      if (!az_array_element_null(e, pArray->element_size)) break;
    } while (1);
    pArray->last = id + 1;
  } while (0);
  az_array_unlock_write(pArray);

  return result;
}
static inline az_r_t az_array_del8(az_array_t *pArray, az_array_id_t id)
{
  uint8_t e;
  az_r_t result = AZ_ERR(ENTITY_NULL);

  az_array_lock_write(pArray);
  do {
    if (id >= pArray->size) {
      break;
    }
    e = az_array_get_element8_at(pArray, id);
    if ( 0 == e) break; 
    result = AZ_SUCCESS;
    az_array_set_element8_at(pArray, id, 0);
    az_atomic_dec_size(&pArray->count);
    az_index_pool_release(&pArray->id_list, id);
    do {
      if (--id == 0) break;
      e = az_array_get_element8_at(pArray, id);
      if (e != 0) break;
    } while (1);
    pArray->last = id + 1;
  } while (0);
  az_array_unlock_write(pArray);

  return result;
}
static inline az_r_t az_array_del16(az_array_t *pArray, az_array_id_t id)
{
  uint16_t e;
  az_r_t result = AZ_ERR(ENTITY_NULL);

  az_array_lock_write(pArray);
  do {
    if (id >= pArray->size) {
      break;
    }
    e = az_array_get_element16_at(pArray, id);
    if ( 0 == e) break; 
    result = AZ_SUCCESS;
    az_array_set_element16_at(pArray, id, 0);
    az_atomic_dec_size(&pArray->count);
    az_index_pool_release(&pArray->id_list, id);
    do {
      if (--id == 0) break;
      e = az_array_get_element16_at(pArray, id);
      if (e != 0) break;
    } while (1);
    pArray->last = id + 1;
  } while (0);
  az_array_unlock_write(pArray);

  return result;
}
static inline az_r_t az_array_del32(az_array_t *pArray, az_array_id_t id)
{
  uint32_t e;
  az_r_t result = AZ_ERR(ENTITY_NULL);

  az_array_lock_write(pArray);
  do {
    if (id >= pArray->size) {
      break;
    }
    e = az_array_get_element32_at(pArray, id);
    if ( 0 == e) break; 
    result = AZ_SUCCESS;
    az_array_set_element32_at(pArray, id, 0);
    az_atomic_dec_size(&pArray->count);
    az_index_pool_release(&pArray->id_list, id);
    do {
      if (--id == 0) break;
      e = az_array_get_element32_at(pArray, id);
      if (e != 0) break;
    } while (1);
    pArray->last = id + 1;
  } while (0);
  az_array_unlock_write(pArray);

  return result;
}
static inline az_r_t az_array_del64(az_array_t *pArray, az_array_id_t id)
{
  uint64_t e;
  az_r_t result = AZ_ERR(ENTITY_NULL);

  az_array_lock_write(pArray);
  do {
    if (id >= pArray->size) {
      break;
    }
    e = az_array_get_element64_at(pArray, id);
    if ( 0 == e) break; 
    result = AZ_SUCCESS;
    az_array_set_element64_at(pArray, id, 0);
    az_atomic_dec_size(&pArray->count);
    az_index_pool_release(&pArray->id_list, id);
    do {
      if (--id == 0) break;
      e = az_array_get_element64_at(pArray, id);
      if (e != 0) break;
    } while (1);
    pArray->last = id + 1;
  } while (0);
  az_array_unlock_write(pArray);

  return result;
}
static inline az_r_t az_array_delptr(az_array_t *pArray, az_array_id_t id)
{
  az_ref_t e;
  az_r_t result = AZ_ERR_L(ENTITY_NULL,0);

  az_array_lock_write(pArray);
  do {
    if (id >= pArray->size) {
      break;
    }
    e = az_array_get_element_ptr_at(pArray, id);
    if ( NULL == e) break; 
    result = AZ_SUCCESS;
    az_array_set_element_ptr_at(pArray, id, NULL);
    az_atomic_dec_size(&pArray->count);
    az_index_pool_release(&pArray->id_list, id);
    do {
      if (--id == 0) break;
      e = az_array_get_element_ptr_at(pArray, id);
      if (e != NULL) break;
    } while (1);
    pArray->last = id + 1;
  } while (0);
  az_array_unlock_write(pArray);

  return result;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
static inline az_r_t az_array_find(az_array_t *pArray, az_array_element_ref_t e,
    az_r_t (*find_f)(az_array_element_ref_t, az_array_element_ref_t))
{
  az_r_t result = AZ_ERR(NOT_FOUND);
  az_pos_t    j, max;

  az_assert(NULL != pArray && NULL != e);
  az_assert(NULL != find_f); 

  az_array_lock_read(pArray);
  do {
    max = pArray->last - 1;
    for (j = 0;j < (max+1)/2; j++) {
      if (find_f(e, az_array_element_ref_at(pArray, j))) {
        result = (az_r_t)(j);
        break;
      }
      if (find_f(e, az_array_element_ref_at(pArray,max-j))) {
        result = (az_r_t)(max-j);
        break;
      }
    }
  } while (0);
  az_array_unlock_read(pArray);

  return result;
}

/* function prototypes exposed */

#ifdef __cplusplus
}
#endif
#endif
