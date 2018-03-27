/**
 * @file   az_xml.h
 * @brief  
 * @date   06/07/16
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
 *   - 2016-07-06 fware initial create
 */

#ifndef AZ_XML_H
#define AZ_XML_H

#include "az_def.h"
#include "az_assert.h"
#include "az_malloc.h"
#include "az_string.h"
#include "az_link.h"
#include "az_kv.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* constants */

#define AZ_XML_TAG_OPEN            '<'
#define AZ_XML_TAG_CLOSE           '>'
#define AZ_XML_TAG_ESC             '!'
#define AZ_XML_TAG_DECL            '?'
#define AZ_XML_TAG_SLASH           '/'

#define AZ_XML_CDATA_START_TAG      "<![CDATA["
#define AZ_XML_CDATA_END_TAG        "]]>"
#define AZ_XML_COMMENT_START_TAG    "<!--"
#define AZ_XML_COMMENT_END_TAG      "-->"
#define AZ_XML_DECL_START_TAG       "<?"
#define AZ_XML_DECL_END_TAG         "?>"

/* basic macros */

/* basic types */
enum az_xml_parser_state {
  //xml_parse_stop,
  //xml_parse_start,
  xml_tag_start,
  xml_tag_end,
  xml_esc_start,
  xml_cdata_start,
  xml_cdata_end,
  xml_comment_start,
  xml_comment_end,
  xml_parse_tag_name,
  xml_parse_tag_content,
};

enum az_xml_token {
  xml_start_tag,
  xml_end_tag,
  xml_empty_tag,
  xml_decl_tag,
  xml_string,
  xml_attribute,
  xml_comment,
  xml_parse_error,
  xml_tag_content,
};

enum az_xml_parse_result {
  xml_parse_success,          //0
  xml_parse_no_cdata_end,     //1
  xml_parse_no_comment_end,   //2
  xml_parse_invalid_esc_tag,  //3
  xml_parse_invalid_close_tag,//4
  xml_parse_no_decl_end,      //5
  xml_parse_space_in_tag0,    //6
  xml_parse_space_in_tag1,    //7
  xml_parse_space_in_tag2,    //8
  xml_parse_endless_tag,      //9
  xml_parse_tag_not_closed,   //10
  xml_parse_no_attr_value,    //11
  xml_parse_attr_value_open,  //12
  xml_parse_attr_value_close, //13
  xml_parse_invalid_state,    //14
};

#ifdef  AZ_XML_ALLOC
char *az_xml_parse_result_str[] = {
  #xml_parse_success,
  #xml_parse_no_cdata_end,
  #xml_parse_no_comment_end,
  #xml_parse_invalid_esc_tag,
  #xml_parse_invalid_close_tag,
  #xml_parse_no_decl_end,
  #xml_parse_space_in_tag0,
  #xml_parse_space_in_tag1,
  #xml_parse_space_in_tag2,
  #xml_parse_endless_tag,
  #xml_parse_tag_not_closed,
  #xml_parse_no_attr_value,
  #xml_parse_attr_value_open,
  #xml_parse_attr_value_close,
  #xml_parse_invalid_state,
}
#else
extern char *az_xml_parse_result_str[];
#endif

/* structures */
typedef struct az_xml_parser {
  int   state;
  int   token;
  
  char  *start;
  char  *end;

  char  *tag;
  int   tag_length;

  char  *content;
  int   content_length;

  int   result;

  int   (*callback)(struct az_xml_parser *, char *);
  void  *priv;
} az_xml_parser_t;

typedef struct az_xml_element {
  az_kv_t               kv;
  az_double_link_t      link;
  
  struct az_xml_element *parent; 
  az_double_link_list_t child_list;
  az_double_link_list_t attr_list;

  az_refcount_t refCount;
} az_xml_element_t;

/* structured types */

/* macros */

/* variabls exposed */

/* inline functions */
static inline az_xml_element_t *az_xml_element_init(az_xml_element_t *e, char *key, char *value)
{
  do {
    if (NULL == e) {
      e = az_malloc(sizeof(az_xml_element_t));
      if (NULL == e) break;
      az_refcount_init_dynamic(&e->refCount);
    }
    az_refcount_atomic_inc(&e->refCount);
    
    e->kv.key = key;
    e->kv.value = value;

    e->parent = NULL;
    az_double_link_init(&e->link);
    az_double_link_initList(&e->child_list);
    az_double_link_initList(&e->attr_list);
  } while (0);

  return e;
}

static inline void az_xml_element_release(az_xml_element_t *e)
{
  az_xml_element_t *c;
  az_double_link_t *l;
  do {
    if (NULL == e) break;
    
    do {
      l = az_double_link_getLast(&e->child_list); 
      if (NULL == l) break;
      c = container_of(l, az_xml_element_t, link);
      az_xml_element_release(c);
    } while(1);

    do {
      l = az_double_link_getLast(&e->attr_list); 
      if (NULL == l) break;
      c = container_of(l, az_xml_element_t, link);
      az_assert(c->child_list.count == 0);
      az_assert(c->attr_list.count == 0);
      az_refcount_atomic_dec(&c->refCount);
      if (AZ_REFCOUNT_IS_ZERO(&c->refCount)) {
        az_free(c);
      } else {
        c->parent = NULL;
      }
    } while(1);

    az_assert(e->child_list.count == 0);
    az_assert(e->attr_list.count == 0);

    az_refcount_atomic_dec(&e->refCount);
    if (AZ_REFCOUNT_IS_ZERO(&e->refCount)) {
      az_free(e);
    } else {
      e->parent = NULL;
    }
  } while (0);
}

static inline az_xml_element_t *az_xml_element_add_child(az_xml_element_t *e, az_xml_element_t *c, char *key, char *value) 
{
  az_assert(e != NULL);
  //az_sys_printf("%s <= %s\n", e->kv.key, key);
  do {
    c = az_xml_element_init(c, key, value);
    if (NULL == c) {
      break;
    }
    c->parent = e;
    az_double_link_putLast(&e->child_list, &c->link); 
  } while (0);

  return c;
}
static inline void az_xml_element_remove_child(az_xml_element_t *c)
{
    az_double_link_del(&c->link); 
}

static inline az_xml_element_t *az_xml_element_add_attr(az_xml_element_t *e, az_xml_element_t *c, char *key, char *value) 
{
  az_assert(e != NULL);
  do {
    c = az_xml_element_init(c, key, value);
    if (NULL == c) {
      break;
    }
    c->parent = e;
    az_double_link_putLast(&e->attr_list, &c->link); 
  } while (0);

  return c;
}
static inline az_xml_element_t *az_xml_element_next_child(az_xml_element_t *e, az_xml_element_t *child) 
{
  az_assert(NULL != e);
  az_double_link_t *l;
  do {
    if (NULL == child) {
      if (e->child_list.count == 0) {
        break;
      }
      l = e->child_list.next;
      child = container_of(l, az_xml_element_t, link);
      break;
    } 
    if (child->parent != e) {
      break;
    }
    l = child->link.next;
    if (AZ_EQUAL_PTR(l, &e->child_list)) {
      child = NULL;
      break;
    }
    child = container_of(l, az_xml_element_t, link);
  } while (0);

  return child;
}
static inline az_xml_element_t *az_xml_element_next_attr(az_xml_element_t *e, az_xml_element_t *attr) 
{
  az_assert(NULL != e);
  az_double_link_t *l;
  do {
    if (NULL == attr) {
      if (e->attr_list.count == 0) {
        break;
      }
      l = e->attr_list.next;
      attr = container_of(l, az_xml_element_t, link);
      break;
    } 
    if (attr->parent != e) {
      break;
    }
    l = attr->link.next;
    if (AZ_EQUAL_PTR(l, &e->attr_list)) {
      attr = NULL;
      break;
    }
    attr = container_of(l, az_xml_element_t, link);
  } while (0);

  return attr;
}

static inline az_xml_element_t *az_xml_element_find_child(az_xml_element_t *e, char *key)
{
  az_assert(e != NULL);
  az_double_link_t *l;
  az_xml_element_t *c = NULL;
  l = e->child_list.next;
  do {
    if (AZ_EQUAL_PTR(l, &e->child_list)) {
      c = NULL;
      break;
    }
    c = container_of(l, az_xml_element_t, link);
    if (!strcmp(c->kv.key, key)) {
      break;
    }
    l = l->next;
  } while (1);

  return c;
}

static inline az_xml_element_t *az_xml_element_find_attr(az_xml_element_t *e, char *key)
{
  az_assert(e != NULL);
  az_double_link_t *l;
  az_xml_element_t *c = NULL;
  l = e->attr_list.next;
  do {
    if (AZ_EQUAL_PTR(l, &e->attr_list)) {
      c = NULL;
      break;
    }
    c = container_of(l, az_xml_element_t, link);
    if (!strcmp(c->kv.key, key)) {
      break;
    }
    l = l->next;
  } while (1);

  return c;
}

static inline az_r_t az_xml_element_iterateOnChild(az_xml_element_t *e, void *arg, az_r_t r, az_r_t (*func)(az_xml_element_t *, void *, az_r_t)) 
{
  az_assert(e != NULL);
  az_double_link_t *l;
  az_xml_element_t *c = NULL;
  l = e->child_list.next;
  do {
    if (AZ_EQUAL_PTR(l, &e->child_list)) {
      c = NULL;
      break;
    }
    c = container_of(l, az_xml_element_t, link);
    if ((r = (func)(c, arg, r)) < 0) {
      break;
    }
    l = l->next;
  } while (1);

  return r;
}
typedef az_r_t (*az_xml_element_proc_t)(az_xml_element_t *, void *, az_r_t);

static inline az_r_t az_xml_element_iterateOnAttr(az_xml_element_t *e, void *arg, az_r_t r, az_r_t (*func)(az_xml_element_t *, void *, az_r_t)) 
{
  az_assert(e != NULL);
  az_double_link_t *l;
  az_xml_element_t *c = NULL;
  l = e->attr_list.next;
  do {
    if (AZ_EQUAL_PTR(l, &e->attr_list)) {
      c = NULL;
      break;
    }
    c = container_of(l, az_xml_element_t, link);
    if ((r = (func)(c, arg, r)) < 0) {
      break;
    }
    l = l->next;
  } while (1);

  return r;
}

static inline az_xml_element_t *az_xml_element_del_child(az_xml_element_t *e, char *key)
{
  az_assert(e != NULL);
  az_double_link_t *l;
  az_xml_element_t *c = NULL;
  l = e->child_list.next;
  do {
    if (AZ_EQUAL_PTR(l, &e->child_list)) {
      c = NULL;
      break;
    }
    c = container_of(l, az_xml_element_t, link);
    if (!strcmp(c->kv.key, key)) {
      az_double_link_remove(&e->child_list, l);
      c->parent = NULL;
      break;
    }
    l = l->next;
  } while (1);

  return c;
}
static inline az_xml_element_t *az_xml_element_del_attr(az_xml_element_t *e, char *key)
{
  az_assert(e != NULL);
  az_double_link_t *l;
  az_xml_element_t *c = NULL;
  l = e->child_list.next;
  do {
    if (AZ_EQUAL_PTR(l, &e->attr_list)) {
      c = NULL;
      break;
    }
    c = container_of(l, az_xml_element_t, link);
    if (!strcmp(c->kv.key, key)) {
      az_double_link_remove(&e->attr_list, l);
      c->parent = NULL;
      break;
    }
    l = l->next;
  } while (1);

  return c;
}

/* function prototypes exposed */
extern void az_xml_parser_init(az_xml_parser_t *parser, char *start, char *end, int (*callback)(az_xml_parser_t *, char *));
extern void az_xml_parser_run(az_xml_parser_t *parser);
extern int az_xml_parser_default_callback(az_xml_parser_t *parser, char *);
extern az_r_t az_xml_tree_parse(char *, char *, az_xml_element_t *);
extern az_size_t  az_xml_tree_element_count(az_xml_element_t *m);
extern az_size_t az_xml_print_attr(char *bp, az_size_t blen, az_xml_element_t *m, int depth);
extern az_size_t az_xml_print_element(char *bp, az_size_t blen, az_xml_element_t *m, int depth);

#ifdef __cplusplus
}
#endif
#endif
