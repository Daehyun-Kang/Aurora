/**
 * @file   az_socket.c
 * @brief  
 * @date   13/04/18
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
 *   - 2018-04-13 dhkang initial create
 */

/* include header files */
#include "az_socket.h"

/* declare global variables */

/* declare static variables */


/* implement static functions */

/**
 * @fn        function name
 * @brief     function-description
 * @param     input-output-parameters
 * @return    return-value
 * @warning   warnings
 * @exception none
 */


/* implement global functions */

/**
 * @fn        function name
 * @brief     function-description
 * @param     input-output-parameters
 * @return    return-value
 * @warning   warnings
 * @exception none
 */

az_ion_id_t az_socket_create(int domain, int type, int protocol, az_socket_t *p)
{
  az_r_t r = AZ_SUCCESS;
  az_socket_t s = NULL;

  do {
    if (p) {
      s = *p;
    }
    if (NULL == s) {
      s = az_malloc(sizeof(az_socket_entity_t));
      az_if_alloc_null_break(s, r);
      az_ion_invalidate(&s->ion, 0);
    } else {
      if (!AZ_ION_IS_IDLE_VALID(&s->ion, AZ_ION_TYPE_SOCK)) {
        r = AZ_ERR(INVALID_ARG);
        break;
      }
    }

    r = az_sys_socket_create(domain, type, protocol, &s->sys_socket);
    if (r != AZ_SUCCESS) {
      if (AZ_REFCOUNT_IS_ZERO(&s->ion.refCount)) {
        az_free(s);
        s = NULL;
      }
      break;
    }

    s->ion.id = s->sys_socket;

    r = az_ion_register(&(s->ion), AZ_ION_TYPE_SOCK);
    if (AZ_SUCCESS != r) {
      az_sys_socket_delete(s->sys_socket);
      if (AZ_REFCOUNT_IS_ZERO(&s->ion.refCount)) {
        az_free(s);
        if (p) *p = NULL;
        s = NULL;
      }
      break;
    } else {
      az_refcount_atomic_inc(&s->ion.refCount);
    }
    if (p) *p = s;
  } while (0);

  if (r != AZ_SUCCESS) {
    az_sys_rprintf(r, "socket id %d\n",(s == NULL)? -1:s->ion.id);
  }
  return (r < 0)? (az_ion_id_t)r:s->ion.id;
}

az_r_t az_socket_delete(az_ion_id_t id)
{
  az_r_t result = AZ_SUCCESS;
  az_socket_t s = (az_socket_t)az_ion(id);
  do {
    az_if_arg_null_break(s, result);
    az_assert_ion_type(s->ion.type, AZ_ION_TYPE_SOCK);
    if (az_refcount_atomic_dec(&s->ion.refCount) <= 0) {
      result = AZ_ERR(AGAIN);
      break;
    }

    if (0) {
      az_sys_eprintf("socket%d refCount=%d\n", id, AZ_REFCOUNT_VALUE(&s->ion.refCount));
    }

    result = az_ion_deregister(&(s->ion));
    if (AZ_SUCCESS != result) {
      break;
    }
    if (AZ_REFCOUNT_VALUE(&s->ion.refCount) == 0) {
      result = az_sys_socket_delete(s->sys_socket);
      s->sys_socket = AZ_SOCK_INVALID;
    }
    if (AZ_REFCOUNT_IS_ZERO(&s->ion.refCount)) {
      az_free(s);
    }
  } while (0);

  return result;
}

/* === end of AZ_SOCKET_C === */
