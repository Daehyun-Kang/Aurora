/**
 * @file   az_socket.h
 * @brief  
 * @date   28/07/16
 * @author fware
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
 *   - 2016-07-28 fware initial create
 */

#ifndef AZ_SOCKET_H
#define AZ_SOCKET_H

#include  "az_def.h"
#include  "az_err.h"
#include  "az_macros.h"
#include  "az_ion.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* constants */

/* basic macros */

/* basic types */
typedef struct {
  az_ion_t    ion;
  az_sys_socket_t sys_socket;
} az_socket_entity_t;

typedef az_socket_entity_t  *az_socket_t;

/* structures */

/* structured types */

/* macros */

/* variabls exposed */

/* inline functions */

/* function prototypes exposed */
extern az_ion_id_t az_socket_create(int domain, int type, int protocol, az_socket_t *p);
extern az_r_t az_socket_delete(az_ion_id_t id);

#ifdef __cplusplus
}
#endif
#endif
