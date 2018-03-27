/**
 * @file   az_fsm.h
 * @brief  
 * @date   29/07/16
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
 *   - 2016-07-29 fware initial create
 */

#ifndef AZ_FSM_H
#define AZ_FSM_H

#include "az_event.h"
#include "mon/az_probe.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* constants */

/* basic macros */
#define AZ_FSM_TYPE_STATE_ARB         0
#define AZ_FSM_TYPE_STATE_SEQ         1

/* basic types */
typedef az_int32_t  az_fsm_state_t;
struct az_fsm;
typedef az_fsm_state_t (*az_fsm_handle_t)(void *, az_fsm_state_t, az_event_t );
typedef void (*az_fsm_run_t)(struct az_fsm *, az_event_t );

/* structures */
typedef struct az_fsm_vector {
  az_fsm_state_t  state;
  az_fsm_handle_t handler;
} az_fsm_vector_t;

/* structured types */
typedef struct az_fsm {
  char                  *name;
  int8_t                type;

  int8_t                substate;

  int16_t               size;
  az_fsm_vector_t       *vtable;
  az_fsm_vector_t       *vcur;
  az_fsm_run_t          run;
  void  (*onError)(void *, az_fsm_state_t, az_event_t);

  void                 *owner;

  AZ_PROBE_DEF_REF(probe);
} az_fsm_t;

/* macros */

/* variabls exposed */
static inline void az_fsm_set_owner(az_fsm_t *fsm, void *owner)
{
  az_assert(NULL != fsm);
  fsm->owner = owner;
}
static inline char *az_fsm_probe(az_fsm_t *fsm, int loc, az_event_t event)
{
  az_assert(fsm != NULL);
  az_assert(fsm->vcur != NULL);
  snprintf(az_xu_prtbuf,sizeof(az_xu_prtbuf), 
      "%s: (%d) state=h%x event=(h"AZ_FMT_EVENT_ID(1)":%d:h%p)\n",
      fsm->name, loc, 
      fsm->vcur->state, AZ_EVENT_ID(event), 
      AZ_EVENT_BUFFER_SIZE(event), AZ_EVENT_BUFFER_DATA(event));
  return az_xu_prtbuf;
}

/* inline functions */

static inline void az_fsm_run(az_fsm_t *fsm, az_event_t event)
{
  (fsm->run)(fsm, event);
}

static inline void az_fsm_run_state_seq(az_fsm_t *fsm, az_event_t event)
{
  az_fsm_state_t nstate;
  az_fsm_vector_t *vector = fsm->vcur;
  az_assert(vector != NULL);
  AZ_PROBE(fsm->probe, 1, event);
  do {
    nstate = (vector->handler)(fsm, vector->state, event);
    if (nstate < 0 || nstate >= fsm->size) {
      if (fsm->onError) {
        (fsm->onError)(fsm, nstate, event);
      } else {
        az_sys_printf("%s out state=%d OOR\n", fsm->name, nstate); 
      }
      break;
    }
    fsm->vcur = &fsm->vtable[nstate];
  } while (0);
  //az_logt("%s\n", az_fsm_probe(fsm, 2, event));
  //AZ_PROBE(fsm->probe, 2, event);
}

static inline void az_fsm_run_state_arb(az_fsm_t *fsm, az_event_t event)
{
  az_fsm_state_t nstate;
  az_fsm_vector_t *vector = fsm->vcur;
  az_assert(vector != NULL);
  AZ_PROBE_PRINTF(fsm->probe, 3, "state=h%x event=(h"AZ_FMT_EVENT_ID(1)":%d:h%p)\n",
      vector->state, AZ_EVENT_ID(event), 
      AZ_EVENT_BUFFER_SIZE(event), AZ_EVENT_BUFFER_DATA(event));
  int16_t size = 0;
  do {
    nstate = (vector->handler)(fsm, vector->state, event);
    for (vector = fsm->vtable; size < fsm->size; size++) {
      if (nstate == vector->state) {
        fsm->vcur = vector;
        break;
      }
    }
    if (size == fsm->size) {
      if (fsm->onError) {
        (fsm->onError)(fsm, nstate, event);
      } else {
        az_sys_printf("%s out state=%d OOR\n", fsm->name, nstate); 
      }
      break;
    }
  } while (0);
  AZ_PROBE_PRINTF(fsm->probe, 4, "state=h%x event=(h"AZ_FMT_EVENT_ID(1)":%d:h%p)\n",
      fsm->vcur->state, AZ_EVENT_ID(event), 
      AZ_EVENT_BUFFER_SIZE(event), AZ_EVENT_BUFFER_DATA(event));
}

static inline void az_fsm_activate(az_fsm_t *fsm, void *owner)
{
  az_assert(fsm != NULL);
  if (fsm->owner == NULL) {
    fsm->owner = owner;
    AZ_PROBE_REG(fsm->name, fsm, probe, (az_probe_callback_t)az_fsm_probe);
  } else {
    az_sys_printf("%s activate again\n", fsm->name);
  }
}

static inline void az_fsm_init(az_fsm_t *fsm, char *name, int8_t type,
    int16_t size, az_fsm_state_t istate, az_fsm_vector_t *vector, 
    void (*onError)(void *, az_fsm_state_t, az_event_t ), void *owner)
{
  fsm->name = name;
  fsm->type = type;
  fsm->size = size;
  fsm->vtable = vector;
  fsm->onError = onError;
  fsm->owner = NULL;
  fsm->run = NULL;

  if (type == AZ_FSM_TYPE_STATE_ARB) {
    fsm->vcur = NULL;
    for (; size > 0; size--) {
      if (vector->state == istate) {
        fsm->vcur = vector;
        break;
      }
    }
    if (fsm->vcur == NULL) {
      az_sys_printf("%s state=%d invalid\n", fsm->name, istate); 
    } else {
      fsm->run = az_fsm_run_state_arb;
    }
  } else if (type == AZ_FSM_TYPE_STATE_SEQ) {
    if (istate < 0 || istate >= size) {
      az_sys_printf("%s state=%d OOR\n", fsm->name, istate); 
      fsm->vcur = NULL;
    } else {
      fsm->vcur = &vector[istate];
      fsm->run = az_fsm_run_state_seq;
    }
  } else {
    az_sys_printf("%s type=%d invalid\n", fsm->name, fsm->type); 
  }

  if (fsm->run) {
    az_fsm_activate(fsm, owner);
  }
}


#define AZ_FSM_INIT_SEQ(_name, _size, _vector, _vcur, _onError, _owner) \
  { .name = _name, \
    .type = AZ_FSM_TYPE_STATE_SEQ, \
    .size = _size, \
    .vtable = _vector, \
    .vcur = _vcur, \
    .owner=_owner, \
    .run = az_fsm_run_state_seq, \
    .onError = _onError, \
    .probe = AZ_PROBE_REF_INVALID};

#define AZ_FSM_INIT_ARB(_name, _size, _vector, _vcur, _onError, _owner) \
  { .name = _name, .size = _size, .type=AZ_FSM_TYPE_STATE_ARB, .vtable = _vector, .vcur = _vcur, .owner=_owner, .run = az_fsm_run_state_arb, .onError = _onError, .probe = AZ_PROBE_REF_INVALID};

/* function prototypes exposed */

#ifdef __cplusplus
}
#endif
#endif
