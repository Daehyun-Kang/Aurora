/**
 * @file   az_tcpserver.h
 * @brief  
 * @date   25/09/17
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
 *   - 2017-09-25 dhkang initial create
 */

#ifndef AZ_TCPSERVER_H
#define AZ_TCPSERVER_H

#include "sys/az_inet.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* constants */

/* basic macros */
#define AZ_TCPSERVER_STATE_IDLE     0x0000
#define AZ_TCPSERVER_STATE_INIT     0x0001
#define AZ_TCPSERVER_STATE_START    0x0002
#define AZ_TCPSERVER_STATE_RUN      0x0004
#define AZ_TCPSERVER_STATE_CLIBUSY  0x0008
#define AZ_TCPSERVER_STATE_MASK     0x0fff
#define AZ_TCPSERVER_STATE_DALLOC   0x8000

/* basic types */

/* structures */
struct az_tcpserver;

struct az_tcpserver_oprs {
  int (*init)(struct az_tcpserver *);
  int (*start)(struct az_tcpserver *);
  void *(*run)(void *);
  int (*stop)(struct az_tcpserver *);
  int (*onClientConnection)(void *, az_sock_t cliSock, void *cliAddr);
};
struct az_tcpserver_cfg {
  char  *name;
  char  *ipAddrStr;
  int   port;
  int   backlog;
  int   flags;
  long  timeout;
}; 
typedef struct  az_tcpserver {
  volatile int state;

  struct az_tcpserver_cfg config;

  az_sock_t sock;
  void  *thread;
  void  *priv;

  struct az_tcpserver_oprs *oprs;

} az_tcpserver_t;

/* structured types */

/* macros */

/* variabls exposed */
extern struct az_tcpserver_oprs az_tcpserver_oprs_default;

/* inline functions */

/* function prototypes exposed */
extern az_tcpserver_t *az_tcpserver_create(az_tcpserver_t *svr, 
    struct az_tcpserver_cfg *cfg, struct az_tcpserver_oprs *oprs, void *priv);
extern int az_tcpserver_delete(az_tcpserver_t *svr);

#ifdef __cplusplus
}
#endif
#endif
