/**
 * @file   az_log_dup.h
 * @brief  
 * @date   02/03/18
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
 *   - 2018-03-02 dhkang initial create
 */

#ifndef AZ_LOG_DUP_H
#define AZ_LOG_DUP_H

#ifdef __cplusplus
extern "C"
{
#endif

/* constants */

/* basic macros */

/* basic types */
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

  void  *priv;
} az_log_dump_ctrl_t;

/* structures */

/* structured types */

/* macros */

/* variabls exposed */
extern az_log_dump_ctrl_t az_log_stdout_dump_ctrl; 

/* inline functions */

/* function prototypes exposed */
extern int az_log_dump_start(az_log_dump_ctrl_t *ctrl, FILE *fh, az_sys_fd_t fd, void *priv);
extern int az_log_dump_renew(az_log_dump_ctrl_t *ctrl);
extern int az_log_dump_stop(az_log_dump_ctrl_t *ctrl);

extern int az_log_dup_stdout_start(); 
#ifdef __cplusplus
}
#endif
#endif
