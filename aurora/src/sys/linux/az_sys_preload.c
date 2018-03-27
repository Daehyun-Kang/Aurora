/**
 * @file   az_sys_preload.c
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

#define  ALLOC_EXTERN  
#include  "az_syscall_preload.h"
#undef  ALLOC_EXTERN

#ifdef  CONFIG_AZ_PRELOAD_SO

/* declare global variables */
//asymbol **az_symtab = NULL;
//az_size_t az_nsyms = 0;
void *(*az_real_malloc)(size_t size) = NULL;
void (*az_real_free)(void *) = NULL;

int (*az_real_open)(const char *pathname, int flags, mode_t mode) = NULL;
int (*az_real_creat)(const char *pathname, mode_t mode) = NULL;
int (*az_real_close)(int fd) = NULL;
ssize_t (*az_real_write)(int fd, const void *buf, size_t count) = NULL;
ssize_t (*az_real_read)(int fd, void *buf, size_t count) = NULL;

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
int open(const char *pathname, int flags, ...)
{
  va_list argp;
  int r;
  mode_t mode;

  va_start(argp, flags);
  mode = va_arg(argp, mode_t);
  r = az_open(pathname, flags, mode);
  va_end(argp);

  return r;
}
int creat(const char *pathname, mode_t mode)
{
  return az_creat(pathname, mode);
}
int close(int fd)
{
  return az_close(fd);
}

ssize_t read(int fd, void *buf, size_t count)
{
  return az_read(fd, buf, count);
}

ssize_t write(int fd, const void *buf, size_t count)
{
  return az_write(fd, buf, count);
}


#endif
/* === end of AZ_SYS_PRELOAD_C === */
