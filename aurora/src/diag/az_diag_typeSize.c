/**
 * @file   az_diag_typeSize.c
 * @brief  
 * @date   02/09/16
 * @author fware
 *
 * Copyright (c) Fware, 2013-2016 - All Rights Reserved
 * You may use, distribute and modify this code under the 
 * terms of the "Aurora Source Code License".
 * See the file LICENSE for full license details.

 * You should have received a copy of  the LICENSE with this file.

 * If not, please contact to Fware with the information in the file CONTACTS.
 *
 * History: @date 
 *   - 2016-09-02 fware initial create
 */

/* include header files */
//#include  "aurora.h"
#include <stdint.h>
#include <stdio.h>

#define AZ_DIAG_PRT_TYPE_SIZE(_t, bp, blen) \
    snprintf(bp, blen,"%19s: %d", #_t, (int)sizeof(_t)) 

#define AZ_DIAG_PRT_PTR(_t, _p, bp, blen) \
    snprintf(bp, blen,"%19s: %p", #_t, _p) 

#define AZ_DIAG_PRT_STR(buf) printf("%s\n", buf);

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

void az_diag_prt_type_size()
{
  char buf[64];
  AZ_DIAG_PRT_TYPE_SIZE(char, buf, sizeof(buf)); AZ_DIAG_PRT_STR(buf);
  AZ_DIAG_PRT_TYPE_SIZE(unsigned char, buf, sizeof(buf)); AZ_DIAG_PRT_STR(buf);
  AZ_DIAG_PRT_TYPE_SIZE(short, buf, sizeof(buf)); AZ_DIAG_PRT_STR(buf);
  AZ_DIAG_PRT_TYPE_SIZE(unsigned short, buf, sizeof(buf)); AZ_DIAG_PRT_STR(buf);
  AZ_DIAG_PRT_TYPE_SIZE(int, buf, sizeof(buf)); AZ_DIAG_PRT_STR(buf);
  AZ_DIAG_PRT_TYPE_SIZE(unsigned int, buf, sizeof(buf)); AZ_DIAG_PRT_STR(buf);
  AZ_DIAG_PRT_TYPE_SIZE(long, buf, sizeof(buf)); AZ_DIAG_PRT_STR(buf);
  AZ_DIAG_PRT_TYPE_SIZE(unsigned long, buf, sizeof(buf)); AZ_DIAG_PRT_STR(buf);
  AZ_DIAG_PRT_TYPE_SIZE(long long, buf, sizeof(buf)); AZ_DIAG_PRT_STR(buf);
  AZ_DIAG_PRT_TYPE_SIZE(unsigned long long, buf, sizeof(buf)); AZ_DIAG_PRT_STR(buf);
  AZ_DIAG_PRT_TYPE_SIZE(float, buf, sizeof(buf)); AZ_DIAG_PRT_STR(buf);
  AZ_DIAG_PRT_TYPE_SIZE(double, buf, sizeof(buf)); AZ_DIAG_PRT_STR(buf);
  AZ_DIAG_PRT_TYPE_SIZE(int8_t, buf, sizeof(buf)); AZ_DIAG_PRT_STR(buf);
  AZ_DIAG_PRT_TYPE_SIZE(uint8_t, buf, sizeof(buf)); AZ_DIAG_PRT_STR(buf);
  AZ_DIAG_PRT_TYPE_SIZE(int16_t, buf, sizeof(buf)); AZ_DIAG_PRT_STR(buf);
  AZ_DIAG_PRT_TYPE_SIZE(uint16_t, buf, sizeof(buf)); AZ_DIAG_PRT_STR(buf);
  AZ_DIAG_PRT_TYPE_SIZE(int32_t, buf, sizeof(buf)); AZ_DIAG_PRT_STR(buf);
  AZ_DIAG_PRT_TYPE_SIZE(uint32_t, buf, sizeof(buf)); AZ_DIAG_PRT_STR(buf);
  AZ_DIAG_PRT_TYPE_SIZE(int64_t, buf, sizeof(buf)); AZ_DIAG_PRT_STR(buf);
  AZ_DIAG_PRT_TYPE_SIZE(uint64_t, buf, sizeof(buf)); AZ_DIAG_PRT_STR(buf);
  AZ_DIAG_PRT_TYPE_SIZE(void, buf, sizeof(buf)); AZ_DIAG_PRT_STR(buf);
  AZ_DIAG_PRT_TYPE_SIZE(void *, buf, sizeof(buf)); AZ_DIAG_PRT_STR(buf);

  {
    char *c = "abc";
    AZ_DIAG_PRT_PTR(char *c, c, buf, sizeof(buf)); AZ_DIAG_PRT_STR(buf);
    AZ_DIAG_PRT_PTR(char *c + 1, c+1, buf, sizeof(buf)); AZ_DIAG_PRT_STR(buf);
  }
  {
    short *s = (short *)"abcd";
    AZ_DIAG_PRT_PTR(short *s, s, buf, sizeof(buf)); AZ_DIAG_PRT_STR(buf);
    AZ_DIAG_PRT_PTR(short *s + 1, s+1, buf, sizeof(buf)); AZ_DIAG_PRT_STR(buf);
  }
  {
    int *i = (int *)"abcdefgh";
    AZ_DIAG_PRT_PTR(int *i, i, buf, sizeof(buf)); AZ_DIAG_PRT_STR(buf);
    AZ_DIAG_PRT_PTR(int *i + 1, i+1, buf, sizeof(buf)); AZ_DIAG_PRT_STR(buf);
  }
  {
    long *l = (long *)"abcdefgh";
    AZ_DIAG_PRT_PTR(long *l, l, buf, sizeof(buf)); AZ_DIAG_PRT_STR(buf);
    AZ_DIAG_PRT_PTR(long *l + 1, l+1, buf, sizeof(buf)); AZ_DIAG_PRT_STR(buf);
  }
}
/*
int main(int argc, char *argv[])
{

  az_diag_prt_type_size();
}
*/
/* === end of AZ_DIAG_TYPESIZE_C === */
