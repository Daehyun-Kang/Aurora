/**
 * @file   az_bio.h
 * @brief  
 * @date   17/06/16
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
 *   - 2016-06-17 fware initial create
 */

#ifndef AZ_BIO_H
#define AZ_BIO_H

#include "az_buffer.h"
#include "az_ring.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* constants */

/* basic macros */

/* basic types */
typedef struct {
  az_size_t   buffer_size;
  az_size_t   block_size;
  az_uint8_t  *buffer_data;

  az_size_t   descr_count;
  az_int32_t  descr_type;
  az_ref_t    descr_list;
} az_bio_cfg_t;

typedef struct {
  az_sys_lw_lock_t  busy;
} az_bio_status_t;

typedef struct {
  az_bio_status_t status;
  az_bio_cfg_t    cfg;

  az_sys_lw_lock_t  *lock;

  az_buffer_t buffer_ctrl;
  az_ring_t   descr_ctrl;
} az_bio_t;

/* structures */

/* structured types */

/* macros */
#define az_bio_lock(bio) \
          if(bio->lock) az_sys_lw_lock(bio->lock)

#define az_bio_unlock(bio) \
          if(bio->lock) az_sys_lw_unlock(bio->lock)

                                    /*123456789abcdef0*/
#define AZ_BIO_DESCR64_BUSY_FLAG    0x8000000000000000L 
#define AZ_BIO_DESCR64_FULL_FLAG    0x4000000000000000L 
#define AZ_BIO_DESCR64_DATA_READY   (AZ_BIO_DESCR64_BUSY_FLAG|AZ_BIO_DESCR64_FULL_FLAG)
#define AZ_BIO_DESCR64_LENGTH_MASK  0x0000000000ffffffL 
#define AZ_BIO_DESCR64_OFFSET_POS   24    //16M 

#define AZ_BIO_DESCR32_BUSY_FLAG    0x80000000
#define AZ_BIO_DESCR32_FULL_FLAG    0x40000000
#define AZ_BIO_DESCR32_DATA_READY   (AZ_BIO_DESCR32_BUSY_FLAG|AZ_BIO_DESCR32_FULL_FLAG)
#define AZ_BIO_DESCR32_LENGTH_MASK  0x000000ff
#define AZ_BIO_DESCR32_OFFSET_POS   16    //64k 
/* variabls exposed */

/* inline functions */
static inline void az_bio_init(az_bio_t *bio, az_bio_cfg_t *pCfg) 
{
  az_assert(bio != NULL);
  az_assert(pCfg != NULL);
  az_assert(pCfg->block_size > 0);
  bio->lock = &bio->status.busy;
  if (pCfg != &(bio->cfg)) {
    bio->cfg = *pCfg;
  }

  az_ring_init(&(bio->descr_ctrl), 
      bio->cfg.descr_type, bio->cfg.descr_count, bio->cfg.descr_list);
  az_buffer_init(&(bio->buffer_ctrl), bio->cfg.buffer_size/bio->cfg.block_size);
  az_sys_lw_lock_init(bio->lock);
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
static inline void az_bio_deinit(az_bio_t *bio)
{
  az_sys_lw_lock_deinit(bio->lock);
  az_buffer_deinit(&(bio->buffer_ctrl));
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
static inline az_r_t az_bio_beginDataIn64(az_bio_t *bio, az_size_t len, az_ref_t *bp, az_pos_t *pDescrIndex)
{
  az_r_t r = AZ_ERR(BUSY);
  az_size_t blen;
  az_pos_t inp;

  az_bio_lock(bio);
  do {
    blen = (len + bio->cfg.block_size - 1)/bio->cfg.block_size;
    inp = az_buffer_reserve(&(bio->buffer_ctrl), blen);
    if (inp > 0) {
      r = AZ_SUCCESS;
      *pDescrIndex = inp;
      *bp = (az_ref_t)(bio->cfg.buffer_data + inp*bio->cfg.block_size);
       az_uint64_t descr = AZ_BIO_DESCR64_BUSY_FLAG|(inp << AZ_BIO_DESCR64_OFFSET_POS)|len;
      if (az_ring_push64(&(bio->descr_ctrl), &descr) < 0) {
        az_buffer_restore(&(bio->buffer_ctrl), inp, blen);
        r = AZ_ERR(NO_BD);
      }
    }
  } while (0);
  az_bio_unlock(bio);

  return r;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
static inline az_r_t az_bio_endDataIn64(az_bio_t *bio, az_pos_t descrIndex)
{
  az_r_t r = AZ_ERR(BUSY);
  az_uint64_t flag = AZ_BIO_DESCR64_FULL_FLAG;

  az_bio_lock(bio);
  do {
    r = az_ring_setflag64(&(bio->descr_ctrl), descrIndex, &flag);
  } while (0);
  az_bio_unlock(bio);

  return r;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
static inline az_r_t az_bio_beginDataOut64(az_bio_t *bio, az_size_t *pLen, az_ref_t *bp, az_pos_t *pDescrIndex)
{
  az_r_t r = AZ_ERR(BUSY);
  az_uint64_t descr;

  az_bio_lock(bio);
  do { 
    az_ring_first64(&(bio->descr_ctrl), &descr);
    if ((descr & AZ_BIO_DESCR64_DATA_READY) != AZ_BIO_DESCR64_DATA_READY) {
      r = AZ_ERR(NO_DATA);
      break;
    }
    if (az_ring_pop64(&(bio->descr_ctrl), &descr) < 0) {
      r = AZ_ERR(EMPTY);
      break;
    } 
    az_pos_t outp = (descr & ~AZ_BIO_DESCR64_DATA_READY) >> AZ_BIO_DESCR64_OFFSET_POS;
    az_size_t len = (descr & AZ_BIO_DESCR64_LENGTH_MASK);
    *pDescrIndex = outp;
    *pLen = len;
    *bp = (az_ref_t)(bio->cfg.buffer_data + outp*bio->cfg.block_size);
  } while (0);
  az_bio_unlock(bio);

  return r;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
static inline az_r_t az_bio_endDataOut64(az_bio_t *bio, az_pos_t descrIndex, az_size_t len) 
{
  az_size_t blen;
  az_r_t r = AZ_ERR(BUSY);

  az_bio_lock(bio);
  do { 
    blen = (len + bio->cfg.block_size - 1)/bio->cfg.block_size;
    r =  az_buffer_advance(&(bio->buffer_ctrl), descrIndex, blen);
  } while (0);
  az_bio_unlock(bio);

  return r;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
static inline az_r_t az_bio_beginDataIn32(az_bio_t *bio, az_size_t len, az_ref_t *bp, az_pos_t *pDescrIndex)
{
  az_r_t r = AZ_ERR(BUSY);
  az_size_t blen;
  az_pos_t inp;

  az_bio_lock(bio);
  do { 
    blen = (len + bio->cfg.block_size - 1)/bio->cfg.block_size;
    inp = az_buffer_reserve(&(bio->buffer_ctrl), blen);
    if (inp > 0) {
      r = AZ_SUCCESS;
      *pDescrIndex = inp;
      *bp = (az_ref_t)(bio->cfg.buffer_data + inp*bio->cfg.block_size);
       az_uint32_t descr = AZ_BIO_DESCR32_BUSY_FLAG|(inp << AZ_BIO_DESCR32_OFFSET_POS)|len;
      if (az_ring_push32(&(bio->descr_ctrl), &descr) < 0) {
        az_buffer_restore(&(bio->buffer_ctrl), inp, blen);
        r = AZ_ERR(NO_BD);
      }
    }
  } while (0);
  az_bio_unlock(bio);

  return r;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
static inline az_r_t az_bio_endDataIn32(az_bio_t *bio, az_pos_t descrIndex)
{
  az_r_t r = AZ_ERR(BUSY);
  az_bio_lock(bio);
  az_uint32_t flag = AZ_BIO_DESCR32_FULL_FLAG;
  do { 
    r = az_ring_setflag32(&(bio->descr_ctrl), descrIndex, &flag);
  } while (0);
  az_bio_unlock(bio);

  return r;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
static inline az_r_t az_bio_beginDataOut32(az_bio_t *bio, az_size_t *pLen, az_ref_t *bp, az_pos_t *pDescrIndex)
{
  az_r_t r = AZ_ERR(BUSY);
  az_bio_lock(bio);
  do {
    az_uint32_t descr;
    az_ring_first32(&(bio->descr_ctrl), &descr);
    if ((descr & AZ_BIO_DESCR32_DATA_READY) != AZ_BIO_DESCR32_DATA_READY) {
      r = AZ_ERR(NO_DATA);
      break;
    }
    if (az_ring_pop32(&(bio->descr_ctrl), &descr) < 0) {
      r = AZ_ERR(EMPTY);
      break;
    } 
    az_pos_t outp = (descr & ~AZ_BIO_DESCR32_DATA_READY) >> AZ_BIO_DESCR32_OFFSET_POS;
    az_size_t len = (descr & AZ_BIO_DESCR32_LENGTH_MASK);
    *pDescrIndex = outp;
    *pLen = len;
    *bp = (az_ref_t)(bio->cfg.buffer_data + outp*bio->cfg.block_size);

  } while (0);
  az_bio_unlock(bio);

  return r;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
static inline az_r_t az_bio_endDataOut32(az_bio_t *bio, az_pos_t descrIndex, az_size_t len) 
{
  az_size_t blen;
  az_r_t r = AZ_ERR(BUSY);
  az_bio_lock(bio);
  do {
    blen = (len + bio->cfg.block_size - 1)/bio->cfg.block_size;
    r =  az_buffer_advance(&(bio->buffer_ctrl), descrIndex, blen);
  } while (0);
  az_bio_unlock(bio);

  return r;
}

/* function prototypes exposed */

#ifdef __cplusplus
}
#endif
#endif
