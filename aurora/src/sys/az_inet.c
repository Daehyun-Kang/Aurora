/**
 * @file   az_inet.c
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

/* include header files */
#include "sys/az_inet.h"

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

int az_inet_openTcpClient(char *svrIpStr, uint16_t svrPort, 
    char *cliIpStr, uint16_t cliPort, az_sock_t *pSock)
{
  az_sock_t sd = AZ_SOCK_INVALID;
  int r = AZ_FAIL;

  az_assert(NULL != svrIpStr);
  az_assert(NULL != pSock);

  struct  sockaddr_in  saddr;
  
  bzero(&saddr, sizeof(saddr));
  do {
    r = az_inet_str2IpAddr(&saddr, svrIpStr); 
    if (r < 0) break;

    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(svrPort);
    sd = az_socket_create(AF_INET, SOCK_STREAM, 0, NULL);
    if (sd < 0) {
      break;
    }
    if (NULL != cliIpStr && 0 != cliPort) {
      struct  sockaddr_in  caddr;
      saddr.sin_family = AF_INET;
      saddr.sin_port = htons(cliPort);
      r = az_inet_str2IpAddr(&caddr, cliIpStr); 
      if (r < 0) {
        az_socket_delete(sd);
        sd = AZ_SOCK_INVALID;
        break;
      }
      r = bind(sd, (struct sockaddr *)&saddr, sizeof(saddr));
      if (r < 0) {
        az_socket_delete(sd);
        sd = AZ_SOCK_INVALID;
        break;
      }
    }
    r = az_inet_setTcpNodelay(sd);
    if (r < 0) {
      az_socket_delete(sd);
      sd = AZ_SOCK_INVALID;
      break;
    }
    r = connect(sd, (struct sockaddr *)&saddr, sizeof(struct sockaddr_in));
    if (r < 0) {
      az_socket_delete(sd);
      sd = AZ_SOCK_INVALID;
      break;
    }
    r = AZ_SUCCESS;
  } while (0);

  *pSock = sd;

  return r;
}
int az_inet_closeTcpClient(az_sock_t sd)
{
  int r = AZ_SUCCESS;

  az_socket_delete(sd);

  return r;
}

int az_inet_openTcpServer(char *svrIpStr, uint16_t svrPort, az_sock_t *pSock)
{
  az_sock_t sd = AZ_SOCK_INVALID;
  int r = AZ_FAIL;

  az_assert(NULL != pSock);

  struct  sockaddr_in  saddr;
  
  bzero(&saddr, sizeof(saddr));
  do {
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(svrPort);
    if (NULL != svrIpStr) { 
      r = az_inet_str2IpAddr(&saddr, svrIpStr); 
      if (r < 0) break;
    } else {
      saddr.sin_addr.s_addr = htonl(INADDR_ANY);
    }

    sd = az_socket_create(AF_INET, SOCK_STREAM, 0, NULL);
    if (sd < 0) {
      break;
    }
    r = bind(sd, (struct sockaddr *)&saddr, sizeof(saddr));
    if (r < 0) {
      az_socket_delete(sd);
      sd = AZ_SOCK_INVALID;
      break;
    }
    r = az_inet_setReuseAddr(sd);
    if (r < 0) {
      az_socket_delete(sd);
      sd = AZ_SOCK_INVALID;
      break;
    }
    r = AZ_SUCCESS;
  } while (0);

  *pSock = sd;

  return r;
}

int az_inet_closeTcpServer(az_sock_t sd)
{
  int r = AZ_SUCCESS;

  az_socket_delete(sd);

  return r;
}
int az_inet_getTcpConnection(az_sock_t svrSock, az_sock_t *pCliSock, struct sockaddr_in *pCliAddr)
{
  struct sockaddr_in cliAddr;
  az_sock_t cliSock = AZ_SOCK_INVALID;
  int r = AZ_SUCCESS;
  socklen_t addrlen = sizeof(cliAddr);

  az_assert(NULL != pCliSock);

  do {
    cliSock = accept(svrSock, (struct sockaddr *)&cliAddr, &addrlen);
    if (cliSock < 0) {
      r = AZ_FAIL;
      break;
    }
    if (NULL != pCliAddr) {
      *pCliAddr = cliAddr;
    }
  } while (0);

  *pCliSock = cliSock;

  return r;
}

int az_inet_tcpRead(az_sock_t sock, uint8_t *bp, ssize_t len)
{
  fd_set fds;
  ssize_t rxlen;
  assert(sock != AZ_SOCK_INVALID);

  FD_ZERO(&fds);
  FD_SET(sock, &fds);

  if (select(sock + 1, &fds, NULL, NULL, NULL) > 0) {
    if (FD_ISSET(sock, &fds)) {
      rxlen = recv(sock, (void *)bp, len, 0);
#ifdef  _WIN32
      if (rxlen < 0) {
        az_sys_printf("error = %d\n", WSAGetLastError());
      }
#endif
    }
  }

  return (int)rxlen;
}
int az_inet_tcpWrite(az_sock_t sock, uint8_t *bp, ssize_t len)
{
  fd_set fds;
  struct timeval tv;
  ssize_t ret = -1;

  assert(sock != AZ_SOCK_INVALID);
  
  FD_ZERO(&fds);
  FD_SET(sock, &fds);
  tv.tv_sec = 1;
  tv.tv_usec = 0;

  if ((ret = select(sock + 1, NULL, &fds, NULL, &tv)) > 0) {
    if (FD_ISSET(sock, &fds)) {
      ret = send(sock, bp, len, 0);
    }
  }

  return ret;
}

/* === end of AZ_INET_C === */
