/**
 * @file   az_sys_init.c
 * @brief  
 * @date   08/02/16
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
 *   - 2016-02-08 Daehyun Kang initial create
 */

/* include header files */
#include "az_def.h"
#include "az_err.h"

#pragma comment(lib, "Ws2_32.lib")

/* declare global variables */
az_size_t az_sys_malloc_count;
az_size_t az_sys_free_count;

/* declare static variables */
static az_state_t az_sys_state = AZ_ST_IDLE;

//DWORD   az_sys_priorityClass = IDLE_PRIORITY_CLASS; // 0x0040
//DWORD   az_sys_priorityClass = PROCESS_MODE_BACKGROUND_BEGIN; 0x00100000
//DWORD   az_sys_priorityClass = PROCESS_MODE_BACKGROUND_END; 0x00200000
//DWORD   az_sys_priorityClass = BELOW_NORMAL_PRIORITY_CLASS; //0x4000
//DWORD   az_sys_priorityClass = NORMAL_PRIORITY_CLASS; // 0x0020
//DWORD   az_sys_priorityClass = ABOVE_NORMAL_PRIORITY_CLASS; // 0x8000
//DWORD   az_sys_priorityClass = HIGH_PRIORITY_CLASS; // 0x0080
DWORD   az_sys_priorityClass = REALTIME_PRIORITY_CLASS; // 0x0100

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

az_r_t az_sys_init()
{
  az_r_t r = AZ_SUCCESS;
  extern az_r_t az_sys_timer_init();
  HANDLE curProcess;
  WSADATA wsaData;

  az_sys_eprintf0("...");
  switch (az_sys_state) {
    case AZ_ST_IDLE:
      az_sys_state = AZ_ST_INIT;
      az_sys_malloc_count = 0;
      az_size_t az_sys_free_count = 0;
      // TODO: sys dependant init....
      //
      //az_sys_timer_init();
      curProcess = GetCurrentProcess();
      az_sys_eprintf("priorityClass = %x\n", GetPriorityClass(curProcess)); 
      SetPriorityClass(curProcess, az_sys_priorityClass);
      r = WSAStartup(MAKEWORD(2,2), &wsaData);
      if (r != NO_ERROR) {
        az_sys_eprintf("WSAStartup error=%d\n", r);
        break;
      }
      az_sys_state = AZ_ST_NORM;
      break;
    case AZ_ST_INIT:
      r = AZ_ERR(AGAIN);
      break;
    default:
      r = AZ_ERR(STATE);
      break;
  }

  return r;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
az_r_t az_sys_deinit()
{
  az_r_t r = AZ_SUCCESS;
  extern az_r_t az_sys_timer_deinit();

  az_sys_eprintf0("...");
  switch (az_sys_state) {
    case AZ_ST_IDLE:
      r = AZ_ERR(STATE);
      break;
    case AZ_ST_TINI:
      r = AZ_ERR(AGAIN);
      break;
    default:
      az_sys_state = AZ_ST_TINI;
      // TODO: sys dependant deinit....
      //
      //az_sys_timer_deinit();
      WSACleanup();
      az_sys_state = AZ_ST_IDLE;
      break;
  }

  return r;
}



/* === end of AUR_SYS_INIT_C === */
