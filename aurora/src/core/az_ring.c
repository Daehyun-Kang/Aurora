/**
 * @file   az_ring.c
 * @brief  
 * @date   25/03/18
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
 *   - 2018-03-25 dhkang initial create
 */

/* include header files */
#include "az_def.h"
#include "az_ring.h"

/* declare global variables */

/* declare static variables */


/* implement static functions */

/**
 * @fn        function name
 * @brief     function-description
 * @param     input-output-parameters
 * @return    return-value
 * @warning   warnings
 * @exception none
 */


/* implement global functions */

/**
 * @fn        function name
 * @brief     function-description
 * @param     input-output-parameters
 * @return    return-value
 * @warning   warnings
 * @exception none
 */

int az_ring_toStr(char *bp, int blen, az_ring_t *ring)
{
  int tlen = 0;
  _AZ_SNPRINTF(tlen, bp, blen, "push=%d pop=%d size=%d count=%d type=%x data=%p\n",
      (int)ring->push, 
      (int)ring->pop, 
      (int)ring->size, 
      (int)ring->count, 
      ring->type, 
      ring->data);
  _AZ_SNPRINTF(tlen, bp, blen, "pushfails=%d popfails=%d oprs(%p,%p,%p,%p)\n",
      (int)ring->pushfails, 
      (int)ring->popfails, 
      ring->oprs.push, 
      ring->oprs.pop, 
      ring->oprs.first, 
      ring->oprs.setflag);
  return tlen;
}

/* === end of AZ_RING_C === */
