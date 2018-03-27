/**
 * @file   az_sys.c
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

/* include header files */
#include "az_def.h"
#include "aurora.h"
#include  "az_syscall_preload.h"

/* declare global variables */

/* declare static variables */


/* implement static functions */

/**
 * @fn        function name
 * @brief     function-description
 * @param     input-output-parameters
 * @return    return-value
 * @warning   warnings
 * @exception none
 */


/* implement global functions */

/**
 * @fn        function name
 * @brief     function-description
 * @param     input-output-parameters
 * @return    return-value
 * @warning   warnings
 * @exception none
 */
int az_sys_open(const char *pathname, int flags, mode_t mode)
{
  int fd;
  do {
    #ifdef CONFIG_AZ_PRELOAD_SO
    fd = az_real_open(pathname, flags, mode);
    #else
    fd = open(pathname, flags, mode);
    #endif
  } while (0);
  return fd;
}
int az_sys_creat(const char *pathname, mode_t mode)
{
  int fd;
  do {
    #ifdef CONFIG_AZ_PRELOAD_SO
    fd = az_real_creat(pathname, mode);
    #else
    fd = creat(pathname, mode);
    #endif
  } while (0);

  return fd; 
}
int az_sys_close(int fd)
{
  int r = -1;
  do {
    #ifdef CONFIG_AZ_PRELOAD_SO
    r = az_real_close(fd);
    #else
    r = close(fd);
    #endif
  } while (0);
  return r; 
}

ssize_t az_sys_read(int fd, void *buf, size_t count)
{
  ssize_t r = -1;
  do {
    #ifdef CONFIG_AZ_PRELOAD_SO
    r = az_real_read(fd, buf, count);
    #else
    r = read(fd, buf, count);
    #endif
  } while (0);
  return r; 
}

ssize_t az_sys_write(int fd, const void *buf, size_t count)
{
  ssize_t r = -1;
  do {
    #ifdef CONFIG_AZ_PRELOAD_SO
    r = az_real_write(fd, buf, count);
    #else
    r = write(fd, buf, count);
    #endif
  } while (0);
  return r; 
}

ssize_t az_sys_send(int sockfd, const void *buf, size_t len, int flags)
{
  ssize_t r = -1;
  do {
    #ifdef CONFIG_AZ_PRELOAD_SO
    r = az_real_send(sockfd, buf, len, flags);
    #else
    r = send(sockfd, buf, len, flags);
    #endif
  } while (0);
  return r;
}

ssize_t az_sys_recv(int sockfd, void *buf, size_t len, int flags)
{
  ssize_t r = -1;
  do {
    #ifdef CONFIG_AZ_PRELOAD_SO
    r = az_real_recv(sockfd, buf, len, flags);
    #else
    r = recv(sockfd, buf, len, flags);
    #endif
  } while (0);
  return r;
}

/* === end of AZ_SYS_C === */
