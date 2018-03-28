/**
 * @file   az_linux_def.h
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

#ifndef AZ_LINUX_DEF_H
#define AZ_LINUX_DEF_H

#include "az_sys_linux.h"
#if defined(CONFIG_AZ_PRELOAD_SO)
#include "az_syscall_preload.h"
#endif

/* basic defines */
typedef time_t                    az_sys_time_t;
typedef struct timespec          az_sys_timespec_t;
typedef struct tm                az_sys_datetime_t;

static inline az_sys_datetime_t *az_sys_localtime(az_sys_time_t *t)
{
  return localtime(t);
}

#define AZ_SYS_NL        "\n"

#define az_sys_errno     errno
#define az_sys_errorstr() strerror(az_sys_errno)
#define az_sys_sockerror() errno 

#include "sys/az_sys_printf.h"

#include "az_xu_types.h"
#include "az_ion_types.h"
#include "az_err.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* atomic operations */
#include "az_atomic.h"

static inline az_ref_t az_sys_malloc(az_size_t size);
static inline void az_sys_free(az_ref_t ptr);
/* constants */

/* basic macros */

/* basic types */

typedef int az_sys_fd_t;
#define AZ_SYS_FD_INVALID    (-1)
#include "az_linux_fs.h"

//#ifndef AZ_SYS_IO_T_DEFINED
//#define AZ_SYS_IO_T_DEFINED
//typedef az_sys_fd_t         az_sys_io_t;
//#endif

typedef pthread_mutex_t *        az_sys_mutex_t; 
#define AZ_SYS_MUTEX_INVALID    NULL
#include "az_linux_mutex.h"

typedef sem_t *               az_sys_sem_t;
#define AZ_SYS_SEM_INVALID    NULL
#include "az_linux_sem.h"

#ifndef AZ_SYS_SHM_NAME_MAX
#ifdef CONFIG_AZ_NAME_MAX
#define AZ_SYS_SHM_NAME_MAX    CONFIG_AZ_NAME_MAX
#else
#define AZ_SYS_SHM_NAME_MAX    64
#endif
#endif

#ifndef AZ_SYS_PATH_DELIMITER
#define AZ_SYS_PATH_DELIMITER       "/"
#define AZ_SYS_PATH_SEPERATOR       '/' 
#endif

/* structures */

typedef struct {
  char              name[AZ_SYS_SHM_NAME_MAX];
  az_sys_fd_t      fd;
  az_ref_t         phyAddr;
  az_ref_t         virAddr;
  az_size_t        length;

  az_refcount_t    refCount;
} az_sys_shm_entity_t;

typedef az_sys_shm_entity_t*   az_sys_shm_t;
#define AZ_SYS_SHM_INVALID    NULL

/*
 * event related defines
 */
#ifndef AZ_LINUX_FUTEX_DEFINED
#define AZ_LINUX_FUTEX_DEFINED    1
static inline int futex(int *uaddr, int futex_op, int val,
    const struct timespec *timeout, int *uaddr2, int val3)
{
  return syscall(SYS_futex, uaddr, futex_op, val, timeout, uaddr2, val3);
}
#endif

#define AZ_SYS_EVENT_OPT_AND_ALL    1
#define AZ_SYS_EVENT_OPT_PRESERVE   2
typedef uint32_t az_sys_event_t;
#include "az_linux_event.h"


/*
 * XU related defines
 */
typedef struct {
  pthread_t       thread;
  void            *(*entry)(void *); 
  void            *arg;
  az_xu_attr_t    *attr;

  az_sys_ep_t     ep;
  sem_t           sem_suspend;
  az_sys_iomux_t  iomux;

  void            *priv;
  const char      *name;
  int             state;

  jmp_buf         env;
} az_sys_xu_entity_t;

typedef   az_sys_xu_entity_t *  az_sys_xu_t;
#define   AZ_SYS_XU_INVALID     NULL
#include  "az_linux_xu.h"


/*
 * timer related defines
 */
typedef struct {
  #ifdef  CONFIG_AZ_SYS_TIMER_USE_TIMER_SIG
  timer_t     timer;
  #else
  int         timer;
  #endif
  int             state;
  az_uint64_t     interval;
  int             repeat;
  void            (*handler)(void *);
  void            *arg;
  int32_t         owner;

  const char            *name;
} az_sys_timer_entity_t;
typedef az_sys_timer_entity_t*   az_sys_timer_t;
#define AZ_SYS_TIMER_INVALID    NULL
#include "az_linux_timer.h"


/*
 * define socket stuffs
 */
typedef int az_sys_socket_t;
typedef socklen_t         az_sys_socklen_t;
typedef struct sockaddr   az_sys_sockaddr_t;
#define AZ_SYS_SOCKET_INVALID   (-1)
#include "az_linux_socket.h"

typedef pid_t                    az_sys_pid_t;

/* structured types */

/* macros */

/* variabls exposed */

/* inline functions */

static inline az_ref_t az_attr_no_instrument az_sys_malloc(az_size_t size)
{
  az_ref_t mptr;
  extern az_size_t az_sys_malloc_count;

  #ifdef  CONFIG_AZ_USE_WRAP
  extern int __real_malloc(void **, size_t, size_t);
  if (__real_malloc(&mptr, sizeof(az_ref_t), size)) return NULL;
  #elif defined(CONFIG_AZ_PRELOAD_SO)
  extern void *(*az_real_malloc)(size_t size);
  if ((mptr = az_real_malloc(size)) == NULL) return NULL;
  #else
  if (posix_memalign(&mptr, sizeof(az_ref_t), size)) return NULL;
  #endif
  else {
    az_sys_malloc_count++;
    return mptr;
  }
}

static inline void az_attr_no_instrument az_sys_free(az_ref_t ptr)
{
  extern az_size_t az_sys_free_count;
  #ifdef  CONFIG_AZ_USE_WRAP
  extern void __real_free(void *);
  __real_free(ptr);
  #elif defined(CONFIG_AZ_PRELOAD_SO)
  extern void (*az_real_free)(void *);
  az_real_free(ptr);
  #else
  free(ptr);
  #endif
  az_sys_free_count++;
}

static inline char az_attr_no_instrument *az_getExeFileName()
{
  static char exefilename[PATH_MAX+1] = {0};
  if (exefilename[0] == 0) {
    ssize_t size = readlink("/proc/self/exe", exefilename, PATH_MAX);
    if (size >= 0) {
      exefilename[size] = 0;
      return exefilename;
    } else {
      return NULL;
    }
  } else {
    return exefilename;
  }
}
  


typedef void *  az_sys_lib_t;
#include "az_linux_dllib.h"

/* function prototypes exposed */

#ifdef __cplusplus
}
#endif
#endif
