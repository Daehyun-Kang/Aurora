/**
 * @file   az_sighandle.h
 * @brief  
 * @date   11/04/18
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
 *   - 2018-04-11 dhkang initial create
 */

#ifndef AZ_SIGHANDLE_H
#define AZ_SIGHANDLE_H

#include "sys/az_sig.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* constants */
#define AZ_MAX_SIG_HANDLERS CONFIG_AZ_SIGHANDLE_MAX

/* basic macros */

/* basic types */

/* structures */

/* structured types */

/* macros */

/* variabls exposed */

/* inline functions */

/* function prototypes exposed */
extern int az_init_cleanup_sighandler();
extern int az_register_cleanup_sighandler(az_sighandler_t handler); 

#ifdef __cplusplus
}
#endif
#endif

/* === end of AZ_SIGHANDLE_H === */
