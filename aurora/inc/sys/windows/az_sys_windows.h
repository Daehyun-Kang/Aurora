/**
 * @file   az_sys_windows.h
 * @brief  
 * @date   05/02/16
 * @author Daehyun Kang
 *
 * Copyright (c) Fware, 2013-2017 - All Rights Reserved
 * You may use, distribute and modify this code under the 
 * terms of the "Aurora Source Code License".
 * See the file LICENSE for full license details.

 * You should have received a copy of  the LICENSE with this file.

 * If not, please contact to Fware with the information in the file CONTACTS.
 *
 * History: @date 
 *   - 2016-02-05 Daehyun Kang initial create
 */

#ifndef AZ_SYS_WINDOWS_H
#define AZ_SYS_WINDOWS_H


#include <intrin.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <io.h>
#include <time.h>
#include <assert.h>

#include <process.h>
//#include <threads.h>
#include <conio.h>

#include <winsock2.h>

#include <windows.h>


#pragma warning(disable:4996)

#ifdef __cplusplus
extern "C"
{
#endif

/* constants */

/* basic macros */
#define AZ_SYS_TLS  __declspec(thread)

#ifndef __LITTLE_ENDIAN
#define __LITTLE_ENDIAN 1234
#define __BIG_ENDIAN    4321
#define __PDP_ENDIAN    3412
#endif

#ifndef __BYTE_ORDER
#define __BYTE_ORDER  __LITTLE_ENDIAN
#endif
/* basic types */

/* structures */

/* structured types */

/* macros */
#ifdef  UNICODE
#define AZ_GET_TSTR(_in, _out, _out_len) \
  wchar_t _wstrbuf[_out_len];\
  int _wstrlen = MultiByteToWideChar(CP_UTF8, 0, _in, -1, _wstrbuf, _out_len-1);\
  if (_wstrlen >= 0) _wstrbuf[_wstrlen] = 0;\
  _out = (LPCTSTR)_wstrbuf;
#else
#define AZ_GET_TSTR(_in, _out, _out_len)  \
  _out = (LPCTSTR)(_in);
#endif

#ifndef _SSIZE_T_DEFINED
#define _SSIZE_T_DEFINED
#undef  ssize_t
  #ifdef  _WIN64
    typedef __int64 ssize_t;
  #else
    typedef int ssize_t;
  #endif
#endif

/* variabls exposed */

/* inline functions */

/* function prototypes exposed */

#ifdef __cplusplus
}
#endif
#endif
