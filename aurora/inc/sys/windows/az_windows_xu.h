/**
 * @file   az_windows_xu.h
 * @brief  
 * @date   09/06/16
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
 *   - 2016-06-09 fware initial create
 */

#ifndef AZ_WINDWS_XU_H
#define AZ_WINDWS_XU_H
#include "az_sys_windows.h"
#include "az_def.h"
#include "az_err.h"
#include "az_assert.h"
#include "az_windows_event.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* constants */

/* basic macros */

/* basic types */
#ifndef AZ_WINDOWS_DEF_H
typedef struct {
  HANDLE        thread;
  void          *(*entry)(void *); 
  void          *arg;
  az_thread_attr_t    *attr;

  az_sys_ep_t   ep;

  DWORD      threadId;

  void          *priv;
  char          *name;
} az_sys_xu_entity_t;

typedef az_sys_xu_entity_t*   az_sys_xu_t;
#define AZ_SYS_XU_INVALID    NULL

#endif
/* structures */

/* structured types */

/* macros */

/* variabls exposed */

/* inline functions */
/**
 * @fn        function name
 * @brief     function-description
 * @param     input-output-parameters
 * @return    return-value
 * @warning   warnings
 * @exception none
 */

/* function prototypes exposed */
extern az_r_t az_sys_xu_create(const char *name, void *(*entry)(void *), 
                    void *arg, void *pOptions, az_sys_xu_t *pXu);
extern az_r_t az_sys_xu_delete(az_sys_xu_t xu);

extern az_r_t az_sys_xu_setPriority(az_sys_xu_t xu, az_thread_attr_t *pAttr);
extern az_r_t az_sys_xu_getPriority(az_sys_xu_t xu, az_thread_attr_t *pAttr);
extern az_r_t az_sys_xu_setAffinity(az_sys_xu_t xu, az_thread_core_mask_t );
extern az_r_t az_sys_xu_getAffinity(az_sys_xu_t xu, az_thread_core_mask_t *);

extern az_r_t az_sys_xu_suspend(az_sys_xu_t xu);
extern az_r_t az_sys_xu_resume(az_sys_xu_t xu);
extern az_r_t az_sys_xu_sleep(az_int64_t nsec);
extern az_r_t az_sys_xu_sendEvent(az_sys_xu_t xu, az_sys_event_t event);
extern az_r_t az_sys_xu_recvEvent(az_sys_event_t toReceive, int8_t options,
                        az_int64_t nsec, az_sys_event_t *pReceived);

extern az_sys_xu_t az_sys_xu_self();
extern void  az_sys_xu_exit(void * ret);
extern az_r_t az_sys_xu_getcpu();

#ifdef __cplusplus
}
#endif
#endif
