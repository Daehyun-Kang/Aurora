/**
 * @file   az_addr2line.c
 * @brief  
 * @date   30/03/18
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
 *   - 2018-03-30 dhkang initial create
 */

/* include header files */
#include "az_def.h"

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

char  *az_addr2line(void *p, char *exefilename)
{
  static char info[256];
  sprintf(info, "addr2line %p -e %s > /tmp/azr", p, exefilename);
  system(info);
  FILE *f = fopen("/tmp/azr", "r");
  if (f) {
    int len = fread(info, 1, sizeof(info), f); 
    while ( len > 0 && (info[len-1] == '\n' || info[len-1] == '\r')) len--; 
    info[len] = 0;
    fclose(f);
  } else {
    info[0] = 0;
  }
  return info;
}

/* implement global functions */

/**
 * @fn        function name
 * @brief     function-description
 * @param     input-output-parameters
 * @return    return-value
 * @warning   warnings
 * @exception none
 */


/* === end of AZ_ADDR2LINE_C === */
