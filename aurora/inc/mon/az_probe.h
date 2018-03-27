/**
 * @file   az_probe.h
 * @brief  
 * @date   29/07/16
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
 *   - 2016-07-29 fware initial create
 */

#ifndef AZ_PROBE_H
#define AZ_PROBE_H

#include "az_core.h"


#ifdef __cplusplus
extern "C"
{
#endif

/* constants */

/* basic macros */
#define AZ_PROBE_REF_INVALID   (az_probe_ref_t)(-1)
#define AZ_PROBE_DEF_REF(var) az_probe_ref_t  var;
#define AZ_PROBE_REG(_t, _m, _p, _f) \
  do { (_m)->_p = az_probe_register(_t, _m, _f); } while (0);

/* basic types */
typedef uint32_t   az_probe_ref_t;
typedef char  *(*az_probe_callback_t)(void *,...);
typedef struct az_probe {
  az_probe_ref_t  index;
  char  *tag;
  void  *module;
  char  *(*cb_f)(void *,...);
} az_probe_t;

/* structures */

/* structured types */

/* macros */

/* variabls exposed */
extern az_pos_t az_probe_size;
extern az_pos_t az_probe_count;
extern az_probe_ref_t az_probe_last;
extern az_probe_t *az_probe_list[];

/* inline functions */
static inline az_probe_ref_t az_probe_register(char *tag, void *m,  
  char  *(*cb_f)(void *,...)) 
{
  az_probe_ref_t index = AZ_PROBE_REF_INVALID;
  do {
    az_probe_t *p;

    if (az_probe_last == az_probe_size) {
      break;
    }
    p = az_malloc(sizeof(az_probe_t));
  
    if (az_probe_last == az_probe_count) {
      index = az_probe_last++;
      az_probe_count++;
    } else {
      for (index = 0; index < az_probe_last; index++) {
        if (az_probe_list[index] == NULL) {
          az_probe_count++;
          break;
        }
      }
      if (az_probe_last == index) {
        index = az_probe_last++;
        az_probe_count++;
      }
    }
    p->tag = tag;
    p->module = m;
    p->cb_f = cb_f;
    az_probe_list[index] = p;
  } while (0);

  return index;
}
static inline void az_probe_deregister(az_probe_ref_t index)
{
  do {
    az_probe_t *p;

    if (index >= az_probe_last) break;
    p = az_probe_list[index];
    if (p == NULL) break;
    az_probe_list[index] = NULL;
    az_free(p);
    if (index == (az_probe_last-1)) {
      az_probe_last--;
    }
    az_probe_count--;
  } while (0);
}

#define AZ_PROBE(_id, _loc, ...) \
  do { \
    if (_id >= az_probe_last) break; \
    az_probe_t *_p = az_probe_list[_id]; \
    if (NULL == _p->cb_f) break;\
    char *_info = (_p->cb_f)(_p->module, _loc, __VA_ARGS__); \
    az_printf("%s\n", _info); \
  } while (0);

#define AZ_PROBE_PRINTF(_id, _loc, ...) \
  do { \
    char _buffer[128];\
    if (_id >= az_probe_last) break; \
    az_probe_t *_p = az_probe_list[_id]; \
    if (snprintf(_buffer, sizeof(_buffer), __VA_ARGS__) <= 0) break;\
    az_printf("%s: (%d) %s\n", _p->tag, _loc, _buffer); \
  } while (0);


/* function prototypes exposed */

#ifdef __cplusplus
}
#endif
#endif
