/**
 * @file   az_debug.h
 * @brief  
 * @date   16/03/18
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
 *   - 2018-03-16 dhkang initial create
 */

#ifndef AZ_DEBUG_H
#define AZ_DEBUG_H

#ifdef __cplusplus
extern "C"
{
#endif

/* constants */

/* basic macros */

/* basic types */

/* structures */

/* structured types */

/* macros */

/* variabls exposed */

/* inline functions */

/* function prototypes exposed */
extern int az_debug_init_watchpoint();
extern int az_debug_set_watchpoint(int bpno, void *addr, int var_size);
extern int az_debug_reset_watchpoint(int bpno);

#ifdef __cplusplus
}
#endif
#endif
