/**
 * @file   az_xcfg.h
 * @brief  
 * @date   09/11/17
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
 *   - 2017-11-09 dhkang initial create
 */

#ifndef AZ_XCFG_H
#define AZ_XCFG_H

#include "az_var.h"
#include "az_assert.h"
#include "az_err.h"
#include "az_malloc.h"
#include "az_xml.h"
#include "az_fs.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* constants */

/* basic macros */

/* basic types */
typedef struct az_xcfg_var {
  az_xml_element_t *xml;
  az_var_t          var;
} az_xcfg_var_t;

typedef struct az_xcfg_tree {
  az_xml_element_t xml;
  az_xml_element_t *cur;
  char  *mem;
} az_xcfg_tree_t;
 
/* structures */

/* structured types */

/* macros */
#define  AZ_VAR_TYPE_KEY_INT8       "i8"
#define  AZ_VAR_TYPE_KEY_INT16      "i16"
#define  AZ_VAR_TYPE_KEY_INT32      "i32"
#define  AZ_VAR_TYPE_KEY_INT64      "i64"
#define  AZ_VAR_TYPE_KEY_UINT8      "u8"
#define  AZ_VAR_TYPE_KEY_UINT16     "u16"
#define  AZ_VAR_TYPE_KEY_UINT32     "u32"
#define  AZ_VAR_TYPE_KEY_UINT64     "u64"
#define  AZ_VAR_TYPE_KEY_BIT8       "h8"
#define  AZ_VAR_TYPE_KEY_BIT16      "h16"
#define  AZ_VAR_TYPE_KEY_BIT32      "h32"
#define  AZ_VAR_TYPE_KEY_BIT64      "h64"
#define  AZ_VAR_TYPE_KEY_FLOAT      "float"
#define  AZ_VAR_TYPE_KEY_DOUBLE     "double"
#define  AZ_VAR_TYPE_KEY_CHAR       "char"
#define  AZ_VAR_TYPE_KEY_SIZE       "size"
#define  AZ_VAR_TYPE_KEY_POS        "pos"
#define  AZ_VAR_TYPE_KEY_USIZE      "usize"
#define  AZ_VAR_TYPE_KEY_UPOS       "upos"
#define  AZ_VAR_TYPE_KEY_BOOL       "bool"
#define  AZ_VAR_TYPE_KEY_ENUM       "enum"
#define  AZ_VAR_TYPE_KEY_RET        "ret"
#define  AZ_VAR_TYPE_KEY_REF        "ref"
#define  AZ_VAR_TYPE_KEY_ARRAY      "array"
#define  AZ_VAR_TYPE_KEY_STRING     "str"
#define  AZ_VAR_TYPE_KEY_VECTOR     "vec"

/* variabls exposed */

/* inline functions */
static inline az_r_t az_xcfg_load_config(char *filepath, az_xcfg_tree_t *cfg)
{
  az_assert(NULL != filepath);
  az_assert(NULL != cfg);
  az_r_t  r = AZ_ERR(NO_DATA);
#ifdef  CONFIG_LINUX
  az_size_t size = az_fs_file2mem(filepath, (az_ref_t *)&(cfg->mem));
  if (size > 0) {
    az_refcount_init_static(&cfg->xml.refCount);
    az_xml_element_init(&(cfg->xml), "root", "tree");
    r = az_xml_tree_parse(cfg->mem, cfg->mem + size, &(cfg->xml));
  }
#endif
  return r;
}

static inline az_r_t az_xcfg_free_config(az_xcfg_tree_t *cfg)
{
  az_assert(NULL != cfg);
  az_r_t  r = AZ_SUCCESS;
  az_xml_element_release(&(cfg->xml));
  az_free(cfg->mem);
  cfg->mem = NULL;

  return r;
}

static inline az_xml_element_t * az_xcfg_find_element(az_xcfg_tree_t *cfg, 
    az_xml_element_t *parent, char *key, az_xml_element_t **next)
{
  az_assert(NULL != cfg);
  az_assert(NULL != next);
  az_xml_element_t *child; 
  child = *next;
  if (NULL == parent) { 
    parent = &cfg->xml;
  }
  if (child) {
    child = az_xml_element_next_child(parent, child);
  } else {
    child = az_xml_element_find_child(parent, key); 
  }
  *next = child;
  return child;
}

static inline az_size_t az_xcfg_var_toStr(az_xcfg_var_t *pVar, char *bp, int blen)
{
  az_assert(NULL != pVar);
  az_assert(NULL != bp);
  int tlen = 0;
  do {
    switch (pVar->var.type) {
      case AZ_VAR_TYPE_INT8:
        tlen = snprintf(bp, blen, "%d", pVar->var.value.i8);
        break;
      case AZ_VAR_TYPE_UINT8:
        tlen = snprintf(bp, blen, "%u", pVar->var.value.u8);
        break;
      case AZ_VAR_TYPE_BIT8:
        tlen = snprintf(bp, blen, "0x%02x", pVar->var.value.b8);
        break;
      case AZ_VAR_TYPE_INT16:
        tlen = snprintf(bp, blen, "%d", pVar->var.value.i16);
        break;
      case AZ_VAR_TYPE_UINT16:
        tlen = snprintf(bp, blen, "%u", pVar->var.value.u16);
        break;
      case AZ_VAR_TYPE_BIT16:
        tlen = snprintf(bp, blen, "0x%04x", pVar->var.value.b16);
        break;
      case AZ_VAR_TYPE_INT32:
        tlen = snprintf(bp, blen, "%d", pVar->var.value.i32);
        break;
      case AZ_VAR_TYPE_UINT32:
        tlen = snprintf(bp, blen, "%u", pVar->var.value.u32);
        break;
      case AZ_VAR_TYPE_BIT32:
        tlen = snprintf(bp, blen, "0x%08x", pVar->var.value.b32);
        break;
      case AZ_VAR_TYPE_INT64:
        tlen = snprintf(bp, blen, "%ld", pVar->var.value.i64);
        break;
      case AZ_VAR_TYPE_UINT64:
        tlen = snprintf(bp, blen, "%lu", pVar->var.value.u64);
        break;
      case AZ_VAR_TYPE_BIT64:
        tlen = snprintf(bp, blen, "0x%lx", pVar->var.value.b64);
        break;
      case AZ_VAR_TYPE_BOOL:
        tlen = snprintf(bp, blen, "%s", pVar->var.value.b? "TRUE":"FALSE");
        break;
      case AZ_VAR_TYPE_FLOAT:
        tlen = snprintf(bp, blen, "%.3f", pVar->var.value.f); 
        break;
      case AZ_VAR_TYPE_DOUBLE:
        tlen = snprintf(bp, blen, "%.6f", pVar->var.value.d); 
        break;
      case AZ_VAR_TYPE_REF:
        tlen = snprintf(bp, blen, "%p", pVar->var.value.ref); 
        break;
      case AZ_VAR_TYPE_STRING:
        tlen = snprintf(bp, blen, "%s", pVar->var.value.s); 
        break;
      case AZ_VAR_TYPE_CHAR:
        tlen = snprintf(bp, blen, "%c", pVar->var.value.c); 
        break;
      default:
        break;
    }
  } while (0);

  return tlen;
}

static inline az_size_t az_xcfg_parse_var(az_xml_element_t *attr, 
    az_xcfg_var_t *pVar, az_size_t index)
{
  az_assert(NULL != attr);
  az_assert(NULL != pVar);
  pVar = pVar + index;
  do {
    if (strstr(attr->kv.key, AZ_VAR_TYPE_KEY_INT8)) {
      pVar->var.type = AZ_VAR_TYPE_INT8;
      pVar->var.value.i8 =(int8_t)strtol(attr->kv.value, NULL, 10); 
      index++;
      break;
    }
    if (strstr(attr->kv.key, AZ_VAR_TYPE_KEY_UINT8)) {
      pVar->var.type = AZ_VAR_TYPE_UINT8;
      pVar->var.value.u8 =(uint8_t)strtol(attr->kv.value, NULL, 10); 
      index++;
      break;
    }
    if (strstr(attr->kv.key, AZ_VAR_TYPE_KEY_INT16)) {
      pVar->var.type = AZ_VAR_TYPE_INT16;
      pVar->var.value.i16 =(int16_t)strtol(attr->kv.value, NULL, 10); 
      index++;
      break;
    }
    if (strstr(attr->kv.key, AZ_VAR_TYPE_KEY_UINT16)) {
      pVar->var.type = AZ_VAR_TYPE_UINT16;
      pVar->var.value.u16 =(uint16_t)strtol(attr->kv.value, NULL, 10); 
      index++;
      break;
    }
    if (strstr(attr->kv.key, AZ_VAR_TYPE_KEY_INT32)) {
      pVar->var.type = AZ_VAR_TYPE_INT32;
      pVar->var.value.i32 =(int32_t)strtol(attr->kv.value, NULL, 10); 
      index++;
      break;
    }
    if (strstr(attr->kv.key, AZ_VAR_TYPE_KEY_UINT32)) {
      pVar->var.type = AZ_VAR_TYPE_UINT32;
      pVar->var.value.u32 =(uint32_t)strtol(attr->kv.value, NULL, 10); 
      index++;
      break;
    }
    if (strstr(attr->kv.key, AZ_VAR_TYPE_KEY_INT64)) {
      pVar->var.type = AZ_VAR_TYPE_INT64;
      pVar->var.value.i64 =(int64_t)strtol(attr->kv.value, NULL, 10); 
      index++;
      break;
    }
    if (strstr(attr->kv.key, AZ_VAR_TYPE_KEY_UINT64)) {
      pVar->var.type = AZ_VAR_TYPE_UINT64;
      pVar->var.value.u64 =(uint64_t)strtol(attr->kv.value, NULL, 10); 
      index++;
      break;
    }
    if (strstr(attr->kv.key, AZ_VAR_TYPE_KEY_BIT8)) {
      pVar->var.type = AZ_VAR_TYPE_BIT8;
      pVar->var.value.b8 =(uint8_t)strtol(attr->kv.value, NULL, 16); 
      index++;
      break;
    }
    if (strstr(attr->kv.key, AZ_VAR_TYPE_KEY_BIT16)) {
      pVar->var.type = AZ_VAR_TYPE_BIT16;
      pVar->var.value.b16 =(uint16_t)strtol(attr->kv.value, NULL, 16); 
      index++;
      break;
    }
    if (strstr(attr->kv.key, AZ_VAR_TYPE_KEY_BIT32)) {
      pVar->var.type = AZ_VAR_TYPE_BIT32;
      pVar->var.value.b32 =(uint32_t)strtol(attr->kv.value, NULL, 16); 
      index++;
      break;
    }
    if (strstr(attr->kv.key, AZ_VAR_TYPE_KEY_BIT64)) {
      pVar->var.type = AZ_VAR_TYPE_BIT64;
      pVar->var.value.b64 =(uint64_t)strtol(attr->kv.value, NULL, 16); 
      //printf("%s: %lx\n", attr->kv.value, pVar->var.value.b64);
      index++;
      break;
    }
    if (strstr(attr->kv.key, AZ_VAR_TYPE_KEY_BOOL)) {
      pVar->var.type = AZ_VAR_TYPE_BOOL;
      pVar->var.value.b = (strtol(attr->kv.value, NULL, 10) == 0)? AZ_FALSE:AZ_TRUE; 
      index++;
      break;
    }
    if (strstr(attr->kv.key, AZ_VAR_TYPE_KEY_FLOAT)) {
      pVar->var.type = AZ_VAR_TYPE_FLOAT;
      pVar->var.value.f =(float)strtof(attr->kv.value, NULL);
      index++;
      break;
    }
    if (strstr(attr->kv.key, AZ_VAR_TYPE_KEY_DOUBLE)) {
      pVar->var.type = AZ_VAR_TYPE_DOUBLE;
      pVar->var.value.d =(double)strtod(attr->kv.value, NULL);
      index++;
      break;
    }
    if (strstr(attr->kv.key, AZ_VAR_TYPE_KEY_REF)) {
      pVar->var.type = AZ_VAR_TYPE_REF;
      pVar->var.value.ref = (az_ref_t)strtol(attr->kv.value, NULL, 16);
      index++;
      break;
    }

    pVar->var.type = AZ_VAR_TYPE_STRING;
    pVar->var.value.s = attr->kv.value;
    index++;
  } while (0);

  pVar->xml = attr;
  return index;
}

static inline az_size_t az_xcfg_get_element(az_xml_element_t *cur, az_xcfg_var_t *pVar)
{
  az_assert(NULL != pVar);
  az_size_t count = AZ_ERR(NOT_FOUND);
  do {
    if (NULL == cur) {
      break;
    }
    count = az_xml_element_iterateOnAttr(cur, pVar, 0, (az_xml_element_proc_t)az_xcfg_parse_var);
  } while (0);

  return count;
}

static inline az_r_t az_xcfg_convAttrValueDefault(az_xml_element_t *attr, void *pVar, int varlen)
{
  az_assert(NULL != attr);
  az_assert(NULL != pVar);
  az_r_t r = AZ_SUCCESS;
  az_assert(pVar != NULL);
  do {
    if (strstr(attr->kv.key, AZ_VAR_TYPE_KEY_INT8)) {
      az_assert(varlen == sizeof(int8_t));
      *(int8_t *)pVar = (int8_t)strtol(attr->kv.value, NULL, 10); 
      break;
    }
    if (strstr(attr->kv.key, AZ_VAR_TYPE_KEY_UINT8)) {
      az_assert(varlen == sizeof(uint8_t));
      *(uint8_t *)pVar = (uint8_t)strtol(attr->kv.value, NULL, 10); 
      break;
    }
    if (strstr(attr->kv.key, AZ_VAR_TYPE_KEY_INT16)) {
      az_assert(varlen == sizeof(int16_t));
      *(int16_t *)pVar = (int16_t)strtol(attr->kv.value, NULL, 10); 
      break;
    }
    if (strstr(attr->kv.key, AZ_VAR_TYPE_KEY_UINT16)) {
      az_assert(varlen == sizeof(uint16_t));
      *(uint16_t *)pVar = (uint16_t)strtol(attr->kv.value, NULL, 10); 
      break;
    }
    if (strstr(attr->kv.key, AZ_VAR_TYPE_KEY_INT32)) {
      az_assert(varlen == sizeof(int32_t));
      *(int32_t *)pVar = (int32_t)strtol(attr->kv.value, NULL, 10); 
      break;
    }
    if (strstr(attr->kv.key, AZ_VAR_TYPE_KEY_UINT32)) {
      az_assert(varlen == sizeof(uint32_t));
      *(uint32_t *)pVar = (uint32_t)strtol(attr->kv.value, NULL, 10); 
      break;
    }
    if (strstr(attr->kv.key, AZ_VAR_TYPE_KEY_INT64)) {
      az_assert(varlen == sizeof(int64_t));
      *(int64_t *)pVar = (int64_t)strtol(attr->kv.value, NULL, 10); 
      break;
    }
    if (strstr(attr->kv.key, AZ_VAR_TYPE_KEY_UINT64)) {
      az_assert(varlen == sizeof(uint64_t));
      *(uint64_t *)pVar = (uint64_t)strtol(attr->kv.value, NULL, 10); 
      break;
    }
    if (strstr(attr->kv.key, AZ_VAR_TYPE_KEY_BIT8)) {
      az_assert(varlen == sizeof(uint8_t));
      *(uint8_t *)pVar =(uint8_t)strtol(attr->kv.value, NULL, 16); 
      break;
    }
    if (strstr(attr->kv.key, AZ_VAR_TYPE_KEY_BIT16)) {
      az_assert(varlen == sizeof(uint16_t));
      *(uint16_t *)pVar =(uint16_t)strtol(attr->kv.value, NULL, 16); 
      break;
    }
    if (strstr(attr->kv.key, AZ_VAR_TYPE_KEY_BIT32)) {
      az_assert(varlen == sizeof(uint32_t));
      *(uint32_t *)pVar =(uint32_t)strtol(attr->kv.value, NULL, 16); 
      break;
    }
    if (strstr(attr->kv.key, AZ_VAR_TYPE_KEY_BIT64)) {
      az_assert(varlen == sizeof(uint64_t));
      *(uint64_t *)pVar =(uint64_t)strtol(attr->kv.value, NULL, 16); 
      break;
    }
    if (strstr(attr->kv.key, AZ_VAR_TYPE_KEY_BOOL)) {
      az_assert(varlen == sizeof(az_bool_t));
      *(az_bool_t *)pVar = (strtol(attr->kv.value, NULL, 10) == 0)? AZ_FALSE:AZ_TRUE; 
      break;
    }
    if (strstr(attr->kv.key, AZ_VAR_TYPE_KEY_FLOAT)) {
      az_assert(varlen == sizeof(float));
      *(float *)pVar =(float)strtof(attr->kv.value, NULL);
      break;
    }
    if (strstr(attr->kv.key, AZ_VAR_TYPE_KEY_DOUBLE)) {
      az_assert(varlen == sizeof(double));
      *(double *)pVar =(double)strtod(attr->kv.value, NULL);
      break;
    }
    if (strstr(attr->kv.key, AZ_VAR_TYPE_KEY_REF)) {
      az_assert(varlen == sizeof(az_ref_t));
      *(az_ref_t *)pVar = (az_ref_t)strtol(attr->kv.value, NULL, 16);
      break;
    }

    if (varlen > 0) {
      strncpy(pVar, attr->kv.value, varlen-1);
      ((char *)pVar)[varlen-1] = 0;
    }
  } while (0);

  return r; 
}

static inline az_r_t az_xcfg_get_value(az_xml_element_t *elem, char *key, void *pVar, int varlen, az_r_t (*conv_f)(az_xml_element_t *, void *, int ))
{
  az_assert(NULL != elem);
  az_assert(NULL != key);
  az_assert(NULL != pVar);
  //az_assert(NULL != conv_f);
  az_r_t  r = AZ_ERR(NOT_FOUND);
  az_xml_element_t *attr;

  do {
    attr = az_xml_element_find_attr(elem, key);
    if (NULL == attr) break;
    if (NULL != conv_f) {
      r = (conv_f)(attr, pVar, varlen);
    } else {
      r = az_xcfg_convAttrValueDefault(attr, pVar, varlen);
    }
  } while (0);

  return r;
}
/* function prototypes exposed */

#ifdef __cplusplus
}
#endif
#endif
