/**
 * @file   az_link.h
 * @brief  
 * @date   29/04/16
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
 *   - 2016-04-29 fware initial create
 */

#ifndef AZ_LINK_H
#define AZ_LINK_H

#include "az_assert.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* constants */

/* basic macros */

/* basic types */
typedef struct az_link {
  struct az_link *next;
} az_link_t;

typedef struct az_link_list {
  az_link_t *next;
  az_link_t *prev;
  az_size_t count;
} az_link_list_t;

typedef struct az_double_link {
  struct az_double_link *next;
  struct az_double_link *prev;
} az_double_link_t;

typedef struct az_double_link_list {
  struct az_double_link *next;
  struct az_double_link *prev;
  az_size_t count;
} az_double_link_list_t;

/* structures */

/* structured types */

/* macros */

/* variabls exposed */

/* inline functions */
/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
static inline void az_link_initList(az_link_list_t *list)
{
  az_assert(NULL != list);
  list->count = 0;
  list->next = list->prev = (az_link_t *)list;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
static inline void az_link_init(az_link_t *link)
{
  az_assert(NULL != link);
  link->next = NULL;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
static inline void az_link_putLast(az_link_list_t *list, az_link_t *link)
{
  az_assert(NULL != list && NULL != link);
  link->next = (list->prev)->next;
  (list->prev)->next = link;
  list->prev = link;
  list->count++;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
static inline void az_link_putFirst(az_link_list_t *list, az_link_t *link)
{
  az_assert(NULL != list && NULL != link);
  if (0 == list->count) {
    az_link_putLast(list, link);
  } else {
    link->next = list->next;
    list->next = link;
    list->count++;
  }
}
/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
static inline void az_link_remove(az_link_list_t *list, az_link_t *prev, az_link_t *link)
{
  az_assert(NULL != list && NULL != prev && NULL != link);
  az_assert(prev != link && prev->next == link);
  prev->next = link->next;
  if (list->prev == link) {
    list->prev = prev;
  }
  link->next = NULL;
  list->count--;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
static inline az_link_t *az_link_getFirst(az_link_list_t *list)
{
  az_link_t *link = NULL;
  az_assert(NULL != list);
  if (list->count > 0) {
    link = list->next;
    az_assert(link == list->next);
    az_link_remove(list, (az_link_t *)list, link);
  } 
  return link;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
static inline void az_double_link_init(az_double_link_t *link)
{
  az_assert(NULL != link);
  link->prev = link->next = link; 
}
/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
static inline void az_double_link_initList(az_double_link_list_t *list)
{
  az_assert(NULL != list);
  list->prev = list->next = (az_double_link_t *)list; 
  list->count = 0;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
static inline void az_double_link_add(az_double_link_t *p, az_double_link_t *n)
{
  (p->next)->prev = n;
  n->prev = p;
  n->next = p->next;
  p->next = n;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
static inline void az_double_link_del(az_double_link_t *n)
{
  az_double_link_t *p = n->prev;
  p->next = n->next;
  (n->next)->prev = n->prev;
  n->next = n->prev = n;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
static inline void az_double_link_putLast(az_double_link_list_t *list, az_double_link_t *link)
{
  az_assert(NULL != list && NULL != link);

  az_double_link_add(list->prev, link);

  list->count++;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
static inline void az_double_link_putFirst(az_double_link_list_t *list, az_double_link_t *link)
{
  az_assert(NULL != list && NULL != link);

  az_double_link_add((az_double_link_t *)list, link);

  list->count++;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
static inline az_double_link_t *az_double_link_getFirst(az_double_link_list_t *list)
{
  az_double_link_t *link = NULL;
  az_assert(NULL != list);

  if ((void *)list != (void *)list->next) {
    az_assert(list->count > 0);
    az_double_link_del((link = list->next));
    list->count--;
  }

  return link;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
static inline az_double_link_t *az_double_link_getLast(az_double_link_list_t *list)
{
  az_double_link_t *link = NULL;
  az_assert(NULL != list);

  if ((void *)list != (void *)list->prev) {
    az_assert(list->count > 0);
    az_double_link_del((link = list->prev));
    list->count--;
  }
  return link;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
static inline void az_double_link_remove(az_double_link_list_t *list, az_double_link_t *link)
{
  az_assert(NULL != list && NULL != link);
  if (list->count > 0) {
    az_double_link_del(link);
    list->count--;
  }
}

/* function prototypes exposed */
extern az_size_t az_link_toStr(az_link_t *, char *, char *, az_size_t);
extern az_size_t az_link_list_toStr(az_link_list_t *, char *, char *, az_size_t);
extern az_size_t az_double_link_toStr(az_double_link_t *, char *, char *, az_size_t);
extern az_size_t az_double_link_list_toStr(az_double_link_list_t *, char *, char *, az_size_t);

#ifdef __cplusplus
}
#endif
#endif
