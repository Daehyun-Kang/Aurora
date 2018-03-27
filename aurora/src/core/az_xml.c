/**
 * @file   az_xml.c
 * @brief  
 * @date   06/07/16
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
 *   - 2016-07-06 fware initial create
 */

/* include header files */
#include "az_core.h"
#include "az_printf.h"
#include "az_xml.h"

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
void az_xml_parser_init(az_xml_parser_t *parser, char *start, char *end, int (*callback)(az_xml_parser_t *, char *))
{
  parser->state = xml_tag_end;
  parser->token = xml_empty_tag;
  parser->start = start;
  parser->end = end;
  parser->tag = NULL;
  parser->tag_length = 0;
  parser->content = start;
  parser->content_length = 0;
  parser->callback = callback;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
void az_xml_parser_run(az_xml_parser_t *parser)
{
  az_assert(NULL != parser);

  char *p = parser->start;
  char *end = parser->end;
  int len = 0;

  for (; p != end; p++, len++) {
    switch (*p) {
      case  AZ_XML_TAG_OPEN:
        if (len > 0 && parser->state == xml_tag_end) {
          parser->token = xml_string;
          parser->content_length = len - 1;
          (parser->callback)(parser, p);
        }
        parser->tag = p+1;
        parser->content = p+1;
        len = 0;
        parser->state = xml_tag_start;
        break;
      case AZ_XML_TAG_ESC: { 
        if (parser->state != xml_tag_start) {
          break;
        }
        if (string_begins_no_case(AZ_XML_CDATA_START_TAG, p-1) > 0) {
          parser->state = xml_cdata_start;
          parser->tag = p;
          parser->tag_length = 8;
          p += 8;
          parser->content = p;
          len = 0;
          do {
            if (string_begins_no_case(AZ_XML_CDATA_END_TAG, p-2) == 1) {
              parser->state = xml_cdata_end;
              parser->token = xml_string;
              parser->content_length = len - 3;
              (parser->callback)(parser, p);
              break;
            }
            p++;
            len++;
          } while (p != end);
          if (parser->state != xml_cdata_end) {
            parser->token = xml_parse_error; 
            parser->result = xml_parse_no_cdata_end;
            (parser->callback)(parser, p);
          }
          parser->state = xml_tag_end;
          parser->content = p + 1;
          len = 0;
          break;
        }
        if (string_begins_no_case(AZ_XML_COMMENT_START_TAG, p-1) > 0) {
          parser->state = xml_comment_start;
          parser->tag = p;
          parser->tag_length = 3;
          p += 3;
          parser->content = p+1;
          len = 0;
          do {
            if (string_begins_no_case(AZ_XML_COMMENT_END_TAG, p-2) == 1) {
              parser->state = xml_comment_end;
              parser->token = xml_comment;
              parser->content_length = len - 3;
              (parser->callback)(parser, p);
              break;
            }
            p++;
            len++;
          } while (p != end);
          if (parser->state != xml_comment_end) {
            parser->token = xml_parse_error; 
            parser->result = xml_parse_no_comment_end;
            (parser->callback)(parser, p);
          }
          parser->state = xml_tag_end;
          parser->content = p + 1;
          len = 0;
          break;
        }
        parser->token = xml_parse_error; 
        parser->result = xml_parse_invalid_esc_tag;
        (parser->callback)(parser, p);
      } break; //case AZ_XML_START_ESC:
      case AZ_XML_TAG_DECL: {
        if (parser->state != xml_tag_start) {
          break;
        }
        parser->tag = p;
        parser->tag_length = 1;
        parser->content = p + 2;
        len = 0;
        do {
          if (string_begins_no_case(AZ_XML_DECL_END_TAG, p-1) == 1) {
            parser->state = xml_decl_tag;
            parser->token = xml_decl_tag;
            parser->content_length = len - 2;
            //az_sys_printf("%p\n", parser->tag);
            (parser->callback)(parser, p);
            break;
          }
          p++;
          len++;
        } while (p != end);
        if (parser->state != xml_decl_tag) {
          parser->token = xml_parse_error; 
          parser->result = xml_parse_no_decl_end;
          (parser->callback)(parser, p);
        }
        parser->state = xml_tag_end;
          parser->content = p + 1;
        len = 0;
      } break; //case AZ_XML_DECL_TAG:
      case AZ_XML_TAG_SLASH: {
          if (parser->state == xml_tag_start) {
            parser->tag = p+1;
            len = 0;
            while (p != end) {
              if (*p == AZ_XML_TAG_CLOSE) {
                len--;
                parser->state = xml_tag_end;
                break;
              }
              p++;
              len++;
              if (is_space(*p)) break;
            }
            if (parser->state == xml_tag_end) {
              parser->token = xml_end_tag;
              //parser->tag_length = len-1;
              parser->tag_length = len;
              //az_sys_printf("%s:%d\n", __FUNCTION__, __LINE__);
              (parser->callback)(parser, p);
            } else {
              parser->token = xml_parse_error;
              parser->result = xml_parse_space_in_tag2;
              (parser->callback)(parser, p);
              break;
            }
            parser->state = xml_tag_end;
            parser->content = p + 1;
            len = 0;
          }
      } break; 
      case AZ_XML_TAG_CLOSE: {
        if (parser->state == xml_tag_end) {
          parser->token = xml_parse_error;
          parser->result = xml_parse_invalid_close_tag;
          (parser->callback)(parser, p);
          break;
        }
        int is_endtag = 0;
        if (*(p-1) == AZ_XML_TAG_SLASH) {
          len--;
          is_endtag = 1;
        }
        //len--;
        if (parser->state == xml_tag_start) {
          parser->token = xml_empty_tag;
          (parser->callback)(parser, p);
        }
        if (parser->state == xml_parse_tag_name) { 
          parser->token = xml_start_tag;
          parser->tag_length = len;
          (parser->callback)(parser, p);

          if (is_endtag) {
            parser->token = xml_end_tag;
            //az_sys_printf("%s:%d\n", __FUNCTION__, __LINE__);
            (parser->callback)(parser, p);
          }
        }
        if (parser->state == xml_parse_tag_content) {
          if (is_endtag) {
            parser->token = xml_end_tag;
            parser->tag_length = 0;
            //az_sys_printf("%s:%d\n", __FUNCTION__, __LINE__);
            (parser->callback)(parser, p);
          }
        }
        parser->state = xml_tag_end;
        parser->content = p + 1;
        len = 0;
      } break; 
      default: {
        if (parser->state == xml_tag_start) {
          if (is_space(*p)) {
            parser->token = xml_parse_error; 
            parser->result = xml_parse_space_in_tag0;
            (parser->callback)(parser, p);
            break;
          } 
          parser->tag = p;
          len = 0;
          // find tag name
          parser->state = xml_parse_tag_content;
          for (; p != end && !is_space(*p); p++, len++) {
            if (*p == AZ_XML_TAG_CLOSE) {
              p--; len--;
              //az_sys_printf("%s:%d %p\n", __FUNCTION__, __LINE__, p);
              parser->state = xml_parse_tag_name;
              break;
            }
          }
          if (p == end) {
            parser->token = xml_parse_error; 
            parser->result = xml_parse_endless_tag;
            (parser->callback)(parser, p);
            break;
          }
          if (parser->state == xml_parse_tag_content) {
            parser->token = xml_start_tag;
            parser->tag_length = len;
            (parser->callback)(parser, p);
          }
        } else if (parser->state == xml_parse_tag_content) {
          char *tag_saved = parser->tag;
          int   tag_len_saved = parser->tag_length;
          char  *last = p;
          //az_sys_printf("%s:%d %p\n", __FUNCTION__, __LINE__, tag_saved);
          // find '>'
          len = 0;
          for (; p != end && *p != AZ_XML_TAG_CLOSE; p++, len++);
          if (p == end) {
            parser->token = xml_parse_error;
            parser->result = xml_parse_tag_not_closed;
            (parser->callback)(parser, p);
            break;
          }
          if (*(p-1) == AZ_XML_TAG_SLASH) {
            len -= 1;
          } else {
            tag_saved = NULL;
          }
          // save and init context for parsing attributes
          p = last;
          last = end; 
          end = p + len; 

          /*
          parser->token = xml_tag_content;
          parser->tag = p;
          parser->tag_length = len;
          (parser->callback)(parser, p);

          az_sys_printf("%s:%d %p\n", __FUNCTION__, __LINE__, p);
          */
          do {
            //az_sys_printf("%s:%d %p\n", __FUNCTION__, __LINE__, p);
            // skip spaces
            for (; p != end && is_space(*p); p++);
            if (p == end) {
              break;
            }
            // attr name
            parser->tag = p;
            len = 0;
            for (;p != end && !is_space(*p) && *p != '='; p++, len++); 
            if (p == end) {
              parser->tag_length = len;
              parser->token = xml_tag_content;
              az_sys_printf("%s:%d\n", __FUNCTION__, __LINE__);
              (parser->callback)(parser, p);
              break;
            }
            for (; p != end && is_space(*p); p++);
            if (p == end || *p != '=') {
              parser->tag_length = len;
              parser->token = xml_tag_content;
              az_sys_printf("%s:%d %p\n", __FUNCTION__, __LINE__, p);
              (parser->callback)(parser, p);
              break;
            }
            parser->tag_length = len;
            // parse attr value
            for (++p; p != end && is_space(*p); p++);
            if (p == end) {
              parser->token = xml_parse_error;
              parser->result = xml_parse_no_attr_value;
              (parser->callback)(parser, p);
              break;
            }
            if (*p != '\'' && *p != '\"') {
              parser->token = xml_parse_error;
              parser->result = xml_parse_attr_value_open;
              (parser->callback)(parser, p);
              break;
            }
            char q = *p;
            parser->content = p + 1;
            len = 0;
            for (++p;p != end && *p != q; p++, len++); 
            if (p == end) {
              parser->token = xml_parse_error;
              parser->result = xml_parse_attr_value_close;
              (parser->callback)(parser, p);
              break;
            }
            parser->content_length = len;
            parser->token = xml_attribute;
            (parser->callback)(parser, p);
            p++; 
          } while (1);
          // restore context
          if (tag_saved) {
            p = end + 2; 
          } else {
            p = end + 1; 
          }
          end = last;

          if (tag_saved) {
            parser->token = xml_end_tag;
            parser->tag = tag_saved;
            parser->tag_length = tag_len_saved;
            //az_sys_printf("%s:%d\n", __FUNCTION__, __LINE__);
            (parser->callback)(parser, p);
          }
          parser->state = xml_tag_end;
          parser->content = p + 1;
          len = 0;
        } else {
            // discard
            //parser->token = xml_parse_error;
            parser->token = xml_empty_tag;
            //parser->result = xml_parse_invalid_state;
            //(parser->callback)(parser, p);
            break;
        }
      } break; //default
    }
    if (parser->token == xml_parse_error) break;
  }
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
int az_xml_parser_default_callback(az_xml_parser_t *parser, char *p)
{
  char c, d;
  char *tag, *content;
  int j;
  int trace_f = 0;

  tag = parser->tag;
  content = parser->content;
  switch (parser->token) {
    case xml_parse_error:
      if (trace_f) {
        char buf[11];
        az_sys_printf("error reason: %d, content ", parser->result);
        for (j = 0; j < 10 && p != parser->end; p++, j++) {
          buf[j] = *p;
        }
        buf[j] = 0;
        az_sys_printf("%s" AZ_NL, buf);
      }
      break;
    case xml_start_tag:
      c = tag[parser->tag_length];
      tag[parser->tag_length] = 0;
      if (trace_f) az_sys_printf("tag start: %s %p\n", tag, p);
      tag[parser->tag_length] = c;
      break;
    case xml_end_tag:
      c = tag[parser->tag_length];
      tag[parser->tag_length] = 0;
      if (trace_f) az_sys_printf("tag end: %s \n", tag);
      tag[parser->tag_length] = c;
      break;
    case xml_decl_tag:
      c = tag[parser->tag_length];
      tag[parser->tag_length] = 0;
      d = content[parser->content_length];
      content[parser->content_length] = 0;
      if (trace_f) az_sys_printf("decl =%s, content=%s \n", tag, content);
      tag[parser->tag_length] = c;
      content[parser->content_length] = d;
      break;
    case xml_comment:
      c = tag[parser->tag_length];
      tag[parser->tag_length] = 0;
      d = content[parser->content_length];
      content[parser->content_length] = 0;
      if (trace_f) az_sys_printf("comment %s, content=%s \n", tag, content);
      tag[parser->tag_length] = c;
      content[parser->content_length] = d;
      break;
    case xml_string:
      d = content[parser->content_length];
      content[parser->content_length] = 0;
      if (trace_f) az_sys_printf("string content=%s \n", content);
      content[parser->content_length] = d;
      break;
    case xml_attribute:
      c = tag[parser->tag_length];
      tag[parser->tag_length] = 0;
      d = content[parser->content_length];
      content[parser->content_length] = 0;
      if (trace_f) az_sys_printf("attribute %s, content=%s \n", tag, content);
      tag[parser->tag_length] = c;
      content[parser->content_length] = d;
      break;
    case xml_tag_content:
      c = tag[parser->tag_length];
      tag[parser->tag_length] = 0;
      if (trace_f) az_sys_printf("tag=%s content=%s %p\n", tag, content, p);
      tag[parser->tag_length] = c;
      break;
  }
  return 0;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
int az_xml_tree_handle_parser_event(az_xml_parser_t *parser, char *p)
{
  int r = 0;
  char c, d;
  char *tag, *content;
  int j;
  int trace_f = 0;
  az_xml_element_t *cur = (az_xml_element_t *)(parser->priv);
  az_xml_element_t *n = NULL; 

  if (trace_f) az_sys_printf("cur kv=%s:%s\n", cur->kv.key, cur->kv.value);

  tag = parser->tag;
  content = parser->content;
  switch (parser->token) {
    case xml_parse_error:
      if (trace_f) {
        char buf[11];
        az_sys_printf("error reason: %d, content ", parser->result);
        for (j = 0; j < 10 && p != parser->end; p++, j++) {
          buf[j] = *p;
        }
        buf[j] = 0;
        az_sys_printf("%s" AZ_NL, buf);
      }
      break;
    case xml_start_tag:
      tag[parser->tag_length] = 0;
      n = az_xml_element_add_child(cur, NULL, tag, "element");
      if (NULL != n) {
        parser->priv = n;
      } else {
        az_sys_printf("tag %s not added\n", tag);
        r = AZ_ERR(MALLOC);
      }
      if (trace_f) az_sys_printf("tag start: %s %p\n", tag, p);
      break;
    case xml_end_tag:
      if (trace_f) az_sys_printf("tag end: %s \n", tag);
      if (cur->parent) {
        parser->priv = (cur->parent);
      } else {
        if (trace_f) az_sys_printf("%s is root\n", cur->kv.key);
      }
      break;
    case xml_decl_tag:
      c = tag[parser->tag_length];
      tag[parser->tag_length] = 0;
      d = content[parser->content_length];
      content[parser->content_length] = 0;
      if (trace_f) az_sys_printf("decl =%s, content=%s \n", tag, content);
      tag[parser->tag_length] = c;
      content[parser->content_length] = d;
      break;
    case xml_comment:
      c = tag[parser->tag_length];
      tag[parser->tag_length] = 0;
      d = content[parser->content_length];
      content[parser->content_length] = 0;
      if (trace_f) az_sys_printf("comment %s, content=%s \n", tag, content);
      tag[parser->tag_length] = c;
      content[parser->content_length] = d;
      break;
    case xml_string:
      //content[parser->content_length] = 0;
      //cur->kv.value = content;
      break;
    case xml_attribute:
      tag[parser->tag_length] = 0;
      content[parser->content_length] = 0;
      if (trace_f) az_sys_printf("attribute %s, content=%s \n", tag, content);
      n = az_xml_element_add_attr(cur, NULL, tag, content); 
      break;
    case xml_tag_content:
      tag[parser->tag_length] = 0;
      if (trace_f) az_sys_printf("tag=%s content=%s %p\n", tag, content, p);
      cur->kv.value = tag;
      break;
  }
  if (n) {
  if (trace_f) az_sys_printf("new kv=%s:%s\n", n->kv.key, n->kv.value);
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
az_r_t  az_xml_tree_parse(char *start, char *end, az_xml_element_t *root)
{
  az_r_t result;
  az_xml_parser_t parser;
  az_assert(NULL != start && NULL != end && NULL != root);

  az_xml_parser_init(&parser, start, end, az_xml_tree_handle_parser_event); 
  parser.priv = root;

  parser.result = xml_parse_success;
  az_xml_parser_run(&parser);
  if (parser.result != xml_parse_success) {
    result = AZ_ERR(XML_PARSE);
  } else {
    result = AZ_SUCCESS;
  }
  return result;
}

/**
 * @fn 
 * @brief 
 * @param 
 * @return 
 * @exception    none
 */
az_size_t  az_xml_tree_element_count(az_xml_element_t *m)
{
  az_size_t count = 1 + m->attr_list.count;
  int j;
  az_double_link_t *l;
  az_xml_element_t *n;
  
  for (j = 0, l = m->child_list.next; j < m->child_list.count; j++) {
    n = container_of(l, az_xml_element_t, link);
    count += az_xml_tree_element_count(n);
    l = l->next;
  }

  return count;
}

static char *tab_str[] = {
  "", "  ", "    ", "      ", "        ", "          " };
az_size_t az_xml_print_attr(char *bp, az_size_t blen, az_xml_element_t *m, int depth)
{
  char *tab;
  az_size_t tlen = 0;

  tab = tab_str[((depth < 5)? depth:5)];

  _AZ_SNPRINTF(tlen, bp, blen, "%s" AZ_FMT_KV_STR(1, 1) "\n", tab, m->kv.key, m->kv.value);

  return tlen;
}
az_size_t az_xml_print_element(char *bp, az_size_t blen, az_xml_element_t *m, int depth)
{
  char *tab;
  int j;
  az_double_link_t *l;
  az_xml_element_t *n;
  az_size_t tlen = 0, nlen;

  tab = tab_str[(depth < 5)? depth:5];

  do {
    _AZ_SNPRINTF(tlen, bp, blen, "%s[%d]" AZ_FMT_KV_STR(1,1) "    child:"AZ_FMT_SIZE(1)" attr:"AZ_FMT_SIZE(1)"\n", tab, depth, m->kv.key, m->kv.value, m->child_list.count, m->attr_list.count);
    if (blen <= 0) break;
    for (j = 0, l = m->attr_list.next; j < m->attr_list.count; j++) {
      n = container_of(l, az_xml_element_t, link);
      nlen = az_xml_print_attr(bp, blen, n, depth+1);
      _AZ_FORMAT_UPDATE(tlen, bp, blen, nlen);
      if (blen <= 0) break;
      l = l->next;
    }
    if (blen <= 0) break;
    for (j = 0, l = m->child_list.next; j < m->child_list.count; j++) {
      n = container_of(l, az_xml_element_t, link);
      nlen = az_xml_print_element(bp, blen, n, depth+1);
      _AZ_FORMAT_UPDATE(tlen, bp, blen, nlen);
      if (blen <= 0) break;
      l = l->next;
    }
  } while (0);

  return tlen;
}

/* === end of AZ_XML_C === */
