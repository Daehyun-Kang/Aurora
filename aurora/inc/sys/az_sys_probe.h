/**
 * @file   az_sys_probe.h
 * @brief  
 * @date   16/04/18
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
 *   - 2018-04-16 dhkang initial create
 */

#ifndef AZ_SYS_PROBE_H
#define AZ_SYS_PROBE_H

#include "az_def.h"
#include "mon/az_probe.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* constants */
#define AZ_SYS_FLAG_PROBE_THREAD    CONFIG_AZ_PROBE_FLAG_THREAD  
#define AZ_SYS_FLAG_PROBE_TIMER     CONFIG_AZ_PROBE_FLAG_TIMER  
#define AZ_SYS_FLAG_PROBE_MUTEX     CONFIG_AZ_PROBE_FLAG_MUTEX 
#define AZ_SYS_FLAG_PROBE_SEM       CONFIG_AZ_PROBE_FLAG_SEM  
#define AZ_SYS_FLAG_PROBE_SOCKET    CONFIG_AZ_PROBE_FLAG_SOCKET  

/* basic macros */

/* basic types */

/* structures */

/* structured types */

/* macros */
#ifdef  CONFIG_AZ_PROBE_SYS
#define AZ_PROBE_SET_SYS(mode, a) \
  if (az_sys_probe_flags & AZ_SYS_FLAG_PROBE_##mode) { AZ_PROBE_SET(a);}

#define AZ_PROBE_INC_SYS(mode, a) \
  if (az_sys_probe_flags & AZ_SYS_FLAG_PROBE_##mode) { AZ_PROBE_INC(a);}

#define AZ_PROBE_DEC_SYS(mode, a) \
  if (az_sys_probe_flags & AZ_SYS_FLAG_PROBE_##mode) { AZ_PROBE_DEC(a);}
#else 
#define AZ_PROBE_SET_SYS(mode, a) 
#define AZ_PROBE_INC_SYS(mode, a) 
#define AZ_PROBE_DEC_SYS(mode, a) 
#endif

/* variabls exposed */
#ifdef  CONFIG_AZ_PROBE_SYS
extern uint32_t az_sys_probe_flags;
#endif

/* inline functions */

/* function prototypes exposed */

#ifdef __cplusplus
}
#endif
#endif

/* === end of AZ_SYS_PROBE_H === */
