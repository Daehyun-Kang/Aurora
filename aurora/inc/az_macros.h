/**
 * @file   az_macros.h
 * @brief  
 * @date   10/04/16
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
 *   - 2016-04-10 fware initial create
 */

#ifndef AZ_MACROS_H
#define AZ_MACROS_H

#ifdef __cplusplus
extern "C"
{
#endif

/* constants */

/* basic macros */
#define az_if_arg_null_break(_v, _r) \
  if (NULL == (void *)_v) { _r = AZ_ERR(ARG_NULL); break; }

#define az_if_alloc_null_break(_v, _r) \
  if (NULL == (void *)_v) { _r = AZ_ERR(ALLOC); break; }

#define az_not_success_fware(_r) \
  if (AZ_SUCCESS != _r) { break; }

#define az_not_success_do_break(_do, _r) \
  if (AZ_SUCCESS != _r) { _do; break; }

/* basic types */

/* structures */

/* structured types */

/* macros */

/* variabls exposed */

/* inline functions */

/* function prototypes exposed */

#ifdef __cplusplus
}
#endif
#endif
