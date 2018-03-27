/**
 * @file   az_sys_def.h
 * @brief  
 * @date   05/02/16
 * @author Daehyun Kang
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
 *   - 2016-02-05 Daehyun Kang initial create
 */

#ifndef AZ_SYS_DEF_H
#define AZ_SYS_DEF_H

#include "az_def.h"

#ifdef  CONFIG_LINUX
#include  "sys/linux/az_linux_def.h"
#include  "sys/linux/az_linux_lock.h"
#endif

#ifdef  CONFIG_VXWORKS
#include  "sys/vxworks/az_vxworks_def.h"
#endif

#ifdef  CONFIG_WINDOWS
#include  "sys/windows/az_windows_def.h"
#include  "sys/windows/az_windows_lock.h"
#endif

#ifdef __cplusplus
extern "C"
{
#endif

/* constants */

/* basic macros */

#ifdef AZ_SYS_SEM_Q_FIFO
#define AZ_SEM_Q_FIFO    AZ_SYS_SEM_Q_FIFO
#else
#define AZ_SEM_Q_FIFO    1
#endif

#ifdef  AZ_SYS_SEM_Q_PRIORITY
#define  AZ_SEM_Q_PRIORITY AZ_SYS_SEM_Q_PRIORITY
#else
#define AZ_SEM_Q_PRIORITY    2
#endif

#ifdef  AZ_SYS_SEM_INVERSION_SAFE
#define  AZ_SEM_INVERSION_SAFE AZ_SYS_SEM_INVERSION_SAFE
#else
#define AZ_SEM_INVERSION_SAFE    4
#endif

/* basic types */

typedef union {
  struct {
    int32_t   options;
    int32_t   init_value;
  } Create;
} az_sys_sem_arg_t;
typedef az_sys_sem_arg_t AurSysSemArgT;

typedef union {
  struct {
    uint32_t  attr;
    char  name[AZ_SYS_SHM_NAME_MAX];
    int32_t   mode;
    az_size_t    size;
    az_ref_t     phyAddress;
  } Create;
  struct {
    uint32_t  attr;
  } Delete;
} az_sys_shm_arg_t;

//typedef az_sys_shm_arg_t;

typedef az_ref_t (*az_sys_thread_entry_t)(az_ref_t);
typedef az_sys_thread_entry_t AurSysThreadEntryT;

typedef union {
  struct {
    int32_t  policy;
    int32_t  priority;
    int32_t  newPriority;
  } SetPriority;
} az_sys_thread_priority_arg_t;
typedef az_sys_thread_priority_arg_t AurSysThreadPriorityArgT; 


typedef union {
  struct {
    void (*entry)(az_ref_t);
    az_ref_t arg;
  } Create;
  struct {
	  az_ref_t arg;
  } Delete;
} az_sys_process_arg_t;
typedef az_sys_process_arg_t AurSysProcessArgT; 

typedef union {
  struct {
    int32_t    domain;
    int32_t    type;
    int32_t    protocol;
  } Create;
  struct {
    int32_t    level;
    int32_t    optname;
    az_ref_t      optval;
  } SetOptions;
  struct {
    int32_t    backlog;
  } Listen;
  struct {
    struct sockaddr_in    sockaddr;
  } Connect;
  struct {
    struct sockaddr_in    sockaddr;
  } Accept;
  struct {
    struct sockaddr_in    sockaddr;
  } Bind;
} az_sys_socket_arg_t;
typedef az_sys_socket_arg_t AurSysSocketArgT;

typedef union az_sys_inet_address {
  struct sockaddr_in    sa4;
  //struct sockaddr_un    sau;
} az_sys_inet_address_t;
typedef az_sys_inet_address_t  AurSysInetAddressT;  

typedef void*       az_sys_lib_t;
typedef az_sys_lib_t   AurSysLibT;  


/* structured types */

/* macros */

/* variabls exposed */

/* inline functions */

/* function prototypes exposed */

#ifdef __cplusplus
}
#endif
#endif
