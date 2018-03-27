/**
 * @file   az_sys_fs.c
 * @brief  
 * @date   20/05/16
 * @author fware
 *
 * @copyright
 * Copyright (c) Fware, 2013-2016 - All Rights Reserved
 * You may use, distribute and modify this code under the 
 * terms of the "Aurora Source Code License".
 * See the file LICENSE for full license details.\n\n
 *
 * You should have received a copy of  the LICENSE with this file.\n\n
 *
 * If not, please contact to Fware with the information in the file CONTACTS.
 *
 * History:
 *   - 2016-05-20 fware initial create
 */

/* include header files */
#include "linux/az_linux_fs.h"
#include "az_malloc.h"

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
az_r_t  az_sys_fs_listDir(char *path, az_sys_dirent_t *list, az_size_t listCount, void **ctx) 
{
  az_r_t result = AZ_SUCCESS;
  az_sys_dirlist_ctx_t *pCtx = (az_sys_dirlist_ctx_t *)ctx;
  DIR *dirp;
  int j;
  az_assert(NULL != path);
  az_assert(NULL != list);
  az_assert(NULL != ctx);
  do {
    az_if_arg_null_break(path, result);
    if (NULL == pCtx) {
      pCtx = az_malloc(sizeof(az_sys_dirlist_ctx_t));
      if (NULL == pCtx) {
        result = AZ_ERR(MALLOC);
        break;
      }
      dirp = pCtx->dirp = opendir(path);
    } else {
      dirp = pCtx->dirp; 
    }
    if (NULL == dirp) {
      result = AZ_ERR(OPEN_DIR);
      break;
    }
    if (listCount <= 0) {
        closedir(pCtx->dirp);
        az_free(pCtx);
        break;
    }
    for (j = 0; j < listCount; j++) {
      result = readdir_r(dirp, list, &(pCtx->cur)); 
      if (result < 0) {
        break;
      }
      if (NULL == pCtx->cur) {
        closedir(pCtx->dirp);
        az_free(pCtx);
        break;
      }
      list++;
    }
    result = (az_r_t)j;
  } while (0);

  return result;
}
/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
az_r_t  az_sys_fs_findDir(char *path, char *entryname, az_sys_dirent_t *entry)
{
  az_r_t result = AZ_ERR(NOT_FOUND);
  DIR *dirp;
  struct dirent *cur; 
  //int j;
  az_assert(NULL != path);
  az_assert(NULL != entryname);
  az_assert(NULL != entry);
  do {
    az_if_arg_null_break(path, result);

    dirp = opendir(path);
    if (NULL == dirp) {
      result = AZ_ERR(OPEN_DIR);
      break;
    }
    do { 
      result = readdir_r(dirp, entry, &cur);
      if (result < 0 || NULL == cur) {
        break;
      }
      if (!strcmp(entryname, entry->d_name)) {
        result = AZ_SUCCESS;
        break;
      }
    } while (1);

    closedir(dirp);
  } while (0);

  return result;
}
/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
az_size_t  az_sys_fs_dirSize(char *path)
{
  az_r_t result = AZ_ERR(NOT_FOUND);
  az_size_t size = 0;
  struct stat stats;
  DIR *dirp;
  struct dirent *cur = NULL; 
  struct dirent entry;
  //int j;
  az_assert(NULL != path);
  char *name = az_malloc(strlen(path) + CONFIG_AZ_NAME_MAX + 2);
  do {
    az_if_arg_null_break(path, result);

    result = lstat(path, &stats);
    if (AZ_SUCCESS != result) {
      break;
    }
    if (S_ISREG(stats.st_mode)) {
      size = stats.st_size;
      break;
    }

    dirp = opendir(path);
    if (NULL == dirp) {
      result = AZ_ERR(OPEN_DIR);
      break;
    }
    do { 
      result = readdir_r(dirp, &entry, &cur);
      if (result < 0 || NULL == cur) {
        break;
      }
      az_sys_fs_combinePath(path, cur->d_name, name);
      if (cur->d_type == DT_DIR) {
        if (strcmp(cur->d_name, ".") && strcmp(cur->d_name, "..")) {
          size += az_sys_fs_dirSize(name);
        }
      } else {
        result = lstat(name, &stats);
        if (AZ_SUCCESS == result) {
          size += stats.st_size;
        }
      }
    } while (1);
    closedir(dirp);

    az_free(name);
  } while (0);

  return size;
}
#if 0
#endif

/* === end of AZ_SYS_FS_C === */
