/**
 * @file   az_log_dup.c
 * @brief  
 * @date   26/09/17
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
 *   - 2017-09-26 dhkang initial create
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
#include "az_inet.h"
#include "cli/az_cli.h"

#ifdef CONFIG_AZ_LOG

#ifdef CONFIG_AZ_LOG_DUP_STDOUT

/* declare global variables */

/*
typedef struct az_log_dump_ctrl {
  char  *logfileprefix;

  FILE  *fh;
  az_sys_fd_t fd;
  az_sys_fd_t org_fd;
  az_sys_fd_t log_fd;
  az_sys_fd_t read_fd;
  az_sys_fd_t write_fd;

  int filecount;
  int sizelimit;

  char  logfilepath[AZ_PATH_MAX];
} az_log_dump_ctrl_t;
*/
typedef struct az_log_tap_cli_ctrl {
  az_sys_fd_t org_fd;
  az_sys_fd_t read_fd;
  az_sys_fd_t write_fd;
} az_log_tap_cli_ctrl_t;
az_log_tap_cli_ctrl_t az_log_tap_ctrl_default_cli = {
  .read_fd = AZ_SYS_FD_INVALID,
  .write_fd = AZ_SYS_FD_INVALID,
};

az_log_dump_ctrl_t az_log_stdout_dump_ctrl = {
  .logfileprefix = CONFIG_AZ_LOG_STDOUT_DUMP_FILE_PREFIX,
  .fh = NULL,
  .fd = STDOUT_FILENO,
  .org_fd = AZ_SYS_FD_INVALID,
  .log_fd = AZ_SYS_FD_INVALID,
  .read_fd = AZ_SYS_FD_INVALID,
  .write_fd = AZ_SYS_FD_INVALID,
  .filecount = 0,
  .sizelimit = CONFIG_AZ_LOG_STDOUT_DUMP_FILE_SIZE,
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
#ifdef CONFIG_AZ_LOG_DUP_USE_LOCAL_SOCKET
int az_log_tap_cli_shell_start(az_cli_shell_t *pSh, az_log_tap_cli_ctrl_t *ctrl)
{
  az_r_t r;
  do {
    /*
    char exename[AZ_NAME_MAX]; 
    az_fs_getLastPathComponent(az_getExeFileName(), exename);
    struct sockaddr_un svraddr;
    r = az_inet_openLocalSocket("/tmp/az_cli_%s_%s_svr", exename, pSh->name, &svraddr, &ctrl->read_fd);
    if (r < 0) {
      az_sys_eprintf("socket error=%d" AZ_NL, r); 
      break;
    }

    r = az_inet_openLocalSocket("/tmp/az_cli_%s_%s", exename, pSh->name, 
        NULL, &ctrl->write_fd);
    if (r < 0) {
      az_inet_closeLocalSocket(&ctrl->read_fd);
      az_sys_eprintf("socket error=%d" AZ_NL, r); 
      break;
    }
    r = az_inet_connectLocalSocket(ctrl->write_fd, (struct sockaddr *)&svraddr, sizeof(svraddr));
    if (r < 0) {
      az_sys_eprintf("connect error=%d\n", r );
      az_inet_closeLocalSocket(&ctrl->read_fd);
      az_inet_closeLocalSocket(&ctrl->write_fd);
      break;
    }
    */
    int sv[2];
    r = (az_r_t)socketpair(AF_LOCAL, SOCK_DGRAM|SOCK_NONBLOCK|SOCK_CLOEXEC, 0,
        sv);
    if (r < 0) {
      az_sys_eprintf("socketpair error=%d" AZ_NL, errno); 
      break;
    }
    ctrl->read_fd = sv[0];
    ctrl->write_fd = sv[1];

    ctrl->org_fd = az_cli_shell_change_txport(pSh, ctrl->write_fd, 1);

  } while (0);

  return (int)r;
}
int az_log_tap_cli_shell_stop(az_cli_shell_t *pSh, az_log_tap_cli_ctrl_t *ctrl)
{
  az_r_t r = AZ_SUCCESS;
  do {
    ctrl->org_fd = az_cli_shell_change_txport(pSh, ctrl->org_fd, 0);
    az_inet_closeLocalSocket(&ctrl->read_fd);
    az_inet_closeLocalSocket(&ctrl->write_fd);
  } while (0);

  return (int)r;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
int az_log_dump_start(az_log_dump_ctrl_t *ctrl, FILE *fh, az_sys_fd_t fd, void *priv)
{
  az_r_t r;
  ctrl->fh = fh;
  ctrl->org_fd = fd;
  ctrl->priv = priv;
  do {
    #ifdef CONFIG_AZ_LOG_STDOUT_DUMP_FILE
    snprintf(ctrl->logfilepath, sizeof(ctrl->logfilepath), "%s_%d.log", ctrl->logfileprefix, ctrl->filecount++);
    r = az_sys_fs_open(ctrl->logfilepath, O_RDWR|O_CREAT, 0666, &ctrl->log_fd);
    if (r < 0) break;
    az_sys_fs_ftruncate(ctrl->log_fd, 0);
    *((az_sys_fd_t *)(ctrl->priv)) = ctrl->log_fd;
    #endif 

    ctrl->org_fd = az_sys_dup(ctrl->fd);

    /*
    char exename[AZ_NAME_MAX]; 
    az_fs_getLastPathComponent(az_getExeFileName(), exename);
    struct sockaddr_un svraddr;
    r = az_inet_openLocalSocket("/tmp/az_%s_stdio", exename, NULL, &svraddr, &ctrl->read_fd);
    if (r < 0) {
      az_sys_eprintf("socket error=%d" AZ_NL, r); 
      break;
    }

    r = az_inet_openLocalSocket("/tmp/az_%s_stdio_cli", exename, NULL, NULL, &ctrl->write_fd);
    if (r < 0) {
      az_inet_closeLocalSocket(&ctrl->read_fd);
      az_sys_eprintf("socket error=%d" AZ_NL, r); 
      break;
    }

    r = az_inet_connectLocalSocket(ctrl->write_fd, (struct sockaddr *)&svraddr, sizeof(svraddr));
    if (r < 0) {
      az_inet_closeLocalSocket(&ctrl->read_fd);
      az_inet_closeLocalSocket(&ctrl->write_fd);
      az_sys_eprintf("connect error=%d\n", r );
      break;
    }
    */
    int sv[2];
    r = (az_r_t)socketpair(AF_LOCAL, SOCK_DGRAM|SOCK_NONBLOCK|SOCK_CLOEXEC, 0, sv);
    if (r < 0) {
      az_sys_eprintf("socketpair error=%d" AZ_NL, errno); 
      break;
    }
    ctrl->read_fd = sv[0];
    ctrl->write_fd = sv[1];

    r = az_sys_dup2(ctrl->write_fd, ctrl->fd);
    if (r < 0) {
      az_inet_closeLocalSocket(&ctrl->read_fd);
      az_inet_closeLocalSocket(&ctrl->write_fd);
      az_sys_eprintf("dup2 error=%d\n", r );
    }

  } while (0);

  return (int)r;
}

int az_log_dump_renew(az_log_dump_ctrl_t *ctrl)
{
  #ifdef CONFIG_AZ_LOG_STDOUT_DUMP_FILE
  az_r_t r = AZ_SUCCESS;
  az_sys_fd_t log_fd_new;
  do {
    snprintf(ctrl->logfilepath, sizeof(ctrl->logfilepath), "%s_%d.log", ctrl->logfileprefix, ctrl->filecount++);

    r = az_sys_fs_open(ctrl->logfilepath, O_RDWR|O_CREAT, 0666, &log_fd_new);
    if (r < 0) break;
    az_sys_fs_ftruncate(log_fd_new, 0);
    *((az_sys_fd_t *)(ctrl->priv)) = log_fd_new; 

    az_sys_fs_close(ctrl->log_fd);

    if (r < 0) break;
    ctrl->log_fd = log_fd_new; 
  } while (0);

  return (int)r;
  #endif
  return AZ_SUCCESS;
}
int az_log_dump_stop(az_log_dump_ctrl_t *ctrl)
{
  int r;

  do {
    fflush(ctrl->fh);
    r = az_sys_dup2(ctrl->org_fd, ctrl->fd);
    #ifdef CONFIG_AZ_LOG_STDOUT_DUMP_FILE
    az_sys_fs_close(ctrl->log_fd);
    #endif
    az_inet_closeLocalSocket(&ctrl->read_fd);
    az_inet_closeLocalSocket(&ctrl->write_fd);
  } while (0);

  return r;
}
#else
int az_log_tap_cli_shell_start(az_cli_shell_t *pSh, az_log_tap_cli_ctrl_t *ctrl)
{
  int r;
  do {
    /*
    char exename[AZ_NAME_MAX]; 
    az_fs_getLastPathComponent(az_getExeFileName(), exename);
    struct sockaddr_un svraddr;
    r = az_inet_openLocalSocket("/tmp/az_cli_%s_%s_svr", exename, pSh->name, &svraddr, &ctrl->read_fd);
    if (r < 0) {
      az_sys_eprintf("socket error=%d" AZ_NL, r); 
      break;
    }

    r = az_inet_openLocalSocket("/tmp/az_cli_%s_%s", exename, pSh->name, 
        NULL, &ctrl->write_fd);
    if (r < 0) {
      az_inet_closeLocalSocket(&ctrl->read_fd);
      az_sys_eprintf("socket error=%d" AZ_NL, r); 
      break;
    }
    r = az_inet_connectLocalSocket(ctrl->write_fd, (struct sockaddr *)&svraddr, sizeof(svraddr));
    if (r < 0) {
      az_sys_eprintf("connect error=%d\n", r );
      az_inet_closeLocalSocket(&ctrl->read_fd);
      az_inet_closeLocalSocket(&ctrl->write_fd);
      break;
    }
    */
    int sv[2];
    r = (az_r_t)socketpair(AF_LOCAL, SOCK_DGRAM|SOCK_NONBLOCK|SOCK_CLOEXEC, 0, sv);
    if (r < 0) {
      az_sys_eprintf("socketpair error=%d" AZ_NL, errno); 
      break;
    }
    ctrl->read_fd = sv[0];
    ctrl->write_fd = sv[1];

    ctrl->org_fd = az_cli_shell_change_txport(pSh, ctrl->write_fd);

  } while (0);

  return (int)r;
}

int az_log_tap_cli_shell_stop(az_cli_shell_t *pSh, az_log_tap_cli_ctrl_t *ctrl)
{
  az_r_t r = AZ_SUCCESS;
  do {
    ctrl->org_fd = az_cli_shell_change_txport(pSh, ctrl->org_fd);
    az_inet_closeLocalSocket(&ctrl->read_fd);
    az_inet_closeLocalSocket(&ctrl->write_fd);
  } while (0);

  return (int)r;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
int az_log_dump_start(az_log_dump_ctrl_t *ctrl, FILE *fh, az_sys_fd_t fd)
{
  az_r_t r;
  ctrl->fh = fh;
  ctrl->org_fd = fd;
  do {
    #ifdef CONFIG_AZ_LOG_STDOUT_DUMP_FILE
    snprintf(ctrl->logfilepath, sizeof(ctrl->logfilepath), "%s_%d.log", ctrl->logfileprefix, ctrl->filecount++);
    r = az_sys_fs_open(ctrl->logfilepath, O_RDWR|O_CREAT, 0666, &ctrl->log_fd);
    if (r < 0) break;
    az_sys_fs_ftruncate(ctrl->log_fd, 0);
    ctrl->org_fd = az_sys_dup(ctrl->fd);
    #endif

    /*
    char exename[AZ_NAME_MAX]; 
    az_fs_getLastPathComponent(az_getExeFileName(), exename);
    struct sockaddr_un svraddr;
    r = az_inet_openLocalSocket("/tmp/az_%s_stdio", exename, pSh->name, &svraddr, &ctrl->read_fd);
    if (r < 0) {
      az_sys_eprintf("socket error=%d" AZ_NL, r); 
      break;
    }

    r = az_inet_openLocalSocket("/tmp/az_%s_stdio_cli", exename, pSh->name, NULL, &ctrl->write_fd);
    if (r < 0) {
      az_inet_closeLocalSocket(&ctrl->read_fd);
      az_sys_eprintf("socket error=%d" AZ_NL, r); 
      break;
    }

    r = az_inet_connectLocalSocket(ctrl->write_fd, (struct sockaddr *)&svraddr, sizeof(svraddr));
    if (r < 0) {
      az_inet_closeLocalSocket(&ctrl->read_fd);
      az_inet_closeLocalSocket(&ctrl->write_fd);
      az_sys_eprintf("connect error=%d\n", r );
      break;
    }
    */
    int sv[2];
    r = (az_r_t)socketpair(AF_LOCAL, SOCK_DGRAM|SOCK_NONBLOCK|SOCK_CLOEXEC, 0, sv);
    if (r < 0) {
      az_sys_eprintf("socketpair error=%d" AZ_NL, errno); 
      break;
    }
    ctrl->read_fd = sv[0];
    ctrl->write_fd = sv[1];

    r = az_sys_dup2(ctrl->write_fd, ctrl->fd);
    if (r < 0) {
      az_inet_closeLocalSocket(&ctrl->read_fd);
      az_inet_closeLocalSocket(&ctrl->write_fd);
      az_sys_eprintf("dup2 error=%d\n", r );
    }
  } while (0);

  return (int)r;
}

int az_log_dump_renew(az_log_dump_ctrl_t *ctrl)
{
  #ifdef CONFIG_AZ_LOG_STDOUT_DUMP_FILE
  az_r_t r;
  az_sys_fd_t log_fd_new;
  do {
    snprintf(ctrl->logfilepath, sizeof(ctrl->logfilepath), "%s_%d.log", ctrl->logfileprefix, ctrl->filecount++);

    r = az_sys_fs_open(ctrl->logfilepath, O_RDWR|O_CREAT, 0666, &log_fd_new);
    if (r < 0) break;
    az_sys_fs_ftruncate(log_fd_new, 0);

    az_sys_fs_close(ctrl->log_fd);

    if (r < 0) break;
    ctrl->log_fd = log_fd_new; 
  } while (0);

  return (int)r;
  #else
  return AZ_SUCCESS;
  #endif
}
int az_log_dump_stop(az_log_dump_ctrl_t *ctrl)
{
  int r;

  do {
    fflush(ctrl->fh);
    r = az_sys_dup2(ctrl->org_fd, ctrl->fd);
    #ifdef CONFIG_AZ_LOG_STDOUT_DUMP_FILE
    az_sys_fs_close(ctrl->log_fd);
    #endif
    az_inet_closeLocalSocket(&ctrl->read_fd);
    az_inet_closeLocalSocket(&ctrl->write_fd);
  } while (0);

  return r;
}
#endif

int az_log_dup_stdout_start() 
{
  return az_log_dump_start( &az_log_stdout_dump_ctrl, stdout, STDOUT_FILENO, &az_cli_shell_default.logport);
}

int az_log_dup_thread_state = 0;
az_thread_t az_log_dup_thread_default = NULL;

void *az_log_dup_thread_proc_default(void *arg)
{
  az_log_dump_ctrl_t *ctrl = &az_log_stdout_dump_ctrl; 
  az_log_tap_cli_ctrl_t *tap = &az_log_tap_ctrl_default_cli;
  az_cli_shell_t  *cli_shell = az_cli_get_default();
  az_log_dup_thread_state = 1;
#ifdef  CONFIG_AZ_LOG_USE_SELECT
  fd_set fds;
  struct timeval tv = { .tv_sec = 3, .tv_usec = 0 };
#endif
  int r, tlen = 0;
  az_size_t nlen;
  char  buffer[512];
  int nfds = 0;

  az_sys_printf("%s start...\n", __FUNCTION__);

  
  //az_log_dump_start(ctrl, stdout, STDOUT_FILENO, &cli_shell->logport));

  az_log_tap_cli_shell_start(cli_shell, tap);
  az_cli_shell_enable_rxstdout(cli_shell, 0);

  if (ctrl->read_fd != AZ_SYS_FD_INVALID) {
    nfds = ctrl->read_fd;
  }
  if (tap->read_fd != AZ_SYS_FD_INVALID) {
    if (nfds < tap->read_fd) nfds = tap->read_fd;
  }
  long count = 0;
  #ifdef  CONFIG_AZ_LOG_USE_SELECT
  FD_ZERO(&fds);
  #else
  az_sys_io_event_t ioevt[2];
  int j;
  az_sys_xu_open_iomux();
  az_sys_xu_iomux_add(ctrl->read_fd, AZ_SYS_IO_IN);
  az_sys_xu_iomux_add(tap->read_fd, AZ_SYS_IO_IN);
  #endif
  while (az_log_dup_thread_state) {
    #ifdef  CONFIG_AZ_LOG_USE_SELECT
    if (ctrl->read_fd != AZ_SYS_FD_INVALID) {
      FD_SET(ctrl->read_fd, &fds);
    }
    if (tap->read_fd != AZ_SYS_FD_INVALID) { 
      FD_SET(tap->read_fd, &fds);
    }
    tv.tv_sec = 3;
    r = select(nfds + 1, &fds, NULL, NULL, &tv);
    #else
    j = 0;
    r = az_thread_wait_iomux(ioevt, 2, 3000);
    #endif
    if (az_log_dup_thread_state & 2) continue;
    if (r > 0) {
    #ifdef  CONFIG_AZ_LOG_USE_SELECT
    if (FD_ISSET(ctrl->read_fd, &fds)) {
      nlen = az_sys_fs_read(ctrl->read_fd, buffer, sizeof(buffer));  
      if (nlen > 0) {
        #ifdef CONFIG_AZ_LOG_STDOUT_DUMP_FILE
        tlen += nlen;
        az_sys_fs_write(ctrl->log_fd, buffer, nlen); 
        #endif
        az_sys_fs_write(ctrl->org_fd, buffer, nlen); 
        az_cli_shell_bcast_stdout(buffer, nlen);
      }
    }
    if (FD_ISSET(tap->read_fd, &fds)) {
      nlen = az_sys_fs_read(tap->read_fd, buffer, sizeof(buffer));  
      if (nlen > 0) {
        #ifdef CONFIG_AZ_LOG_STDOUT_DUMP_FILE
        tlen += nlen;
        az_sys_fs_write(ctrl->log_fd, buffer, nlen); 
        #endif
        az_sys_fs_write(ctrl->org_fd, buffer, nlen); 
      }
    }

    if (tlen >= ctrl->sizelimit) {
      tlen = 0;
      az_log_dump_renew(ctrl);
      FD_ZERO(&fds);
      if (ctrl->read_fd != AZ_SYS_FD_INVALID) {
        nfds = ctrl->read_fd;
      }
      if (tap->read_fd != AZ_SYS_FD_INVALID) {
        if (nfds < tap->read_fd) nfds = tap->read_fd;
      }
    }
    fflush(ctrl->fh);
    #else

    for (j = 0; j < r; j++) {
      if (ioevt[j].data.fd == ctrl->read_fd ) {
       if (ioevt[j].events & AZ_SYS_IO_IN ) {
        nlen = az_sys_fs_read(ctrl->read_fd, buffer, sizeof(buffer));  
        if (nlen > 0) {
        #ifdef CONFIG_AZ_LOG_STDOUT_DUMP_FILE
        tlen += nlen;
        az_sys_fs_write(ctrl->log_fd, buffer, nlen); 
        #endif
        az_sys_fs_write(ctrl->org_fd, buffer, nlen); 
        az_cli_shell_bcast_stdout(buffer, nlen);
        }
       }
       continue;
      }
      if (ioevt[j].data.fd == tap->read_fd ) {
       if (ioevt[j].events & AZ_SYS_IO_IN ) {
        nlen = az_sys_fs_read(tap->read_fd, buffer, sizeof(buffer));  
        if (nlen > 0) {
        #ifdef CONFIG_AZ_LOG_STDOUT_DUMP_FILE
        tlen += nlen;
        az_sys_fs_write(ctrl->log_fd, buffer, nlen); 
        #endif
        az_sys_fs_write(ctrl->org_fd, buffer, nlen); 
        }
       }
      }
    }
    #endif
    continue;
    }

    if (r == 0) {
      //az_sys_eprintf("timeout: %ld\n", count++);
      fflush(ctrl->fh);
      continue;
    }
    if (r < 0) break;
  }
  #ifndef  CONFIG_AZ_LOG_USE_SELECT
  az_sys_xu_iomux_del(ctrl->read_fd);
  az_sys_xu_iomux_del(tap->read_fd);
  az_sys_xu_close_iomux();
  #endif

  az_log_tap_cli_shell_stop(az_cli_get_default(), tap);
  az_log_dump_stop(ctrl);
    

  return NULL;
}

az_r_t  az_log_dup_start_default_thread()
{
  az_r_t r;

  r = (az_r_t)az_thread_create("logDupDefault", az_log_dup_thread_proc_default, NULL, NULL, &az_log_dup_thread_default);
  //az_sys_printf("%s: %p\n", __FUNCTION__, az_log_thread_default);

  return (r < AZ_SUCCESS)? r:AZ_SUCCESS;
}

az_r_t  az_log_dup_stop_default_thread()
{
  int r;

  az_log_dup_thread_state = 0;
  r = az_thread_delete(az_log_dup_thread_default);

  return r;
}
#else
int az_log_dup_stdout_start() 
{
  return AZ_ERR(NOT_SUPPORT);
}
int az_log_dump_start(az_log_dump_ctrl_t *ctrl, FILE *fh, az_sys_fd_t fd, void *priv)
{
  return AZ_ERR(NOT_SUPPORT);
}
az_r_t  az_log_dup_start_default_thread()
{
  return AZ_ERR(NOT_SUPPORT);
}
az_r_t  az_log_dup_stop_default_thread()
{
  return AZ_ERR(NOT_SUPPORT);
}

#endif
#endif

/* === end of AZ_LOG_DUP_C === */
