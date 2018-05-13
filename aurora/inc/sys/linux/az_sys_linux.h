/**
 * @file   az_sys_linux.h
 * @brief  
 * @date   05/02/16
 * @author Daehyun Kang
 *
 * Copyright (c) Fware, 2013-2017 - All Rights Reserved
 * You may use, distribute and modify this code under the 
 * terms of the "Aurora Source Code License".
 * See the file LICENSE for full license details.

 * You should have received a copy of  the LICENSE with this file.

 * If not, please contact to Fware with the information in the file CONTACTS.
 *
 * History: @date 
 *   - 2016-02-05 Daehyun Kang initial create
 */

#ifndef AZ_SYS_LINUX_H
#define AZ_SYS_LINUX_H

#ifndef __USE_GNU
#define __USE_GNU
#endif

#define __GNU_SOURCE

#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#ifdef  CONFIG_AZ_WCHAR
#include <locale.h>
#include <wchar.h>
#endif
#include <signal.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <utmpx.h>
#include <pwd.h>
#include <grp.h>
#include <limits.h>
#include <dirent.h>
#include <dlfcn.h>
#include <semaphore.h>
#include <pthread.h>
#include <assert.h>
#include <setjmp.h>
#include <bfd.h>

#include <sched.h>
#include <poll.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

#include <sys/types.h>
#include <sys/eventfd.h>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <linux/futex.h>
#include <sys/time.h>
#include <sys/timerfd.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <sys/ptrace.h>
#include <x86_64-linux-gnu/sys/user.h>
#include <x86_64-linux-gnu/sys/debugreg.h>
//#include <instrinsics.h>

#ifdef __cplusplus
extern "C"
{
#endif

/* constants */

/* basic macros */
#define AZ_SYS_TLS  __thread

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
