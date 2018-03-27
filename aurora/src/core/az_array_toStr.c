/**
 * @file   az_array_toStr.c
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
#include "az_array.h"

extern az_var_descr_t az_link_t_descr[]; 
extern az_var_descr_t az_link_list_t_descr[]; 

/* declare global variables */

az_var_descr_t az_array_t_descr[] = {
  AZ_VAR_DESCR_ARRAY(list, az_array_t, list, sizeof(az_array_element_ref_t)),
  AZ_VAR_DESCR_REF(NULL, az_array_t, list, 0), 
  AZ_VAR_DESCR_DEF(size, az_array_t, size, AZ_VAR_TYPE_SIZE), 
  AZ_VAR_DESCR_DEF(count, az_array_t, count, AZ_VAR_TYPE_SIZE), 
  AZ_VAR_DESCR_DEF(refCount, az_array_t, refCount, AZ_VAR_TYPE_INT32), 

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
#ifdef  AZ_ARRAY_USE_LINKED_LIST
az_size_t az_array_id_info_toStr(az_array_id_info_t *info, char *tag, char *bp, az_size_t blen)
{ 
  az_var_print_format_t fmt = AZ_VAR_PRINT_KV_FMT_DEFAULT(tag, "%6s:", 8); 
  az_size_t tlen = az_var_printVars((az_uint8_t *)info, az_array_id_info_t_descr, &fmt, bp, blen, NULL);

  return tlen;
}
#endif
az_size_t az_array_toStr(az_array_t *arr, char *tag, char *bp, az_size_t blen)
{ 
  az_var_print_format_t fmt = AZ_VAR_PRINT_KV_FMT_DEFAULT(tag, "%6s:", 8); 
  az_size_t tlen = az_var_printVars((az_uint8_t *)arr, 
      az_array_t_descr, &fmt, bp, blen, NULL);

  return tlen;
}


/* === end of AZ_ARRAY_TOSTR_C === */
