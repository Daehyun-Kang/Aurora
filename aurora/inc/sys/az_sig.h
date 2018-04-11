/**
 * @file   az_sig.h
 * @brief  
 * @date   02/10/17
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
 *   - 2017-10-02 dhkang initial create
 */

#ifndef AZ_SIG_H
#define AZ_SIG_H

#ifdef __cplusplus
extern "C"
{
#endif

/* constants */
#ifndef CONFIG_AZ_SIGHANDLE_MAX 
#define CONFIG_AZ_SIGHANDLE_MAX   32
#endif

/* basic macros */

/* basic types */
typedef void  (*az_sighandler_t)(int);

/* structures */

/* structured types */
typedef struct sigaction az_sigaction_t;

typedef struct az_sighandler_descr {
  az_sighandler_t handler;
  struct az_sighandler_t *next;
} az_sighandler_descr_t;

typedef struct az_sig_descr {
  int signo;
  az_sigaction_t org_action;
  az_sighandler_descr_t **descrlist;
} az_sig_descr_t;

/* macros */

/* variabls exposed */

/* inline functions */

/* function prototypes exposed */
extern int  az_sig_initDescr(int sig);
extern int  az_sig_deinitDescr(int sig);
extern int  az_sig_setSighandlerList(int sig, az_sighandler_descr_t **);
extern int  az_sig_regSighandler(int sig, az_sighandler_descr_t *);
extern int  az_sig_deregSighandler(int sig, az_sighandler_descr_t *);
extern int  az_sig_send(int sig, int dest);

#ifdef __cplusplus
}
#endif
#endif
