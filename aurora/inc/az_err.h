/**
 * @file   az_err.h
 * @brief  
 * @date   11/02/16
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
 *   - 2016-02-11 Daehyun Kang initial create
 */

#ifndef AZ_ERR_H
#define AZ_ERR_H

#ifdef __cplusplus
extern "C"
{
#endif

/* constants */

/* basic macros */

/* basic types */
#define AZ_ERRNO_BASE  1000 


typedef enum {
  AZ_ERRNO_SUCCESS = 0,
  AZ_ERRNO_FAIL,
  AZ_ERRNO_OVERFLOW=AZ_ERRNO_BASE,
  AZ_ERRNO_ARG_NULL,
  AZ_ERRNO_ENTITY_NULL,
  AZ_ERRNO_ALLOC,
  AZ_ERRNO_EVENT_FD,
  AZ_ERRNO_INVALID_OPR,
  AZ_ERRNO_STATE,
  AZ_ERRNO_AGAIN,
  AZ_ERRNO_TIMEOUT,
  AZ_ERRNO_INVALID_ARG,
  AZ_ERRNO_FULL,
  AZ_ERRNO_EMPTY,
  AZ_ERRNO_INVALID_TYPE,
  AZ_ERRNO_SIGACTION,
  AZ_ERRNO_EXIST,
  AZ_ERRNO_TOO_LONG,
  AZ_ERRNO_NO_MEM,
  AZ_ERRNO_NO_SPACE,
  AZ_ERRNO_INVALID,
  AZ_ERRNO_NO_RIGHT,
  AZ_ERRNO_READONLY,
  AZ_ERRNO_FAULT,
  AZ_ERRNO_ENTRY,
  AZ_ERRNO_BUSY,
  AZ_ERRNO_IO,
  AZ_ERRNO_NOT_DIR,
  AZ_ERRNO_NOT_EMPTY,
  AZ_ERRNO_OPEN_DIR,
  AZ_ERRNO_MALLOC,
  AZ_ERRNO_NOT_FOUND,
  AZ_ERRNO_ACCESS,
  AZ_ERRNO_NO_ENTRY,
  AZ_ERRNO_READ_ONLY,
  AZ_ERRNO_REG_ION,
  AZ_ERRNO_CREAT,
  AZ_ERRNO_QUIT,
  AZ_ERRNO_NO_BD,
  AZ_ERRNO_NO_DATA,
  AZ_ERRNO_INTERRUPT,
  AZ_ERRNO_SYS_READ,
  AZ_ERRNO_XML_PARSE,
  AZ_ERRNO_NOT_SUPPORT,
  AZ_ERRNO_CTX_NULL,
  AZ_ERRNO_LEVEL,
  AZ_ERRNO_PARAM,
  AZ_ERRNO_NO_RESOURCE,
  AZ_ERRNO_NOT_ACTV,
  AZ_ERRNO_OOR,
  AZ_ERRNO_NOT_READY,
  AZ_ERRNO_NO_APP_CFG,

  AZ_ERRNO_MAX
} az_errorno_t;

#define AZ_ERRNO(err)  AZ_ERRNO_##err

#define AZ_SUCCESS    AZ_ERRNO_SUCCESS 
#define AZ_FAIL       -AZ_ERRNO_FAIL

#define AZ_ERR_STR(x)  #x

#ifdef  ALLOC_ERR_STR
char *az_err_str_list[] = {
  AZ_ERR_STR(SUCCESS),
  AZ_ERR_STR(FAIL),
  AZ_ERR_STR(OVERFLOW),
  AZ_ERR_STR(ARG_NULL),
  AZ_ERR_STR(ENTITY_NULL),
  AZ_ERR_STR(ALLOC),
  AZ_ERR_STR(EVENT_FD),
  AZ_ERR_STR(INVALID_OPR),
  AZ_ERR_STR(STATE),
  AZ_ERR_STR(AGAIN),
  AZ_ERR_STR(TIMEOUT),
  AZ_ERR_STR(INVALID_ARG),
  AZ_ERR_STR(FULL),
  AZ_ERR_STR(EMPTY),
  AZ_ERR_STR(INVALID_TYPE),
  AZ_ERR_STR(SIGACTION),
  AZ_ERR_STR(EXIST),
  AZ_ERR_STR(TOO_LONG),
  AZ_ERR_STR(NO_MEM),
  AZ_ERR_STR(NO_SPACE),
  AZ_ERR_STR(INVALID),
  AZ_ERR_STR(NO_RIGHT),
  AZ_ERR_STR(READONLY),
  AZ_ERR_STR(FAULT),
  AZ_ERR_STR(ENTRY),
  AZ_ERR_STR(BUSY),
  AZ_ERR_STR(IO),
  AZ_ERR_STR(NOT_DIR),
  AZ_ERR_STR(NOT_EMPTY),
  AZ_ERR_STR(OPEN_DIR),
  AZ_ERR_STR(MALLOC),
  AZ_ERR_STR(NOT_FOUND),
  AZ_ERR_STR(ACCESS),
  AZ_ERR_STR(NO_ENTRY),
  AZ_ERR_STR(READ_ONLY),
  AZ_ERR_STR(REG_ION),
  AZ_ERR_STR(CREAT),
  AZ_ERR_STR(QUIT),
  AZ_ERR_STR(NO_BD),
  AZ_ERR_STR(NO_DATA),
  AZ_ERR_STR(INTERRUPT),
  AZ_ERR_STR(SYS_READ),
  AZ_ERR_STR(XML_PARSE),
  AZ_ERR_STR(NOT_SUPPORT),
  AZ_ERR_STR(CTX_NULL),
  AZ_ERR_STR(LEVEL),
  AZ_ERR_STR(PARAM),
  AZ_ERR_STR(NO_RESOURCE),
  AZ_ERR_STR(NOT_ACTV),
  AZ_ERR_STR(OOR),
  AZ_ERR_STR(NOT_READY),
  AZ_ERR_STR(NO_APP_CFG),
};
#else
extern char *az_err_str_list[]; 
#endif

#define az_err_str(err) \
  ((((err) > -AZ_ERRNO_MAX) && ((err) <= -AZ_ERRNO_BASE))? az_err_str_list[-1*(err+AZ_ERRNO_BASE) + 2]:((err == AZ_SUCCESS)? az_err_str_list[0]:((err == AZ_FAIL)? az_err_str_list[1]:"SYS_ERR")))

/* structures */

/* structured types */

/* macros */
#ifdef  CONFIG_AZ_ERR_LOG
#define AZ_ERR_L(err, level)  az_err_log_and_set(AZ_FILENAME, __FUNCTION__, __LINE__, -AZ_ERRNO_##err, level)
#define AZ_ERR(err)  az_err_log_and_set(AZ_FILENAME, __FUNCTION__, __LINE__, -AZ_ERRNO_##err, 0)
#define AZ_ERR_LOG(err) \
  if (err != AZ_SUCCESS) {\
    (void) az_err_log_and_set(AZ_FILENAME, __FUNCTION__, __LINE__, err, 1); \
  }
#else
#define AZ_ERR(err)  -AZ_ERRNO_##err
#define AZ_ERR_L(err, level)  -AZ_ERRNO_##err
#define AZ_ERR_LOG(err)  
#endif

/* variabls exposed */

/* inline functions */

/* function prototypes exposed */
extern az_r_t az_err_log_and_set(const char *f, const char *s, int n, az_r_t err, int);

#ifdef __cplusplus
}
#endif
#endif
