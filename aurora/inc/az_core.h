/**
 * @file   az_core.h
 * @brief  
 * @date   01/05/16
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
 * History: @date 
 *   - 2016-05-01 fware initial create
 */

#ifndef AZ_CORE_H
#define AZ_CORE_H

#include "az_def.h"
#include "az_err.h"
#include "az_assert.h"
#include "az_atomic.h"
#include "az_malloc.h"
#include "az_bits.h"
#include "az_var.h"
#include "az_rw_lock.h"
#include "az_string.h"
#include "az_array.h"
#include "az_ion.h"
#include "az_link.h"
#include "az_fifo.h"
#include "az_buffer.h"
#include "az_ring.h"
#include "az_event.h"
#include "az_event_bus.h"
#include "az_sighandle.h"
#include "az_thread_cfg.h"
#include "az_thread.h"
#include "az_thread_main.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* constants */

/* basic macros */

/* basic types */

/* structures */

/* structured types */

/* macros */

/* variabls exposed */

/* inline functions */

/* function prototypes exposed */
extern az_r_t az_core_init();
extern az_r_t az_core_deinit();
extern az_state_t az_core_get_state();

#ifdef __cplusplus
}
#endif
#endif
