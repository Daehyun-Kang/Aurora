/**
 * @file   az_ver.h
 * @brief  
 * @date   05/11/17
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
 *   - 2017-11-05 dhkang initial create
 */

#ifndef AZ_VER_H
#define AZ_VER_H

#ifdef __cplusplus
extern "C"
{
#endif

/* constants */

/* basic macros */

/* basic types */
typedef struct az_version {
  unsigned char   major;
  unsigned char   minor;
  unsigned char   stage;  //alpha,beta,rc,relase,stable
  unsigned char   build;
} az_version_t;

/* structures */

/* structured types */

/* macros */

/* variabls exposed */
extern az_version_t az_version;
extern char *az_build_time;

/* inline functions */

/* function prototypes exposed */

#ifdef __cplusplus
}
#endif
#endif
