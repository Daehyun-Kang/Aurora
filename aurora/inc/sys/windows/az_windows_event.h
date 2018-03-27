/**
 * @file   az_windows_event.h
 * @brief  
 * @date   10/06/16
 * @author fware
 *
 * Copyright (c) Fware, 2013-2017 - All Rights Reserved
 * You may use, distribute and modify this code under the 
 * terms of the "Aurora Source Code License".
 * See the file LICENSE for full license details.

 * You should have received a copy of  the LICENSE with this file.

 * If not, please contact to Fware with the information in the file CONTACTS.
 *
 * History: @date 
 *   - 2016-06-10 fware initial create
 */

#ifndef AZ_WINDOWS_EVENT_H
#define AZ_WINDOWS_EVENT_H
#include "az_assert.h"
#include "az_windows_lock.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* constants */

/* basic macros */

/* basic types */
#ifndef AZ_WINDOWS_DEF_H
#define AZ_SYS_EVENT_OPT_AND_ALL    1
#define AZ_SYS_EVENT_OPT_PRESERVE   2
typedef uint32_t az_sys_event_t;
#endif

/* structures */

/* structured types */
typedef struct {
  az_sys_event_t   events;
  az_sys_event_t   revents;

  az_sys_lw_lock_t     lock;
  HANDLE            event_wait;
} az_sys_ep_t;

/* macros */

/* variabls exposed */

/* inline functions */

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
static inline void az_sys_ep_init(az_sys_ep_t *ep)
{
  ep->events = 0;
  ep->revents = 0;

  az_sys_lw_lock_init(&(ep->lock));
  ep->event_wait = CreateEvent(NULL, FALSE, FALSE, NULL);
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
static inline void az_sys_ep_deinit(az_sys_ep_t *ep)
{
  ep->events = 0;
  ep->revents = 0;

  CloseHandle(ep->event_wait);
  az_sys_lw_lock_deinit(&(ep->lock));
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
static inline az_r_t az_sys_ep_send(az_sys_ep_t *ep, az_sys_event_t sevent)
{
  az_r_t r = AZ_SUCCESS;
  az_assert(NULL != ep);
  az_sys_lw_lock(&(ep->lock));
  ep->events |= sevent;
  if (0 == SetEvent(ep->event_wait)) {
    r = AZ_FAIL;
  }
  az_sys_lw_unlock(&(ep->lock));

  return r;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
static inline az_r_t az_sys_ep_recv(az_sys_ep_t *ep, az_sys_event_t wevents, az_int8_t options, az_sys_event_t *pevent)
{
  az_r_t r = AZ_ERR(ENTITY_NULL);
  az_assert(NULL != ep);
  az_assert(NULL != pevent);
  az_sys_event_t revents = ep->revents;

  if (options & AZ_SYS_EVENT_OPT_AND_ALL) {
    if ((wevents & revents) == wevents) {
      if (options & AZ_SYS_EVENT_OPT_PRESERVE) {
        ep->revents = (revents & ~wevents);
      } else {
        ep->revents = 0;
      }
      *pevent = revents; 
      r = AZ_SUCCESS;
    } else {
      *pevent = 0; 
      r = AZ_ERR(EMPTY);
    }
  } else {
    if (wevents & revents) {
      if (options & AZ_SYS_EVENT_OPT_PRESERVE) {
        ep->revents = (revents & ~wevents);
      } else {
        ep->revents = 0;
      }
      *pevent = revents; 
      r = AZ_SUCCESS;
    } else {
      *pevent = 0; 
      r = AZ_ERR(EMPTY);
    }
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
static inline az_r_t az_sys_ep_wait(az_sys_ep_t *ep, az_int64_t nsec)
{
  az_r_t r = AZ_ERR(ENTITY_NULL);
  DWORD result, tmo_ms;
  az_assert(NULL != ep);
  az_sys_event_t sevents;

  az_sys_lw_lock(&(ep->lock));
  sevents = ep->events;
  if (sevents) {
    ep->events &= ~sevents;
    r = AZ_SUCCESS;
    ep->revents |= sevents; 
  }
  az_sys_lw_unlock(&(ep->lock));

  do {
    if (AZ_SUCCESS == r) break;

    if (nsec > 0) {
      tmo_ms = nsec / 1000000;
      result = WaitForSingleObjectEx(ep->event_wait, tmo_ms, TRUE);
    } else {
      tmo_ms = INFINITE; 
      result = WaitForSingleObjectEx(ep->event_wait, tmo_ms, TRUE);
    }
    switch (result) {
      case WAIT_OBJECT_0:
        az_sys_lw_lock(&(ep->lock));
        sevents = ep->events;
        ep->events &= ~sevents;
        az_sys_lw_unlock(&(ep->lock));
        // no race on revents
        ep->revents |= sevents; 
        r = AZ_SUCCESS;
        break;
      case WAIT_IO_COMPLETION:
        r = AZ_ERR(NO_DATA);
        break;
      case WAIT_ABANDONED:
        r = AZ_ERR(INTERRUPT);
        break;
      case WAIT_TIMEOUT:
        r = AZ_ERR(TIMEOUT);
        break;
      case WAIT_FAILED:
      default:
        r = AZ_ERR(FAIL);
        break;
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
static inline az_r_t az_sys_ep_recv_wait(az_sys_ep_t *ep, az_int64_t nsec,
    az_sys_event_t wevents, az_int8_t options, az_sys_event_t *pevent)
{
  az_r_t r;

  do {
    r = az_sys_ep_wait(ep, nsec);
    if (AZ_SUCCESS != r) break;
    r = az_sys_ep_recv(ep, wevents, options, pevent);
  } while (0);

  return r;
}

/* function prototypes exposed */

#ifdef __cplusplus
}
#endif
#endif
