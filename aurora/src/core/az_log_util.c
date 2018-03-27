/**
 * @file   az_log_util.c
 * @brief  
 * @date   23/01/18
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
 *   - 2018-01-23 dhkang initial create
 */

/* include header files */
#include "az_def.h"
#include "az_err.h"
#include "az_assert.h"
#include "az_macros.h"
#include "az_malloc.h"
#include "az_atomic.h"
#include "az_string.h"
#include "az_log.h"
#include "az_printf.h"

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
static inline int az_log_format_info00(const char *filename, const char *funcname, int line, char *bp, int blen)
{
  return 0;
}
/* timestamp only enabled flag */
static inline int az_log_format_info01(const char *filename, const char *funcname, int line, char *bp, int blen)
{
  int tlen = 0;
  AZ_LOG_FORMAT_INFO_TIMESTAMP(bp, blen, tlen);

  return tlen;
}
/* function only enabled flag */
static inline int az_log_format_info02(const char *filename, const char *funcname, int line, char *bp, int blen)
{
  int tlen = 0;
  int nlen;
  nlen = snprintf(&bp[tlen], blen, "[%s] ", funcname);
  blen -= nlen; tlen += nlen;

  return tlen;
}
/* time|function enabled flag */
static inline int az_log_format_info03(const char *filename, const char *funcname, int line, char *bp, int blen)
{
  int tlen = 0;
  int nlen;

  AZ_LOG_FORMAT_INFO_TIMESTAMP(bp, blen, tlen);

  nlen = snprintf(&bp[tlen], blen, "[%s] ", funcname);
  blen -= nlen; tlen += nlen;

  return tlen;
}
/* line only enabled flag */
static inline int az_log_format_info04(const char *filename, const char *funcname, int line, char *bp, int blen)
{
  int tlen = 0;
  int nlen;
  
  nlen = snprintf(&bp[tlen], blen, "[%s  ", filename);
  blen -= nlen; tlen += nlen;

  return tlen;
}
/* time|line only enabled flag */
static inline int az_log_format_info05(const char *filename, const char *funcname, int line, char *bp, int blen)
{
  int tlen = 0;
  int nlen;
  
  AZ_LOG_FORMAT_INFO_TIMESTAMP(bp, blen, tlen);

  nlen = snprintf(&bp[tlen], blen, "[%s  ", filename);
  blen -= nlen; tlen += nlen;

  return tlen;
}
/* func|line only enabled flag */
static inline int az_log_format_info06(const char *filename, const char *funcname, int line, char *bp, int blen)
{
  int tlen = 0;
  int nlen;

  nlen = snprintf(&bp[tlen], blen, "[%s:%d] ", funcname, line);
  blen -= nlen; tlen += nlen;

  return tlen;
}
/* time|func|line only enabled flag */
static inline int az_log_format_info07(const char *filename, const char *funcname, int line, char *bp, int blen)
{
  int tlen = 0;
  int nlen;
  
  AZ_LOG_FORMAT_INFO_TIMESTAMP(bp, blen, tlen);

  nlen = snprintf(&bp[tlen], blen, "[%s:%d] ", funcname, line);
  blen -= nlen; tlen += nlen;

  return tlen;
}
/* file only enabled flag */
static inline int az_log_format_info08(const char *filename, const char *funcname, int line, char *bp, int blen)
{
  int tlen = 0;
  int nlen;
  nlen = snprintf(&bp[tlen], blen, "[%s] ", filename);
  blen -= nlen; tlen += nlen;

  return tlen;
}

/* time|file only enabled flag */
static inline int az_log_format_info09(const char *filename, const char *funcname, int line, char *bp, int blen)
{
  int tlen = 0;
  int nlen;
  AZ_LOG_FORMAT_INFO_TIMESTAMP(bp, blen, tlen);
  nlen = snprintf(&bp[tlen], blen, "[%s] ", filename);
  blen -= nlen; tlen += nlen;

  return tlen;
}

/* func|file only enabled flag */
static inline int az_log_format_info0A(const char *filename, const char *funcname, int line, char *bp, int blen)
{
  int tlen = 0;
  int nlen;
  
  nlen = snprintf(&bp[tlen], blen, "[%s:%s] ", filename, funcname);
  blen -= nlen; tlen += nlen;

  return tlen;
}

/* time|func|file only enabled flag */
static inline int az_log_format_info0B(const char *filename, const char *funcname, int line, char *bp, int blen)
{
  int tlen = 0;
  int nlen;
  
  AZ_LOG_FORMAT_INFO_TIMESTAMP(bp, blen, tlen);

  nlen = snprintf(&bp[tlen], blen, "[%s:%s] ", filename, funcname);
  blen -= nlen; tlen += nlen;

  return tlen;
}

/* file|line only enabled flag */
static inline int az_log_format_info0C(const char *filename, const char *funcname, int line, char *bp, int blen)
{
  int tlen = 0;
  int nlen;
  
  nlen = snprintf(&bp[tlen], blen, "[%s:%d] ", filename, line);
  blen -= nlen; tlen += nlen;

  return tlen;
}

/* time|file|line only enabled flag */
static inline int az_log_format_info0D(const char *filename, const char *funcname, int line, char *bp, int blen)
{
  int tlen = 0;
  int nlen;
  
  AZ_LOG_FORMAT_INFO_TIMESTAMP(bp, blen, tlen);

  nlen = snprintf(&bp[tlen], blen, "[%s:%d] ", filename, line);
  blen -= nlen; tlen += nlen;

  return tlen;
}

/* file|func|line only enabled flag */
static inline int az_log_format_info0E(const char *filename, const char *funcname, int line, char *bp, int blen)
{
  int tlen = 0;
  int nlen;
  
  nlen = snprintf(&bp[tlen], blen, "[%s:%s:%d] ", filename, funcname, line);
  blen -= nlen; tlen += nlen;

  return tlen;
}

/* time|file|func|line only enabled flag */
static inline int az_log_format_info0F(const char *filename, const char *funcname, int line, char *bp, int blen)
{
  int tlen = 0;
  int nlen;
  
  AZ_LOG_FORMAT_INFO_TIMESTAMP(bp, blen, tlen);

  nlen = snprintf(&bp[tlen], blen, "[%s:%s:%d] ", filename, funcname, line);
  blen -= nlen; tlen += nlen;

  return tlen;
}

/* proc only enabled flag */
static inline int az_log_format_info10(const char *filename, const char *funcname, int line, char *bp, int blen)
{
  int tlen = 0;
  AZ_LOG_FORMAT_INFO_PROC(bp, blen, tlen);

  return tlen;
}
/* proc|timestamp enabled flag */
static inline int az_log_format_info11(const char *filename, const char *funcname, int line, char *bp, int blen)
{
  int tlen = 0;
  AZ_LOG_FORMAT_INFO_TIMESTAMP(bp, blen, tlen);
  AZ_LOG_FORMAT_INFO_PROC(&bp[tlen], blen, tlen);

  return tlen;
}
/* proc|function only enabled flag */
static inline int az_log_format_info12(const char *filename, const char *funcname, int line, char *bp, int blen)
{
  int tlen = 0;
  int nlen;

  AZ_LOG_FORMAT_INFO_PROC(bp, blen, tlen);

  nlen = snprintf(&bp[tlen], blen, "[%s] ", funcname);
  blen -= nlen; tlen += nlen;

  return tlen;
}
/* proc|time|function enabled flag */
static inline int az_log_format_info13(const char *filename, const char *funcname, int line, char *bp, int blen)
{
  int tlen = 0;
  int nlen;

  AZ_LOG_FORMAT_INFO_TIMESTAMP(bp, blen, tlen);
  AZ_LOG_FORMAT_INFO_PROC(&bp[tlen], blen, tlen);

  nlen = snprintf(&bp[tlen], blen, "[%s] ", funcname);
  blen -= nlen; tlen += nlen;

  return tlen;
}
/* proc|line only enabled flag */
static inline int az_log_format_info14(const char *filename, const char *funcname, int line, char *bp, int blen)
{
  int tlen = 0;
  int nlen;

  AZ_LOG_FORMAT_INFO_PROC(bp, blen, tlen);
  
  nlen = snprintf(&bp[tlen], blen, "[%s  ", filename);
  blen -= nlen; tlen += nlen;

  return tlen;
}
/* proc|time|line only enabled flag */
static inline int az_log_format_info15(const char *filename, const char *funcname, int line, char *bp, int blen)
{
  int tlen = 0;
  int nlen;
  
  AZ_LOG_FORMAT_INFO_TIMESTAMP(bp, blen, tlen);
  AZ_LOG_FORMAT_INFO_PROC(&bp[tlen], blen, tlen);

  nlen = snprintf(&bp[tlen], blen, "[%s  ", filename);
  blen -= nlen; tlen += nlen;

  return tlen;
}
/* proc|func|line only enabled flag */
static inline int az_log_format_info16(const char *filename, const char *funcname, int line, char *bp, int blen)
{
  int tlen = 0;
  int nlen;

  AZ_LOG_FORMAT_INFO_PROC(bp, blen, tlen);

  nlen = snprintf(&bp[tlen], blen, "[%s:%d] ", funcname, line);
  blen -= nlen; tlen += nlen;

  return tlen;
}
/* proc|time|func|line only enabled flag */
static inline int az_log_format_info17(const char *filename, const char *funcname, int line, char *bp, int blen)
{
  int tlen = 0;
  int nlen;
  
  AZ_LOG_FORMAT_INFO_TIMESTAMP(bp, blen, tlen);
  AZ_LOG_FORMAT_INFO_PROC(&bp[tlen], blen, tlen);

  nlen = snprintf(&bp[tlen], blen, "[%s:%d] ", funcname, line);
  blen -= nlen; tlen += nlen;

  return tlen;
}
/* proc|file only enabled flag */
static inline int az_log_format_info18(const char *filename, const char *funcname, int line, char *bp, int blen)
{
  int tlen = 0;
  int nlen;

  AZ_LOG_FORMAT_INFO_PROC(bp, blen, tlen);

  nlen = snprintf(&bp[tlen], blen, "[%s] ", filename);
  blen -= nlen; tlen += nlen;

  return tlen;
}

/* proc|time|file only enabled flag */
static inline int az_log_format_info19(const char *filename, const char *funcname, int line, char *bp, int blen)
{
  int tlen = 0;
  int nlen;
  AZ_LOG_FORMAT_INFO_TIMESTAMP(bp, blen, tlen);
  AZ_LOG_FORMAT_INFO_PROC(&bp[tlen], blen, tlen);

  nlen = snprintf(&bp[tlen], blen, "[%s] ", filename);
  blen -= nlen; tlen += nlen;

  return tlen;
}

/* proc|func|file only enabled flag */
static inline int az_log_format_info1A(const char *filename, const char *funcname, int line, char *bp, int blen)
{
  int tlen = 0;
  int nlen;

  AZ_LOG_FORMAT_INFO_PROC(bp, blen, tlen);
  
  nlen = snprintf(&bp[tlen], blen, "[%s:%s] ", filename, funcname);
  blen -= nlen; tlen += nlen;

  return tlen;
}

/* proc|time|func|file only enabled flag */
static inline int az_log_format_info1B(const char *filename, const char *funcname, int line, char *bp, int blen)
{
  int tlen = 0;
  int nlen;
  
  AZ_LOG_FORMAT_INFO_TIMESTAMP(bp, blen, tlen);
  AZ_LOG_FORMAT_INFO_PROC(&bp[tlen], blen, tlen);

  nlen = snprintf(&bp[tlen], blen, "[%s:%s] ", filename, funcname);
  blen -= nlen; tlen += nlen;

  return tlen;
}

/* proc|file|line only enabled flag */
static inline int az_log_format_info1C(const char *filename, const char *funcname, int line, char *bp, int blen)
{
  int tlen = 0;
  int nlen;

  AZ_LOG_FORMAT_INFO_PROC(bp, blen, tlen);
  
  nlen = snprintf(&bp[tlen], blen, "[%s:%d] ", filename, line);
  blen -= nlen; tlen += nlen;

  return tlen;
}

/* proc|time|file|line only enabled flag */
static inline int az_log_format_info1D(const char *filename, const char *funcname, int line, char *bp, int blen)
{
  int tlen = 0;
  int nlen;
  
  AZ_LOG_FORMAT_INFO_TIMESTAMP(bp, blen, tlen);
  AZ_LOG_FORMAT_INFO_PROC(&bp[tlen], blen, tlen);

  nlen = snprintf(&bp[tlen], blen, "[%s:%d] ", filename, line);
  blen -= nlen; tlen += nlen;

  return tlen;
}

/* proc|file|func|line only enabled flag */
static inline int az_log_format_info1E(const char *filename, const char *funcname, int line, char *bp, int blen)
{
  int tlen = 0;
  int nlen;

  AZ_LOG_FORMAT_INFO_PROC(bp, blen, tlen);
  
  nlen = snprintf(&bp[tlen], blen, "[%s:%s:%d] ", filename, funcname, line);
  blen -= nlen; tlen += nlen;

  return tlen;
}

/* proc|time|file|func|line only enabled flag */
static inline int az_log_format_info1F(const char *filename, const char *funcname, int line, char *bp, int blen)
{
  int tlen = 0;
  int nlen;
  
  AZ_LOG_FORMAT_INFO_TIMESTAMP(bp, blen, tlen);
  AZ_LOG_FORMAT_INFO_PROC(&bp[tlen], blen, tlen);

  nlen = snprintf(&bp[tlen], blen, "[%s:%s:%d] ", filename, funcname, line);
  blen -= nlen; tlen += nlen;

  return tlen;
}

az_log_format_info_t az_log_format_info[0x20] = {
  az_log_format_info00, az_log_format_info01, az_log_format_info02, az_log_format_info03, 
  az_log_format_info04, az_log_format_info05, az_log_format_info06, az_log_format_info07, 
  az_log_format_info08, az_log_format_info09, az_log_format_info0A, az_log_format_info0B, 
  az_log_format_info0C, az_log_format_info0D, az_log_format_info0E, az_log_format_info0F, 
  az_log_format_info10, az_log_format_info11, az_log_format_info12, az_log_format_info13, 
  az_log_format_info14, az_log_format_info15, az_log_format_info16, az_log_format_info17, 
  az_log_format_info18, az_log_format_info19, az_log_format_info1A, az_log_format_info1B, 
  az_log_format_info1C, az_log_format_info1D, az_log_format_info1E, az_log_format_info1F 
};


/* implement global functions */

/**
 * @fn        function name
 * @brief     function-description
 * @param     input-output-parameters
 * @return    return-value
 * @warning   warnings
 * @exception none
 */


/* === end of AZ_LOG_UTIL_C === */
