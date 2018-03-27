/**
 * @file   az_rstdio.c
 * @brief  
 * @date   12/03/18
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
 *   - 2018-03-12 dhkang initial create
 */

/* include header files */
#include "aurora.h"
#include "net/az_tcpserver.h"
//#include "mon/az_rstdio.h"

typedef struct az_rstdio_ctrl {
  int state;
  az_sock_t   read_fd;
  az_sock_t   write_fd;
  az_sys_fd_t log_fd;

  char      svrIpStr[CONFIG_AZ_NAME_MAX];
  uint16_t  svrPort;
} az_rstdio_ctrl_t;

/* declare global variables */
az_rstdio_ctrl_t az_rstdio_ctrl = {
  .state = 0,
  .read_fd = AZ_SOCK_INVALID,
  .write_fd = AZ_SOCK_INVALID,
  .log_fd = AZ_SYS_FD_INVALID,
  //.svrIpStr = "127.0.0.1",
  .svrIpStr = "192.168.9.32", //"127.0.0.1",
  .svrPort = CONFIG_AZ_REMOTE_STDIO_SVR_PORT,
};

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
//void  __attribute__((constructor)) az_rstdio_begin(void);
void  az_rstdio_begin(void)
{
  char *name; 
  az_rstdio_ctrl_t *ctrl = &az_rstdio_ctrl;
  do {
    az_sys_socket_t cliSock;
    int r = az_inet_openTcpClient(ctrl->svrIpStr, ctrl->svrPort,
        NULL, 0, &cliSock);
    if (r != AZ_SUCCESS) {
      az_sys_eprintf("connect to remote stdio %s:%u error = %d" AZ_NL, 
          ctrl->svrIpStr, ctrl->svrPort, r);
      break;
    } else {
      az_sys_eprintf("connect to remote stdio %s:%u ok" AZ_NL, 
          ctrl->svrIpStr, ctrl->svrPort);
      r = az_sys_dup2(cliSock, STDOUT_FILENO);
      if (r < 0) {
        az_sys_eprintf("dup2 error=%d on stdout" AZ_NL, r); 
      }
      r = az_sys_dup2(cliSock, STDIN_FILENO);
      if (r < 0) {
        az_sys_eprintf("dup2 error=%d on stdin" AZ_NL, r); 
      }
      ctrl->state = 1;
    }
  } while (0);
  //az_sys_eprintf0("begin\n");
}


/* === end of AZ_RSTDIO_C === */
