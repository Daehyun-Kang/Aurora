/**
 * @file   az_sys_sig.c
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

/* include header files */

#include "linux/az_sys_linux.h"
#include "az_def.h"
#include "az_err.h"
#include "az_sig.h"

/* declare global variables */
az_sig_descr_t az_sig_descr_SIGINT = {.signo = -1};
az_sig_descr_t az_sig_descr_SIGABRT = {.signo = -1};
az_sig_descr_t az_sig_descr_SIGTERM = {.signo = -1};
az_sig_descr_t az_sig_descr_SIGSEGV = {.signo = -1};
az_sig_descr_t az_sig_descr_SIGUSR1 = {.signo = -1};
az_sig_descr_t az_sig_descr_SIGUSR2 = {.signo = -1};

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

static inline void  az_sigaction_default(int signo, siginfo_t *siginfo, void *context, az_sig_descr_t *pSigDescr)
{
  az_sighandler_descr_t *descr;
  do {
    if (NULL == pSigDescr->descrlist) break;

    descr = *(pSigDescr->descrlist);

    while (descr) {
      (descr->handler)(signo);
      descr = descr->next;
    }
  } while (0);

  signal(signo, SIG_DFL);
  raise(signo);
}

/* implement global functions */

/**
 * @fn        function name
 * @brief     function-description
 * @param     input-output-parameters
 * @return    return-value
 * @warning   warnings
 * @exception none
 */

void  az_sigaction_SIGINT(int signo, siginfo_t *siginfo, void *context)
{
  az_sigaction_default(signo, siginfo, context, &az_sig_descr_SIGINT);
}
void  az_sigaction_SIGABRT(int signo, siginfo_t *siginfo, void *context)
{
  az_sigaction_default(signo, siginfo, context, &az_sig_descr_SIGABRT);
}
void  az_sigaction_SIGTERM(int signo, siginfo_t *siginfo, void *context)
{
  az_sigaction_default(signo, siginfo, context, &az_sig_descr_SIGTERM);
}
void  az_sigaction_SIGSEGV(int signo, siginfo_t *siginfo, void *context)
{
  az_sigaction_default(signo, siginfo, context, &az_sig_descr_SIGSEGV);
}
void  az_sigaction_SIGUSR1(int signo, siginfo_t *siginfo, void *context)
{
  az_sigaction_default(signo, siginfo, context, &az_sig_descr_SIGUSR1);
}
void  az_sigaction_SIGUSR2(int signo, siginfo_t *siginfo, void *context)
{
  az_sigaction_default(signo, siginfo, context, &az_sig_descr_SIGUSR2);
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
int az_sig_initDescr(int  signo)
{
  az_sig_descr_t  *pSigDescr = NULL;
  int r = AZ_ERR(NOT_SUPPORT);
  void  (*pSigaction)(int , siginfo_t *, void *);

  do {
    switch (signo) {
      case SIGINT: 
        pSigDescr = &az_sig_descr_SIGINT; 
        pSigaction = az_sigaction_SIGINT;
        break;
      case SIGABRT: 
        pSigDescr = &az_sig_descr_SIGABRT; 
        pSigaction = az_sigaction_SIGABRT;
        break;
      case SIGTERM: 
        pSigDescr = &az_sig_descr_SIGTERM; 
        pSigaction = az_sigaction_SIGTERM;
        break;
      case SIGSEGV: 
        pSigDescr = &az_sig_descr_SIGSEGV; 
        pSigaction = az_sigaction_SIGSEGV;
        break;
      case SIGUSR1: 
        pSigDescr = &az_sig_descr_SIGUSR1; 
        pSigaction = az_sigaction_SIGUSR1;
        break;
      case SIGUSR2: 
        pSigDescr = &az_sig_descr_SIGUSR2; 
        pSigaction = az_sigaction_SIGUSR2;
        break;
      default: pSigDescr = NULL; break;
    }
    if (NULL == pSigDescr) break;

    if (pSigDescr->signo == signo) {
      r = AZ_ERR(AGAIN);
      break;
    }
    if (pSigDescr->signo != -1) {
      r = AZ_ERR(STATE);
      break;
    }

    sigset_t  mask_sigs;
    az_sigaction_t  new_action;

    sigemptyset(&mask_sigs);
    sigaddset(&mask_sigs, signo);

    new_action.sa_sigaction = pSigaction;
    new_action.sa_mask = mask_sigs;
    new_action.sa_flags = SA_SIGINFO; 

    r = sigaction(signo, &new_action, &(pSigDescr->org_action));
    if (0 == r) {
      pSigDescr->signo = signo;
      pSigDescr->descrlist = NULL;
    }

  } while (0);

  return r;
}

int az_sig_deinitDescr(int  signo)
{
  az_sig_descr_t  *pSigDescr = NULL;
  int r = AZ_ERR(NOT_SUPPORT);

  do {
    switch (signo) {
      case SIGINT: 
        pSigDescr = &az_sig_descr_SIGINT; 
        break;
      case SIGABRT: 
        pSigDescr = &az_sig_descr_SIGABRT; 
        break;
      case SIGTERM: 
        pSigDescr = &az_sig_descr_SIGTERM; 
        break;
      case SIGSEGV: 
        pSigDescr = &az_sig_descr_SIGSEGV; 
        break;
      case SIGUSR1: 
        pSigDescr = &az_sig_descr_SIGUSR1; 
        break;
      case SIGUSR2: 
        pSigDescr = &az_sig_descr_SIGUSR2; 
        break;
      default: pSigDescr = NULL; break;
    }
    if (NULL == pSigDescr) break;

    if (pSigDescr->signo != signo) {
      r = AZ_ERR(STATE);
      break;
    }
    if (pSigDescr->signo == -1) {
      r = AZ_ERR(AGAIN);
      break;
    }
    r = sigaction(signo, &(pSigDescr->org_action), NULL);
    if (0 == r) {
      pSigDescr->signo = -1;
      pSigDescr->descrlist = NULL;
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
int  az_sig_regSighandler(int signo, az_sighandler_descr_t *descr)
{
  az_sig_descr_t  *pSigDescr = NULL;
  int r = AZ_ERR(NOT_SUPPORT);

  do {
    switch (signo) {
      case SIGINT: 
        pSigDescr = &az_sig_descr_SIGINT; 
        break;
      case SIGABRT: 
        pSigDescr = &az_sig_descr_SIGABRT; 
        break;
      case SIGTERM: 
        pSigDescr = &az_sig_descr_SIGTERM; 
        break;
      case SIGSEGV: 
        pSigDescr = &az_sig_descr_SIGSEGV; 
        break;
      case SIGUSR1: 
        pSigDescr = &az_sig_descr_SIGUSR1; 
        break;
      case SIGUSR2: 
        pSigDescr = &az_sig_descr_SIGUSR2; 
        break;
      default: pSigDescr = NULL; break;
    }
    if (NULL == pSigDescr) break;

    if (pSigDescr->signo != signo) {
      r = AZ_ERR(STATE);
      break;
    }
    if (pSigDescr->signo == -1) {
      r = AZ_ERR(AGAIN);
      break;
    }
    if (NULL == pSigDescr->descrlist) {
      r = AZ_ERR(INVALID);
      break;
    }

    az_sighandler_descr_t *head = *(pSigDescr->descrlist);
    if (NULL == head) {
      *(pSigDescr->descrlist) = descr;
    } else {
      while (head) {
        if (head->next == NULL) {
          head->next = descr;
          r = AZ_SUCCESS;
          break;
        }
        head = head->next;
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
int  az_sig_setSighandlerList(int signo, az_sighandler_descr_t **descrlist)
{
  az_sig_descr_t  *pSigDescr = NULL;
  int r = AZ_ERR(NOT_SUPPORT);

  do {
    switch (signo) {
      case SIGINT: 
        pSigDescr = &az_sig_descr_SIGINT; 
        break;
      case SIGABRT: 
        pSigDescr = &az_sig_descr_SIGABRT; 
        break;
      case SIGTERM: 
        pSigDescr = &az_sig_descr_SIGTERM; 
        break;
      case SIGSEGV: 
        pSigDescr = &az_sig_descr_SIGSEGV; 
        break;
      case SIGUSR1: 
        pSigDescr = &az_sig_descr_SIGUSR1; 
        break;
      case SIGUSR2: 
        pSigDescr = &az_sig_descr_SIGUSR2; 
        break;
      default: pSigDescr = NULL; break;
    }
    if (NULL == pSigDescr) break;

    if (pSigDescr->signo != signo) {
      r = AZ_ERR(STATE);
      break;
    }
    if (pSigDescr->signo == -1) {
      r = AZ_ERR(AGAIN);
      break;
    }

    pSigDescr->descrlist = descrlist;
    r = AZ_SUCCESS;
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
int  az_sig_deregSighandler(int signo, az_sighandler_descr_t *descr) 
{
  az_sig_descr_t  *pSigDescr = NULL;
  int r = AZ_ERR(NOT_SUPPORT);

  do {
    switch (signo) {
      case SIGINT: 
        pSigDescr = &az_sig_descr_SIGINT; 
        break;
      case SIGABRT: 
        pSigDescr = &az_sig_descr_SIGABRT; 
        break;
      case SIGTERM: 
        pSigDescr = &az_sig_descr_SIGTERM; 
        break;
      case SIGSEGV: 
        pSigDescr = &az_sig_descr_SIGSEGV; 
        break;
      case SIGUSR1: 
        pSigDescr = &az_sig_descr_SIGUSR1; 
        break;
      case SIGUSR2: 
        pSigDescr = &az_sig_descr_SIGUSR2; 
        break;
      default: pSigDescr = NULL; break;
    }
    if (NULL == pSigDescr) break;

    if (pSigDescr->signo != signo) {
      r = AZ_ERR(STATE);
      break;
    }
    if (pSigDescr->signo == -1) {
      r = AZ_ERR(AGAIN);
      break;
    }
    if (NULL == pSigDescr->descrlist) {
      r = AZ_ERR(INVALID);
      break;
    }

    az_sighandler_descr_t *prev = NULL; 
    az_sighandler_descr_t *head = *(pSigDescr->descrlist);
    while (head) {
      if (head == descr) {
        if (prev) prev->next = head->next;
        else *(pSigDescr->descrlist) = NULL;
        r = AZ_SUCCESS;
        break;
      }
      prev = head;
      head = head->next;
    }
  } while (0);

  return r;
}
int  az_sig_send(int sig, int dest)
{
  return kill(dest, sig);
}

/* === end of AZ_SYS_SIG_C === */
