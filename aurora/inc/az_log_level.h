/**
 * @file   az_log_level.h
 * @brief  
 * @date   28/03/18
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
 *   - 2018-03-28 dhkang initial create
 */

#ifndef AZ_LOG_LEVEL_H
#define AZ_LOG_LEVEL_H

#ifdef __cplusplus
extern "C"
{
#endif

/* constants */

/* basic macros */
// define log levels
#define AZ_LOG_LEVEL_SEVERE   10
#define AZ_LOG_LEVEL_ASSERT   20
#define AZ_LOG_LEVEL_ERROR    30
#define AZ_LOG_LEVEL_WARN     40
#define AZ_LOG_LEVEL_INFO     50
#define AZ_LOG_LEVEL_TRACE    60
#define AZ_LOG_LEVEL_DEBUG    70
#define AZ_LOG_LEVEL_VERBOSE  80

  // define log flags
#define AZ_LOG_FLAGS_TIME   0x0001
#define AZ_LOG_FLAGS_FUNC   0x0002
#define AZ_LOG_FLAGS_LINE   0x0004
#define AZ_LOG_FLAGS_FILE   0x0008
#define AZ_LOG_FLAGS_PROC   0x0010
#define AZ_LOG_FLAGS_DOUT   0x0020
#define AZ_LOG_FLAGS_SPOT   (AZ_LOG_FLAGS_FUNC|AZ_LOG_FLAGS_LINE|AZ_LOG_FLAGS_FILE)
#define AZ_LOG_FLAGS_ALL   (AZ_LOG_FLAGS_TIME|AZ_LOG_FLAGS_SPOT|AZ_LOG_FLAGS_PROC)

/* basic types */

/* structures */

/* structured types */

/* macros */

/* variabls exposed */

/* inline functions */

/* function prototypes exposed */

#ifdef __cplusplus
}
#endif
#endif
