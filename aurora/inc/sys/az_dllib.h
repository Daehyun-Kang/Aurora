/**
 * @file   az_dllib.h
 * @brief  
 * @date   12/11/17
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
 *   - 2017-11-12 dhkang initial create
 */

#ifndef AZ_DLLIB_H
#define AZ_DLLIB_H

#include "az_def.h"
#include "az_err.h"
#include "az_macros.h"

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
extern az_r_t az_dllib_getSymbol(char *name, az_ref_t *pAddr);

#ifdef __cplusplus
}
#endif
#endif
