/**
 * @file   az_mcheck.h
 * @brief  
 * @date   08/03/18
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
 *   - 2018-03-08 dhkang initial create
 */

#ifndef AZ_MCHECK_H
#define AZ_MCHECK_H

#ifdef __cplusplus
extern "C"
{
#endif

/* constants */

/* basic macros */
#define AZ_MCHK_INFO_ITEM_COUNT_MAX   72 

/* basic types */

/* structures */
struct az_trace_info;
typedef struct az_trace_info az_trace_info_t;

typedef AZ_PACKED_STRUCT(az_mchk_info) {
  short code;
  unsigned short length;
  union {
    struct {
      unsigned long nTotal;
      struct {
        az_ref_t addr;
        az_ref_t call;
        uint32_t size;
      } list[AZ_MCHK_INFO_ITEM_COUNT_MAX];
    };
  }
} az_mchk_info_t;

#define AZ_MCK_INFO_LEN(lcnt)   \
  ( sizeof(az_mchk_info_t) \
  - sizeof(((az_mchk_info_t *)0)->list) \
  + sizeof(((az_mchk_info_t *)0)->list[0]) * (lcnt)) 

#define AZ_MCK_INFO_LIST_SZ(len) (AZ_MCHK_INFO_ITEM_COUNT_MAX - (sizeof(az_mchk_info_t) - (len))/sizeof(((az_mchk_info_t *)0)->list[0]))

/* structured types */

/* macros */

/* variabls exposed */

/* inline functions */

/* function prototypes exposed */
extern int az_mcheck_printMallocStats(char *bp, int blen);
extern az_r_t az_mcheck_init();
extern az_r_t az_mcheck_deinit();

#ifdef  CONFIG_AZ_MTRACE
extern void az_mcheck_disableMtrace();
extern int az_mcheck_mkMallocRpt(az_mchk_info_t *info);
extern int az_mcheck_mkFreeRpt(az_mchk_info_t *info);
#endif
#ifdef __cplusplus
}
#endif
#endif
