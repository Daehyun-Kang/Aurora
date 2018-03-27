/**
 * @file   az_ion_toStr.c
 * @brief  
 * @date   19/03/16
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
 *   - 2016-03-19 mercury initial create
 */

/* include header files */
#include "az_var.h"
#include "az_ion.h"

/* declare global variables */
az_var_descr_t az_ion_t_descr[] = {
  AZ_VAR_DESCR_DEF(id, az_ion_t, id, AZ_VAR_TYPE_INT32),
  AZ_VAR_DESCR_SET("addr", AZ_VAR_TYPE_REF, 0, sizeof(az_ref_t), 1), 
  AZ_VAR_DESCR_DEF(type, az_ion_t, type, AZ_VAR_TYPE_INT32),
  AZ_VAR_DESCR_DEF(refCount, az_ion_t, refCount, AZ_VAR_TYPE_INT32),
  AZ_VAR_DESCR_NULL 
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

az_size_t az_ion_toStr(az_ion_t *ion, char *tag, char *bp, az_size_t blen)
{ 
  az_var_print_format_t fmt = AZ_VAR_PRINT_KV_FMT_DEFAULT(tag, "%8s:", 8); 
  az_size_t tlen = az_var_printVars((az_uint8_t *)ion, az_ion_t_descr, &fmt, bp, blen, NULL);

  return tlen;
}

az_size_t az_ion_valuesOnRow(az_ion_t *ion, char *tag, char *wlist, az_var_value2Str_t *convlist, char *bp, az_size_t blen)
{ 
  az_var_print_format_t fmt = AZ_VAR_PRINT_ROW_FMT_DEFAULT(tag, wlist, 8); 
  fmt.value2StrList = convlist;
  az_size_t tlen = az_var_printVars((az_uint8_t *)ion, az_ion_t_descr, &fmt, bp, blen, NULL);

  return tlen;
}

/* === end of AZ_ION_TOSTR_C === */
