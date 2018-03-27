/**
 * @file   az_sys.h
 * @brief  
 * @date   01/02/16
 * @author Daehyun Kang
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
 *   - 2016-02-01 Daehyun Kang initial create
 */

#ifndef AZ_SYS_H
#define AZ_SYS_H

#include "az_config.h"

#ifndef AZ_CONFIG_H
#error  "az_config.h is missed"
#endif

#ifdef  AZ_DEF_H
#error  "az_def.h should be placed after az_sys.h"
#endif

#ifdef  __GNUC__
#define az_attr_constructor __attribute__((constructor))
#define az_attr_destructor __attribute__((destructor))
#define az_attr_no_instrument __attribute__((no_instrument_function))
#else
#define az_attr_constructor 
#define az_attr_destructor 
#define az_attr_no_instrument 
#endif

#ifdef  CONFIG_LINUX
#include  "sys/linux/az_sys_linux.h"
#endif

#ifdef  CONFIG_VXWORKS
#include  "sys/vxworks/az_sys_vxworks.h"
#endif

#ifdef  CONFIG_WINDOWS
#include  "sys/windows/az_sys_windows.h"
#endif

#endif
