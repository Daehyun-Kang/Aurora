/**
 * @file   az_fs.h
 * @brief  
 * @date   20/05/16
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
 *   - 2016-05-20 fware initial create
 */

#ifndef AZ_FS_H
#define AZ_FS_H

#include "az_def.h"
#include "az_err.h"
#include "az_macros.h"
#include "az_malloc.h"
#include "az_ring.h"
#include "az_ion.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* constants */

/* basic macros */
#ifdef CONFIG_AZ_PATH_MAX
#define AZ_PATH_MAX       CONFIG_AZ_PATH_MAX
#else
#define AZ_PATH_MAX       512
#endif

#ifdef CONFIG_AZ_PATH_COMP_MAX
#define AZ_PATH_COMP_MAX       CONFIG_AZ_PATH_COMP_MAX
#else
#define AZ_PATH_COMP_MAX       64
#endif

#ifdef CONFIG_AZ_NAME_MAX
#define AZ_NAME_MAX           CONFIG_AZ_NAME_MAX
#else
#define AZ_NAME_MAX           64
#endif

#define AZ_PATH_DELIMITER     AZ_SYS_PATH_DELIMITER 
#define AZ_PATH_SEPERATOR     AZ_SYS_PATH_SEPERATOR

/* basic types */
typedef struct {
  az_ion_t      ion;
  az_sys_dir_t  sys_dir;
} az_dir_entity_t; 
typedef az_dir_entity_t * az_dir_t;
#define AZ_DIR_INVALID    NULL

typedef struct {
  az_ion_t        ion;
  az_sys_file_t   sys_file;
} az_file_entity_t; 
typedef az_file_entity_t * az_file_t;
#define AZ_FILE_INVALID   NULL 

typedef az_sys_dirent_t   az_dirent_t;

/* structures */

/* structured types */

/* macros */

/* variabls exposed */

/* inline functions */
static inline az_file_t az_fs_allocateFile()
{
  az_file_t file = az_malloc(sizeof(az_file_entity_t));
  return file;
}
static inline void az_fs_freeFile(az_file_t file)
{
  if (NULL != file) {
    az_free(file);
  }
}

static inline az_r_t  az_fs_parsePath(const char *path, char *parsed_path, char *comp[])
{
  az_r_t result = AZ_ERR(ARG_NULL);
  char *cur, *saved;
  int j = 0;

  az_assert(NULL != path && NULL != parsed_path && NULL != comp);
  if (path != parsed_path) {
    strncpy(parsed_path, path, AZ_PATH_MAX);
  }
  comp[j++] = parsed_path;
  cur = strtok_r(parsed_path, AZ_PATH_DELIMITER, &saved);
  while (cur) {
    comp[j++] = cur;
    cur = strtok_r(NULL, AZ_PATH_DELIMITER, &saved);
  }
  comp[j] = NULL;
  result = (az_r_t)j;
  
  return result;
}
static inline az_r_t  az_fs_getBasePathComponent(char *path, char *out)
{
  az_r_t result = AZ_ERR(ARG_NULL);
  char *cur;
  az_assert(NULL != path);
  cur = strrchr(path, AZ_PATH_SEPERATOR);
  if (NULL == cur) {
    cur = path + strlen(path);
  }
  if (out == path) {
    *cur = 0;
  } else if (NULL != out) {
    char *in = path;
    while (in != cur) {
      *out++ = *in++;
    }
    *out = 0;
  }
  result = (az_r_t)(cur - path);

  return result;
}
static inline az_r_t  az_fs_getLastPathComponent(char *path, char *comp)
{
  az_r_t result = AZ_ERR(ARG_NULL);
  char *cur;
  az_assert(NULL != path);
  cur = strrchr(path, AZ_PATH_SEPERATOR);
  if (NULL == cur) {
    cur = path;
  } else {
    cur++;
  }
  if (NULL != comp) {
    strcpy(comp, cur);
  }
  result = (az_r_t)(cur - path);

  return result;
}

static inline az_r_t  az_fs_getFileExt(char *filename, char *ext)
{
  az_r_t result = AZ_ERR(ARG_NULL);
  char *cur;
  az_assert(NULL != filename);
  cur = strrchr(filename, '.');
  if (NULL == cur) {
    cur = filename;
  } else {
    cur++;
  }
  if (NULL != ext) {
    strcpy(ext, cur);
  }
  result = (az_r_t)(cur - filename);

  return result;
}

static inline az_r_t  az_fs_getFileBase(char *filename, char *base)
{
  az_r_t result = AZ_ERR(ARG_NULL);
  char *cur;
  az_assert(NULL != filename);
  cur = strchr(filename, '.');
  if (NULL == cur) {
    cur = filename + strlen(filename); 
  } 
  result = (az_r_t)(cur - filename);
  if (NULL != base) {
    memcpy((void *)base, (void *)filename, (size_t)result);
    base[(size_t)result] = '\0';
  }

  return result;
}

static inline az_r_t az_fs_combinePath(char *path1, char *path2, char *dest)
{
  //az_r_t result = AZ_ERR(ARG_NULL);
  az_size_t pos;
  char *src = path2;
  az_assert(NULL != path1 && NULL != path2 && NULL != dest);

  pos = strlen(path1);
  if (path1[pos-1] == AZ_PATH_SEPERATOR) {
    if (path2[0] == AZ_PATH_SEPERATOR) {
      pos--;
    }
    if (dest != path1) {
      memcpy(dest, path1, pos);
    }
    dest += pos;
  } else {
    if (dest != path1) {
      memcpy(dest, path1, pos);
    }
    dest += pos;
    if (path2[0] != AZ_PATH_SEPERATOR) {
      *dest++ = AZ_PATH_SEPERATOR;
      pos++;
    }
  }
  strcpy(dest, src); 

  return pos + strlen(src);
}

static inline az_r_t az_fs_makePath(char *comp[], char *dest)
{
  az_r_t result = AZ_ERR(ARG_NULL);
  int j = 0;
  az_assert(NULL != comp && NULL != dest);
  az_assert(NULL != comp[0]);

  strcpy(dest, comp[j++]);
  while (comp[j]) {
    result = az_fs_combinePath(dest, comp[j++], dest);
  }

  return result;
}

/* function prototypes exposed */
extern az_r_t   az_fs_createDir(const az_str_t path, int mode);
extern az_r_t   az_fs_deleteDir(const az_str_t path);
extern az_r_t   az_fs_getcwd(char *path, az_size_t path_size);
extern az_r_t   az_fs_chdir(char *path);
extern az_r_t   az_fs_listDir(char *path, az_dirent_t *list, az_size_t listCount, void **ctx);
extern az_r_t   az_fs_findDir(char *path, char *entryname, az_dirent_t *); 
extern az_size_t   az_fs_dirSize(char *path);

extern az_ion_id_t  az_fs_createFile(const az_str_t path, int mode, az_file_t *pfile);
extern az_r_t  az_fs_deleteFile(const az_str_t path, az_ion_id_t id);
extern az_ion_id_t  az_fs_openFile(const az_str_t path, int flag, int mode, az_file_t *pfile);
extern az_r_t  az_fs_closeFile(az_ion_id_t id);

extern az_bool_t  az_fs_exist(const az_str_t path);
extern az_size_t  az_fs_fileSize(const az_str_t path, az_ion_id_t id);

extern az_size_t  az_fs_readFile(az_ion_id_t id, az_uint8_t *bp, az_size_t size);
extern az_size_t  az_fs_writeFile(az_ion_id_t id, az_uint8_t *bp, az_size_t size);
extern az_size_t  az_fs_lseekFile(az_ion_id_t id, az_pos_t offset, int whence);

extern az_r_t  az_fs_truncate(const az_str_t path, az_ion_id_t id, az_size_t size);

extern az_size_t  az_fs_file2mem(const az_str_t path, az_ref_t *pMem);

#ifdef __cplusplus
}
#endif
#endif
