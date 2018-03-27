/**
 * @file   az_options.h
 * @brief  
 * @date   24/03/18
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
 *   - 2018-03-24 dhkang initial create
 */

#ifndef AZ_OPTIONS_H
#define AZ_OPTIONS_H
#include "az_def.h"
#include "az_err.h"
#include "az_kv.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* constants */

/* basic macros */
#define AZ_OPTIONS_VALUE_NONE   (char *)(-1)
#define AZ_OPTIONS_VALUE_EXIST  (char *)(1)

/* basic types */

/* structures */

/* structured types */

/* macros */

/* variabls exposed */

/* inline functions */

/* function prototypes exposed */
extern int az_options_parse(int argc, char *argv[], az_kv_t *listOptKV, int nOptKV);

#ifdef __cplusplus
}
#endif
#endif
