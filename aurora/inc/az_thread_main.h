/**
 * @file   az_thread_main.h
 * @brief  
 * @date   16/04/18
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
 *   - 2018-04-16 dhkang initial create
 */

#ifndef AZ_THREAD_MAIN_H
#define AZ_THREAD_MAIN_H

#include "az_def.h"
#include "az_thread_cfg.h"
#include "az_thread.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* constants */

/* basic macros */

/* basic types */

/* structures */
struct az_thread_main_arg {
  int argc;
  char  **argv;
};

/* structured types */

/* macros */

/* variabls exposed */
extern az_thread_t  az_thread_main;
extern int  az_main(int argc, char *argv[]);

/* inline functions */

/* function prototypes exposed */
extern void *__az_weak az_thread_main_entry(struct az_thread_main_arg *arg);

#ifdef __cplusplus
}
#endif
#endif

/* === end of AZ_THREAD_MAIN_H === */
