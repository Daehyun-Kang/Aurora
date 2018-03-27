/**
 * @file   az_linux_dllib.h
 * @brief  
 * @date   05/02/16
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
 *   - 2016-02-05 fware initial create
 */

#ifndef AZ_LINUX_DLLIB_H
#define AZ_LINUX_DLLIB_H
#include "az_def.h"
#include "az_err.h"
#include "az_malloc.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* constants */

/* basic macros */

/* basic types */
#ifndef AZ_LINUX_DEF_H
typedef void *  az_sys_lib_t;
#endif

/* structures */

/* structured types */

/* macros */

/* variabls exposed */

/* inline functions */
static inline az_r_t az_sys_dllib_load(az_str_t path, az_sys_lib_t *plibId) 
{
  az_r_t r = AZ_SUCCESS;
  az_sys_lib_t h = NULL;
  h = dlopen(path, RTLD_LAZY);
  if (!h) { 
    r = AZ_FAIL; 
  }
  if (plibId) { 
    *plibId = h; 
  }
  return r;
}

static inline az_r_t az_sys_dllib_unload(az_sys_lib_t libId) 
{
  az_r_t r = AZ_SUCCESS;
  r = (az_r_t)dlclose(libId);

  return r;
}

static inline az_r_t az_sys_dllib_getSymbol(az_sys_lib_t libId, char *name, az_ref_t *pAddr) 
{
  az_r_t r = AZ_SUCCESS;
  az_ref_t addr;
  
  addr = dlsym(libId, name);
  if (!addr) {
    r = AZ_FAIL;
  }
  if (pAddr) {
    *pAddr = addr;
  }

  return r;
}

static az_ref_t __attribute__((no_instrument_function)) az_sys_dllib_findBfdSymbol(char *filename, char *symname) 
{
  extern asymbol **az_symtab;
  extern az_size_t az_nsyms;

  bfd *ibfd;
  asymbol **symtab;
  az_size_t nsize, nsyms, i;
  symbol_info syminfo;
  char **matching;
  az_ref_t addr = NULL;

  //az_sys_eprintf("%s %p %ld\n", filename, az_symtab, az_nsyms);
  do {
    if (az_symtab) {
      symtab = az_symtab;
      nsyms = az_nsyms;
    } else {
      bfd_init();
      ibfd = bfd_openr(filename, NULL);
      if (ibfd == NULL) {
        az_sys_printf("%s", "bfd_openr error\n");
        break;
      }
      if (!bfd_check_format_matches(ibfd, bfd_object, &matching)) {
      }
      nsize = bfd_get_symtab_upper_bound(ibfd);
      if (nsize <= 0) {
        az_sys_eprintf("%p %ld\n", symtab, nsyms);
        break;
      }
      az_symtab = symtab = az_malloc(nsize);
      az_nsyms = nsyms = bfd_canonicalize_symtab(ibfd, symtab);
      //az_sys_eprintf("3 %p %ld\n", symtab, nsyms);
      //bfd_close(ibfd);
    }
    if (NULL == az_symtab) break;
    //az_sys_eprintf("%p %ld\n", symtab, nsyms);
    for (i = 0; i < nsyms; i++) {
      if(!strcmp(symtab[i]->name, symname)) {
        bfd_symbol_info(symtab[i], &syminfo);
        addr = (az_ref_t)syminfo.value;
        break;
      }
    }
    //az_free(symtab);
  } while (0);

  return addr;
}

static char __attribute__((no_instrument_function)) *az_sys_dllib_findBfdAddr2Name(az_ref_t addr) 
{
  extern asymbol **az_symtab;
  extern az_size_t az_nsyms;

  asymbol **symtab;
  az_size_t nsyms, i;
  symbol_info syminfo;
  char *name = "????";

  do {
    if (NULL == az_symtab) {
      az_sys_dllib_findBfdSymbol(az_getExeFileName(), "main");
    } 
    symtab = az_symtab;
    if (NULL == symtab) {
      break;
    }
    nsyms = az_nsyms;
    for (i = 0; i < nsyms; i++) {
      bfd_symbol_info(symtab[i], &syminfo);
      if ((az_ref_t)syminfo.value == addr) {
        name = symtab[i]->name;
        break;
      }
    }
  } while (0);

  return name;
}

static inline az_r_t az_sys_dllib_getStaticSymbol(char *name, az_ref_t *pAddr) 
{
  az_r_t r = AZ_FAIL;
  char *filename = az_getExeFileName();
  if (pAddr) {
    *pAddr = az_sys_dllib_findBfdSymbol(filename, name);
    if (NULL != *pAddr) {
      r = AZ_SUCCESS;
    }
  }
  //az_sys_rprintf(r, "filename=%s name=%s\n", filename, name);
  return r;
}

/* function prototypes exposed */

#ifdef __cplusplus
}
#endif
#endif
