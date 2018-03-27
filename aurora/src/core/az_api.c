/**
 * @file   az_api.c
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
int az_open(const char *pathname, int flags, mode_t mode)
{
  az_file_t file = NULL;
  az_r_t  r = -1;
  r = az_fs_openFile(pathname, flags, mode, &file);
  if (r == AZ_SUCCESS) {
    r = (az_r_t) file->ion.id;
  }
  return (int)r;
}
int az_creat(const char *pathname, mode_t mode)
{
  az_file_t file = NULL;
  az_r_t  r = -1;
  do {
    r = az_fs_createFile(pathname, mode, &file);
    if (r == AZ_SUCCESS) {
      r = (az_r_t) file->ion.id;
    }
  } while (0);

  return (int)r;
}
int az_close(int fd)
{
  extern int az_sys_close(int);
  az_ion_t *ion = az_ion(fd); 
  int r = -1;
  if (ion) {
    az_assert(fd == ion->id);
    r = az_sys_close(fd);
  }
  return r;
}

ssize_t az_read(int fd, void *buf, size_t count)
{
  extern ssize_t az_sys_read(int fd, void *buf, size_t count);
  az_ion_t *ion = az_ion(fd); 
  ssize_t r = -1;
  if (ion) {
    az_assert(fd == ion->id);
    r = az_sys_read(fd, buf, count);
  }
  return r;
}

ssize_t az_write(int fd, const void *buf, size_t count)
{
  extern ssize_t az_sys_write(int fd, const void *buf, size_t count);
  az_ion_t *ion = az_ion(fd); 
  ssize_t r = -1;
  if (ion) {
    az_assert(fd == ion->id);
    r = az_sys_write(fd, buf, count);
  }
  return r;
}

ssize_t az_send(int sockfd, const void *buf, size_t len, int flags)
{
  extern ssize_t az_sys_send(int sockfd, const void *buf, size_t len, int flags);
  az_ion_t *ion = az_ion(sockfd); 
  ssize_t r = -1;
  if (ion) {
    az_assert(sockfd == ion->id);
    r = az_sys_send(sockfd, buf, len, flags);
  }
  return r;
}

ssize_t az_recv(int sockfd, void *buf, size_t len, int flags)
{
  extern ssize_t az_sys_recv(int sockfd, void *buf, size_t len, int flags);
  az_ion_t *ion = az_ion(sockfd); 
  ssize_t r = -1;
  if (ion) {
    az_assert(sockfd == ion->id);
    r = az_sys_recv(sockfd, buf, len, flags);
  }
  return r;
}

/* === end of AZ_API_C === */
