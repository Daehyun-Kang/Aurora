/**
 * @file   az_inetUtils.c
 * @brief  
 * @date   13/03/18
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
 *   - 2018-03-13 dhkang initial create
 */

/* include header files */
#include "az_inet.h"
#include "az_printf.h"

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
int az_inet_interface_flags(char *ifname)
{
  struct ifreq  if_req;
  int result = 0;
  az_socket_id_t sd;
  do {
    result  = (int)az_sys_socket_create(AF_INET, SOCK_DGRAM, IPPROTO_IP, &sd);
    if (result < 0) break;

    (void)strncpy(if_req.ifr_name, ifname, sizeof(if_req.ifr_name));
    result = ioctl(sd, SIOCGIFFLAGS, &if_req);
    az_sys_socket_delete(sd);

    if (result < 0) break;

    result |= (if_req.ifr_flags);
  } while (0);

  return result;
}

int az_inet_is_interface_up(char *ifname)
{
  int result = 0;
  int flags = az_inet_interface_flags(ifname);
  do {
    if (flags < 0) break;
    if ((flags & IFF_UP) && (flags && IFF_RUNNING)) {
      result = 1;
    }
  } while (0); 
  return result;
}

int az_inet_enumerate_interfaces(struct ifaddrs **pIfList)
{
  struct ifaddrs *ifaddr, *ifa;
  int count = 0;

  do {
    if (getifaddrs(&ifaddr) == -1) {
      az_printf("getifaddrs: %d\n", az_sys_errno);
      if (pIfList) *pIfList = NULL;
      count = -1;
      break;
    }
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
      count++;
      if (ifa->ifa_addr == NULL) {
        continue;
      }
      if (ifa->ifa_addr->sa_family == AF_INET) {
        az_printf("%s addr %s %s\n", ifa->ifa_name,
            inet_ntoa(((struct sockaddr_in *)(ifa->ifa_addr))->sin_addr),
              az_inet_is_interface_up(ifa->ifa_name)? "UP":"DONW");
      }
    }
  } while (0);
  if (pIfList) *pIfList = ifaddr;

  return count;
}

void az_inet_free_interfaces(struct ifaddrs **pIfList)
{
  if (pIfList) {
    if (*pIfList) {
      freeifaddrs(*pIfList);
    }
    *pIfList = NULL;
  }
}

int az_inet_inet4_address(char *ifname, struct sockaddr_in *pAddr)
{
  struct ifaddrs *ifaddr, *ifa;
  int result = 0;

  do {
    if (getifaddrs(&ifaddr) == -1) {
      result = -1;
      break;
    }
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
      if (ifa->ifa_addr == NULL) continue;
      if (ifa->ifa_addr->sa_family == AF_INET) {
        if (pAddr) {
          *pAddr = *((struct sockaddr_in *)(ifa->ifa_addr));
        }
      }
    }
  } while (0);

  return result;
}


/* === end of AZ_INETUTILS_C === */
