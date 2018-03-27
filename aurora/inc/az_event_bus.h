/**
 * @file   az_event_bus.h
 * @brief  
 * @date   24/06/16
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
 *   - 2016-06-24 fware initial create
 */

#ifndef AZ_EVENT_BUS_H
#define AZ_EVENT_BUS_H

#include "az_ring.h"
#include "az_event.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* constants */
#define   AZ_EVENT_PORT_LIST_SZ           2048

/* basic macros */

/* basic types */

/* structures */
typedef struct {
  az_uint32_t     state;
  az_uint32_t      port_count;
  az_uint32_t      port_list_sz;
  az_event_port_t  *port_list;
} az_event_bus_t;

/* structured types */

/* macros */
#define AZ_EVENT_BUS()  (az_event_bus_pDefault)

/* variabls exposed */
extern az_event_bus_t *az_event_bus_pDefault;

/* inline functions */

/* function prototypes exposed */
extern az_r_t  az_event_bus_init();
extern az_r_t  az_event_bus_start();
extern az_r_t  az_event_bus_stop(); 
extern az_r_t  az_event_bus_denit();

extern az_event_port_t az_event_port_get(az_event_port_no_t portno);
extern az_r_t az_event_port_search(az_event_id_t event_id, az_event_id_t event_mask, int type, az_event_port_t *pPort); 
extern az_r_t az_event_port_create(az_event_id_t event_id, az_event_id_t event_mask, int type, az_event_port_t *pPort);
extern az_r_t az_event_port_attach(az_event_port_t port);
extern az_r_t az_event_port_detach(az_event_port_t port); 
extern az_r_t az_event_port_delete(az_event_port_t port);

extern void az_event_port_link_pool_init();
extern void az_event_port_link_pool_deinit();
extern az_event_port_link_t *az_event_port_link_create(az_event_port_t port);
extern void az_event_port_link_delete(az_event_port_link_t *link);

extern az_r_t az_event_port_in_default(struct az_event_port_entity *, az_event_t);
extern az_r_t az_event_port_in_exclusive(struct az_event_port_entity *, az_event_t);
extern az_r_t az_event_port_in_shared(struct az_event_port_entity *, az_event_t);
extern az_r_t az_event_port_out_default(struct az_event_port_entity *, az_event_t);
extern az_r_t az_event_port_out_exclusive(struct az_event_port_entity *, az_event_t);
extern az_r_t az_event_port_out_shared(struct az_event_port_entity *, az_event_t);

extern az_r_t az_event_port_add_receiver(az_event_port_t port, az_event_receiver_t *rcvr); 
extern az_r_t az_event_port_del_receiver(az_event_port_t port, az_event_receiver_t *rcvr);
#ifdef __cplusplus
}
#endif
#endif
