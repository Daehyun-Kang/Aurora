/**
 * @file   az_inet.h
 * @brief  
 * @date   25/09/17
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
 *   - 2017-09-25 dhkang initial create
 */

#ifndef AZ_INET_H
#define AZ_INET_H

#include "az_sys.h"
#include "az_def.h"
#include "az_err.h"
#include "az_assert.h"
#include "az_macros.h"
#include "az_malloc.h"
#include "sys/az_socket.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* constants */

/* basic macros */

/* basic types */
typedef az_ion_id_t       az_sock_t;
#define AZ_SOCK_INVALID   AZ_ION_ID_INVALID

/* structures */

/* structured types */

/* macros */

#define AZ_INET_PORT(addr)  (((struct sockaddr_in *)(addr))->sin_port)

/* variabls exposed */

/* inline functions */
static inline char  *az_inet_IpAddr2Str(struct sockaddr_in *addr, char *ipAddrStr, int ipAddrStrLen)
{
  if (NULL == inet_ntop(AF_INET, &addr->sin_addr.s_addr, ipAddrStr, ipAddrStrLen-1)) {
    ipAddrStr[0] = 0;
  }
  ipAddrStr[ipAddrStrLen-1] = 0;

  return ipAddrStr;
}
static inline int az_inet_str2IpAddr(struct sockaddr_in *addr, char *ipAddrStr) 
{
  return inet_pton(AF_INET, ipAddrStr, &(addr->sin_addr.s_addr));
}

static inline int az_inet_isValidIpAddr(char *ipAddr)
{
  struct sockaddr_in sa;
  int result = inet_pton(AF_INET, ipAddr, &(sa.sin_addr));
  return result;
}

static inline int az_inet_openSocket(int domain, int type, int protocol, az_sock_t *p)
{
  return (int)az_sys_socket_create(domain, type, protocol, p);
}
static inline int az_inet_closeSocket(az_sock_t p)
{
  return (int)az_sys_socket_delete(p);
}

#ifdef  _WIN32
static inline int az_inet_setSocketNonBlockMode(az_sock_t sd) 
{
  return (int)az_sys_socket_nonblock(sd);
}
static inline int az_inet_setSocketBlockMode(az_sock_t sd) 
{
  return (int)az_sys_socket_enblock(sd);
}
static inline int az_inet_setTcpNodelay(az_sock_t sd)
{
  return (int)az_sys_socket_nodelay(sd);
}
#else
static inline int az_inet_setSocketNonBlockMode(az_sock_t sd) 
{
  int flags;
  int r = AZ_SUCCESS;

  flags = fcntl(sd, F_GETFL);
  if (flags == -1 || fcntl(sd, F_SETFL, flags|O_NONBLOCK) == -1) {
    r = AZ_FAIL;
  }
  return r;
}
static inline int az_inet_setSocketBlockMode(az_sock_t sd) 
{
  int flags;
  int r = AZ_SUCCESS;

  flags = fcntl(sd, F_GETFL);
  if (flags == -1 || fcntl(sd, F_SETFL, flags & ~O_NONBLOCK) == -1) {
    r = AZ_FAIL;
  }
  return r;
}

static inline int az_inet_setTcpNodelay(az_sock_t sd)
{
  int on = 1;
  int r = AZ_SUCCESS;
  if (setsockopt(sd, IPPROTO_TCP, TCP_NODELAY, (const char *)&on, sizeof(on)) < 0) {
    r = AZ_FAIL;
  }

  return r;
}
#endif
static inline int az_inet_setReuseAddr(az_sock_t sd)
{
  int on = 1;
  int r = AZ_SUCCESS;
  if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (const char *)&on, sizeof(on)) < 0) {
	  az_sys_eprintf("%d\n", az_sys_sockerror());
    r = AZ_FAIL;
  }

  return r;
}

static inline void az_inet_mkLocalSocketAddr(char *name, char *key1, char *key2, 
    az_sys_sockaddr_t *pAddr)
{
  az_assert(NULL != name);
  if (pAddr) {
    char _name[CONFIG_AZ_PATH_MAX];
    if (key1) {
      if (key2) {
        snprintf(_name, sizeof(_name), name, key1, key2);
      } else {
        snprintf(_name, sizeof(_name), name, key1);
      }
      name = _name;
    } 
    struct sockaddr_un *addr = (struct sockaddr_un *)pAddr;

    bzero(addr, sizeof(*addr));

    addr->sun_family = AF_UNIX;
    strcpy(addr->sun_path, name);
  }
}
static inline int az_inet_openLocalSocket(char *name, char *key1, char *key2, 
    az_sys_sockaddr_t *pAddr, az_sock_t *pSd)
{
  az_r_t r = AZ_FAIL;
  az_assert(NULL != name);
  az_assert(NULL != pSd);
  az_sock_t sd;
  char _name[CONFIG_AZ_PATH_MAX];
  do {
    r = az_sys_socket_create(AF_UNIX, SOCK_DGRAM, 0, &sd);
    if (r != AZ_SUCCESS) {
	    az_sys_eprintf("%d\n", az_sys_sockerror());
      break;
    }
    if (key1) {
      if (key2) {
        snprintf(_name, sizeof(_name), name, key1, key2);
      } else {
        snprintf(_name, sizeof(_name), name, key1);
      }
      name = _name;
    }
    unlink(name);

    struct sockaddr_un *addr = (struct sockaddr_un *)pAddr;
    struct sockaddr_un _addr;
    if (addr == NULL) addr = &_addr;

    az_inet_mkLocalSocketAddr(name, NULL, NULL, addr);

    r = az_sys_socket_bind(sd, (az_sys_sockaddr_t *)addr, sizeof(*addr));
    if (r < 0) {
	    az_sys_eprintf("%s: %d\n", name, az_sys_sockerror());
      az_sys_socket_delete(sd); 
      break;
    }
    *pSd = sd;
  } while (0);

  return (int)r;
}
static inline int az_inet_closeLocalSocket(az_sock_t *pSd)
{
  az_assert(NULL != pSd);
  az_r_t r = AZ_FAIL;
  az_sock_t sd = *pSd;
  *pSd = AZ_SOCK_INVALID;
  r = az_sys_socket_delete(sd); 
  return (int)r;
}
static inline int az_inet_connectLocalSocket(az_sock_t sd, az_sys_sockaddr_t *addr, az_sys_socklen_t addrlen)
{
  az_r_t r = AZ_FAIL;
  r = az_sys_socket_connect(sd, addr, addrlen);
  if (r < 0) {
	    az_sys_eprintf("%d\n", az_sys_sockerror());
  }
  return (int)r;
}

/* function prototypes exposed */
extern int az_inet_openTcpClient(char *svrIpStr, uint16_t svrPort, char *cliIpStr, uint16_t cliPort, az_sock_t *pSock);
extern int az_inet_closeTcpClient(az_sock_t sd);

extern int az_inet_openTcpServer(char *svrIpStr, uint16_t svrPort, az_sock_t *pSock);
extern int az_inet_closeTcpServer(az_sock_t sd);
extern int az_inet_getTcpConnection(az_sock_t svrSock, az_sock_t *pCliSock, struct sockaddr_in *pCliAddr);

extern int az_inet_tcpRead(az_sock_t sock, uint8_t *bp, ssize_t len);
extern int az_inet_tcpWrite(az_sock_t sock, uint8_t *bp, ssize_t len);
#ifdef __cplusplus
}
#endif
#endif
