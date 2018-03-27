/**
 * @file   az_linux_xu.h
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

#ifndef AZ_LINUX_XU_H
#define AZ_LINUX_XU_H
#include "az_def.h"
#include "az_err.h"
#include "az_linux_event.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* constants */

/* basic macros */

/* basic types */
#ifndef AZ_LINUX_DEF_H
typedef struct {
  pthread_t       thread;
  void            *(*entry)(void *); 
  void            *arg;
  az_xu_attr_t    *attr;

  az_sys_ep_t     ep;
  sem_t           sem_suspend;
  az_sys_iomux_t  iomux;

  void            *priv;
  char            *name;
  int             state;

  jmp_buf         env;
} az_sys_xu_entity_t;

typedef az_sys_xu_entity_t*   az_sys_xu_t;
#define AZ_SYS_XU_INVALID    NULL

#endif
/* structures */

/* structured types */

/* macros */

/* variabls exposed */
extern AZ_SYS_TLS az_sys_xu_t az_sys_xu;

#define az_sys_xu_self()    az_sys_xu

/* inline functions */
/**
 * @fn        function name
 * @brief     function-description
 * @param     input-output-parameters
 * @return    return-value
 * @warning   warnings
 * @exception none
 */
static inline az_sys_ep_t *az_sys_xu_eport()
{
  az_sys_xu_t xu = az_sys_xu;
  if (NULL == xu) return NULL;
  else return &(xu->ep);
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
static inline az_sys_iomux_t az_sys_xu_iomux()
{
  az_sys_xu_t xu = az_sys_xu;
  if (NULL == xu) return AZ_SYS_IOMUX_INVALID;
  else return xu->iomux;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
static inline az_r_t az_sys_xu_open_iomux()
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

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
static inline az_r_t az_sys_xu_close_iomux()
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
static inline az_r_t az_sys_xu_iomux_add(az_sys_io_t fd, uint32_t ioevt)
{
  az_assert(NULL != az_sys_xu);
  return (az_r_t)az_sys_iomux_add(az_sys_xu->iomux, fd, ioevt);
}
static inline az_r_t az_sys_xu_iomux_del(az_sys_io_t fd)
{
  az_assert(NULL != az_sys_xu);
  return (az_r_t)az_sys_iomux_del(az_sys_xu->iomux, fd);
}


static inline az_sys_io_t az_sys_xu_io_default()
{
  az_assert(NULL != az_sys_xu);
  az_sys_xu_t xu = az_sys_xu;

  return (xu->ep.fd);
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
static inline az_r_t az_sys_xu_wait_iomux(az_sys_io_event_t *ioevt, int maxevents, int tmo_ms)
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

/* function prototypes exposed */
extern void az_sys_xu_register_exception_handler();
extern int az_sys_xu_save_context();

extern az_r_t az_sys_xu_init(const char *name, 
    void *(*entry)(void *), void *arg, void *pOptions, az_sys_xu_t *pXu);
extern az_r_t az_sys_xu_create(const char *name, void *(*entry)(void *), 
                    void *arg, void *pOptions, az_sys_xu_t *pXu);

extern void az_sys_xu_cleanup(az_sys_xu_t xu);

extern az_r_t az_sys_xu_delete(az_sys_xu_t xu);

extern az_r_t az_sys_xu_setPriority(az_sys_xu_t xu, az_xu_attr_t *pAttr);
extern az_r_t az_sys_xu_getPriority(az_sys_xu_t xu, az_xu_attr_t *pAttr);
extern az_r_t az_sys_xu_setAffinity(az_sys_xu_t xu, az_xu_core_mask_t );
extern az_r_t az_sys_xu_getAffinity(az_sys_xu_t xu, az_xu_core_mask_t *);

extern az_r_t az_sys_xu_suspend(az_sys_xu_t xu);
extern az_r_t az_sys_xu_resume(az_sys_xu_t xu);
extern az_r_t az_sys_xu_sleep(az_int64_t nsec);
extern az_r_t az_sys_xu_sendEvent(az_sys_xu_t xu, az_sys_event_t event);
extern az_r_t az_sys_xu_recvEvent(az_sys_event_t toReceive, int8_t options,
                        az_int64_t nsec, az_sys_event_t *pReceived);
extern az_sys_event_t az_sys_xu_readEvent(az_sys_event_t aevent);
extern az_r_t az_sys_xu_waitEvent(az_sys_event_t aevent, 
                        az_sys_event_t toReceive, int8_t options, 
                        az_int64_t nsec, 
                        az_sys_event_t *qReceived,
                        az_sys_event_t *pReceived);
extern void  az_sys_xu_exit(void * ret);
extern az_sys_ep_t *az_sys_xu_eport();
extern az_r_t az_sys_xu_getcpu();

#ifdef __cplusplus
}
#endif
#endif
