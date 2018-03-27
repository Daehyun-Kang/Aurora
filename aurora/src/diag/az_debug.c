/**
 * @file   az_debug.c
 * @brief  
 * @date   15/03/18
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
 *   - 2018-03-15 dhkang initial create
 */

/* include header files */
#include "az_def.h"
#include "mon/az_debug.h"

typedef unsigned long long int az_debugreg_t;

#define AZ_EN_BREAKPOINT(x) (0x1 << (x*2))
#define AZ_EN_BREAK_OPRS(t, x) ((t) << (16 + (x*4)))
#define AZ_EN_BREAK_LEN(t, x) ((t) << (16 + (x*4)))

#define AZ_GET_DR6_BR(dr6, i) (dr6 & (0x1 << i))
#define AZ_GET_DR6_BT(dr6)    (dr6 & (0x1 << 15))
#define AZ_GET_DR6_BS(dr6)    (dr6 & (0x1 << 14))
#define AZ_GET_DR6_BD(dr6)    (dr6 & (0x1 << 13))

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
az_ref_t  breakpoints[4] = {0};
az_debugreg_t dr[4];
int           drlen[4];
az_debugreg_t dr7 = 0;

/**
 * @fn        function name
 * @brief     function-description
 * @param     input-output-parameters
 * @return    return-value
 * @warning   warnings
 * @exception none
 */
int az_debug_get_value()
{
  pid_t child;
  pid_t parent = getpid();
  int child_stat =0 ;
  int dr6; 

  if ((child = fork()) == 0) {
    int retval = EXIT_SUCCESS;

    if (ptrace(PTRACE_ATTACH, parent, NULL, NULL)) {
      az_sys_printf("1 err = %d\n", errno);
      exit(EXIT_FAILURE);
    }

    dr6 = ptrace(PTRACE_PEEKUSER, parent, offsetof(struct user, u_debugreg[6]), NULL);
    if (errno != 0) {
      az_sys_printf("2 err = %d\n", errno);
      exit(EXIT_FAILURE);
    } else {
      ptrace(PTRACE_POKEUSER, parent, offsetof(struct user, u_debugreg[6]), 0); 
      ptrace(PTRACE_POKEDATA, parent, &dr6, dr6); 
      az_sys_printf("dr6 %lx\n", dr6); 
    }

    if (ptrace(PTRACE_DETACH, parent, NULL, NULL)) {
      az_sys_printf("4 err = %d\n", errno);
      exit(EXIT_FAILURE);
    }

    exit(retval);
  }

  waitpid(child, &child_stat, 0);
  if (WEXITSTATUS(child_stat)) {
    return 1;
  }
  az_sys_printf("dr6 %lx\n", dr6); 

  return dr6;
}

struct sigaction trap_action_save;
void az_debug_trap(int sig, siginfo_t *info, void *context)
{
  void *addr;
  int len;
  //struct user_regs_struct regs;
  int dr6; 
  int bpno = -1;
  //__asm__ __volatile__ ("mov %%dr6, %0\n\t" :"=r"(dr6));
  //ptrace(PTRACE_GETREGS, get_pid(), NULL, &regs); 
  //dr6 = ptrace(PTRACE_PEEKUSER, getpid(), offsetof(struct user, u_debugreg[6]), NULL);
  dr6 = az_debug_get_value();
  if (AZ_GET_DR6_BR(dr6, 0)) {
    bpno = 0;
    addr = (void *)dr[bpno];
    len = drlen[bpno];
  }
  if (AZ_GET_DR6_BR(dr6, 1)) {
    bpno = 1;
    addr = (void *)dr[bpno];
    len = drlen[bpno];
  }
  if (AZ_GET_DR6_BR(dr6, 2)) {
    bpno = 2;
    addr = (void *)dr[bpno];
    len = drlen[bpno];
  }
  if (AZ_GET_DR6_BR(dr6, 3)) {
    bpno = 3;
    addr = (void *)dr[bpno];
    len = drlen[bpno];
  }
  printf("%lx %d\n", dr6, bpno);
  printf("%d \n", info->si_errno);
  printf("%d \n", info->si_code);
  printf("%d \n", info->si_value);
  switch (len) {
    case 1:
      printf("new value: bpno = %d *%p = %d\n", bpno, addr, *(uint8_t *)addr);
      break;
    case 2:
      printf("new value: bpno = %d *%p = %d\n", bpno, addr, *(uint16_t *)addr);
      break;
    case 4:
      printf("new value: bpno = %d *%p = %d\n", bpno, addr, *(uint32_t *)addr);
      break;
    case 8:
      printf("new value: bpno = %d *%p = %ld\n", bpno, addr, *(uint64_t *)addr);
      break;
  }
}
int az_debug_init_watchpoint()
{
  struct sigaction trap_action;

  sigaction(SIGTRAP, NULL, &trap_action);
  trap_action.sa_sigaction = az_debug_trap;
  trap_action.sa_flags = SA_SIGINFO|SA_RESTART|SA_NODEFER;
  sigaction(SIGTRAP, &trap_action, &trap_action_save);
}

int az_debug_set_watchpoint(int bpno, void *addr, int var_size) 
{
  pid_t child;
  pid_t parent = getpid();
  int child_stat =0 ;


  dr[bpno] = addr;
  drlen[bpno] = var_size;

  if ((child = fork()) == 0) {
    int retval = EXIT_SUCCESS;

    if (ptrace(PTRACE_ATTACH, parent, NULL, NULL)) {
      az_sys_printf("1 err = %d\n", errno);
      exit(EXIT_FAILURE);
    }
    dr7 = ptrace(PTRACE_PEEKUSER, parent, offsetof(struct user, u_debugreg[7]), 0); if (errno != 0) {
      az_sys_printf("5 err = %d\n", errno);
      exit(EXIT_FAILURE);
    }

    dr7 |= AZ_EN_BREAKPOINT(bpno);
    dr7 |= AZ_EN_BREAK_OPRS(DR_RW_WRITE, bpno); 
    switch (var_size) {
      case 1: var_size = DR_LEN_1; break;
      case 2: var_size = DR_LEN_2; break;
      case 4: var_size = DR_LEN_4; break;
      case 8: var_size = DR_LEN_8; break;
    }
    dr7 |= AZ_EN_BREAK_LEN(var_size, bpno); 


    if (ptrace(PTRACE_POKEUSER, parent, offsetof(struct user, u_debugreg[bpno]), addr)) {
      az_sys_printf("2 err = %d\n", errno);
      exit(EXIT_FAILURE);
    } else {
      az_sys_printf("set address %p for watch %d\n", addr, bpno); 
    }

    if (ptrace(PTRACE_POKEUSER, parent, offsetof(struct user, u_debugreg[7]), dr7)) {
      az_sys_printf("3 err = %d\n", errno);
      exit(EXIT_FAILURE);
    }

    if (ptrace(PTRACE_DETACH, parent, NULL, NULL)) {
      az_sys_printf("4 err = %d\n", errno);
      exit(EXIT_FAILURE);
    }

    exit(retval);
  }

  waitpid(child, &child_stat, 0);
  if (WEXITSTATUS(child_stat)) {
    return 1;
  }

  return 0;
}

int az_debug_reset_watchpoint(int bpno)
{
  pid_t child;
  pid_t parent = getpid();
  int child_stat =0 ;
  void *addr = NULL;


  if ((child = fork()) == 0) {
    int retval = EXIT_SUCCESS;

    dr7 &= ~AZ_EN_BREAKPOINT(bpno);

    if (ptrace(PTRACE_ATTACH, parent, NULL, NULL)) {
      exit(EXIT_FAILURE);
    }

    if (ptrace(PTRACE_POKEUSER, parent, offsetof(struct user, u_debugreg[0]), &addr)) {
      exit(EXIT_FAILURE);
    }

    if (ptrace(PTRACE_POKEUSER, parent, offsetof(struct user, u_debugreg[7]), &dr7)) {
      exit(EXIT_FAILURE);
    }

    if (ptrace(PTRACE_DETACH, parent, NULL, NULL)) {
      exit(EXIT_FAILURE);
    }

    exit(retval);
  }

  waitpid(child, &child_stat, 0);
  if (WEXITSTATUS(child_stat)) {
    return 1;
  }

  return 0;
}


/* === end of AZ_DEBUG_C === */
