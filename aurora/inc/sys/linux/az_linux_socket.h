/**
 * @file   az_linux_socket.h
 * @brief  
 * @date   26/07/16
 * @author fware
 *
 * Copyright (c) Fware, 2013-2017 - All Rights Reserved
 * You may use, distribute and modify this code under the 
 * terms of the "Aurora Source Code License".
 * See the file LICENSE for full license details.

 * You should have received a copy of  the LICENSE with this file.

 * If not, please contact to Fware with the information in the file CONTACTS.
 *
 * History: @date 
 *   - 2016-07-26 fware initial create
 */

#ifndef AZ_LINUX_SOCKET_H
#define AZ_LINUX_SOCKET_H
#include "az_sys_linux.h"
#include "az_def.h"
#include "az_err.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* constants */

/* basic macros */

/* basic types */
#ifndef AZ_LINUX_DEF_H
typedef int               az_sys_socket_t;
typedef socklen_t         az_sys_socklen_t;
typedef struct sockaddr   az_sys_sockaddr_t;
#define AZ_SYS_SOCKET_INVALID   (-1)
#endif

/* structures */

/* structured types */

/* macros */
#define az_sys_inetIP(p) (((az_sys_inet_t *)(p))->sa4.sin_addr.s_addr)

#define az_sys_inetPort(p) ((((az_sys_inet_t *)(p))->sa4.sin_port)

/* variabls exposed */

/* inline functions */
static inline az_r_t az_sys_socket_create(int domain, int type, int protocol, az_sys_socket_t *p)
{
  az_r_t r = AZ_SUCCESS;
  az_assert(NULL != p);

  int sd = socket(domain, type, protocol);
  if (sd < 0) {
    *p = AZ_SYS_SOCKET_INVALID;
    r = AZ_ERR(CREAT);
  } else {
    *p = sd;
  }
  return r;
}
static inline az_r_t az_sys_socket_delete(az_sys_socket_t sd)
{
  az_r_t r = AZ_SUCCESS;
  if (sd != AZ_SYS_SOCKET_INVALID) {
    shutdown(sd, SHUT_RDWR);
    close(sd);
  } else {
    r = AZ_ERR(INVALID);
  }
  return r;
}
static inline az_r_t az_sys_socket_setopt(az_sys_socket_t sd,
    int level, int optname, const void *optval, az_sys_socklen_t optlen)
{
  return (az_r_t)setsockopt(sd, level, optname, optval, optlen);
}

static inline az_r_t az_sys_socket_getopt(az_sys_socket_t sd,
    int level, int optname, void *optval, az_sys_socklen_t *optlen)
{
  return (az_r_t)getsockopt(sd, level, optname, optval, optlen);
}

static inline az_r_t az_sys_socket_enblock(az_sys_socket_t sd)
{
  az_r_t r = AZ_SUCCESS;
  int flags = fcntl(sd, F_GETFL, 0);
  flags &= ~O_NONBLOCK;
  r = (az_r_t)fcntl(sd, F_SETFL, flags);

  return r;
}
static inline az_r_t az_sys_socket_nonblock(az_sys_socket_t sd)
{
  az_r_t r = AZ_SUCCESS;
  int flags = fcntl(sd, F_GETFL, 0);
  flags |= O_NONBLOCK;
  r = (az_r_t)fcntl(sd, F_SETFL, flags);

  return r;
}
static inline az_r_t az_sys_socket_nodelay(az_sys_socket_t sd)
{
  az_r_t r = AZ_SUCCESS;
  int onoff = 1;
  r = (az_r_t)setsockopt(sd, IPPROTO_TCP, TCP_NODELAY, (void *)&onoff, sizeof(onoff));
  return r; 
}

static inline az_r_t az_sys_socket_listen(az_sys_socket_t sd, int backlog)
{
  return (az_r_t)listen(sd, backlog);
}

static inline az_r_t az_sys_socket_bind(az_sys_socket_t sd, az_sys_sockaddr_t*addr, az_sys_socklen_t addrlen)
{
  return (az_r_t)bind(sd, addr, addrlen);
}

static inline az_r_t az_sys_socket_connect(az_sys_socket_t sd, az_sys_sockaddr_t*addr, az_sys_socklen_t addrlen)
{
  return (az_r_t)connect(sd, addr, addrlen);
}

static inline az_r_t az_sys_socket_accept(az_sys_socket_t sd, az_sys_sockaddr_t*addr, az_sys_socklen_t *addrlen)
{
  return (az_r_t)accept(sd, addr, addrlen);
}
static inline az_r_t az_sys_socket_write(az_sys_socket_t sd, const void* buf, az_size_t len)
{
  return (az_r_t)write(sd, buf, len);
}
static inline az_r_t az_sys_socket_send(az_sys_socket_t sd, const void* buf, az_size_t len, int flags)
{
  return (az_r_t)send(sd, buf, len, flags);
}

static inline az_r_t az_sys_socket_sendto(az_sys_socket_t sd, const void* buf, az_size_t len, int flags, const az_sys_sockaddr_t *dest_addr, az_sys_socklen_t addrlen)
{
  return (az_r_t)sendto(sd, buf, len, flags, dest_addr, addrlen);
}

static inline az_r_t az_sys_socket_read(az_sys_socket_t sd, void* buf, az_size_t len)
{
  return (az_r_t)read(sd, buf, len);
}
static inline az_r_t az_sys_socket_recv(az_sys_socket_t sd, void* buf, az_size_t len, int flags)
{
  return (az_r_t)recv(sd, buf, len, flags);
}
static inline az_r_t az_sys_socket_recvfrom(az_sys_socket_t sd, void* buf, az_size_t len, int flags, az_sys_sockaddr_t *src_addr, az_sys_socklen_t *addrlen)
{
  return (az_r_t)recvfrom(sd, buf, len, flags, src_addr, addrlen);
}

/* function prototypes exposed */


#ifdef __cplusplus
}
#endif
#endif
