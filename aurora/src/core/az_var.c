/**
 * @file   az_var.c
 * @brief  
 * @date   10/03/16
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
 *   - 2016-03-10 mercury initial create
 */

/* include header files */
#include "az_def.h"
#include "az_var.h"

/* declare global variables */
char *az_var_type_names[] = {
  "int8", "uint8",
  "int16", "uint16",
  "int32", "uint32",
  "int64", "uint64",
  "float", "double",
  "str", "ref",
  NULL
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
int az_var_print_int8(char *bp, az_size_t blen, void *pValue, int size)
{
  return snprintf(bp, blen, AZ_VAR_DFT_INT_FMT, *(az_int8_t *)pValue);
}
int az_var_print_int16(char *bp, az_size_t blen,  void *pValue, int size)
{
  return snprintf(bp, blen, AZ_VAR_DFT_INT_FMT, *(az_int16_t *)pValue);
}
int az_var_print_int32(char *bp, az_size_t blen,  void *pValue, int size)
{
  return snprintf(bp, blen, AZ_VAR_DFT_INT_FMT, *(az_int32_t *)pValue);
}
int az_var_print_int64(char *bp, az_size_t blen,  void *pValue, int size)
{
  return snprintf(bp, blen, AZ_VAR_DFT_INT64_FMT, *(az_int64_t *)pValue);
}

int az_var_print_uint8(char *bp, az_size_t blen,  void *pValue, int size)
{
  return snprintf(bp, blen, AZ_VAR_DFT_UINT_FMT, *(az_uint8_t *)pValue);
}
int az_var_print_uint16(char *bp, az_size_t blen,  void *pValue, int size)
{
  return snprintf(bp, blen, AZ_VAR_DFT_UINT_FMT, *(az_uint16_t *)pValue);
}
int az_var_print_uint32(char *bp, az_size_t blen,  void *pValue, int size)
{
  return snprintf(bp, blen, AZ_VAR_DFT_UINT_FMT, *(az_uint32_t *)pValue);
}
int az_var_print_uint64(char *bp, az_size_t blen,  void *pValue, int size)
{
  return snprintf(bp, blen, AZ_VAR_DFT_UINT64_FMT, *(az_uint64_t *)pValue);
}

int az_var_print_bit8(char *bp, az_size_t blen,  void *pValue, int size)
{
  return snprintf(bp, blen, AZ_VAR_DFT_HEX_FMT, *(az_uint8_t *)pValue);
}
int az_var_print_bit16(char *bp, az_size_t blen,  void *pValue, int size)
{
  return snprintf(bp, blen, AZ_VAR_DFT_HEX_FMT, *(az_uint16_t *)pValue);
}
int az_var_print_bit32(char *bp, az_size_t blen,  void *pValue, int size)
{
  return snprintf(bp, blen, AZ_VAR_DFT_HEX_FMT, *(az_uint32_t *)pValue);
}
int az_var_print_bit64(char *bp, az_size_t blen,  void *pValue, int size)
{
  return snprintf(bp, blen, AZ_VAR_DFT_HEX64_FMT, *(az_uint64_t *)pValue);
}

int az_var_print_float(char *bp, az_size_t blen,  void *pValue, int size)
{
  return snprintf(bp, blen, AZ_VAR_DFT_FLOAT_FMT, *(float *)pValue);
}
int az_var_print_double(char *bp, az_size_t blen,  void *pValue, int size)
{
  return snprintf(bp, blen, AZ_VAR_DFT_DOUBLE_FMT, *(double *)pValue);
}
int az_var_print_char(char *bp, az_size_t blen,  void *pValue, int size)
{
  return snprintf(bp, blen, AZ_VAR_DFT_CHAR_FMT, *(char *)pValue);
}

int az_var_print_size(char *bp, az_size_t blen,  void *pValue, int size)
{
  return snprintf(bp, blen, AZ_VAR_DFT_SIZE_FMT, *(az_size_t *)pValue);
}
int az_var_print_pos(char *bp, az_size_t blen,  void *pValue, int size)
{
  return snprintf(bp, blen, AZ_VAR_DFT_POS_FMT, *(az_pos_t *)pValue);
}
int az_var_print_usize(char *bp, az_size_t blen,  void *pValue, int size)
{
  return snprintf(bp, blen, AZ_VAR_DFT_USIZE_FMT, *(az_usize_t *)pValue);
}
int az_var_print_upos(char *bp, az_size_t blen,  void *pValue, int size)
{
  return snprintf(bp, blen, AZ_VAR_DFT_UPOS_FMT, *(az_upos_t *)pValue);
}

int az_var_print_signed(char *bp, az_size_t blen,  void *pValue, int size)
{
  int len = 0;
  switch (size) {
    case sizeof(az_int8_t):
      len = snprintf(bp, blen, AZ_VAR_DFT_INT_FMT, *(az_int8_t *)pValue);
      break;
    case sizeof(az_int16_t):
      len = snprintf(bp, blen, AZ_VAR_DFT_INT_FMT, *(az_int16_t *)pValue);
      break;
    case sizeof(az_int32_t):
      len = snprintf(bp, blen, AZ_VAR_DFT_INT_FMT, *(az_int32_t *)pValue);
      break;
    case sizeof(az_int64_t):
      len = snprintf(bp, blen, AZ_VAR_DFT_INT64_FMT, *(az_int64_t *)pValue);
      break;
  }
  return len;
}
int az_var_print_unsigned(char *bp, az_size_t blen,  void *pValue, int size)
{
  int len = 0;
  switch (size) {
    case sizeof(az_uint8_t):
      len = snprintf(bp, blen, AZ_VAR_DFT_UINT_FMT, *(az_uint8_t *)pValue);
      break;
    case sizeof(az_uint16_t):
      len = snprintf(bp, blen, AZ_VAR_DFT_UINT_FMT, *(az_uint16_t *)pValue);
      break;
    case sizeof(az_uint32_t):
      len = snprintf(bp, blen, AZ_VAR_DFT_UINT_FMT, *(az_uint32_t *)pValue);
      break;
    case sizeof(az_uint64_t):
      len = snprintf(bp, blen, AZ_VAR_DFT_UINT64_FMT, *(az_uint64_t *)pValue);
      break;
  }
  return len;
}
int az_var_print_pointer(char *bp, az_size_t blen,  void *pValue, int size)
{
  return snprintf(bp, blen, AZ_VAR_DFT_PTR_FMT, *(void **)pValue);
}

az_var_print_scalar_value_t  az_var_print_scalar_value_table[] = { NULL,
  az_var_print_int8, az_var_print_int16, az_var_print_int32, az_var_print_int64,
  az_var_print_uint8, az_var_print_uint16, az_var_print_uint32, az_var_print_uint64,
  az_var_print_bit8, az_var_print_bit16, az_var_print_bit32, az_var_print_bit64,
  az_var_print_float, az_var_print_double, az_var_print_char,
  az_var_print_size, az_var_print_pos, az_var_print_usize, az_var_print_upos, 
  az_var_print_unsigned, // BOOL
  az_var_print_signed, // ENUM
  az_var_print_signed, // RET
  az_var_print_pointer //REF
};

/* implement global functions */

/**
 * @fn        function name
 * @brief     function-description
 * @param     input-output-parameters
 * @return    return-value
 * @warning   warnings
 * @exception none
 */
az_size_t az_var_print(az_uint8_t *pVar, az_var_descr_t *pDescr, 
    az_var_print_format_t *format, char *bp, az_size_t blen)
{
  az_size_t tlen = 0;
  //az_size_t nlen = 0;
  char valueStr[17];
  char valueFmt[8];
  az_uint8_t *pData = pVar + pDescr->offset; 

  do { 
    if (format->mode == AZ_VAR_PRINT_ROW) {
      snprintf(valueFmt, sizeof(valueFmt), "%%%ds",format->content_width);
    } else   {
      snprintf(valueFmt, sizeof(valueFmt), "%%-%ds",format->content_width);
    }
    if (format->value2Str) {
      char *str = (format->value2Str)(pData); 
      _AZ_SNPRINTF(tlen, bp, blen, valueFmt, str); 
      break;
    }
    if (pDescr->type < AZ_VAR_TYPE_ARRAY && pDescr->type > 0) {
      az_var_print_scalar_value_t print_scalar_value_f = NULL; 
      print_scalar_value_f = az_var_print_scalar_value_table[pDescr->type];
      (print_scalar_value_f)(valueStr, sizeof(valueStr), pData, pDescr->size);
    } else {
      valueStr[0] = 0;
    }
    if (NULL != format->title_fmt) {
      _AZ_SNPRINTF(tlen, bp, blen, format->title_fmt, pDescr->name);
    }
    if (blen <= 0) break;
    if (valueStr[0]) {
      _AZ_SNPRINTF(tlen, bp, blen, valueFmt, valueStr); 
    } else {
      switch (pDescr->type) {
        case AZ_VAR_TYPE_STRING: 
          { 
            _AZ_SNPRINTF(tlen, bp, blen, valueFmt, pData); 
          } break; 
        default: 
          { 
            _AZ_SNPRINTF(tlen, bp, blen, valueFmt, " "); 
          } break;
      }
    }
  } while (0);

  return tlen;
}
az_size_t az_var_printArray(az_uint8_t *pVar, az_var_descr_t *pDescr, 
    az_size_t arraySize, 
    az_var_print_format_t *format, char *bp, az_size_t blen)
{
  az_size_t tlen = 0;
  az_size_t nlen = 0;
  az_size_t slen = 0;
  az_size_t elementSize = pDescr->size;
  az_size_t j;

  pVar = (az_uint8_t *)(*(az_ref_t *)(pVar + pDescr->offset));
  for (j = 0; j < arraySize; j++) {
      nlen = az_var_print(pVar, pDescr, format, bp, blen);
      _AZ_FORMAT_UPDATE(tlen, bp, blen, nlen); slen += nlen;
      pVar += elementSize;
      if (blen <= 0) break;
  }

  return tlen;
}

az_size_t az_var_printVars(az_uint8_t *pVar, az_var_descr_t *pDescr, 
    az_var_print_format_t *format, char *bp, az_size_t blen,
    void (*init_descr)(az_uint8_t *, az_var_descr_t *))
{
  az_size_t tlen = 0;
  az_size_t nlen = 0;
  az_size_t slen = 0;

  az_assert(NULL != format);

  char  *tag = format->tag;
  char  *width_list = format->width_list;
  az_var_value2Str_t  *convlist = format->value2StrList;

  if (NULL != init_descr) {
    (init_descr)(pVar, pDescr);
  }
  if (tag) {
    _AZ_SNPRINTF(tlen, bp, blen, "%s", tag);
  }
  while (pDescr->type != AZ_VAR_TYPE_NULL) {
    if (!(pDescr->onoff)) {
      pDescr++;
    }
    if (width_list) {
      format->content_width = *width_list;
      width_list++;
    }
    if (convlist) {
      format->value2Str = *convlist;
      convlist++;
    }
    if (pDescr->type == AZ_VAR_TYPE_ARRAY) {
      if ((pDescr+1)->type != AZ_VAR_TYPE_DESCR) {
        az_var_descr_t descr = *pDescr;
        descr.type = AZ_VAR_TYPE_REF;
        nlen = az_var_print(pVar, &descr, format, bp, blen);
        _AZ_FORMAT_UPDATE(tlen, bp, blen, nlen); slen += nlen;
      } else {
        az_var_descr_t *arrayDescr = pDescr; 
        pDescr++;
        az_var_descr_t descr = AZ_VAR_DESCR_SET(arrayDescr->name,
            pDescr->offset, arrayDescr->offset, arrayDescr->size, 1);
        az_size_t arraySize = pDescr->size; 
        nlen = az_var_printArray(pVar, &descr, arraySize, format, bp, blen);
        tlen += nlen; bp += nlen; blen -= nlen; slen += nlen;
      }
    } else 
    if (pDescr->type == AZ_VAR_TYPE_VECTOR) {
      az_uint8_t *child = pVar + pDescr->offset;
      az_size_t child_size = pDescr->size;
      if ((pDescr+1)->type != AZ_VAR_TYPE_DESCR) {
        az_var_descr_t descr = *pDescr;
        descr.type = AZ_VAR_TYPE_REF;
        nlen = az_var_print(pVar, &descr, format, bp, blen);
        _AZ_FORMAT_UPDATE(tlen, bp, blen, nlen); slen += nlen;
      } else {
        pDescr++;
        az_size_t child_element_size = pDescr->size;
        az_var_descr_t *pChildDescr = (az_var_descr_t *)(pDescr->name); 
        while (child_size > 0) {
          nlen = az_var_printVars(child, pChildDescr, format, bp, blen,init_descr);
          _AZ_FORMAT_UPDATE(tlen, bp, blen, nlen); slen += nlen;
          child += child_element_size; child_size -= child_element_size;
          if (blen <= 0) break;
        }
      }
    } else {
      nlen = az_var_print(pVar, pDescr, format, bp, blen);
      _AZ_FORMAT_UPDATE(tlen, bp, blen, nlen); slen += nlen;
    }
    if (blen <= 0) break;
    switch (format->mode) {
      case AZ_VAR_PRINT_KV:
        if (slen >= format->line_width) {
          slen = 0; 
          if (tag) {
            _AZ_SNPRINTF(tlen, bp, blen, "\n%s", tag);
          } else { 
            _AZ_SNPRINTF(tlen, bp, blen, "\n");
          }
        }
        break;
      case AZ_VAR_PRINT_COL:
        if (tag) {
            _AZ_SNPRINTF(tlen, bp, blen, "\n%s", tag);
        } else { 
          _AZ_SNPRINTF(tlen, bp, blen, "\n");
        }
        break;
      case AZ_VAR_PRINT_ROW:
        break;
      default:
        break;
    }
    if (blen <= 0) break;
    pDescr++;
  }

  return tlen;
}

/* === end of AZ_VAR_C === */
