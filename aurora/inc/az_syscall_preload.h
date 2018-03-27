/**
 * @file   az_syscall_preload.h
 * @brief  
 * @date   18/03/18
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
 *   - 2018-03-18 dhkang initial create
 */

#ifndef AZ_SYSCALL_PRELOAD_H
#define AZ_SYSCALL_PRELOAD_H

#include  "az_api.h"

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
extern void *(*az_real_malloc)(size_t size);
extern void (*az_real_free)(void *);

extern int (*az_real_open)(const char *pathname, int flags, mode_t mode);
extern int (*az_real_creat)(const char *pathname, mode_t mode);
extern int (*az_real_close)(int fd);
extern ssize_t (*az_real_write)(int fd, const void *buf, size_t count);
extern ssize_t (*az_real_read)(int fd, void *buf, size_t count);

#ifdef  ALLOC_EXTERN
void *(*az_real_malloc)(size_t size) = NULL;
void (*az_real_free)(void *) = NULL;

int (*az_real_open)(const char *pathname, int flags, mode_t mode) = NULL;
int (*az_real_creat)(const char *pathname, mode_t mode) = NULL;
int (*az_real_close)(int fd) = NULL;
ssize_t (*az_real_write)(int fd, const void *buf, size_t count) = NULL;
ssize_t (*az_real_read)(int fd, void *buf, size_t count) = NULL;
#endif

#ifdef __cplusplus
}
#endif
#endif
