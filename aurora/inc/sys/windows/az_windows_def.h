/**
 * @file   az_windows_def.h
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

#ifndef AZ_WINDOWS_DEF_H
#define AZ_WINDOWS_DEF_H

#include "az_sys_windows.h"
#include "az_ion_types.h"
#include "az_thread_types.h"
#include "az_err.h"
#include "az_assert.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* atomic operations */
#include "az_atomic.h"

/* constants */

/* basic macros */
#ifndef STDIN_FILENO
#define STDIN_FILENO    _fileno(stdin)
#endif
#ifndef STDOUT_FILENO
#define STDOUT_FILENO   _fileno(stdout) 
#endif
#ifndef STDERR_FILENO
#define STDERR_FILENO   _fileno(stderr) 
#endif

#ifndef SCHED_OTHER
#define SCHED_OTHER         0
#endif

#ifndef SCHED_FIFO
#define SCHED_FIFO         1
#endif

#ifndef SCHED_RR
#define SCHED_RR         2
#endif

/* basic types */

typedef int                     az_sys_fd_t;
#include "az_windows_fs.h"

typedef HANDLE         az_sys_mutex_t; 
#define AZ_SYS_MUTEX_INVALID    NULL
#include "az_windows_mutex.h"

typedef HANDLE                az_sys_sem_t;
#define AZ_SYS_SEM_INVALID    NULL
#include "az_windows_sem.h"

#ifndef AZ_SYS_SHM_NAME_MAX
#ifdef CONFIG_AZ_NAME_MAX
#define AZ_SYS_SHM_NAME_MAX    CONFIG_AZ_NAME_MAX
#else
#define AZ_SYS_SHM_NAME_MAX    64
#endif
#endif

#ifndef AZ_SYS_PATH_DELIMITER
#define AZ_SYS_PATH_DELIMITER       "\\"
#define AZ_SYS_PATH_SEPERATOR       '\\' 
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

typedef struct {
  int state;
  HANDLE    cond;
  HANDLE   mutex;
} az_sys_thread_wait_t;

/*
 * define windows event relevants
 */
#define AZ_SYS_EVENT_OPT_AND_ALL      1
#define AZ_SYS_EVENT_OPT_PRESERVE     2
typedef uint32_t az_sys_event_t;
#include  "az_windows_event.h"

/*
 * define windows xu relevants
 */
typedef struct {
  HANDLE        thread;
  void          *(*entry)(void *); 
  void          *arg;
  az_thread_attr_t    *attr;

  az_sys_ep_t   ep;

  DWORD      threadId;

  void          *priv;
  char          *name;
} az_sys_xu_entity_t;
typedef az_sys_xu_entity_t *  az_sys_xu_t;
#define AZ_SYS_XU_INVALID     NULL
#include  "az_windows_xu.h"

typedef int             az_sys_thread_key_t;

typedef az_ref_t                 az_sys_thread_value_t;


/*
 * define windows timer relevants
 */
#ifndef CLOCK_REALTIME
#define CLOCK_REALTIME  0
#endif
#ifndef CLOCK_MONOTONIC
#define CLOCK_MONOTONIC 4
#endif

typedef time_t                    az_sys_time_t;

typedef struct {
  HANDLE          timer;
  az_uint64_t     interval;
  int             repeat;
  void            (*handler)(void *);
  void            *arg;
  int32_t         owner;
  const char      *name;
  int             state;
} az_sys_timer_entity_t;
typedef az_sys_timer_entity_t*    az_sys_timer_t;
#define AZ_SYS_TIMER_INVALID      NULL
#include "az_windows_timer.h"

typedef struct timespec          az_sys_timespec_t;
typedef struct tm                az_sys_datetime_t;

/*
 * define socket stuffs
 */
typedef unsigned int  socklen_t; 
#ifndef SHUT_RDWR
#define SHUT_RD   SD_RECEIVE
#define SHUT_WR   SD_SEND
#define SHUT_RDWR SD_BOTH
#endif
typedef SOCKET            az_sys_socket_t;
typedef socklen_t         az_sys_socklen_t;
typedef struct sockaddr   az_sys_sockaddr_t;
#define AZ_SYS_SOCKET_INVALID   INVALID_SOCKET 

#define AZ_SYS_SOCK_T
typedef SOCKET               az_sys_sock_t;
#define AZ_SYS_SOCK_INVALID   INVALID_SOCKET 

#include "az_windows_socket.h"

typedef HANDLE                    az_sys_pid_t;


/* structured types */

/* macros */

/* variabls exposed */

/* inline functions */

#define bzero(s, n) memset((s), 0, (n))
#define bcopy(s1, s2, n)  memmove((s2), (s1), (n))

static inline az_ref_t az_sys_malloc(az_size_t size)
{
  az_ref_t mptr;
  extern az_size_t az_sys_malloc_count;
  //if (posix_memalign(&mptr, sizeof(az_ref_t), size)) return NULL;
  if (NULL == (mptr = malloc(size))) return NULL;
  else {
    az_sys_malloc_count++;
    return mptr;
  }
}

static inline void az_sys_free(az_ref_t ptr)
{
  extern az_size_t az_sys_free_count;
  free(ptr);
  az_sys_free_count++;
}
    
#define CLOCK_GETTIME_DEFINED   1

#define exp7           10000000i64
#define exp9         1000000000i64
#define w2ux 116444736000000000i64

static inline void unix_time(struct timespec *spec)
{
    __int64 wintime;
    FILETIME f;
    GetSystemTimeAsFileTime(&f);
    wintime = f.dwHighDateTime;
    wintime <<= 32;
    wintime |= f.dwLowDateTime;
    wintime -= w2ux;
    spec->tv_sec = wintime / exp7;
    spec->tv_nsec = (wintime % exp7) * 100;
}
  
static inline int clock_gettime(int clock_id, struct timespec *spec)
{
    static struct timespec startspec;
    static double ticks2nano;
    static __int64 starticks, tps = 0;
    __int64 curticks;

    if (0 == tps) {
        QueryPerformanceFrequency((LARGE_INTEGER *)&tps);
        QueryPerformanceCounter((LARGE_INTEGER *)&starticks);
        unix_time(&startspec);
        ticks2nano = (double)exp9 / tps;
    }
    QueryPerformanceCounter((LARGE_INTEGER *)&curticks);
    curticks -= starticks;
    spec->tv_sec = startspec.tv_sec + (curticks / tps);
    spec->tv_nsec = startspec.tv_nsec + (double)(curticks % tps) * ticks2nano;
    if (!(spec->tv_nsec < exp9)) {
        spec->tv_sec++;
        spec->tv_nsec -= exp9;
    }

    return 0;
}
#define az_sys_inetIP(p) (((az_sys_inet_t *)(p))->sa4.sin_addr.s_addr)

#define az_sys_inetPort(p) ((((az_sys_inet_t *)(p))->sa4.sin_port)

static inline void wtoc(char *dest, const wchar_t *src)
{
    int i;
    while (src[i] != '\0') {
        dest[i] = (char)src[i];
        ++i;
    }
}
static inline void ctow(wchar_t *dest, const char *src)
{
	int i;
	while (src[i] != '\0') {
		dest[i] = (wchar_t)src[i];
		++i;
	}
}
static inline char *az_sys_strerror(DWORD ecode)
{
    static char buf[256];
    FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, 
        NULL, ecode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
        buf, 256, NULL);

    return buf; 
}

#define AZ_SYS_NL        "\r\n"

#define az_sys_errno        GetLastError()
#define az_sys_errstr()     az_sys_strerror(az_sys_errno)
#define az_sys_sockerror()  WSAGetLastError() 

#define az_sys_printf(...) \
    printf(__VA_ARGS__)

#define az_sys_eprintf0(fmt) \
    printf("%s:%d " fmt, __FUNCTION__, __LINE__)

#define az_sys_eprintf(fmt, ...) \
    printf("%s:%d " fmt, __FUNCTION__, __LINE__, __VA_ARGS__)

/* function prototypes exposed */

#ifdef __cplusplus
}
#endif
#endif
