/**
 * @file   az_tcpserver_glue.h
 * @brief  
 * @date   25/09/17
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
 *   - 2017-09-25 dhkang initial create
 */

#ifndef AZ_TCPSERVER_GLUE_H
#define AZ_TCPSERVER_GLUE_H
#include "sys/az_xu.h"

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
static inline az_r_t az_tcpserver_thread_start(char *name,
    void *(*entry_f)(void *), void *arg, void **pThread)
{
  az_r_t r = (az_r_t)az_xu_create(name, entry_f, arg, NULL, (az_xu_t *)pThread);

  return (r < AZ_SUCCESS)? r:AZ_SUCCESS;
}
static inline az_r_t az_tcpserver_thread_stop(void *thread)
{
  az_r_t r = az_xu_delete(((az_xu_t)(thread))->ion.id);

  return r;
}

/* function prototypes exposed */

#ifdef __cplusplus
}
#endif
#endif
