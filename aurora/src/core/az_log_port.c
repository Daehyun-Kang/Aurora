/**
 * @file   az_log_port.c
 * @brief  
 * @date   25/02/18
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
 *   - 2018-02-25 dhkang initial create
 */

/* include header files */
#include "az_def.h"
#include "az_err.h"
#include "az_assert.h"
#include "az_macros.h"
#include "az_malloc.h"
#include "az_atomic.h"
#include "az_string.h"
#include "az_log.h"
#include "az_printf.h"
#include "sys/az_fs.h"

#ifdef CONFIG_AZ_LOG

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
int az_log_port_write2fd(az_log_port_t port, const char *bp, int len)
{
  return write(port->ep.fd, bp, len);
}
int az_log_port_write2file(az_log_port_t port, const char *bp, int len)
{
  return az_sys_fs_write(port->ep.file, bp, len);
}
int az_log_port_write2sock(az_log_port_t port, const char *bp, int len)
{
  return az_sys_socket_write(port->ep.sock, bp, len);
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
az_r_t az_log_port_addFdOutput(az_logid_t logid, az_sys_fd_t fd, az_log_port_t *pPort) 
{
  az_log_t *log = AZ_LOGS(logid);
  az_assert(NULL != log);
  az_assert(NULL != pPort);
  az_r_t r = AZ_SUCCESS;
  az_log_port_t port = *pPort;

  do {
    if (NULL == port) {
      port = az_malloc(sizeof(*port));
      port->type = AZ_LOG_PORT_TYPE_FD;
      *pPort = port;
    } else {
      az_assert(AZ_LOG_PORT_TYPE_FD == port->type);
    }
    if (port->state == AZ_LOG_PORT_ACTIVE) {
      r = AZ_ERR(AGAIN);
      break;
    }
    port->ep.fd = fd;
    port->state = AZ_LOG_PORT_EP_ON;
    port->Write = az_log_port_write2fd;
    r = az_log_add_port(log, port);
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
az_r_t az_log_port_delFdOutput(az_logid_t logid, az_log_port_t port) 
{
  az_log_t *log = AZ_LOGS(logid);
  az_assert(NULL != log);
  az_assert(NULL != port);
  az_r_t r = AZ_SUCCESS;

  az_assert(AZ_LOG_PORT_TYPE_FD == port->type);
  do {
    if (port->state != AZ_LOG_PORT_ACTIVE) {
      r = AZ_ERR(INVALID);
      break;
    }
    r = az_log_del_port(log, port);
    if (r == AZ_SUCCESS) {
      port->state = AZ_LOG_PORT_IDLE;
    }
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
az_r_t az_log_port_addFileOutput(az_logid_t logid, const char *path, az_log_port_t *pPort) 
{
  az_log_t *log = AZ_LOGS(logid);
  az_assert(NULL != log);
  az_assert(NULL != pPort);
  az_r_t r = AZ_SUCCESS;
  az_log_port_t port = *pPort;

  do {
    if (NULL == port) {
      port = az_malloc(sizeof(*port));
      port->type = AZ_LOG_PORT_TYPE_FILE;
      *pPort = port;
    }
    if (port->state == AZ_LOG_PORT_ACTIVE) {
      r = AZ_ERR(AGAIN);
      break;
    }
    if (port->ep.file == AZ_SYS_FILE_INVALID) {
      /*
      if (strrchr(path, AZ_PATH_SEPERATOR)) {
        char folder[AZ_PATH_MAX];
        az_fs_getBasePathComponent(path, folder);
        if (!az_fs_exist(folder)) {
          r = az_fs_createDir(folder, 0777);
          if (r != AZ_SUCCESS) {
            break;
          }
        }
      }
      */
      r = az_sys_fs_open(path, O_CREAT|O_RDWR, 0666, &port->ep.file);
      if (r != AZ_SUCCESS) {
        break;
      }
    } 
    az_sys_eprintf("%s: %d\n", path, port->ep.file);
    port->Write = az_log_port_write2file;
    port->state = AZ_LOG_PORT_EP_ON;
    r = az_log_add_port(log, port);
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
az_r_t az_log_port_delFileOutput(az_logid_t logid, az_log_port_t port) 
{
  az_log_t *log = AZ_LOGS(logid);
  az_assert(NULL != log);
  az_assert(NULL != port);
  az_r_t r = AZ_SUCCESS;

  az_assert(AZ_LOG_PORT_TYPE_FILE == port->type);
  do {
    if (port->state != AZ_LOG_PORT_ACTIVE) {
      r = AZ_ERR(INVALID);
      break;
    }
    r = az_log_del_port(log, port);
    if (r == AZ_SUCCESS) {
      az_sys_fs_close(port->ep.file);
      port->ep.file = AZ_SYS_FILE_INVALID;
      port->state = AZ_LOG_PORT_IDLE;
    }
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
az_r_t az_log_port_addSocketOutput(az_logid_t logid, az_sys_socket_t sock, az_log_port_t *pPort) 
{
  az_log_t *log = AZ_LOGS(logid);
  az_assert(NULL != log);
  az_assert(NULL != pPort);
  az_r_t r = AZ_SUCCESS;
  az_log_port_t port = *pPort;

  do {
    if (NULL == port) {
      port = az_malloc(sizeof(*port));
      port->type = AZ_LOG_PORT_TYPE_SOCK;
      *pPort = port;
    }
    if (port->state == AZ_LOG_PORT_ACTIVE) {
      r = AZ_ERR(AGAIN);
      break;
    }
    port->ep.sock = sock;
    port->Write = az_log_port_write2sock;
    port->state = AZ_LOG_PORT_EP_ON;
    r = az_log_add_port(log, port);
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
az_r_t az_log_port_delSocketOutput(az_logid_t logid, az_log_port_t port) 
{
  az_log_t *log = AZ_LOGS(logid);
  az_assert(NULL != log);
  az_assert(NULL != port);
  az_r_t r = AZ_SUCCESS;

  az_assert(AZ_LOG_PORT_TYPE_SOCK == port->type);
  do {
    if (port->state != AZ_LOG_PORT_ACTIVE) {
      r = AZ_ERR(INVALID);
      break;
    }
    r = az_log_del_port(log, port);
    if (r == AZ_SUCCESS) {
      az_sys_fs_close(port->ep.file);
      port->ep.sock = AZ_SYS_SOCKET_INVALID;
      port->state = AZ_LOG_PORT_IDLE;
    }
  } while (0);

  return r;
}

#endif

/* === end of AZ_LOG_PORT_C === */
