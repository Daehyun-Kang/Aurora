/**
 * @file   az_link_toStr.c
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
#include "az_link.h"

/* declare global variables */
az_var_descr_t az_link_t_descr[] = {
  AZ_VAR_DESCR_DEF(next, az_link_t, next, AZ_VAR_TYPE_REF),
  AZ_VAR_DESCR_NULL
};
az_var_descr_t az_link_list_t_descr[] = {
  AZ_VAR_DESCR_DEF(next, az_link_list_t, next, AZ_VAR_TYPE_REF),
  AZ_VAR_DESCR_DEF(prev, az_link_list_t, prev, AZ_VAR_TYPE_REF),
  AZ_VAR_DESCR_DEF(count, az_link_list_t, count, AZ_VAR_TYPE_SIZE),
  AZ_VAR_DESCR_NULL
};
az_var_descr_t az_double_link_t_descr[] = {
  AZ_VAR_DESCR_DEF(next, az_double_link_t, next, AZ_VAR_TYPE_REF),
  AZ_VAR_DESCR_DEF(prev, az_double_link_t, prev, AZ_VAR_TYPE_REF),
  AZ_VAR_DESCR_NULL
};
az_var_descr_t az_double_link_list_t_descr[] = {
  AZ_VAR_DESCR_DEF(next, az_double_link_list_t, next, AZ_VAR_TYPE_REF),
  AZ_VAR_DESCR_DEF(prev, az_double_link_list_t, prev, AZ_VAR_TYPE_REF),
  AZ_VAR_DESCR_DEF(count, az_double_link_list_t, count, AZ_VAR_TYPE_SIZE),
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
az_size_t az_link_toStr(az_link_t *pLink, char *tag, char *bp, az_size_t blen)
{ 
  az_var_print_format_t fmt = AZ_VAR_PRINT_KV_FMT_DEFAULT(tag, "%8s:", 8); 
  az_size_t tlen = az_var_printVars((az_uint8_t *)pLink, az_link_t_descr, &fmt, bp, blen, NULL);

  return tlen;
}
az_size_t az_link_list_toStr(az_link_list_t *pList, char *tag, char *bp, az_size_t blen)
{ 
  az_var_print_format_t fmt = AZ_VAR_PRINT_KV_FMT_DEFAULT(tag, "%8s:", 8); 
  az_size_t tlen = az_var_printVars((az_uint8_t *)pList, az_link_list_t_descr, &fmt, bp, blen, NULL);

  return tlen;
}
az_size_t az_double_link_toStr(az_double_link_t *pLink, char *tag, char *bp, az_size_t blen)
{ 
  az_var_print_format_t fmt = AZ_VAR_PRINT_KV_FMT_DEFAULT(tag, "%8s:", 8); 
  az_size_t tlen = az_var_printVars((az_uint8_t *)pLink, az_double_link_t_descr, &fmt, bp, blen, NULL);

  return tlen;
}
az_size_t az_double_link_list_toStr(az_double_link_list_t *pList, char *tag, char *bp, az_size_t blen)
{ 
  az_var_print_format_t fmt = AZ_VAR_PRINT_KV_FMT_DEFAULT(tag, "%8s:", 8); 
  az_size_t tlen = az_var_printVars((az_uint8_t *)pList, 
      az_double_link_list_t_descr, &fmt, bp, blen, NULL);

  return tlen;
}


/* === end of AZ_LINK_TOSTR_C === */
