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
#include "az_windows_timer.h"

#define AZ_SYS_TIMER_CLOCKID    CLOCK_REALTIME
#define AZ_SYS_TIMER_SIG        SIGRTMIN
#define AZ_SYS_TIMER_RING_MAX   128

/* declare global variables */

/* declare static variables */
static   HANDLE az_sys_timerQueue;

/* implement static functions */

/**
 * @fn        function name
 * @brief     function-description
 * @param     input-output-parameters
 * @return    return-value
 * @warning   warnings
 * @exception none
 */ 
static az_ring_t       az_sys_timer_ring;
static az_sys_timer_t  az_sys_timer_ring_data[AZ_SYS_TIMER_RING_MAX];
static HANDLE           az_sys_timer_sem;
static az_sys_xu_t az_sys_timer_xu = NULL;
static int az_sys_timerThreadOn = 0;

static VOID CALLBACK az_sys_timer_routine(PVOID lpParam, BOOLEAN TimerOrWaitFired)
{
  az_sys_timer_t t = (az_sys_timer_t)lpParam;
  if (az_sys_timerThreadOn && NULL != t) {
    if (TimerOrWaitFired) {
      if (t->repeat > 0) {
        if (--(t->repeat) == 0) {
          az_ring_push(&az_sys_timer_ring, &t);
          SetEvent(az_sys_timer_sem);
        }
      } else if (t->repeat == 0) {
          az_ring_push(&az_sys_timer_ring, &t);
          SetEvent(az_sys_timer_sem);
      }
      (t->handler)(t->arg);
    } else {
      az_ring_push(&az_sys_timer_ring, t);
      SetEvent(az_sys_timer_sem);
    }
  }
}

static void *az_sys_timerThread(void *arg)
{
  az_sys_timer_t t;

  while (az_sys_timerThreadOn) {
    WaitForSingleObjectEx(az_sys_timer_sem, INFINITE, TRUE);
    if (az_ring_pop(&az_sys_timer_ring, &t) < 0) continue;
    if (NULL != t) {
      if (NULL != t->timer) {
        DeleteTimerQueueTimer(az_sys_timerQueue, t->timer, NULL); 
        t->timer = NULL;
        t->state &= ~2;
      }
    }
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
  az_r_t result = AZ_ERR(AGAIN);
  
  if (!az_sys_timerThreadOn) {
    az_sys_timerQueue = CreateTimerQueue();
    az_ring_init(&az_sys_timer_ring, AZ_RING_TYPE_DSREF, AZ_SYS_TIMER_RING_MAX, az_sys_timer_ring_data);

    az_sys_timer_sem = CreateEvent(NULL, TRUE, FALSE, NULL);

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
    result = az_sys_xu_delete(az_sys_timer_xu);
    result = CloseHandle(az_sys_timer_sem);
    DeleteTimerQueue(az_sys_timerQueue);
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
az_r_t  az_sys_timer_create(const char *name, az_uint64_t interval_ns,  int repeat, void (*handler)(void *arg), void *arg, az_sys_timer_t *pTimer)
{
  az_r_t result = AZ_SUCCESS;
  az_sys_timer_t t;

  az_assert(0 != repeat);
  az_assert(NULL != pTimer);

  do {
    az_if_arg_null_break(name, result);
    az_if_arg_null_break(pTimer, result);

    t = az_malloc(sizeof(az_sys_timer_entity_t));
    az_if_alloc_null_break(t, result);

    *pTimer = t;

    t->interval = interval_ns / 1000000; //ms
    t->repeat = repeat;
    t->handler = handler;
    t->arg = arg;
    t->name = name;

    t->timer = NULL;
    t->state = 1;
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

    if (!(t->state & 1)) {
      result = AZ_ERR(STATE);
      break;
    }
    if (t->state & 2) { 
      az_sys_timer_stop(t);
    }
    if (AZ_SUCCESS == result) {
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
  BOOL bResult;

  do {
    az_if_arg_null_break(t, result);

    if (!(t->state & 1)) {
      result = AZ_ERR(STATE);
      break;
    }
    if (t->state & 2) {
      result = AZ_ERR(STATE);
      break;
    }

    bResult = CreateTimerQueueTimer(&(t->timer), 
                az_sys_timerQueue,
                (WAITORTIMERCALLBACK)az_sys_timer_routine, 
                (PVOID)t, 
                (DWORD)t->interval, 
                (DWORD)((t->repeat == 0)? 0:t->interval),
                WT_EXECUTEDEFAULT);
    if (!bResult) {
      result = AZ_ERR(ALLOC);
      break;
    } else {
      t->state |= 2;
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
az_r_t  az_sys_timer_stop(az_sys_timer_t t)
{
  az_r_t result = AZ_SUCCESS;
  do {
    az_if_arg_null_break(t, result);
    
    if (NULL != t->timer) {
      if ((t->state & 3) == 3) {
        if (NULL != t->timer) {
          DeleteTimerQueueTimer(az_sys_timerQueue, t->timer, NULL); 
          t->timer = NULL;
        }
        t->state &= ~2;
      }
    }
  } while (0);

  return result;
}


/* === end of AZ_SYS_TIMER_C === */
