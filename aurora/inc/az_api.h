/**
 * @file   az_api.h
 * @brief  
 * @date   19/03/18
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
 *   - 2018-03-19 dhkang initial create
 */

#ifndef AZ_API_H
#define AZ_API_H

#include "az_def.h"
#include "az_api.h"

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
extern int az_open(const char *pathname, int flags, mode_t mode);
extern int az_creat(const char *pathname, mode_t mode);
extern int az_close(int fd);
extern ssize_t az_read(int fd, void *buf, size_t count);
extern ssize_t az_write(int fd, const void *buf, size_t count);
extern ssize_t az_send(int sockfd, const void *buf, size_t len, int flags);
extern ssize_t az_recv(int sockfd, void *buf, size_t len, int flags);

#ifdef __cplusplus
}
#endif
#endif
