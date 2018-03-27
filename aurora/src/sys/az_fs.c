/**
 * @file   az_fs.c
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
#include "az_fs.h"

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
az_r_t  az_fs_createDir(const az_str_t path, int mode)
{
  az_r_t result = AZ_SUCCESS;

  do {
    az_if_arg_null_break(path, result);

    result = az_sys_fs_createfolder(path, mode);
    if (result == AZ_ERR(NO_ENTRY)) {
      char npath[AZ_PATH_MAX];
      az_size_t path_len = sizeof(npath);
      //az_size_t basepath_len;
      strncpy(npath, path, path_len); 
      
      //basepath_len = az_fs_getBasePathComponent(npath, npath); 
      (void)az_fs_getBasePathComponent(npath, npath); 
      if (AZ_SUCCESS == az_fs_createDir(npath, mode)) {
        result = az_sys_fs_createfolder(path, mode);
      }
    }
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
az_r_t  az_fs_deleteDir(const az_str_t path)
{
  az_r_t result = AZ_SUCCESS;
  do {
    az_if_arg_null_break(path, result);

    result = az_sys_fs_deletefolder(path);
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
az_r_t  az_fs_getcwd(char *path, az_size_t path_size)
{
  az_r_t result = AZ_SUCCESS;
  az_assert(NULL != path && path_size > 0);
  do {
    az_if_arg_null_break(path, result);

    result = az_sys_fs_getcwd(path, path_size);
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
az_r_t  az_fs_chdir(char *path)
{
  az_r_t result = AZ_SUCCESS;
  az_assert(NULL != path);
  do {
    az_if_arg_null_break(path, result);

    result = az_sys_fs_chdir(path);
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
az_r_t  az_fs_listDir(char *path, az_dirent_t *list, az_size_t listCount, void **ctx) 
{
  az_r_t result = AZ_SUCCESS;
  //az_sys_dirlist_ctx_t *pCtx = (az_sys_dirlist_ctx_t *)ctx;
  //int j;
  az_assert(NULL != path);
  az_assert(NULL != list);
  do {
    az_if_arg_null_break(path, result);
    result = az_sys_fs_listDir(path, list, listCount, ctx); 
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
az_r_t  az_fs_findDir(char *path, char *entryname, az_dirent_t *entry)
{
  az_r_t result = AZ_ERR(NOT_FOUND);
  //int j;
  az_assert(NULL != path);
  az_assert(NULL != entryname);
  az_assert(NULL != entry);
  do {
    az_if_arg_null_break(path, result);

    result = az_sys_fs_findDir(path, entryname, entry);
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
az_size_t  az_fs_dirSize(char *path)
{
  az_size_t result = 0; 
  az_assert(NULL != path);
  do {
    az_if_arg_null_break(path, result);

    result = az_sys_fs_dirSize(path); 
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
az_r_t  az_fs_createFile(const az_str_t path, int mode, az_file_t *pfile)
{
  az_r_t r = AZ_ERR(ARG_NULL);
  az_file_t file;
  az_assert(NULL != path);
  az_assert(NULL != pfile);
  do {
    file = *pfile;
    if (NULL == file) {
      file = az_fs_allocateFile();
      if (NULL == file) {
        r = AZ_ERR(ALLOC);
        break;
      }
      az_ion_invalidate(&file->ion, 0);
    } 
    az_assert(file->ion.id == AZ_SYS_IO_INVALID);
    az_sys_file_t sys_file;
    r = az_sys_fs_create(path, mode, file);
    if (AZ_SUCCESS != r) {
      if (AZ_REFCOUNT_IS_ZERO(&file->ion.refCount)) {
        az_fs_freeFile(file);
      }
      break;
    } else {
      file->ion.id = sys_file;
    }
    r = az_ion_register(&(file->ion), AZ_ION_TYPE_FILE);
    if (AZ_SUCCESS != r) {
      az_sys_fs_close(sys_file);
      az_sys_fs_delete(path);
      if (AZ_REFCOUNT_IS_ZERO(&file->ion.refCount)) { 
        az_fs_freeFile(file);
      }
      r = AZ_ERR(REG_ION);
      break;
    } else {
      az_refcount_atomic_inc(&file->ion.refCount);
    }
    *pfile = file;
  } while (0);

  return r;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
az_r_t  az_fs_deleteFile(const az_str_t path, az_file_t file)
{
  az_r_t r = AZ_ERR(ARG_NULL);
  do {
    if (NULL != path && NULL == file) {
      r = az_sys_fs_delete(path);
      break;
    } 
    if (NULL == file) {
      break;
    }
    if (AZ_REFCOUNT_VALUE(&file->ion.refCount) == 0) {
      r = AZ_ERR(AGAIN);
      break;
    }
    if (az_refcount_atomic_dec(&file->ion.refCount) > 0) {
      r = AZ_ERR(BUSY);
      break;
    }
    char npath[AZ_PATH_MAX];
    if (az_sys_fs_getpath(file->ion.id, npath, sizeof(npath)) > 0) {
      az_sys_fs_close(file->ion.id);
      r = az_sys_fs_delete(npath);
    }
    if (AZ_REFCOUNT_IS_ZERO(&file->ion.refCount)) {
      az_fs_freeFile(file);
    } else {
      file->ion.id = AZ_SYS_FILE_INVALID;
    }
  } while (0);

  return r;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
az_r_t  az_fs_openFile(const az_str_t path, int flag, int mode, az_file_t *pfile)
{
  az_r_t r = AZ_ERR(ARG_NULL);
  az_file_t file;
  az_assert(NULL != path);
  az_assert(NULL != pfile);
  do {
    file = *pfile;
    if (NULL == file) {
      file = az_fs_allocateFile();
      if (NULL == file) {
        r = AZ_ERR(ALLOC);
        break;
      }
      az_refcount_init_dynamic(&file->ion.refCount);
    }

    az_sys_file_t sys_file;
    r = az_sys_fs_open(path, flag, mode, &sys_file);
    if (AZ_SUCCESS != r) {
      if (AZ_REFCOUNT_IS_ZERO(&file->ion.refCount)) {
        az_fs_freeFile(file);
      }
      break;
    } else {
      file->ion.id = sys_file;
    }
    r = az_ion_register(&(file->ion), AZ_ION_TYPE_FILE);
    if (AZ_SUCCESS != r) {
      az_sys_fs_close(sys_file);
      if (AZ_REFCOUNT_IS_ZERO(&file->ion.refCount)) {
        az_fs_freeFile(file);
      }
      r = AZ_ERR(REG_ION);
      break;
    } else {
      az_refcount_atomic_inc(&file->ion.refCount);
    }
  } while (0);

  *pfile = file;
  return r;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
az_r_t  az_fs_closeFile(az_file_t file)
{
  az_r_t r = AZ_ERR(ARG_NULL);
  do {
    if (NULL != file) break;
    if (AZ_SYS_FILE_INVALID == file->ion.id) {
      r = AZ_ERR(INVALID);
      break;
    }
    if (az_refcount_atomic_dec(&file->ion.refCount) <= 0) {
      r = AZ_ERR(AGAIN);
      break;
    }
    if (AZ_REFCOUNT_VALUE(&file->ion.refCount) != 0) {
      r = AZ_ERR(BUSY);
      break;
    }
    r = az_sys_fs_close(file->ion.id);
    if (AZ_REFCOUNT_IS_ZERO(&file->ion.refCount)) {
      az_fs_freeFile(file);
    } else {
      file->ion.id = AZ_SYS_FILE_INVALID;
    }
  } while (0);


  return r;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
az_bool_t  az_fs_exist(const az_str_t path)
{
  return az_sys_fs_exist(path);
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
az_size_t  az_fs_fileSize(const az_str_t path, az_file_t file)
{
  az_size_t size;
  if (NULL != path) {
    size =  az_sys_fs_size(path);
  }
  if (NULL != file) {
    size = az_sys_fs_filesize(file->ion.id);
  }

  return size;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
az_size_t  az_fs_readFile(az_file_t file, az_uint8_t *bp, az_size_t size)
{
  az_size_t result = (az_size_t)AZ_ERR(ARG_NULL);
  az_assert(NULL != file);
  do {
    if (AZ_REFCOUNT_VALUE(&file->ion.refCount) == 0) {
      result = (az_size_t)AZ_ERR(INVALID);
      break;
    }
    result = az_sys_fs_read(file->ion.id, bp, size);
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
az_size_t  az_fs_writeFile(az_file_t file, az_uint8_t *bp, az_size_t size)
{
  az_size_t result = (az_size_t)AZ_ERR(ARG_NULL);
  az_assert(NULL != file);
  do {
    if (AZ_REFCOUNT_VALUE(&file->ion.refCount) == 0) {
      result = (az_size_t)AZ_ERR(INVALID);
      break;
    }
    result = az_sys_fs_write(file->ion.id, bp, size);
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
az_size_t  az_fs_lseekFile(az_file_t file, az_pos_t offset, int whence)
{
  az_size_t result = (az_size_t)AZ_ERR(ARG_NULL);
  az_assert(NULL != file);
  do {
    if (AZ_REFCOUNT_VALUE(&file->ion.refCount) == 0) {
      result = (az_size_t)AZ_ERR(INVALID);
      break;
    }
    result = az_sys_fs_lseek(file->ion.id, offset, whence);
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
az_r_t  az_fs_truncate(const az_str_t path, az_file_t file, az_size_t size)
{
  do {
    if (NULL != path && NULL == file) {
      size =  az_sys_fs_truncate(path, size);
      break;
    } 
    if (NULL == file) {
      size = AZ_ERR(ARG_NULL);
      break;
    }
    size = az_sys_fs_ftruncate(file->ion.id, size);
  } while (0);

  return size;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
az_size_t az_fs_file2mem(const az_str_t path, az_ref_t *pMem)
{
  az_sys_file_t   file;
  az_size_t size;
  char *mem;
  az_assert(NULL != path && NULL != pMem);
  do {
    size = (az_size_t)az_sys_fs_open(path, O_RDONLY, 0666, &file);
    if (size < 0) break;
    size = az_sys_fs_filesize(file);
    if (size <= 0) {
      az_sys_fs_close(file);
      break;
    }
    mem = az_malloc(size);
    if (NULL == mem) {
      size = (az_size_t)(AZ_ERR(MALLOC));
      az_sys_fs_close(file);
      break;
    }
    size = az_sys_fs_read(file, mem, size);
    if (size < 0) {
      size = (az_size_t)(AZ_ERR(SYS_READ));
      az_sys_fs_close(file);
      break;
    }
    *pMem = mem;
    az_sys_fs_close(file);
  } while (0);

  return size;
}

/* === end of AZ_FS_C === */
