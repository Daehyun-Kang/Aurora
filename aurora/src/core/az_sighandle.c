/**
 * @file   az_sighandle.c
 * @brief  
 * @date   11/04/18
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
 *   - 2018-04-11 dhkang initial create
 */

/* include header files */
#include "az_def.h"
#include "az_err.h"
#include "az_sighandle.h"

/* declare global variables */

/* declare static variables */

static az_sighandler_descr_t sig_handler_descrs[AZ_MAX_SIG_HANDLERS];
static az_sighandler_descr_t *cleanup_handler_descr_head = NULL;
static int sig_handler_descr_count = 0;

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
int az_init_cleanup_sighandler()
{
  az_sig_initDescr(SIGINT);
  az_sig_initDescr(SIGABRT);
  az_sig_initDescr(SIGTERM);

  az_sig_setSighandlerList(SIGINT, &cleanup_handler_descr_head);
  az_sig_setSighandlerList(SIGABRT, &cleanup_handler_descr_head);
  az_sig_setSighandlerList(SIGTERM, &cleanup_handler_descr_head);
}

int az_register_cleanup_sighandler(az_sighandler_t handler)
{
  int r = AZ_SUCCESS;

  do {
    if (sig_handler_descr_count >= AZ_MAX_SIG_HANDLERS) {
      r = AZ_ERR(FULL);
      break;
    }
    az_sighandler_descr_t *pDescr = &sig_handler_descrs[sig_handler_descr_count];
    pDescr->handler = handler;
    pDescr->next = NULL;

    az_sighandler_descr_t *head = cleanup_handler_descr_head;
    if (head == NULL) cleanup_handler_descr_head = pDescr;
    else {
      while (head) {
        if (head->next == NULL) {
          head->next = pDescr;
          break;
        }
        head = head->next;
      }
    }
  } while (0);

  return r;
}


/* === end of AZ_SIGHANDLE_C === */
