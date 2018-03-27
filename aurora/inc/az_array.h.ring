/**
 * @file   az_array.h
 * @brief  
 * @date   12/03/16
 * @author fware
 *
 * Copyright (c) Fware, 2013-2017 - All Rights Reserved
 * You may use, distribute and modify this code under the 
 * terms of the "Aurora Source Code License".
 * See the file LICENSE for full license details.

 * You should have received a copy of  the LICENSE with this file.

 * If not, please contact to Fware with the information in the file CONTACTS.
 *
 * History: @date 
 *   - 2016-03-12 fware initial create
 */

#ifndef AZ_ARRAY_H
#define AZ_ARRAY_H

#include "az_def.h"
#include "az_err.h"
#include "az_ring.h"
#include "az_rw_lock.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* constants */

/* basic macros */

/* basic types */
typedef az_ref_t  az_array_element_t;
typedef az_pos_t    az_array_id_t;

typedef struct az_array {
  az_array_element_t  *list;
  az_size_t           element_size;
  az_size_t           size;
  az_size_t           count;
  az_size_t           last;

  az_ring_t           id_list;
  az_ref_t            *ids;

  az_rw_lock_t        *lock;

  az_array_element_t  *_list;
  az_ref_t            *_ids;

  az_rw_lock_t        _lock;

  az_refcount_t       refCount;
} az_array_t;

/* structures */

/* structured types */

/* macros */
#define az_array_lock_write(ar)   if((ar)->lock)  { az_rw_lock_write((ar)->lock); }
#define az_array_unlock_write(ar) if ((ar)->lock) { az_rw_unlock_write((ar)->lock); }
#define az_array_lock_read(ar)   if((ar)->lock)  { az_rw_lock_read((ar)->lock); }
#define az_array_unlock_read(ar) if ((ar)->lock) { az_rw_unlock_read((ar)->lock); }

#define az_array_element_at(arr, i)  \
        (az_array_element_t)((char *)(arr)->list + i * (arr)->element_size)

#define az_array_element_index(arr, id_ref)  \
    ((az_array_id_t)((char *)id_ref - (char *)(arr)->list)/(arr)->element_size)

static inline int az_array_element_null(az_array_t *pArray, az_array_element_t e)
{
  int j;
  char *element_start = (char *)e;
  for (j = 0; j < pArray->element_size; j++, element_start++) {
    if (*element_start) return 0;
  }
  return 1;
}
static inline void az_array_element_clear(az_array_t *pArray, az_array_element_t e)
{
  int j;
  char *element_start = (char *)e;
  for (j = 0; j < pArray->element_size; j++, element_start++) {
    *element_start = 0;
  }
}

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
    az_array_element_t *list, az_ref_t ids, az_size_t element_size) 
{
  az_r_t err = AZ_ERR(ENTITY_NULL);
  if (NULL == pArray) {
    pArray = az_sys_malloc(sizeof(az_array_t));
    if (pArray) {
      pArray->refCount = 0;
    }
  }
  do {
    if (NULL == pArray) {
      break;
    }
    pArray->lock = NULL; 
    pArray->refCount++;
    pArray->element_size = element_size;
    if (NULL == list) {
      list = az_sys_malloc(size * sizeof(element_size));
      if (NULL == list) break;
      pArray->_list = list;
    } else {
      pArray->_list = NULL;
    }
    err = AZ_SUCCESS;
    pArray->list = list;
    pArray->size = size;

    if (NULL == ids) {
      ids = az_sys_malloc(sizeof(az_ref_t) * size);
      if (NULL == ids) break;
      pArray->_ids = ids;
    }
    pArray->ids = ids;
    az_ring_init(&pArray->id_list, AZ_RING_TYPE_DSREF, size, ids);

    az_size_t i;
    az_ref_t id_ref;
    char *element_start = (char *)list; 
    for (i = 0; i < size; i++, element_start += element_size) {
      id_ref = (az_ref_t)element_start; 
      az_ring_push(&pArray->id_list, &id_ref);
    }
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
      if (pArray->_ids) az_sys_free(pArray->_ids);
      if (pArray->_list) az_sys_free(pArray->_list);
      if (--pArray->refCount == 0) {
        az_sys_free(pArray);
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
    if (pArray->_ids) az_sys_free(pArray->_ids);
    if (pArray->_list) az_sys_free(pArray->_list);
    if (--pArray->refCount == 0) {
      az_sys_free(pArray);
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
static inline az_array_id_t az_array_add(az_array_t *pArray, az_array_element_t e)
{
  az_array_id_t id;
  az_ref_t id_ref;

  az_assert(NULL != pArray && NULL != e);

  do {
    az_rw_lock_write(pArray->lock);
    if (az_ring_pop(&pArray->id_list, &id_ref) < 0) {
      az_rw_unlock_write(pArray->lock);
      id = AZ_ERR(FULL); 
      break;
    }
    id = az_array_element_index(pArray, id_ref); 
    if (id >= (pArray->size * pArray->element_size)) {
      az_rw_unlock_write(pArray->lock);
      id = AZ_ERR(OVERFLOW); 
      break;
    }
    memcpy(az_array_element_at(pArray, id), e, 0);
    az_atomic_inc_size(&pArray->count);
    if (pArray->last <= id) pArray->last = id + 1;
    az_rw_unlock_write(pArray->lock);
  } while (0);

  return id;
}
/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
static inline az_array_id_t az_array_fill(az_array_t *pArray, az_array_element_t e, void (*fill_f)(az_array_id_t, az_array_element_t, az_array_element_t))
{
  az_array_id_t id;
  az_ref_t id_ref;

  az_assert(NULL != pArray && NULL != e);
  az_assert(NULL != fill_f);

  do {
    az_rw_lock_write(pArray->lock);
    if (az_ring_pop(&pArray->id_list, &id_ref) < 0) {
      az_rw_unlock_write(pArray->lock);
      id = AZ_ERR(FULL); 
      break;
    }
    id = az_array_element_index(pArray, id_ref); 
    if (id >= (pArray->size * pArray->element_size)) {
      az_rw_unlock_write(pArray->lock);
      id = AZ_ERR(OVERFLOW); 
      break;
    }
    (fill_f)(id, az_array_element_at(pArray, id), e);
    az_atomic_inc_size(&pArray->count);
    if (pArray->last <= id) pArray->last = id + 1;
    az_rw_unlock_write(pArray->lock);
  } while (0);

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
  az_array_element_t *e;
  az_r_t result = AZ_ERR(ENTITY_NULL);
  az_ref_t id_ref;

  do {
    if (id >= (pArray->size * pArray->element_size)) {
      break;
    }
    id_ref = e = az_array_element_at(pArray, id); 
    az_rw_lock_write(pArray->lock);
    if (az_array_element_null(pArray, e)) {
      az_rw_unlock_write(pArray->lock);
      break;
    }
    result = AZ_SUCCESS;
    az_array_element_clear(pArray, e);
    az_atomic_dec_size(&pArray->count);
    az_ring_push(&pArray->id_list, &id_ref);
    for (id--; id >= 0; id--) {
      if (!az_array_element_null(pArray, az_array_element_at(pArray, id))) {
        break;
      }
    }
    pArray->last = id + 1;
    az_rw_unlock_write(pArray->lock);
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
static inline az_pos_t az_array_find(az_array_t *pArray, az_array_element_t e,
    az_r_t (*is_equal)(az_array_element_t, az_array_element_t))
{
  az_pos_t result = (az_array_id_t)AZ_ERR(NOT_FOUND);
  az_pos_t    j, max;

  az_assert(NULL != pArray && NULL != e);
  az_assert(NULL != is_equal); 

  do {
    az_array_lock_read(pArray);
    max = pArray->last - 1;
    for (j = 0;j < (max+1)/2; j++) {
      if ((is_equal)(e, az_array_element_at(pArray, j))) {
        result = (j);
        break;
      }
      if ((is_equal)(e, az_array_element_at(pArray, max-j))) {
        result = (max-j);
        break;
      }
    }
    az_array_unlock_read(pArray);
  } while (0);

  return result;
}

/* function prototypes exposed */

#ifdef __cplusplus
}
#endif
#endif
