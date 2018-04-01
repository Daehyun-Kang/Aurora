/**
 * @file   az_memdisp.c
 * @brief  
 * @date   13/03/18
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
 *   - 2018-03-13 dhkang initial create
 */

/* include header files */
#include "az_def.h"
#include "az_string.h"
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


/* implement global functions */

/**
 * @fn        function name
 * @brief     function-description
 * @param     input-output-parameters
 * @return    return-value
 * @warning   warnings
 * @exception none
 */
int az_memdisp_header(char *bp, int blen, int32_t len, int mode) 
{
	int i;
  int tlen = 0, nlen;
  char *fmts[] = {NULL, "%02x ", "%04x ", NULL, "%08x ", NULL, NULL, NULL, "%016lx "}; 
	nlen = snprintf(bp, blen, "%16s ", " "); 
  tlen += nlen; bp += nlen; blen -= nlen;
  char *fmt = fmts[mode];
  for (i = 0; i < MIN(len, 0x10); i += mode) {
    nlen = snprintf(bp, blen, fmt, i);
    tlen += nlen; bp += nlen; blen -= nlen;
  }

  return tlen;
}

#define A(i)  az_getprintable(addr[i])

int az_memdisp_row8(char *bp, int blen, void *addr)
{
  char *fmt = "%02x %02x %02x %02x %02x %02x %02x %02x ";
  int tlen = 0, nlen;
	int i;
  unsigned char *v = (unsigned char *)addr;
  nlen = snprintf(bp, blen, fmt, v[0], v[1], v[2], v[3], v[4], v[5],v[6], v[7]);
  tlen += nlen; bp += nlen; blen -= nlen;
  if (blen <= 0) return tlen;
  nlen = snprintf(bp, blen, fmt, v[8], v[9], v[10], v[11], v[12],v[13], v[14], v[15]);
  tlen += nlen; bp += nlen; blen -= nlen;

  return tlen;
}

int az_memdisp_row16(char *bp, int blen, void *addr)
{
  char *fmt = "%04x %04x %04x %04x %04x %04x %04x %04x ";
  int tlen = 0, nlen;
	int i;
  unsigned short *v = (unsigned short *)addr;
  nlen = snprintf(bp, blen, fmt, v[0], v[1], v[2], v[3], v[4], v[5],v[6], v[7]);
  tlen += nlen; bp += nlen; blen -= nlen;

  return tlen;
}
int az_memdisp_row32(char *bp, int blen, void *addr)
{
  char *fmt = "%08x %08x %08x %08x";
  int tlen = 0, nlen;
	int i;
  unsigned int *v = (unsigned int *)addr;
  nlen = snprintf(bp, blen, fmt, v[0], v[1], v[2], v[3]);
  tlen += nlen; bp += nlen; blen -= nlen;

  return tlen;
}

int az_memdisp_row64(char *bp, int blen, void *addr)
{
  char *fmt = "%016lx %016lx";
  int tlen = 0, nlen;
	int i;
  unsigned long *v = (unsigned long *)addr;
  nlen = snprintf(bp, blen, fmt, v[0], v[1]);
  tlen += nlen; bp += nlen; blen -= nlen;

  return tlen;
}

int az_memdisp_row(char *bp, int blen, char *addr, int mode) 
{ 
  int tlen = 0, nlen;
  int (*f)(char *, int , void *) = NULL;

  nlen = snprintf(bp, blen, "%16p ", (unsigned long)addr);
  tlen += nlen; bp += nlen; blen -= nlen;
  if (blen <= 0) return tlen;

  switch (mode) {
      case 1: f = az_memdisp_row8; break;
      case 2: f = az_memdisp_row16; break;
      case 4: f = az_memdisp_row32; break;
      case 8: f = az_memdisp_row64; break;
    }
  az_assert(NULL != f);
  nlen = (f)(bp, blen, (void *)addr);
  tlen += nlen; bp += nlen; blen -= nlen;
  if (blen <= 0) return tlen;

  nlen = snprintf(bp, blen, "   \"%c%c%c%c%c%c%c%c", A(0), A(1), A(2), A(3), A(4), A(5), A(6), A(7)); 
  tlen += nlen; bp += nlen; blen -= nlen;
  if (blen <= 0) return tlen;

  nlen = snprintf(bp, blen, "%c%c%c%c%c%c%c%c\"",A(8), A(9), A(10), A(11), A(12), A(13), A(14), A(15)); 
  tlen += nlen; bp += nlen; blen -= nlen;
  if (blen <= 0) return tlen;

  return tlen;
}

int az_memdisp_row_partial8(char *bp, int blen, void *addr, int32_t len) 
{
  int tlen = 0, nlen;
  char *fmt = "%02x ";
  int i;
  unsigned char *v = (unsigned char *)addr;
  for (i = 0; i < len; i++) {
    nlen = snprintf(bp, blen, fmt, v[i]);
    tlen += nlen; bp += nlen; blen -= nlen;
  }
  for (; i < 0x10; i++) {
    nlen = snprintf(bp, blen, "%2s ", "..");
    tlen += nlen; bp += nlen; blen -= nlen;
  }
  return tlen;
}
int az_memdisp_row_partial16(char *bp, int blen, void *addr, int32_t len) 
{
  int tlen = 0, nlen;
  char *fmt = "%04x ";
  int i;
  len = len / 2;
  unsigned short *v = (unsigned short *)addr;
  for (i = 0; i < len; i++) {
    nlen = snprintf(bp, blen, fmt, v[i]);
    tlen += nlen; bp += nlen; blen -= nlen;
  }
  for (; i < 0x10/2; i++) {
    nlen = snprintf(bp, blen, "%4s ", "....");
    tlen += nlen; bp += nlen; blen -= nlen;
  }
  return tlen;
}
int az_memdisp_row_partial32(char *bp, int blen, void *addr, int32_t len) 
{
  int tlen = 0, nlen;
  char *fmt = "%08x ";
  int i;
  unsigned int *v = (unsigned int *)addr;
  len = len / 4;
  for (i = 0; i < len; i++ ) {
    nlen = snprintf(bp, blen, fmt, v[i]);
    tlen += nlen; bp += nlen; blen -= nlen;
  }
  for (; i < 0x10/4; i++) {
    nlen = snprintf(bp, blen, "%8s ", "........");
    tlen += nlen; bp += nlen; blen -= nlen;
  }
  return tlen;
}
int az_memdisp_row_partial64(char *bp, int blen, void *addr, int32_t len) 
{
  int tlen = 0, nlen;
  char *fmt = "%16lx ";
  int i;
  unsigned long *v = (unsigned long *)addr;
  len = len / 8;
  for (i = 0; i < len; i += 8) {
    nlen = snprintf(bp, blen, fmt, v[i]);
    tlen += nlen; bp += nlen; blen -= nlen;
  }
  for (; i < 0x10/8; i++) {
    nlen = snprintf(bp, blen, "%16s ", "................");
    tlen += nlen; bp += nlen; blen -= nlen;
  }
  return tlen;
}


int az_memdisp_row_partial(char *bp, int blen, char *addr, int32_t len, int mode) 
{
  az_assert(len < 0x10);

  int tlen = 0, nlen;
  int (*f)(char *, int , void *, int32_t) = NULL;
  int i;

  nlen = snprintf(bp, blen, "%16p ", (unsigned long)addr);
  tlen += nlen; bp += nlen; blen -= nlen;

  switch (mode) {
      case 1: f = az_memdisp_row_partial8; break;
      case 2: f = az_memdisp_row_partial16; break;
      case 4: f = az_memdisp_row_partial32; break;
      case 8: f = az_memdisp_row_partial64; break;
    }
  az_assert(NULL != f);
  nlen = (f)(bp, blen, (void *)addr, len);
  tlen += nlen; bp += nlen; blen -= nlen;

  nlen = snprintf(bp, blen, "   \"");
  tlen += nlen; bp += nlen; blen -= nlen;
  for (i = 0; i < len; i++) {
    nlen = snprintf(bp, blen, "%c", A(i));
    tlen += nlen; bp += nlen; blen -= nlen;
  }
  nlen = snprintf(bp, blen, "\"");
  tlen += nlen; bp += nlen; blen -= nlen;

  return tlen;
}

#undef A(i) 

  static char buf[512]; 
int az_attr_no_instrument az_memdisp(az_sys_fd_t fd, char *addr, int32_t len, int mode, int interfactive, az_sys_fd_t rfd) 
{ 
  int nlen;
  int tlen = 0;
  char ch;
  int count = 0;

  nlen = az_memdisp_header(buf, sizeof(buf), len, mode); 
  tlen += nlen;
  az_sys_fs_write(fd, buf, nlen);
  az_sys_fs_write(fd, AZ_NL, strlen(AZ_NL));
  //az_sys_printf("%s\n", buf);
  do {
    //az_sys_printf("%s\n", buf);
    if (len >= 0x10) {
      nlen = az_memdisp_row(buf, sizeof(buf), addr, mode); 
      tlen += nlen;
      az_sys_fs_write(fd, buf, nlen);
      az_sys_fs_write(fd, AZ_NL, strlen(AZ_NL));
      //az_sys_printf("%s\n", buf);
    } else {
      nlen = az_memdisp_row_partial(buf, sizeof(buf), addr, len, mode); 
      tlen += nlen;
      az_sys_fs_write(fd, buf, nlen);
      az_sys_fs_write(fd, AZ_NL, strlen(AZ_NL));
      //az_sys_printf("%s\n", buf);
      break;
    }
    count++;
  
    if (interfactive) {
      if (count % 16 == 0) {
        az_sys_fs_read(rfd, &ch, 1);
        if (ch == '.') break; 
      }
    } 
    len -= 0x10;
    addr += 0x10;
  } while (len > 0);

  return tlen;
}
#if 0
#endif

/* === end of AZ_MEMDISP_C === */
