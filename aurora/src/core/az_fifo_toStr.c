/**
 * @file   az_fifo_toStr.c
 * @brief  
 * @date   30/04/16
 * @author mercury
 *
 * @copyright
 * Copyright (c) Fware, 2013-2016 - All Rights Reserved
 * You may use, distribute and modify this code under the 
 * terms of the "Aurora Source Code License".
 * See the file LICENSE for full license details.\n\n
 *
 * You should have received a copy of  the LICENSE with this file.\n\n
 *
 * If not, please contact to Fware with the information in the file CONTACTS.
 *
 * History:
 *   - 2016-04-30 mercury initial create
 */

/* include header files */
#include "az_var.h"
#include "az_fifo.h"

/* declare global variables */

az_var_descr_t az_fifo_t_descr[] = {
  AZ_VAR_DESCR_DEF(push, az_fifo_t, push, AZ_VAR_TYPE_POS),
  AZ_VAR_DESCR_DEF(pop, az_fifo_t, pop, AZ_VAR_TYPE_POS),
  AZ_VAR_DESCR_DEF(size, az_fifo_t, size, AZ_VAR_TYPE_SIZE),
  AZ_VAR_DESCR_DEF(count, az_fifo_t, count, AZ_VAR_TYPE_SIZE),
  AZ_VAR_DESCR_SET(NULL, AZ_VAR_TYPE_NULL, 0, 0, 0),
};

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
az_size_t az_fifo_toStr(char *bp, az_size_t blen, void *fmt, az_fifo_t *pLink)
{ 
  az_var_print_format_t sfmt = AZ_VAR_PRINT_KV_FMT_DEFAULT(fmt, "%6s:", 5); 
  az_size_t tlen = az_var_printVars((az_uint8_t *)pLink, az_fifo_t_descr, &sfmt, bp, blen, NULL);

  return tlen;
}


/* === end of AZ_FIFO_TOSTR_C === */
