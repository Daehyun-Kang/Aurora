/**
 * @file   az_linux_event.h
 * @brief  
 * @date   10/06/16
 * @author fware
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
 *   - 2016-06-10 fware initial create
 */

#ifndef AZ_LINUX_EVENT_H
#define AZ_LINUX_EVENT_H
#include <sys/epoll.h>
#include "az_assert.h"
#include "az_linux_lock.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* constants */

/* basic macros */
//#define az_sys_ep_printf(...)  az_sys_printf(__VA_ARGS__)
#define az_sys_ep_printf(...)  

/* basic types */
#ifndef AZ_LINUX_DEF_H
#define AZ_SYS_EVENT_OPT_AND_ALL    1
#define AZ_SYS_EVENT_OPT_PRESERVE   2
typedef uint32_t az_sys_event_t;
#endif

/* structures */

/* structured types */
typedef struct {
  az_sys_event_t   events;
  az_sys_event_t   revents;

  az_sys_fd_t      fd;

  az_sys_lw_lock_t  lock;
} az_sys_ep_t;
#define AZ_SYS_EP_WR_IO_EVT(ep) \
  if (ep->fd != AZ_SYS_FD_INVALID) {\
    uint64_t _v = 1;\
    write(ep->fd, &_v, sizeof(_v));\
  }
#define AZ_SYS_EP_RD_IO_EVT(ep) \
  if (ep->fd != AZ_SYS_FD_INVALID) {\
    uint64_t _v;\
    read(ep->fd, &_v, sizeof(_v));\
  }

typedef epoll_data_t        az_sys_io_event_data_t;
typedef struct epoll_event  az_sys_io_event_t;
#ifndef AZ_SYS_IO_T_DEFINED
#define AZ_SYS_IO_T_DEFINED
typedef az_sys_fd_t         az_sys_io_t;
#endif
typedef az_sys_fd_t         az_sys_iomux_t;

#define AZ_SYS_IO_INVALID      AZ_SYS_FD_INVALID
#define AZ_SYS_IOMUX_INVALID   AZ_SYS_FD_INVALID

/* macros */
#define AZ_SYS_IO_IN        EPOLLIN
#define AZ_SYS_IO_OUT       EPOLLOUT
#define AZ_SYS_IO_RDHUP     EPOLLRDHUP
#define AZ_SYS_IO_PRI       EPOLLPRI
#define AZ_SYS_IO_ERR       EPOLLERR
#define AZ_SYS_IO_HUP       EPOLLHUP
#define AZ_SYS_IO_ET        EPOLLET

#define AZ_SYS_IOMUX_TIMEOUT    -EINTR

/* variabls exposed */

/* inline functions */
#ifndef AZ_LINUX_FUTEX_DEFINED
#define AZ_LINUX_FUTEX_DEFINED    1
static inline futex(int *uaddr, int futex_op, int val,
    const struct timespec *timeout, int *uaddr2, int val3)
{
  return syscall(SYS_futex, uaddr, futex_op, val, timeout, uaddr2, val3);
}
#endif

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
static inline az_sys_io_t az_sys_io_create() 
{
  return eventfd(0, EFD_CLOEXEC|EFD_NONBLOCK); 
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
static inline void az_sys_io_delete(az_sys_io_t fd) 
{
  close(fd);
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
static inline void az_sys_ep_init(az_sys_ep_t *ep)
{
  ep->events = 0;
  ep->revents = 0;

  ep->fd = AZ_SYS_FD_INVALID;

  az_sys_lw_lock_init(&(ep->lock));
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
static inline void az_sys_ep_deinit(az_sys_ep_t *ep)
{
  ep->events = 0;
  ep->revents = 0;

  if (ep->fd != AZ_SYS_FD_INVALID) {
    int fd = ep->fd;
    ep->fd = AZ_SYS_FD_INVALID;

    az_sys_io_delete(fd); 
  }

  az_sys_lw_lock_deinit(&(ep->lock));
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
static inline az_r_t az_sys_ep_send(az_sys_ep_t *ep, az_sys_event_t sevent)
{
  az_r_t r;
  az_assert(NULL != ep);

  az_sys_lw_lock(&(ep->lock));
  az_atomic_fetch_and_or32(&(ep->events), sevent);
  r = (az_r_t)futex((int *)&(ep->events), FUTEX_WAKE, 1, NULL, NULL, 0);
  az_sys_lw_unlock(&(ep->lock));

  AZ_SYS_EP_WR_IO_EVT(ep)

  az_sys_ep_printf("ep=%p, event=%x result = %d\n", ep, sevent, r);

  return r;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
static inline az_r_t az_sys_ep_recv(az_sys_ep_t *ep, az_sys_event_t aevents, az_sys_event_t wevents, az_int8_t options, az_sys_event_t *qevent, az_sys_event_t *pevent)
{
  az_r_t r = AZ_ERR_L(EMPTY, 0);
  az_assert(NULL != ep);
  az_assert(NULL != pevent);
  az_sys_event_t revents = ep->revents;

  if (aevents) {
    az_assert(NULL != qevent);
    *qevent = (revents & aevents);
    if (*qevent) {
      revents &= ~aevents;
      r = AZ_SUCCESS;
    } 
  }
  az_sys_ep_printf("ep=%p, aevent=%x wevent=%x revents=%x\n", ep, aevents, wevents, revents);
  if (options & AZ_SYS_EVENT_OPT_AND_ALL) {
    if ((wevents & revents) == wevents) {
      *pevent = revents;
      if (options & AZ_SYS_EVENT_OPT_PRESERVE) {
        ep->revents = (revents & ~wevents); 
      } else {
        ep->revents = 0; 
      }
      r = AZ_SUCCESS;
    } 
  } else {
    if ((wevents & revents)) {
      *pevent = revents; 
      if (options & AZ_SYS_EVENT_OPT_PRESERVE) {
        ep->revents = (revents & ~wevents);
      } else {
        ep->revents = 0;  
      }
      r = AZ_SUCCESS;
    }
  }
  az_sys_ep_printf("ep=%p, wevent=%x revents=%x result=%d\n", ep, wevents, revents, r);
  return r;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
static inline az_r_t az_sys_ep_wait(az_sys_ep_t *ep, az_int64_t nsec)
{
  int r = AZ_FAIL;
  struct timespec tmo;
  az_assert(NULL != ep);
  az_sys_event_t sevents;

  az_sys_lw_lock(&(ep->lock));
  sevents = ep->events;
  if (sevents) {
    ep->events &= ~sevents; 
    ep->revents |= sevents;
    az_sys_lw_unlock(&(ep->lock));
    r = AZ_SUCCESS;
    AZ_SYS_EP_RD_IO_EVT(ep)
  } else { 
    az_sys_lw_unlock(&(ep->lock));

    while (nsec) {
      if (nsec > 0) {
        tmo.tv_sec = nsec/1000000000;
        tmo.tv_nsec = nsec%1000000000;
        r = futex((int *)&(ep->events), FUTEX_WAIT, 0, &tmo, NULL, 0);
      } else {
        r = futex((int *)&(ep->events), FUTEX_WAIT, 0, NULL, NULL, 0);
      }
      az_sys_ep_printf("ep=%p wait result = %d\n", ep, r);
      if (r < 0) {
        r = errno;
      }
      switch (r) {
        case EAGAIN:
          r = AZ_ERR(AGAIN);
        case 0:
          az_sys_lw_lock(&(ep->lock));
          sevents = ep->events;
          if (sevents) {
            ep->events &= ~sevents; 
            ep->revents |= sevents;
            az_sys_lw_unlock(&(ep->lock));
            // no race on ep->revent
            AZ_SYS_EP_RD_IO_EVT(ep)
            r = AZ_SUCCESS;
          } else {
            az_sys_lw_unlock(&(ep->lock));
            r = AZ_ERR(EMPTY);
          }
          break;
        case ETIMEDOUT:
          //az_sys_ep_printf("tmo %ld %d\n", nsec, (int)r);
          r = AZ_ERR(TIMEOUT);
          break;
        default:
          //if (errno == ETIMEDOUT) {
          //  r = AZ_ERR(TIMEOUT);
          //  az_sys_ep_printf("tmo %ld %d\n", nsec, (int)r);
          //} else {
            //az_sys_ep_printf("fail %ld %d\n", nsec, (int)r);
            r = AZ_ERR(FAIL);
        // }
          break;
      }
      break;
    } 
  }

  return (az_r_t)r;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
static inline az_r_t az_sys_ep_recv_wait(az_sys_ep_t *ep, az_int64_t nsec,
    az_sys_event_t wevents, az_int8_t options, az_sys_event_t *pevent)
{
  az_r_t r;

  do {
    r = az_sys_ep_wait(ep, nsec);
    if (AZ_SUCCESS != r) break;
    r = az_sys_ep_recv(ep, 0, wevents, options, NULL, pevent);
  } while (0);

  return r;
}

static inline az_sys_iomux_t az_sys_iomux_create()
{
  return epoll_create1(EPOLL_CLOEXEC);
}
static inline int az_sys_iomux_add(az_sys_iomux_t mux, az_sys_io_t fd, uint32_t events)
{
  az_sys_io_event_t ioevt;
  
  ioevt.events = events;
  ioevt.data.fd = fd;
  return epoll_ctl(mux, EPOLL_CTL_ADD, fd, &ioevt);
}
static inline int az_sys_iomux_del(az_sys_iomux_t mux, az_sys_io_t fd)
{
  az_sys_io_event_t ioevt;
  return epoll_ctl(mux, EPOLL_CTL_DEL, fd, &ioevt);
}

static inline int az_sys_iomux_wait(az_sys_iomux_t mux, az_sys_io_event_t *ioevt,
    int maxevents, int tmo_ms)
{
  int r = epoll_wait(mux, ioevt, maxevents, tmo_ms); 
  if (r < 0) {
    r = -az_sys_errno;
  }
  return r;
}

static inline void az_sys_iomux_add_ep(az_sys_iomux_t mux, az_sys_ep_t *ep)
{
  int r = AZ_FAIL;
  do {
    if (ep->fd == AZ_SYS_FD_INVALID) {
      ep->fd = az_sys_io_create(); 
    }
    if (ep->fd == AZ_SYS_FD_INVALID) {
      break;
    }
    r = az_sys_iomux_add(mux, ep->fd, AZ_SYS_IO_IN); 
  } while (0);
  return r;
}
static inline void az_sys_iomux_del_ep(az_sys_iomux_t mux, az_sys_ep_t *ep)
{
  int r = AZ_FAIL;
  do {
    if (ep->fd == AZ_SYS_FD_INVALID) {
      break;
    }
    r = az_sys_iomux_del(mux, ep->fd); 
    int fd = ep->fd;
    ep->fd = AZ_SYS_FD_INVALID;
    
    az_sys_io_delete(fd); 
  } while (0);
  return r;
}
/* function prototypes exposed */

#ifdef __cplusplus
}
#endif
#endif
