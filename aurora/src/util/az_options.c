/**
 * @file   az_options.c
 * @brief  
 * @date   24/03/18
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
 *   - 2018-03-24 dhkang initial create
 */

/* include header files */
#include "az_options.h"

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
int az_options_parse(int argc, char *argv[], az_kv_t *listOptKV, int nOptKV)
{
  int j, k;
  char *option;
  az_kv_t *kv;
  int found = 0;

  for (j = 0; j < argc; j++) {
    option = argv[j];
    if (*option++ != '-') continue;
    while (*option) {
      kv = listOptKV;
      for (k = 0; k < nOptKV; k++) {
        if (*option == *(kv->key)) {
          if (kv->value == NULL) {
            if (++j == argc) {
              found = AZ_ERR(INVALID);
              break;
            }
            kv->value = argv[j];
            found++;
            break;
          }
          if (kv->value == AZ_OPTIONS_VALUE_NONE) {
            kv->value = AZ_OPTIONS_VALUE_EXIST; 
            found++;
            break;
          }
        } 
      } // for (k = 0; k < nOptKV; k++) 
      option++;
    } // while (*option) 
  }

  return found; 
}


/* === end of AZ_OPTIONS_C === */
