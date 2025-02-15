/**
 * @file   az_core_preload.c
 * @brief  
 * @date   18/03/18
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
 *   - 2018-03-18 dhkang initial create
 */

/* include header files */
#include "aurora.h"


/* declare global variables */
int az_attr_no_instrument az_core_preload() az_attr_constructor;
void az_attr_no_instrument az_core_postload(int) az_attr_destructor;

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
#ifdef  CONFIG_AZ_PRELOAD_SO
static inline int az_syscall_find_symbol(char *symbol_name, void **func_addr)
{
  int r = 1;
  *func_addr = dlsym(RTLD_NEXT, symbol_name);
  if (*func_addr == NULL) {
    az_sys_eprintf("%s not found\n", symbol_name);
    r = 0;
  }
  return r;
}
#endif

az_file_entity_t az_stdout = {.ion.id = STDOUT_FILENO, .sys_file=STDOUT_FILENO };
az_file_entity_t az_stdin = {.ion.id = STDIN_FILENO, .sys_file=STDIN_FILENO };
az_file_entity_t az_stderr = {.ion.id = STDERR_FILENO, .sys_file = STDERR_FILENO};

int az_core_preload_done = 0;

int az_core_preload() 
{
  int r = 0;

  #ifdef  CONFIG_AZ_PRELOAD_SO
  r += az_syscall_find_symbol("malloc", (void **)&az_real_malloc); 
  r += az_syscall_find_symbol("free", (void **)&az_real_free); 
  #endif

  az_init_cleanup_sighandler();
  az_register_cleanup_sighandler(az_core_postload);

  az_ion_init(NULL);

  az_ion_register(&az_stdout, AZ_ION_TYPE_FILE);
  az_ion_register(&az_stdin, AZ_ION_TYPE_FILE);
  az_ion_register(&az_stderr, AZ_ION_TYPE_FILE);
  
  #ifdef  CONFIG_AZ_PRELOAD_SO
  r += az_syscall_find_symbol("open", (void **)&az_real_open); 
  r += az_syscall_find_symbol("close", (void **)&az_real_close); 
  r += az_syscall_find_symbol("creat", (void **)&az_real_creat); 
  r += az_syscall_find_symbol("read", (void **)&az_real_read); 
  r += az_syscall_find_symbol("write", (void **)&az_real_write); 

  az_sys_eprintf("%d symbols overriden\n", r); 
  #endif

  #ifdef  CONFIG_AZ_TRACE
  extern void  az_trace_begin(void);
  az_trace_begin();
  #endif

  #ifdef  CONFIG_AZ_PROBE
  az_probe_init();
  #endif

  az_core_init();

  az_core_preload_done = 1;

  return r;
}

void az_core_postload(int sig) 
{
  #ifdef  CONFIG_AZ_TRACE
  extern void  az_trace_end(void);

  az_trace_end();
  #endif

  #ifdef  CONFIG_AZ_PROBE
  az_probe_deinit();
  #endif

  az_sys_eprintf("%s", "end...");
  fflush(stdout);
}

int az_core_check_preload()
{
  if (! az_core_preload_done) {
    return az_core_preload() ;
  } else {
    return 1;
  }
}

/* === end of AZ_CORE_PRELOAD_C === */
