/**
 * @file   az_trz_msg.h
 * @brief  
 * @date   20/03/18
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
 *   - 2018-03-20 dhkang initial create
 */

#ifndef AZ_TRZ_MSG_H
#define AZ_TRZ_MSG_H
#include "az_def.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* constants */

/* basic macros */
#define AZ_TRZ_CODE_ERROR_REPORT    0x3000
#define AZ_TRZ_CODE_QUERY_NODEID    0x3001
#define AZ_TRZ_CODE_KEEPALIVE       0x3002

#define AZ_TRZ_CODE_REQ(code)       (AZ_TRZ_CODE_##code|AZ_TRZ_REQ_FLAG) 
#define AZ_TRZ_CODE_RSP(code)       (AZ_TRZ_CODE_##code) 
/* basic types */

/* structures */

/* structured types */
AZ_PACKED_STRUCT(az_trz_msg_error_report) {
  az_trz_msg_hdr_t hdr;
  int errcode; 
};
AZ_PACKED_STRUCT(az_trz_msg_query_nodeid) {
  az_trz_msg_hdr_t hdr;
  az_trz_nodeid_t  local_nodeid;
  az_trz_nodeid_t  remote_nodeid;
};
AZ_PACKED_STRUCT(az_trz_msg_keepalive) {
  az_trz_msg_hdr_t hdr;
};

/* macros */

/* variabls exposed */

/* inline functions */

/* function prototypes exposed */

#ifdef __cplusplus
}
#endif
#endif
