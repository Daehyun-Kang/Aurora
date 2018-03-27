/**
 * @file   az_string.h
 * @brief  
 * @date   06/07/16
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
 *   - 2016-07-06 fware initial create
 */

#ifndef AZ_STRING_H
#define AZ_STRING_H

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
#ifndef isprintable
#define isprintable(c)    ((c) > 0x20 && (c) < 127)
#endif

#define az_getprintable(c) (isprintable(c)? c:'.')

#ifndef MIN
#define MIN(a,b)    (((a) < (b))? (a):(b))
#endif

/* variabls exposed */

/* inline functions */
#ifdef  CONFIG_M64
static inline void *az_memcpy(void *dest, const void *src, az_size_t size)
{
  uint64_t *d64 = (uint64_t *)dest;
  uint64_t *s64 = (uint64_t *)src;
  uint8_t *d8; 
  uint8_t *s8;
  for (;size > sizeof(uint64_t); size -= sizeof(uint64_t)) {
    *d64++ = *s64++;
  }
  d8 = (uint8_t *)d64;
  s8 = (uint8_t *)s64;
  if (size >= sizeof(uint32_t)) {
    *(uint32_t *)d8 = *(uint32_t *)s8;
    d8 += sizeof(uint32_t);
    s8 += sizeof(uint32_t);
    size -= sizeof(uint32_t);
  } 
  for (; size > 0; size--) {
    *d8++ = *s8++;
  }
  return dest; 
}
static inline void *az_memset(void *s, int c, az_size_t n)
{
  uint64_t *s64 = (uint64_t *)s;
  uint64_t c64 = 0;
  uint8_t *s8;
  if (c) {
    c64 = c|(c<<8)|(c<<16)|(c<<24);
    c64 = c64|(c64<<32); 
  }
  for (;n > sizeof(uint64_t); n -= sizeof(uint64_t)) {
    *s64++ = c64; 
  }
  s8 = (uint8_t *)s64;
  if (n >= sizeof(uint32_t)) {
    *(uint32_t *)s8 = c; 
    s8 += sizeof(uint32_t);
    n -= sizeof(uint32_t);
  } 
  for (; n > 0; n--) {
    *s8++ = (uint8_t)c; 
  }
  return (void *)s;
}
#else
static inline void *az_memcpy(void *dest, const void *src, az_size_t size)
{
  uint32_t *d32 = (uint32_t *)dest;
  uint32_t *s32 = (uint32_t *)src;
  uint8_t *d8; 
  uint8_t *s8;
  for (;l32 > sizeof(uint32_t); size -= sizeof(uint32_t)) {
    *d32++ = *s32++;
  }
  d8 = (uint8_t *)d32;
  s8 = (uint8_t *)s32;
  for (; size > 0; size--) {
    *d8++ = *s8++;
  }
  return (void *)d8;
}
static inline void *az_memset(void *s, int c, az_size_t n)
{
  uint32_t *s32 = (uint32_t *)s;
  uint32_t c32 = 0;
  uint8_t *s8;
  if (c) {
    c32 = c|(c<<8)|(c<<16)|(c<<24);
  }
  for (;n > sizeof(uint32_t); n -= sizeof(uint32_t)) {
    *s32++ = c32; 
  }
  s8 = (uint8_t *)s32;
  for (; n > 0; n--) {
    *s8++ = (uint8_t)c; 
  }
  return (void *)s;
}
#endif

static inline int is_space(char c) {
  static const char *ws = " \t\n\r\f\v";
  const char *t = ws;
  for (;*t != 0; t++) { if (c == *t) break; } 
  return (int)(*t);
}
static inline char to_lower(char c) {
  return ((c >= 'A' && c <= 'Z')? (c- 'A' + 'a') : c);
}

static inline int string_begins_no_case(const char *s1, const char *s2)
{
  if (*s1 == 0) return -2;
  while (*s1 != 0) {
    if (*s2 == 0) return -1;
    if (to_lower(*s1) != to_lower(*s2)) return 0;
    ++s1;
    ++s2;
  }
  return 1;
}

static inline unsigned char az_hex2byte(char *val)
{
  unsigned char temp = *val;
  unsigned char temp1 = *(val+1);

  do {
    if (temp >= 'a' && temp <= 'f') { temp = temp - 'a' + 10; break; }
    if (temp >= 'A' && temp <= 'F') { temp = temp - 'A' + 10; break; }
    temp = temp - '0';
  } while (0);

  do {
    if (temp1 >= 'a' && temp1 <= 'f') { temp1 = temp1 - 'a' + 10; break; }
    if (temp1 >= 'A' && temp1 <= 'F') { temp1 = temp1 - 'A' + 10; break; }
    temp1 = temp1 - '0';
  } while (0);

  return (temp * 16 + temp1);
}

static inline int az_hexStringDecode(char *outBytes, int outlen, char *inHexStr)
{
  az_assert(NULL != outBytes);
  az_assert(NULL != inHexStr);

  int len = strlen(inHexStr)/2;
  int i;

  for (i = 0; i < len && outlen > 0; i++) {
    outBytes[i] = az_hex2Byte(&inHexStr[i*2]);
    outlen--;
  }
  if (outlen > 0) outBytes[i] = 0;

  return i;
}
static inline int az_mkTableHeaderStr(int cols, char *wlist, char *nlist[], char *bp, int blen)
{
  char  fmt[8];
  int j;
  int tlen = 0, nlen;

  for (j = 0; j < cols; j++, wlist++) {
    sprintf(fmt, "%%%ds", *wlist);
    nlen = snprintf(bp, blen, fmt, nlist[j]); 
    tlen += nlen; bp += nlen; blen -= nlen;
    if (blen <= 0) break;
  }

  return tlen;
}

/* function prototypes exposed */
extern int az_memdisp(az_sys_fd_t fd, char *addr, int32_t len, int mode, int interfactive, az_sys_fd_t rfd);

#ifdef __cplusplus
}
#endif
#endif
