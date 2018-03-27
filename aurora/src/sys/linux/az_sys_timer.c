/**
 * @file   az_sys_timer.c
 * @brief  
 * @date   20/05/16
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
 *   - 2016-05-20 fware initial create
 */

/* include header files */
#include "az_def.h"
#include "az_err.h"
#include "az_assert.h"
#include "az_malloc.h"
#include "az_ring.h"
#include "az_xu.h"
#include "az_linux_timer.h"

#define AZ_SYS_TIMER_CLOCKID    CLOCK_REALTIME

#ifdef  CONFIG_AZ_SYS_TIMER_USE_TIMER_SIG

#define AZ_SYS_TIMER_SIG        SIGRTMIN
#define AZ_SYS_TIMER_RING_MAX   128

/* declare global variables */

/* declare static variables */
static az_ring_t       az_sys_timer_ring;
static az_sys_timer_t  az_sys_timer_ring_data[AZ_SYS_TIMER_RING_MAX];
static sem_t           az_sys_timer_sem;
static az_sys_xu_t az_sys_timer_xu = NULL;
static int az_sys_timerThreadOn = 0;

/* implement static functions */

/**
 * @fn        function name
 * @brief     function-description
 * @param     input-output-parameters
 * @return    return-value
 * @warning   warnings
 * @exception none
 */ 
static void az_sys_timerSigHandler(int sig, siginfo_t *si, void *uc)
{
  az_sys_timer_t t = (az_sys_timer_t)(si->si_value.sival_ptr);
  if (az_sys_timerThreadOn && NULL != t) {
    if (t->repeat > 0) {
      if (--(t->repeat) == 0) {
        struct itimerspec itimer;
        itimer.it_interval.tv_sec = 0; 
        itimer.it_interval.tv_nsec = 0; 
        itimer.it_value = itimer.it_interval; 
        timer_settime(t->timer, 0, &itimer, NULL);
      }
    }
    az_ring_push(&az_sys_timer_ring, &t);
    sem_post(&az_sys_timer_sem);
  }
}

static void *az_sys_timerThread(void *arg)
{
  az_sys_timer_t t;

  while (az_sys_timerThreadOn) {
    sem_wait(&az_sys_timer_sem);
    if (az_ring_pop(&az_sys_timer_ring, &t) < 0) continue;
    if (NULL == t) continue;
    if (NULL == t->handler) continue; 
    (t->handler)(t->arg);
  }

  return NULL;
}


/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
az_r_t az_sys_timer_init()
{ 
  az_r_t result = AZ_ERR_L(AGAIN, 0);
  
  if (!az_sys_timerThreadOn) {
  az_ring_init(&az_sys_timer_ring, AZ_RING_TYPE_DSREF, AZ_SYS_TIMER_RING_MAX, az_sys_timer_ring_data);

  result = sem_init(&az_sys_timer_sem, 0, 0);

  az_sys_timerThreadOn = 1;
  result = az_sys_xu_create("sys_timer", az_sys_timerThread, NULL, NULL, &az_sys_timer_xu);
  }

  return result;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
az_r_t az_sys_timer_deinit()
{
  az_r_t result = AZ_ERR(AGAIN);
  if (az_sys_timerThreadOn) {
    az_sys_timerThreadOn = 0;
    sem_post(&az_sys_timer_sem);
    result = az_sys_xu_delete(az_sys_timer_xu);
    result = sem_destroy(&az_sys_timer_sem);
  }
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
az_r_t  az_sys_timer_create(const char *name, az_uint64_t interval,  int repeat, void (*handler)(void *arg), void *arg, az_sys_timer_t *pTimer)
{
  az_r_t result = AZ_SUCCESS;
  az_sys_timer_t t;
  struct sigevent sev;
  //struct itimerspec its;
  struct sigaction sa;
  //sigset_t mask;

  az_assert(0 != repeat);
  az_assert(NULL != pTimer);

  do {
    az_if_arg_null_break(name, result);
    az_if_arg_null_break(pTimer, result);

    t = *pTimer;
    if (t == NULL) {
      t = az_malloc(sizeof(az_sys_timer_entity_t));
      t->state = 1;
    } else {
      t->state = 0;
    }
    az_if_alloc_null_break(t, result);


    t->interval = interval;
    t->repeat = repeat;
    t->handler = handler;
    t->arg = arg;

    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = az_sys_timerSigHandler; 
    sigemptyset(&sa.sa_mask);
    if (sigaction(AZ_SYS_TIMER_SIG, &sa, NULL) == -1) {
      result = AZ_ERR(SIGACTION);
      if (t->state & 1) {
        az_free(t);
      }
      *pTimer = NULL;
      break;
    }
    //sigemptyset(&mask);
    //sigaddset(&mask, SIG);
    //sigprocmask(SIG_SETMASK, &mask, NULL);

    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = AZ_SYS_TIMER_SIG;
    sev.sigev_value.sival_ptr = t;
    result = timer_create(AZ_SYS_TIMER_CLOCKID, &sev, &(t->timer));
    if (AZ_SUCCESS != result) {
      az_free(t);
      *pTimer = NULL;
      break;
    }
    t->name = name;
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
az_r_t  az_sys_timer_delete(az_sys_timer_t t)
{
  az_r_t result = AZ_SUCCESS;
  do {
    az_if_arg_null_break(t, result);

    result = timer_delete(t->timer);
    if (AZ_SUCCESS == result) {
      if (t->state & 1) {
        az_free(t);
      }
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
az_r_t  az_sys_timer_start(az_sys_timer_t t)
{
  az_r_t result = AZ_SUCCESS;
  struct itimerspec itimer;
  do {
    az_if_arg_null_break(t, result);

    itimer.it_interval.tv_sec = t->interval / 1000000000;
    itimer.it_interval.tv_nsec = t->interval % 1000000000;
    itimer.it_value = itimer.it_interval; 
    result = timer_settime(t->timer, 0, &itimer, NULL);
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
az_r_t  az_sys_timer_stop(az_sys_timer_t t)
{
  az_r_t result = AZ_SUCCESS;
  struct itimerspec itimer;
  do {
    az_if_arg_null_break(t, result);
    
    itimer.it_interval.tv_sec = 0; 
    itimer.it_interval.tv_nsec = 0; 
    itimer.it_value = itimer.it_interval; 
    result = timer_settime(t->timer, 0, &itimer, NULL);
  } while (0);

  return result;
}

#else
az_r_t az_sys_timer_init()
{ 
  az_r_t result = AZ_SUCCESS;

  return result;
}

az_r_t az_sys_timer_deinit()
{
  az_r_t result = AZ_SUCCESS;

  return result;
}

az_r_t  az_sys_timer_create(const char *name, az_uint64_t interval,  int repeat, void (*handler)(void *arg), void *arg, az_sys_timer_t *pTimer)
{
  az_r_t result = AZ_SUCCESS;
  az_sys_timer_t t;

  az_assert(0 != repeat);
  az_assert(NULL != pTimer);

  do {
    az_if_arg_null_break(name, result);
    az_if_arg_null_break(pTimer, result);

    t = *pTimer;

    if (NULL == t) {
      t = az_malloc(sizeof(az_sys_timer_entity_t));
      t->state = 1;
    } else {
      t->state = 0;
    }
    az_if_alloc_null_break(t, result);

    t->interval = interval;
    t->repeat = repeat;
    t->handler = handler;
    t->arg = arg;

    t->timer = timerfd_create(AZ_SYS_TIMER_CLOCKID, TFD_NONBLOCK|TFD_CLOEXEC); 
    if (t->timer == -1) {
      if (t->state & 1) {
        az_free(t);
      }
      *pTimer = NULL;
      result = AZ_FAIL;
      break;
    }
    t->name = name;
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
az_r_t  az_sys_timer_delete(az_sys_timer_t t)
{
  az_r_t result = AZ_SUCCESS;
  do {
    az_if_arg_null_break(t, result);

    close(t->timer);
    if (t->state & 1) {
      az_free(t);
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
az_r_t  az_sys_timer_start(az_sys_timer_t t)
{
  az_r_t result = AZ_SUCCESS;
  struct itimerspec itimer;
  do {
    az_if_arg_null_break(t, result);

    itimer.it_interval.tv_sec = t->interval / 1000000000;
    itimer.it_interval.tv_nsec = t->interval % 1000000000;
    itimer.it_value = itimer.it_interval; 
    result = timerfd_settime(t->timer, 0, &itimer, NULL);
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
az_r_t  az_sys_timer_stop(az_sys_timer_t t)
{
  az_r_t result = AZ_SUCCESS;
  struct itimerspec itimer;
  do {
    az_if_arg_null_break(t, result);
    
    itimer.it_interval.tv_sec = 0; 
    itimer.it_interval.tv_nsec = 0; 
    itimer.it_value = itimer.it_interval; 
    result = timerfd_settime(t->timer, 0, &itimer, NULL);
  } while (0);

  return result;
}
#endif


/* === end of AZ_SYS_TIMER_C === */
