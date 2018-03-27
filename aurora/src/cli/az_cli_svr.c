/**
 * @file   az_cli_svr.c
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
#include "cli/az_cli.h"
#include "net/az_tcpserver.h"
#include "az_printf.h"
#include "az_malloc.h"

/* declare global variables */
struct az_tcpserver_oprs az_cli_svr_oprs;
az_tcpserver_t  az_cli_svr = {
  .state = 0,
  .config.name = "cliSvr",
  .config.ipAddrStr = NULL,
  .config.port = AZ_CLI_SVR_PORT,
  .config.backlog = 16,
  .config.flags = 0,
  .sock = AZ_SOCK_INVALID,
  .thread = NULL,
  .priv = NULL,
  .oprs = &az_cli_svr_oprs,
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
void az_cli_rshell_cleanup(az_cli_shell_t *pSh)
{
  //shutdown(pSh->rxport, SHUT_RDWR);
  //close(pSh->rxport);
  //az_sys_eprintf("%s ..." AZ_NL,pSh->name);
	az_inet_closeSocket(pSh->rxport);

  az_xu_sleep(1000000000);

  void *thread = pSh->thread;
  if (pSh->state & AZ_CLI_SHELL_STATE_DMALLOC) {
    az_free(pSh);
  }
  fflush(stdout);
  az_cli_thread_stop(thread);
}
int az_cli_svr_onClientConnection(void *ctx, az_sock_t cliSock, void *cliAddrIn)
{
  int r;
  az_cli_shell_t  *pRShell = NULL;
  char cliIpStr[32] = {0};
  struct sockaddr_in *cliAddr = (struct sockaddr_in *)cliAddrIn;

  az_inet_IpAddr2Str(cliAddr, cliIpStr, sizeof(cliIpStr));

  do {
    pRShell = az_malloc(sizeof(az_cli_shell_t));
    if (NULL == pRShell) {
      r = AZ_ERR(MALLOC);
      break;
    }
    pRShell->state = AZ_CLI_SHELL_STATE_DMALLOC;
    pRShell->thread = NULL;
    snprintf(pRShell->name, sizeof(pRShell->name), "cliSh%d", az_cli_shell_nr);
    az_cli_shell_init(pRShell, 
        cliSock, (az_cli_read_t)az_inet_tcpRead, 
        cliSock, (az_cli_write_t)az_inet_tcpWrite, az_cli_rshell_cleanup);
    r = az_cli_shell_start(pRShell);
  } while (0);
  //az_printf("remote cli shell%d on %d from %s:%u start...r:%d" AZ_NL, az_cli_shell_nr, cliSock, cliIpStr, AZ_INET_PORT(cliAddr), r); 

  return r;
}
/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
int az_cli_svr_start()
{
  int r = AZ_SUCCESS;
  az_cli_svr_oprs = az_tcpserver_oprs_default;
  az_cli_svr_oprs.onClientConnection = az_cli_svr_onClientConnection;
  do {
    if (NULL == az_tcpserver_create(&az_cli_svr, NULL, &az_cli_svr_oprs, NULL)) {
	  r = AZ_FAIL;
      az_rprintf(r, "cli server: %s create" AZ_NL, az_cli_svr.config.name);
      break;
    }
    r = (az_cli_svr.oprs->start)(&az_cli_svr);
  } while (0);
  az_rprintf(r, "cli server: %s start" AZ_NL, az_cli_svr.config.name);

  return r;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
int az_cli_svr_stop()
{
  int r;
  //az_printf("cli server: %s stop ..." AZ_NL, az_cli_svr.config.name);
  do {
    r = (az_cli_svr.oprs->stop)(&az_cli_svr);
    if (r < 0) break;
    r = az_tcpserver_delete(&az_cli_svr);
  } while (0);

  return r;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
uint16_t az_cli_getSvrPortNo()
{
  return az_cli_svr.config.port;
}


/* === end of AZ_CLI_SVR_C === */
