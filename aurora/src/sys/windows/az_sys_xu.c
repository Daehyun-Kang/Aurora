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
#include "az_windows_xu.h"
#include "az_malloc.h"

/* declare global variables */

/* declare static variables */
static __declspec(thread) az_sys_xu_t az_sys_xu;


/* implement static functions */

/**
 * @fn        function name
 * @brief     function-description
 * @param     input-output-parameters
 * @return    return-value
 * @warning   warnings
 * @exception none
 */
static void *az_sys_xu_entry(void *argp)
{
  az_sys_xu_t xu = (az_sys_xu_t *)argp;

  az_sys_xu = xu;
  
  return (xu->entry)(xu->arg);
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
az_r_t az_sys_xu_create(const char *name, void *(*entry)(void *), void *arg, void *pOptions, az_sys_xu_t *pXu)
{
  az_r_t result = AZ_SUCCESS;
  az_sys_xu_t xu;

  do {
    az_if_arg_null_break(name, result);
    az_if_arg_null_break(entry, result);
    az_if_arg_null_break(pXu, result);

    xu = az_malloc(sizeof(az_sys_xu_entity_t));
    az_if_alloc_null_break(xu, result);

    *pXu = xu;

    az_sys_ep_init(&(xu->ep));

    xu->name = name;
    xu->entry = entry;
    xu->arg = arg;
    SIZE_T  stackSize = 0;
    if (pOptions) {
        xu->attr = (az_xu_attr_t *)pOptions;
        if (xu->attr->stack_size > 0) {
            stackSize = (SIZE_T)xu->attr->stack_size;
        }
    } else {
        xu->attr = NULL;
    }
    xu->thread = CreateThread(
                    NULL, // LPSECURITY_ATTRIBUTES 
                    stackSize, // SIZE_T dwStackSize 
                    (LPTHREAD_START_ROUTINE)az_sys_xu_entry, 
                    (LPVOID)xu, 
                    0,    //DWORD dwCreationFlags
                    &(xu->threadId));

    if (NULL == xu->thread) { 
      az_free(xu);
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
az_r_t az_sys_xu_delete(az_sys_xu_t xu)
{
  az_r_t result = AZ_SUCCESS;
  HANDLE thread;

  do {
    az_if_arg_null_break(xu, result);

    if (GetCurrentThread() == (thread = xu->thread)) {
      result = AZ_ERR(INVALID_OPR);
      break;
    }

    az_sys_ep_deinit(&(xu->ep));

    az_free(xu);

    if (NULL != thread) {
        TerminateThread(thread, 0);
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
az_r_t az_sys_xu_setPriority(az_sys_xu_t xu, az_xu_attr_t *pAttr)
{
  az_r_t result = AZ_SUCCESS;
  int sched_policy;
  int priority;

  do {
    az_if_arg_null_break(xu, result);

    if (NULL == pAttr) {
        if (NULL != xu->attr) {
            sched_policy = xu->attr->sched_policy;
            priority = xu->attr->priority;
        } else {
            result = AZ_ERR(INVALID_ARG);
            break;
        }
    } else {
        sched_policy = pAttr->sched_policy; 
        priority = pAttr->priority; 
        if (xu->attr) {
            xu->attr->sched_policy = sched_policy;
            xu->attr->priority = priority;
        }
    }

    int nPriority;

    switch (sched_policy) {
        case SCHED_FIFO:
            nPriority = THREAD_PRIORITY_TIME_CRITICAL;
            break;
        case SCHED_RR:
            if (priority == AZ_XU_PRIORITY_MAX) {
                nPriority = THREAD_PRIORITY_HIGHEST;
            } else 
            if (priority == AZ_XU_PRIORITY_MIN) {
                nPriority = THREAD_PRIORITY_LOWEST;
            } else 
            if (priority == (AZ_XU_PRIORITY_MIN+AZ_XU_PRIORITY_MAX)/2) {
                nPriority = THREAD_PRIORITY_NORMAL;
            } else 
            if (priority > (AZ_XU_PRIORITY_MIN+AZ_XU_PRIORITY_MAX)/2) {
                nPriority = THREAD_PRIORITY_ABOVE_NORMAL;
            } else {
                nPriority = THREAD_PRIORITY_BELOW_NORMAL;
            }
            break;
        case SCHED_OTHER:
            nPriority = THREAD_PRIORITY_IDLE;
            break;
    }

    if (FALSE == SetThreadPriority(xu->thread, nPriority)) {
        result = AZ_FAIL;
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
az_r_t az_sys_xu_getPriority(az_sys_xu_t xu, az_xu_attr_t *pAttr)
{
  az_r_t result = AZ_SUCCESS;
  int sched_policy;
  int priority = 0;

  do {
    az_if_arg_null_break(xu, result);
    az_if_arg_null_break(pAttr, result);

    if (NULL != xu->attr) {
        sched_policy = xu->attr->sched_policy;
        priority = xu->attr->priority;
    } 

    int nPriority;
    if (THREAD_PRIORITY_ERROR_RETURN == (nPriority = GetThreadPriority(xu->thread))) {
        result = AZ_FAIL;
        break;
    }

    switch (nPriority) {
        case THREAD_PRIORITY_TIME_CRITICAL:
            sched_policy = SCHED_FIFO;
            break;
        case THREAD_PRIORITY_IDLE:
            sched_policy = SCHED_OTHER;
            break;
        default:
            sched_policy = SCHED_RR;
            if (nPriority == THREAD_PRIORITY_HIGHEST) {
                priority = AZ_XU_PRIORITY_MAX;
            } else 
            if (nPriority == THREAD_PRIORITY_LOWEST) {
                priority = AZ_XU_PRIORITY_MIN;
            } else 
            if ( nPriority == THREAD_PRIORITY_NORMAL) {
                priority = (AZ_XU_PRIORITY_MIN+AZ_XU_PRIORITY_MAX)/2; 
            } else 
            if (nPriority == THREAD_PRIORITY_ABOVE_NORMAL) {
                if (priority == 0) {
                    priority = (AZ_XU_PRIORITY_MIN+AZ_XU_PRIORITY_MAX)/2 + 1;
                }
            } else { //nPriority == THREAD_PRIORITY_BELOW_NORMAL
                if (priority == 0) {
                    priority = (AZ_XU_PRIORITY_MIN+AZ_XU_PRIORITY_MAX)/2 - 1;
                }
            }
            break;
    }
    if (NULL != pAttr) {
        pAttr->sched_policy = sched_policy;
        pAttr->priority = priority;
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
az_r_t az_sys_xu_setAffinity(az_sys_xu_t xu, az_xu_core_mask_t core_mask)
{
  az_r_t result = AZ_SUCCESS;
  DWORD dwMask = (DWORD)core_mask; 

  do {
    az_if_arg_null_break(xu, result);

    if(0 == SetThreadAffinityMask(xu->thread, (DWORD_PTR)dwMask)) {
        result = AZ_FAIL;
        if (xu->attr) {
            xu->attr->core_mask = 0; 
        }
    } else {
        if (xu->attr) {
            xu->attr->core_mask = core_mask;
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
az_r_t az_sys_xu_getAffinity(az_sys_xu_t xu, az_xu_core_mask_t *pCoreMask)
{
  az_r_t result = AZ_SUCCESS;

  do {
    az_if_arg_null_break(xu, result);
    az_if_arg_null_break(pCoreMask, result);

    if (xu->attr) {
        *pCoreMask = xu->attr->core_mask;
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
az_r_t az_sys_xu_getcpu()
{
    return (az_r_t)(GetCurrentProcessorNumber());
}

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

    if (SuspendThread(xu->thread) < 0) {
        result = AZ_FAIL;
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
az_r_t az_sys_xu_resume(az_sys_xu_t xu)
{
  az_r_t result = AZ_SUCCESS;
  do {
    az_if_arg_null_break(xu, result);

    if (ResumeThread(xu->thread) < 0) {
        result = AZ_FAIL;
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
  LARGE_INTEGER interval;

  if (nsec > 0) {
  interval.QuadPart = -(nsec / 100);
  #ifdef NT_NATIVE
  NtDelayExecution(FALSE, &interval);
  #else
  HANDLE timer;
  timer = CreateWaitableTimer(NULL, TRUE, NULL);
  SetWaitableTimer(timer, &interval, 0, NULL, NULL, 0);
  WaitForSingleObjectEx(timer, INFINITE, TRUE);
  CloseHandle(timer);
  #endif
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
void az_sys_xu_exit(void * ret)
{
  az_sys_xu_t xu = az_sys_xu_self();
  if (NULL != xu) {
    xu->thread = NULL;
    ExitThread((DWORD)ret);
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
az_r_t az_sys_xu_recvEvent(az_sys_event_t toReceive, az_int8_t options,
    az_int64_t nsec, az_sys_event_t *pReceived)
{
  az_r_t r = AZ_ERR(ENTITY_NULL);
  az_sys_xu_t xu = az_sys_xu_self();
  //DWORD result, tmo_ms;

  while (NULL != xu)
  {
    r = az_sys_ep_recv(&(xu->ep), toReceive, options, pReceived);
    if (r == AZ_SUCCESS) break;

    r = az_sys_ep_wait(&(xu->ep), nsec);
    if (AZ_SUCCESS != r) {
      break;
    }
  }

  return r;
}


/* === end of AZ_SYS_XU_C === */
