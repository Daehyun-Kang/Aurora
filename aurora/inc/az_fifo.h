/**
 * @file   az_fifo.h
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

#ifndef AZ_FIFO_H
#define AZ_FIFO_H

#include "az_atomic.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* constants */

/* basic macros */
#define AZ_FIFO_FULL     (-1)
#define AZ_FIFO_EMPTY    (-2)

/* basic types */
typedef struct az_fifo {
  az_pos_t  push, pop;
  az_size_t size;
  az_size_t count;

  az_uint32_t pushfails;
  az_uint32_t popfails;
} az_fifo_t;

/* structures */

/* structured types */

/* macros */

/* variabls exposed */

/* inline functions */

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
static inline void az_fifo_init(az_fifo_t *fifo, az_size_t size)
{
  az_assert(NULL != fifo);
  fifo->push = fifo->pop = 0;
  fifo->size = size;
  fifo->count = 0;
  fifo->pushfails = fifo->popfails = 0;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
static inline az_pos_t az_fifo_push(az_fifo_t *fifo)
{
  az_assert(NULL != fifo);

  az_pos_t push = fifo->push;
  az_pos_t npush = fifo->push + 1;
  if (push == fifo->pop) {
    if (fifo->count != 0) {
      push = AZ_FIFO_FULL;
      fifo->pushfails++;
    } else {
      if (npush == fifo->size) npush = 0;
      fifo->push = npush;
      az_atomic_inc_size(&(fifo->count));
    }
  } else {
    if (npush == fifo->size) npush = 0;
    fifo->push = npush;
    az_atomic_inc_size(&(fifo->count));
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
static inline az_pos_t az_fifo_pop(az_fifo_t *fifo)
{
  az_assert(NULL != fifo);
  az_pos_t pop = fifo->pop;
  az_pos_t npop = fifo->pop + 1;
  if (pop == fifo->push) {
    if (fifo->count == 0) {
      pop = AZ_FIFO_EMPTY;
      fifo->popfails++;
    } else {
      if (npop == fifo->size) npop = 0;
      fifo->pop = npop;
      az_atomic_dec_size(&(fifo->count));
    }
  } else {
    if (npop == fifo->size) npop = 0;
    fifo->pop = npop;
    az_atomic_dec_size(&(fifo->count));
  }
  return pop;
}

/* function prototypes exposed */
extern az_size_t az_fifo_toStr(az_fifo_t *, char *, char *, az_size_t);

#ifdef __cplusplus
}
#endif
#endif
