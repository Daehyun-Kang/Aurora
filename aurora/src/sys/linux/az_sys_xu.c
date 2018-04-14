/**
 * @file   az_sys_xu.c
 * @brief  
 * @date   10/04/16
 * @author fware
 *
 * @copyright
 * Copyright (c) Fware, 2013-2016 - All Rights Reserved
 * You may use, distribute and modify this code under the 
 * terms of the "Aurora Source Code License".
 * See the file LICENSE for full license details.\n\n
 *
 * You should have received a copy of  the LICENSE with this file.\n\n
 *
 * If not, please contact to Fware with the information in the file CONTACTS.
 *
 * History:
 *   - 2016-04-10 fware initial create
 */

/* include header files */
#define _GNU_SOURCE
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>
#include <execinfo.h>
#include <signal.h>
#include <sys/ucontext.h>
//#include "az_def.h"
#include "az_linux_xu.h"
#include "az_malloc.h"

/* declare global variables */

/* declare static variables */
#ifdef  CONFIG_AZ_USE_TLS
AZ_SYS_TLS az_sys_xu_t az_sys_xu;
#else
static __thread az_sys_xu_t az_sys_xu;
#endif

/* implement static functions */

/**
 * @fn        function name
 * @brief     function-description
 * @param     input-output-parameters
 * @return    return-value
 * @warning   warnings
 * @exception none
 */
#if 0
int az_sys_xu_save_context()
{
  volatile az_sys_xu_t xu = az_sys_xu;
  volatile int r;
  
  if (xu) {
    /*
    if (xu->env_index < CONFIG_AZ_THRED_EXCPT_STK_SZ - 1) {
      r = setjmp(xu->env[xu->env_index++]);
    } else {
      r = setjmp(xu->env[xu->env_index]);
    }
    */
    return setjmp(xu->env);
    if (r == 0) {
    } else {
      xu = az_sys_xu;
    }
    printf("%s:r=%d\n", xu->name, r); 
  }

  return r;
}
#endif
void az_sys_xu_restore_context(int signo)
{
  az_sys_xu_t xu = az_sys_xu;
  int r;
  
  if (xu) {
    if (xu->env_index > 0) {
      longjmp(xu->env[--(xu->env_index)], signo);
    }
    //az_sys_eprintf("%s\n", xu->name);
    //longjmp(xu->env, signo);
  }

}
void az_sys_xu_remove_context()
{
  az_sys_xu_t xu = az_sys_xu;
  if (xu) {
    if (xu->env_index > 0) {
      --(xu->env_index); 
    }
  }
}

static void az_sys_xu_sigsegv_handler(int sig, siginfo_t *info, void *context)
{
  extern void  az_thread_set_state_excpt(void *);
  void *trace[CONFIG_AZ_EXP_CALLSTACK_DEPTH];
  char  **messages = (char **)NULL;
  ucontext_t *uc = (ucontext_t *)context;
  int trace_size = 0;
  int blen, i;
  char prbuf[256];

  char *xu_name = (az_sys_xu == NULL)? "???":az_sys_xu->name;
  void *callsite;

  trace_size = backtrace(trace, CONFIG_AZ_EXP_CALLSTACK_DEPTH);
  messages = backtrace_symbols(trace, trace_size);

  #ifdef  __SANITIZE_ADDRESS__
  az_thread_set_state_excpt(trace[3]);
  #else
  az_thread_set_state_excpt(trace[2]);
  #endif

  #if defined(__i386__) 
      callsite = (void *)(uc->uc_mcontext.gregs[REG_EIP]);
  #elif defined(__x86_64__)
      callsite = (void *)(uc->uc_mcontext.gregs[REG_RIP]);
  #else
  #error Unsupported architecture
  #endif

  blen = snprintf(prbuf, sizeof(prbuf), "Thread:%s SEGV: addr=%p at=%p\n",
      xu_name, (void *)(info->si_addr), callsite); 
  write(STDOUT_FILENO, prbuf, blen);

  char *exefilename = az_getExeFileName();

  for (i = 2; i < trace_size && messages != NULL; ++i) {
    blen = snprintf(prbuf, sizeof(prbuf), "[bt] #%d %s", i, messages[i]);
    write(STDOUT_FILENO, prbuf, blen);
    int p = 0;
    while (messages[i][p] != '('  && messages[i][p] != ' ' && messages[i][p] != 0) { ++p; }
    sprintf(prbuf, "addr2line %p -e %.*s", trace[i], p, messages[i]);
    //sprintf(prbuf, "addr2line %p -e %s", trace[i], exefilename); 
    system(prbuf);
  }

  az_sys_xu_restore_context(SIGSEGV);
}

static void az_sys_xu_set_sigaction(int sig)
{
  sigset_t  mask_sigs;
  struct sigaction new_action;

  sigemptyset(&mask_sigs);
  sigaddset(&mask_sigs, sig);
  new_action.sa_sigaction = az_sys_xu_sigsegv_handler;
  new_action.sa_mask = mask_sigs;
  new_action.sa_flags = SA_RESTART|SA_SIGINFO;

  if (sigaction(sig, &new_action, NULL) == -1) {
    az_sys_eprintf("sig %d sigaction error=%d\n", sig, az_sys_errno);
  }
}

void az_sys_xu_register_exception_handler()
{
  az_sys_xu_set_sigaction(SIGSEGV);
  az_sys_xu_set_sigaction(SIGBUS);
}


static void az_sys_xu_suspend_handler(int sig)
{
  az_sys_xu_t xu = az_sys_xu;
  //int val;

  if (NULL != xu) {
      sem_wait(&(xu->sem_suspend));
  }
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
static void az_sys_xu_prolog(az_sys_xu_t xu)
{
  struct sigaction sa;
  
  az_sys_xu = xu;
  xu->env_index = 0;
  xu->tid = syscall(SYS_gettid);

  pthread_setname_np(xu->thread, xu->name);
  if (xu->attr) {
    az_sys_xu_setAffinity(xu, xu->attr->core_mask);
    az_sys_xu_setPriority(xu, xu->attr);
  }
  
  sa.sa_flags = 0;
  sa.sa_handler = az_sys_xu_suspend_handler;
  sigaction(SIGUSR1, &sa, NULL);  
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
static void *az_sys_xu_entry(void *argp)
{
  az_sys_xu_t xu = (az_sys_xu_t)argp;

  az_sys_xu_prolog(xu);

  //az_sys_xu_register_exception_handler();

  void *result =  (xu->entry)(xu->arg);

  return result;
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
az_r_t az_sys_xu_init(const char *name, 
    void *(*entry)(void *), void *arg, void *pOptions, az_sys_xu_t *pXu)
{
  az_r_t result = AZ_SUCCESS;
  az_sys_xu_t xu;
  az_thread_attr_t *xu_attr = (az_thread_attr_t *)pOptions;

  do {
    az_if_arg_null_break(name, result);
    az_if_arg_null_break(entry, result);
    az_if_arg_null_break(pXu, result);

    xu = *pXu;
    if (NULL == xu) {
      xu = az_malloc(sizeof(az_sys_xu_entity_t));
      az_if_alloc_null_break(xu, result);
      *pXu = xu;
      xu->state = 1;
    } else {
      xu->state = 0;
    }

    az_sys_ep_init(&(xu->ep));
    
    result = sem_init(&(xu->sem_suspend), 0, 1);

    xu->state |= 2;

    if (AZ_SUCCESS == result) {
      xu->name = name;
      xu->entry = entry;
      xu->arg = arg;
      xu->attr = xu_attr;
      xu->thread = pthread_self();

      az_sys_xu_prolog(xu);
    }

    if (AZ_SUCCESS != result) {
      az_sys_xu_cleanup(xu);
      *pXu = NULL;
      break;
    }
  } while (0);

  return result;
}

/**
 * @fn        function name
 * @brief     function-description
 * @param     input-output-parameters
 * @return    return-value
 * @warning   warnings
 * @exception none
 */
az_r_t az_sys_xu_create(const char *name, 
    void *(*entry)(void *), void *arg, void *pOptions, az_sys_xu_t *pXu)
{
  az_r_t result = AZ_SUCCESS;
  az_sys_xu_t xu;
  az_thread_attr_t *xu_attr = (az_thread_attr_t *)pOptions;

  do {
    az_if_arg_null_break(name, result);
    az_if_arg_null_break(entry, result);
    az_if_arg_null_break(pXu, result);

    xu = *pXu;
    if (NULL == xu) {
      xu = az_malloc(sizeof(az_sys_xu_entity_t));
      az_if_alloc_null_break(xu, result);
      *pXu = xu;
      xu->state = 1;
    } else {
      xu->state = 0;
    }

    az_sys_ep_init(&(xu->ep));
    xu->iomux = AZ_SYS_IOMUX_INVALID;
    
    result = sem_init(&(xu->sem_suspend), 0, 1);

    xu->state |= 2;

    if (AZ_SUCCESS == result) {
      pthread_attr_t attr;
      xu->name = name;
      xu->entry = entry;
      xu->arg = arg;
      xu->attr = xu_attr;
      pthread_attr_init(&attr);
      pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
      if (0) {
        size_t dft_stk_sz;
        pthread_attr_getstacksize(&attr, &dft_stk_sz);
        az_sys_ilog("default stacksize:%dk\n", dft_stk_sz/1024); 
      }
      if (NULL != xu_attr) {
        if (xu_attr->stack_size > 0) {
          pthread_attr_setstacksize(&attr, xu_attr->stack_size);
        }
      } 
      result = pthread_create(&(xu->thread), &attr, az_sys_xu_entry, (void *)xu);
      if (AZ_SUCCESS == result) { 
        pthread_setname_np(xu->thread, xu->name);
      }
      pthread_attr_destroy(&attr);
    }

    if (AZ_SUCCESS != result) {
      az_sys_xu_cleanup(xu);
      *pXu = NULL;
      break;
    }
  } while (0);

  return result;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
void az_sys_xu_cleanup(az_sys_xu_t xu)
{
  az_assert(NULL != xu);

 if (xu->state & 2) {
    xu->state &= ~2;

    sem_destroy(&xu->sem_suspend);

    az_sys_ep_deinit(&(xu->ep));

    if (xu->state & 1) {
      az_free(xu);
    }
  } 
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
az_r_t az_sys_xu_delete(az_sys_xu_t xu)
{
  az_r_t result = AZ_SUCCESS;
  pthread_t thread;
  az_assert(NULL != xu);

  do {
    if (pthread_self() == (thread = xu->thread)) {
      result = AZ_ERR(INVALID_OPR);
      break;
    }

    pthread_cancel(thread);

    az_sys_xu_cleanup(xu);

  } while (0);

  return result;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
az_r_t  az_sys_xu_setPriority(az_sys_xu_t xu, az_thread_attr_t *pAttr)
{
  az_r_t result = AZ_SUCCESS;
  do {
    az_if_arg_null_break(xu, result);
    az_if_arg_null_break(pAttr, result);
    struct sched_param param;

    param.sched_priority = pAttr->priority;
    result = pthread_setschedparam(xu->thread, pAttr->sched_policy, &param);
  } while (0);

  return result;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
az_r_t  az_sys_xu_getPriority(az_sys_xu_t xu, az_thread_attr_t *pAttr)
{
  az_r_t result = AZ_SUCCESS;
  do {
    az_if_arg_null_break(xu, result);
    az_if_arg_null_break(pAttr, result);
    struct sched_param param;

    param.sched_priority = pAttr->priority;
    result = pthread_getschedparam(xu->thread, &(pAttr->sched_policy), &param);
    pAttr->priority = param.sched_priority;
  } while (0);

  return result;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
az_r_t  az_sys_xu_setAffinity(az_sys_xu_t xu, az_thread_core_mask_t core_mask)
{
  az_r_t result = AZ_SUCCESS;
  do {
    cpu_set_t cpuset;
    az_if_arg_null_break(xu, result);

    CPU_ZERO(&cpuset);
    int j;
    az_uint64_t mask;

    mask = 1;
    for (j = 0; j < sizeof(cpu_set_t)*8; j++, mask <<= 1) {
      if (core_mask & mask) {
        CPU_SET(j, &cpuset);
      }
    }
    result = pthread_setaffinity_np(xu->thread, sizeof(cpu_set_t), &cpuset);
  } while (0);

  return result;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
az_r_t  az_sys_xu_getAffinity(az_sys_xu_t xu, az_thread_core_mask_t *pCoreMask)
{
  az_r_t result = AZ_SUCCESS;
  do {
    cpu_set_t cpuset;
    az_if_arg_null_break(xu, result);

    CPU_ZERO(&cpuset);
    result = pthread_getaffinity_np(xu->thread, sizeof(cpu_set_t), &cpuset);
    if (result < 0) break;
    int j;
    az_uint64_t core_mask, mask;

    core_mask = 0;
    mask = 1;
    for (j = 0; j < sizeof(cpu_set_t)*8; j++, mask <<= 1) {
      if (CPU_ISSET(j, &cpuset)) {
        core_mask |= mask;
      }
    }
    *pCoreMask = core_mask;
  } while (0);

  return result;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
az_r_t  az_sys_xu_getcpu()
{
  return (az_r_t)sched_getcpu();
}

#ifndef CONFIG_AZ_USE_TLS
/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
az_sys_xu_t az_sys_xu_self()
{
  return az_sys_xu;
}
#endif

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
az_r_t az_sys_xu_suspend(az_sys_xu_t xu)
{
  az_r_t result = AZ_SUCCESS;
  do {
    az_if_arg_null_break(xu, result);

    pthread_kill(xu->thread, SIGUSR1);
  } while (0);

  return result;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
az_r_t az_sys_xu_resume(az_sys_xu_t xu)
{
  az_r_t result = AZ_SUCCESS;
  do {
    az_if_arg_null_break(xu, result);

    int val;
    sem_getvalue(&xu->sem_suspend, &val);
    if (val <= 0) {
      sem_post(&xu->sem_suspend);
    } else {
      result = AZ_ERR(STATE);
    }
  } while (0);

  return result;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
az_r_t az_sys_xu_sleep(az_int64_t nsec)
{
  az_r_t r = AZ_SUCCESS;
  if (nsec > 0) {
    struct timespec req, rem;
    req.tv_sec = nsec/1000000000;
    req.tv_nsec = nsec%1000000000;
    do {
      r = nanosleep(&req, &rem);
      if (r < 0) { 
        if (errno == EINTR) {
          req = rem;
        } else {
          break;
        }
      }
    } while (r);
  }

  return r;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
void az_sys_xu_exit(void* ret)
{
  az_sys_xu_t xu = az_sys_xu;
  //az_r_t r = AZ_SUCCESS;
  if (NULL == xu) {
    //r = AZ_ERR(ENTITY_NULL);
  } else {
    az_sys_xu_cleanup(xu);

    pthread_exit(ret);
  }
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
az_r_t az_sys_xu_sendEvent(az_sys_xu_t xu, az_sys_event_t event)
{
  az_r_t r = AZ_SUCCESS;
  do {
    az_if_arg_null_break(xu, r);

    r = az_sys_ep_send(&(xu->ep), event); 
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
az_sys_event_t az_sys_xu_readEvent(az_sys_event_t aevents)
{
  az_sys_xu_t xu = az_sys_xu;
  az_assert(NULL != az_sys_xu);
  az_sys_event_t revents = xu->ep.revents;
  revents = (revents & aevents);
  if (revents) {
    xu->ep.revents &= ~aevents;
  }

  return revents; 
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
az_r_t az_sys_xu_recvEvent(az_sys_event_t toReceive, az_int8_t options, az_int64_t nsec, az_sys_event_t *pReceived)
{
  az_r_t r = AZ_FAIL;
  az_sys_xu_t xu = az_sys_xu;
  az_assert(NULL != az_sys_xu);
  az_assert(NULL != pReceived);

  *pReceived = 0;
  if (NULL == xu) {
      r = AZ_ERR(ENTITY_NULL);
  } else {
    do {
      r = az_sys_ep_recv(&(xu->ep), 0, toReceive, options, NULL, pReceived);
      if (r == AZ_SUCCESS) break;

      r = az_sys_ep_wait(&(xu->ep), nsec);
      if (AZ_SUCCESS != r) {
        if (AZ_ERR(AGAIN) == r) {
          continue;
        }
        break;
      }
    } while (1);
  }

  return r;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
az_r_t az_sys_xu_waitEvent(az_sys_event_t aevents, az_sys_event_t toReceive, az_int8_t options, az_int64_t nsec, az_sys_event_t *qReceived, az_sys_event_t *pReceived)
{
  az_r_t r = AZ_FAIL;
  az_sys_xu_t xu = az_sys_xu;
  az_assert(NULL != az_sys_xu);
  az_assert(NULL != pReceived);

  *pReceived = 0;
  if (NULL == xu) {
      r = AZ_ERR(ENTITY_NULL);
  } else {
    do {
      r = az_sys_ep_recv(&(xu->ep), aevents, toReceive, options, qReceived, pReceived);
      if (r == AZ_SUCCESS) break;

      r = az_sys_ep_wait(&(xu->ep), nsec);
      if (AZ_SUCCESS != r) {
        if (AZ_ERR(AGAIN) == r) {
          continue;
        }
        break;
      }
    } while (1);
  }

  return r;
}

/*
az_sys_ep_t *az_sys_xu_eport()
{
  az_sys_xu_t xu = az_sys_xu;
  if (NULL == xu) return NULL;
  else return &(xu->ep);
}

az_sys_iomux_t az_sys_xu_iomux()
{
  az_sys_xu_t xu = az_sys_xu;
  if (NULL == xu) return AZ_SYS_IOMUX_INVALID;
  else return xu->iomux;
}

az_r_t az_sys_xu_open_iomux()
{
  az_r_t r = AZ_SUCCESS;
  az_assert(NULL != az_sys_xu);
  az_sys_xu_t xu = az_sys_xu;

  do {
    if (xu->iomux == AZ_SYS_IOMUX_INVALID) {
      xu->iomux = az_sys_iomux_create();
    }
    if (xu->iomux == AZ_SYS_IOMUX_INVALID) {
      r = AZ_FAIL;
    }
    az_sys_iomux_add_ep(xu->iomux, &xu->ep);
  } while (0);

  return r;
}

az_r_t az_sys_xu_close_iomux()
{
  az_r_t r = AZ_SUCCESS;
  az_assert(NULL != az_sys_xu);
  az_sys_xu_t xu = az_sys_xu;

  do {
    if (xu->iomux == AZ_SYS_IOMUX_INVALID) {
      r = AZ_ERR(AGAIN);
      break;
    }
    az_sys_iomux_del_ep(xu->iomux, &xu->ep);
    az_sys_iomux_t fd = xu->iomux; 
    xu->iomux = AZ_SYS_IOMUX_INVALID;
    close(fd);
  } while (0);

  return r;
}

az_r_t az_sys_xu_wait_iomux(az_sys_io_event_t *ioevt, int maxevents, int tmo_ms)
{
  az_r_t r = AZ_SUCCESS;
  az_assert(NULL != az_sys_xu);
  az_sys_xu_t xu = az_sys_xu;
  az_assert(xu->iomux != AZ_SYS_IOMUX_INVALID); 

  do {
    r = (az_r_t)az_sys_iomux_wait(xu->iomux, ioevt, maxevents, tmo_ms);
  } while (0);

  return r;
}
*/

/* === end of AZ_SYS_XU_C === */
