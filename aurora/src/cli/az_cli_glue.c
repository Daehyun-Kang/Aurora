/**
 * @file   az_cli_glue.c
 * @brief  
 * @date   24/09/17
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
 *   - 2017-09-24 dhkang initial create
 */

/* include header files */
#include "aurora.h"
#include "cli/az_cli.h"

/* declare global variables */
int az_cli_shell_nr = 0;
az_cli_shell_t  *az_cli_shell_list[AZ_CLI_SHELL_MAX] = {0};

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
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
int az_cli_shell_add(az_cli_shell_t *pSh)
{
  int j = 0;
  int r = AZ_SUCCESS;
  int sz = array_size(az_cli_shell_list);
  do {
    for (j = 0; j < sz; j++) {
      if (az_cli_shell_list[j] == pSh) {
        r = AZ_ERR(AGAIN);
        break;
      }
    }
    if (AZ_SUCCESS != r) break;
    r = AZ_ERR(FULL);
    for (j = 0; j < sz; j++) {
      if (az_cli_shell_list[j] == NULL) {
        az_cli_shell_list[j] = pSh; 
        az_cli_shell_nr++;
        r = AZ_SUCCESS;
        break;
      }
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
int az_cli_shell_del(az_cli_shell_t *pSh)
{
  int j = 0;
  int r = AZ_ERR(NOT_FOUND);
  int nr = az_cli_shell_nr;
  int sz = array_size(az_cli_shell_list);
  do {
    for (; j < sz && nr > 0; j++) {
      if (az_cli_shell_list[j] == NULL) continue; 
      nr--;
      if (az_cli_shell_list[j] == pSh) {
        az_cli_shell_list[j] = NULL; 
        az_cli_shell_nr--;
        r = AZ_SUCCESS;
        break;
      }
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
int az_cli_shell_bcast(az_cli_shell_t *pSh, char *dp, int len)
{
  int j = 0;
  int r = AZ_ERR(NOT_FOUND);
  int nr = az_cli_shell_nr;
  int sz = array_size(az_cli_shell_list);
  az_cli_shell_t *pRsh;
  
  do {
    for (j = 0; j < sz && nr > 0; j++) {
      pRsh = az_cli_shell_list[j];
      //az_sys_printf("%s: %p %p\n", __FUNCTION__, pRsh, pSh);
      if (pRsh == NULL) {
        continue; 
      }
      nr--;
      if (pRsh == pSh) {
        continue;
      }
      if (pRsh->state & AZ_CLI_SHELL_STATE_RXON) {
        //az_sys_printf("%s:%s\n", __FUNCTION__, dp);
        pRsh->Write(pRsh->txport, dp, len); 
      } else {
        //az_sys_printf("%s: %s\n", __FUNCTION__, dp);
      }
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
int az_cli_shell_bcast_stdout(char *dp, int len)
{
  int j = 0;
  int r = AZ_ERR(NOT_FOUND);
  int nr = az_cli_shell_nr;
  int sz = array_size(az_cli_shell_list);
  az_cli_shell_t *pRsh;
  do {
    for (; j < sz && nr > 0; j++) {
      pRsh = az_cli_shell_list[j]; 
      if (pRsh == NULL) {
        continue; 
      }
      nr--;
      if (pRsh->state & AZ_CLI_SHELL_STATE_RXSTDOUT) {
        //az_sys_printf("%s:%s\n", __FUNCTION__, dp);
        pRsh->Write(pRsh->txport, dp, len); 
      }
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
int az_cli_shell_enable_bcast(az_cli_shell_t *pSh, int onoff)
{
  az_assert(NULL != pSh);
  if (onoff) {
    pSh->state |= AZ_CLI_SHELL_STATE_BCAST;
  } else {
    pSh->state &= ~AZ_CLI_SHELL_STATE_BCAST;
  }

  return AZ_SUCCESS;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
int az_cli_shell_enable_rxon(az_cli_shell_t *pSh, int onoff)
{
  az_assert(NULL != pSh);
  if (onoff) {
    pSh->state |= AZ_CLI_SHELL_STATE_RXON;
  } else {
    pSh->state &= ~AZ_CLI_SHELL_STATE_RXON;
  }

  return AZ_SUCCESS;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
int az_cli_shell_enable_rxstdout(az_cli_shell_t *pSh, int onoff)
{
  az_assert(NULL != pSh);
  if (onoff) {
    pSh->state |= AZ_CLI_SHELL_STATE_RXSTDOUT;
  } else {
    pSh->state &= ~AZ_CLI_SHELL_STATE_RXSTDOUT;
  }

  return AZ_SUCCESS;
}

/**
 * @fn        function name
 * @brief     function-description
 * @param     input-output-parameters
 * @return    return-value
 * @warning   warnings
 * @exception none
 */
int az_cli_thread_start(void *(*entry_f)(az_cli_shell_t *),
    az_cli_shell_t *pSh)
{
  az_r_t  r;

  pSh->thread = NULL;
  r = (az_r_t)az_xu_create(pSh->name, (az_xu_entry_t)entry_f, pSh, NULL, (az_xu_t *)&(pSh->thread));
  az_sys_eprintf("xu:%p\n", pSh->thread);

  return (r < AZ_SUCCESS)? r:AZ_SUCCESS;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
int az_cli_thread_stop(void *thread)
{
  az_assert(NULL != thread);
  az_r_t r = AZ_ERR_L(ARG_NULL, 0);
  if (thread) {
    r = az_xu_stop(((az_xu_t)thread)->ion.id);
    r = az_xu_delete(((az_xu_t)thread)->ion.id);
  }

  return r;
}


az_sys_fd_t az_cli_shell_change_txport(az_cli_shell_t *pSh, az_sys_fd_t fd, int logOn)
{
  az_assert(NULL != pSh);

  az_sys_fd_t org_fd = pSh->txport;
  pSh->txport = fd;
  if (logOn) {
    pSh->state |= AZ_CLI_SHELL_STATE_LOG;
  } else {
    pSh->state &= ~AZ_CLI_SHELL_STATE_LOG;
  }

  return org_fd;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
int az_cli_printf(const char *fmt, ...)
{
  az_cli_shell_t *pSh = (az_cli_shell_t *)az_xu_getarg();
  //az_cli_shell_t *pSh = az_cli_thread_curShell();
  int r, len;
  va_list ap;
  az_assert(NULL != fmt);

  do {
    if (NULL == pSh) {
      r = AZ_ERR(NOT_FOUND);
      break;
    }
    if (NULL == pSh->Write) {
      break;
    }
    va_start(ap, fmt);
    len = vsnprintf(pSh->buffer, sizeof(pSh->buffer), fmt, ap);
    va_end(ap);
    if (len > 0) {
       r = pSh->Write(pSh->txport, pSh->buffer, len); 
      if (pSh->state & AZ_CLI_SHELL_STATE_BCAST) {
        az_cli_shell_bcast(pSh, pSh->buffer, len);
      } 
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
int az_cli_write(char *dp, int len)
{
  az_cli_shell_t *pSh = (az_cli_shell_t *)az_xu_getarg();
  //az_cli_shell_t *pSh = az_cli_thread_curShell();
  int r;
  //va_list ap;
  az_assert(NULL != dp);

  do {
    if (NULL == pSh) {
      r = AZ_ERR(NOT_FOUND);
      break;
    }
    if (NULL == pSh->Write) {
      break;
    }
    r = pSh->Write(pSh->txport, dp, len); 
    if (pSh->state & AZ_CLI_SHELL_STATE_BCAST) {
        az_cli_shell_bcast(pSh, dp, r);
    }
  } while (0);

  return r;
}


/* === end of AZ_CLI_GLUE_C === */
