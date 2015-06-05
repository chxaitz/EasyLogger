/*
 * This file is part of the EasyLogger Library.
 *
 * Copyright (c) 2015, Armink, <armink.ztl@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Function: Initialize function and other general function.
 * Created on: 2015-04-28
 */

#include "elog.h"
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

/* output log's filter */
typedef struct {
    uint8_t level;
    char tag[ELOG_FILTER_TAG_MAX_LEN + 1];
    char keyword[ELOG_FILTER_KW_MAX_LEN + 1];
} ElogFilter, *ElogFilter_t;

/* easy logger */
typedef struct {
    ElogFilter filter;
    uint8_t enabled_fmt_set[6];                                                 /* be modified   at 2015-06-04 17:09 by chxaitz */
    bool    output_enabled;
}EasyLogger, *EasyLogger_t;

/* EasyLogger object */
static EasyLogger elog;
/* log buffer */
static char log_buf[ELOG_BUF_SIZE] = { 0 };
/* log tag */
static const char *tag = "ELOG";
/* level output info */
static const char *level_output_info[] = {
        "A/",
        "E/",
        "W/",
        "I/",
        "D/",
        "V/",
};

/* be added   at 2015-06-04 17:09 by chxaitz */
/* expand var & func by chxaitz */
typedef struct elog_node
{
  struct elog_node *pNext;
         size_t     len;
         char       Data[0];
}elog_node;

static elog_node *elog_list_head,*elog_list_tail;
static size_t elog_nodes_count;
static size_t elog_take_sapce;
static char *pc_elog_kw_alpha;
_ELOG_SYNC_t    elog_sobj;
//static bool get_fmt_enabled(uint8_t level, uint8_t set);                      /* be deleted   at 2015-06-04 17:09 by chxaitz */

/**
 * EasyLogger initialize.
 *
 * @return result
 */
ElogErrCode elog_init(void) {
    ElogErrCode result = ELOG_NO_ERR;

    /* port initialize */
    result = elog_port_init();
    /* default Fmt value */
    elog.enabled_fmt_set[ELOG_LVL_ASSERT]=ELOG_FMT_LVL|ELOG_FMT_TAG|ELOG_FMT_TIME/*|ELOG_FMT_P_INFO|ELOG_FMT_T_INFO*/|ELOG_FMT_DIR|ELOG_FMT_FUNC|ELOG_FMT_LINE;
    elog.enabled_fmt_set[ELOG_LVL_ERROR]=ELOG_FMT_LVL|ELOG_FMT_TAG|ELOG_FMT_TIME/*|ELOG_FMT_P_INFO|ELOG_FMT_T_INFO|ELOG_FMT_DIR*/|ELOG_FMT_FUNC/*|ELOG_FMT_LINE*/;
    elog.enabled_fmt_set[ELOG_LVL_WARN]=ELOG_FMT_LVL|ELOG_FMT_TAG|ELOG_FMT_TIME/*|ELOG_FMT_P_INFO|ELOG_FMT_T_INFO|ELOG_FMT_DIR*/|ELOG_FMT_FUNC/*|ELOG_FMT_LINE*/;
    elog.enabled_fmt_set[ELOG_LVL_INFO]=ELOG_FMT_LVL|ELOG_FMT_TAG|ELOG_FMT_TIME;
    elog.enabled_fmt_set[ELOG_LVL_DEBUG]=ELOG_FMT_LVL|ELOG_FMT_TAG|ELOG_FMT_TIME;
    elog.enabled_fmt_set[ELOG_LVL_VERBOSE]=ELOG_FMT_LVL|ELOG_FMT_TAG|ELOG_FMT_TIME;
    /* set level is ELOG_LVL_VERBOSE */
    elog_set_filter_lvl(ELOG_LVL_VERBOSE);
    /* enable output */
    elog_set_output_enabled(true);

    /* create sync obj*/
    elog_port_cre_syncobj ( &elog_sobj );
    
    if (result == ELOG_NO_ERR) {
        elog_d(tag, "EasyLogger V%s is initialize success.", ELOG_SW_VERSION);
    } else {
        elog_d(tag, "EasyLogger V%s is initialize fail.", ELOG_SW_VERSION);
    }
    return result;
}

/**
 * set output enable or disable
 *
 * @param enabled TRUE: enable FALSE: disable
 */
void elog_set_output_enabled(bool enabled) {
    ELOG_ASSERT((enabled == false) || (enabled == true));

    if( elog.output_enabled != enabled ) /* not equ */
    {
      if( false == enabled )    /* close the elog */
      { 
        /* if need to free the kw_filter space? */
        /* compulsory to write the logs to the stream and free the taked space */
        elog_output();
      }
      else      /* open the elog */
      {/* nothing need to do */}
      elog.output_enabled = enabled;
    }
}

/**
 * get output is enable or disable
 *
 * @return enable or disable
 */
bool elog_get_output_enabled(void) {
    return elog.output_enabled;
}

/**
 * set log output format. only enable or disable
 *
 * @param set format set
 */
void elog_set_fmt( uint8_t level, uint8_t set) {
    elog.enabled_fmt_set[level] = set;
}

/**
 * set log filter all parameter
 *
 * @param level level
 * @param tag tag
 * @param keyword keyword
 */
void elog_set_filter(uint8_t level, const char *tag, const char *keyword) {
    ELOG_ASSERT(level <= ELOG_LVL_VERBOSE);

    elog_set_filter_lvl(level);
    elog_set_filter_tag(tag);
    elog_set_filter_kw(keyword);
}

/**
 * set log filter's level
 *
 * @param level level
 */
void elog_set_filter_lvl(uint8_t level) {
    ELOG_ASSERT(level <= ELOG_LVL_VERBOSE);

    elog.filter.level = level;
}

/**
 * set log filter's tag
 *
 * @param tag tag
 */
void elog_set_filter_tag(const char *tag) {
    strncpy(elog.filter.tag, tag, ELOG_FILTER_TAG_MAX_LEN);
}

/**
 * set log filter's keyword
 *
 * @param keyword keyword
 */
void elog_set_filter_kw(const char *keyword) {
  if( *keyword )        /* if not null */
  {
    if( NULL == pc_elog_kw_alpha )
      if( (pc_elog_kw_alpha=_elog_malloc(ELOG_KW_ALPHA_MAX_LEN))==NULL )   /* apply mem*/
        return;
    strncpy(elog.filter.keyword, keyword, ELOG_FILTER_KW_MAX_LEN);
    elog_kw_alpha_set( pc_elog_kw_alpha, keyword );
  }
  else                  /* if null */
  {
    if( pc_elog_kw_alpha ) /* free the applyed mem */
    { _elog_free( pc_elog_kw_alpha ); pc_elog_kw_alpha = NULL; elog.filter.keyword[0]='\0';}
  }
  return;
}

/**
 * output RAW format log
 *
 * @param format output format
 * @param ... args
 */
void elog_raw(const char *format, ...) {
    va_list args;
    int fmt_result;

    /* check output enabled */
    if (!elog.output_enabled) {
        return;
    }

    /* args point to the first variable parameter */
    va_start(args, format);

    /* lock output */
    elog_port_req_grant(elog_sobj);

    /* package log data to buffer */
    fmt_result = vsnprintf(log_buf, ELOG_BUF_SIZE, format, args);

    /* output converted log */
    if ((fmt_result > -1) && (fmt_result < ELOG_BUF_SIZE)) {
      log_buf[fmt_result++]='\0';
      /* add log to log_nodes list */                                           /* be added   at 2015-06-04 17:09 by chxaitz */
      elog_node *px_eln;
      if((px_eln = (elog_node*)_elog_malloc( sizeof(elog)+fmt_result ))!=NULL)
      {
        /* init the node */
        px_eln->pNext = NULL;
        px_eln->len   = fmt_result;
        memcpy( px_eln->Data, log_buf, fmt_result );
        
        /* add to the logs' list */
        _elog_enter_critical();
        if( elog_list_head == NULL )       //empty list
        {
          elog_list_head = px_eln;
          elog_list_tail = px_eln;
        }
        else/* unempty list */
        {
          elog_list_tail->pNext = px_eln;
          elog_list_tail        = px_eln;
        }
        _elog_exit_critical();
        /* update some data */
        elog_nodes_count++;
        elog_take_sapce+=fmt_result;
      }
    }

    /* unlock output */
    elog_port_rel_grant(elog_sobj);

    va_end(args);
}

/**
 * output the log
 *
 * @param level level
 * @param tag tag
 * @param file file name
 * @param func function name
 * @param line line number
 * @param format output format
 * @param ... args
 *
 */
void elog_add(uint8_t level, const char *tag, const char *file, const char *func,
        const long line, const char *format, ...) {
    size_t tag_len = strlen(tag), log_len = 0;
//    char line_num[ELOG_LINE_NUM_MAX_LEN + 1] = { 0 };                         /* be deleted at 2015-06-04 17:09 by chxaitz */
    char tag_sapce[ELOG_FILTER_TAG_MAX_LEN / 2 + 1] = { 0 };
    va_list args;
    int fmt_result;

    ELOG_ASSERT(level <= ELOG_LVL_VERBOSE);

    /* check output enabled */
    if (!elog.output_enabled) {
        return;
    }

    /* level filter */
    if (level > elog.filter.level) {
        return;
    } else if (!strstr(tag, elog.filter.tag)) { /* tag filter */
        //TODO 可以考虑采用KMP及朴素模式匹配字符串，提升性能
        return;
    }

    /* lock output */
    elog_port_req_grant(elog_sobj);
    /* package level info */
    if (elog.enabled_fmt_set[level]&(ELOG_FMT_LVL)) {
        log_len += elog_strcpy(log_len, log_buf + log_len, level_output_info[level]);
    }
    /* package tag info */
    if (elog.enabled_fmt_set[level]&(ELOG_FMT_TAG)) {
        log_len += elog_strcpy(log_len, log_buf + log_len, tag);
        /* if the tag length is less than 50% ELOG_FILTER_TAG_MAX_LEN, then fill space */
        if (tag_len <= ELOG_FILTER_TAG_MAX_LEN / 2) {
            memset(tag_sapce, ' ', ELOG_FILTER_TAG_MAX_LEN / 2 - tag_len);
            log_len += elog_strcpy(log_len, log_buf + log_len, tag_sapce);
        }
//        log_len += elog_strcpy(log_len, log_buf + log_len, " ");              /* be deleted at 2015-06-04 17:09 by chxaitz */
        log_buf[log_len++]=' ';                                                 /* be added   at 2015-06-04 17:09 by chxaitz */
    }
    /* package time, process and thread info */
    if (elog.enabled_fmt_set[level]&(ELOG_FMT_TIME|ELOG_FMT_P_INFO|ELOG_FMT_T_INFO)) {
//        log_len += elog_strcpy(log_len, log_buf + log_len, "[");              /* be deleted at 2015-06-04 17:09 by chxaitz */
        log_buf[log_len++]='[';                                                 /* be added   at 2015-06-04 17:09 by chxaitz */
        /* package time info */
        if (elog.enabled_fmt_set[level]&(ELOG_FMT_TIME)) {
            log_len += elog_strcpy(log_len, log_buf + log_len, elog_port_get_time());
            if (elog.enabled_fmt_set[level]&(ELOG_FMT_P_INFO|ELOG_FMT_T_INFO)) {
//                log_len += elog_strcpy(log_len, log_buf + log_len, " ");      /* be deleted at 2015-06-04 17:09 by chxaitz */
                log_buf[log_len++]=' ';                                         /* be added   at 2015-06-04 17:09 by chxaitz */
            }
        }
        /* package process info */
        if (elog.enabled_fmt_set[level]&(ELOG_FMT_P_INFO)) {
            log_len += elog_strcpy(log_len, log_buf + log_len, elog_port_get_p_info());
            if (elog.enabled_fmt_set[level]&(ELOG_FMT_T_INFO)) {
//                log_len += elog_strcpy(log_len, log_buf + log_len, " ");      /* be deleted at 2015-06-04 17:09 by chxaitz */
              log_buf[log_len++]=' ';
            }
        }
        /* package thread info */
        if (elog.enabled_fmt_set[level]&(ELOG_FMT_T_INFO)) {
            log_len += elog_strcpy(log_len, log_buf + log_len, elog_port_get_t_info());
        }
//        log_len += elog_strcpy(log_len, log_buf + log_len, "] ");             /* be deleted at 2015-06-04 17:09 by chxaitz */
        log_buf[log_len++]=']';log_buf[log_len++]=' ';                          /* be added   at 2015-06-04 17:09 by chxaitz */
    }
    /* package file directory and name, function name and line number info */
    if (elog.enabled_fmt_set[level]&(ELOG_FMT_DIR|ELOG_FMT_FUNC|ELOG_FMT_LINE)) {
//        log_len += elog_strcpy(log_len, log_buf + log_len, "(");              /* be deleted at 2015-06-04 17:09 by chxaitz */
        log_buf[log_len++]='(';                                                 /* be added   at 2015-06-04 17:09 by chxaitz */
        /* package time info */
        if (elog.enabled_fmt_set[level]&(ELOG_FMT_DIR)) {
            log_len += elog_strcpy(log_len, log_buf + log_len, file);
            if (elog.enabled_fmt_set[level]&(ELOG_FMT_FUNC)) {
//                log_len += elog_strcpy(log_len, log_buf + log_len, " ");      /* be deleted at 2015-06-04 17:09 by chxaitz */
                log_buf[log_len++]=' ';                                         /* be added   at 2015-06-04 17:09 by chxaitz */
            } else if (elog.enabled_fmt_set[level]&(ELOG_FMT_LINE)) {
//                log_len += elog_strcpy(log_len, log_buf + log_len, ":");      /* be deleted at 2015-06-04 17:09 by chxaitz */
              log_buf[log_len++]=':';                                           /* be added   at 2015-06-04 17:09 by chxaitz */
            }
        }
        /* package process info */
        if (elog.enabled_fmt_set[level]&(ELOG_FMT_FUNC)) {
            log_len += elog_strcpy(log_len, log_buf + log_len, func);
            if (elog.enabled_fmt_set[level]&(ELOG_FMT_LINE)) {
//                log_len += elog_strcpy(log_len, log_buf + log_len, ":");      /* be deleted at 2015-06-04 17:09 by chxaitz */
              log_buf[log_len++]=':';
            }
        }
        /* package thread info */
        if (elog.enabled_fmt_set[level]&(ELOG_FMT_LINE)) {
            //TODO snprintf资源占用可能较高，待优化
//            snprintf(line_num, ELOG_LINE_NUM_MAX_LEN, "%ld", line);           /* be deleted at 2015-06-04 17:09 by chxaitz */
//            log_len += elog_strcpy(log_len, log_buf + log_len, line_num);     /* be deleted at 2015-06-04 17:09 by chxaitz */
          log_len += elog_u32_to_str(log_buf + log_len, line);                  /* be added   at 2015-06-04 17:09 by chxaitz */
        }
//        log_len += elog_strcpy(log_len, log_buf + log_len, ")");              /* be deleted at 2015-06-04 17:09 by chxaitz */
        log_buf[log_len++]=')';                                                 /* be added   at 2015-06-04 17:09 by chxaitz */
    }

    /* add space and colon sign */
    if (log_len != 0) {
//        log_len += elog_strcpy(log_len, log_buf + log_len, ": ");             /* be deleted at 2015-06-04 17:09 by chxaitz */
        log_buf[log_len++]=':';log_buf[log_len++]=' ';                          /* be added   at 2015-06-04 17:09 by chxaitz */
    }

    /* args point to the first variable parameter */
    va_start(args, format);
    
    /* package other log data to buffer. CRLF length is 2. '\0' must be added in the end by vsnprintf. */
    fmt_result = vsnprintf(log_buf + log_len, ELOG_BUF_SIZE - log_len - 2 + 1, format, args);

    va_end(args);

    /* package CRLF */
    if ((fmt_result > -1) && (fmt_result + log_len + 2 <= ELOG_BUF_SIZE)) {
        log_len += fmt_result;
//        log_len += elog_strcpy(log_len, log_buf + log_len, "\r\n");           /* be deleted at 2015-06-04 17:09 by chxaitz */
        log_buf[log_len++]='\r';log_buf[log_len++]='\n';                        /* be added   at 2015-06-04 17:09 by chxaitz */
        log_buf[log_len] = '\0';

    } else {
        log_buf[ELOG_BUF_SIZE - 3] = '\r';
        log_buf[ELOG_BUF_SIZE - 2] = '\n';
        log_buf[ELOG_BUF_SIZE - 1] = '\0';
    }

    /* output log */
//    elog_port_output(log_buf, log_len);                                       /* be deleted at 2015-06-04 17:09 by chxaitz */
    
    /* add log to log_nodes list */                                             /* be added   at 2015-06-04 17:09 by chxaitz */
    elog_node *px_eln;
    if((px_eln = (elog_node*)_elog_malloc( sizeof(elog)+log_len ))!=NULL)
    {
      /* init the node */
      px_eln->pNext = NULL;
      px_eln->len   = log_len;
      memcpy( px_eln->Data, log_buf, log_len );
      
      /* add to the logs' list */
      _elog_enter_critical();
      if( elog_list_head == NULL )       //empty list
      {
        elog_list_head = px_eln;
        elog_list_tail = px_eln;
      }
      else/* unempty list */
      {
        elog_list_tail->pNext = px_eln;
        elog_list_tail        = px_eln;
      }
      _elog_exit_critical();
      /* update some data */
      elog_nodes_count++;
      elog_take_sapce+=log_len;
    }

    /* unlock output */
    elog_port_rel_grant(elog_sobj);
}
/* be added   at 2015-06-04 17:09 by chxaitz */
/**
 * used to get the space logs takes current
 */
size_t elog_get_take_mem()
{
  return elog_take_sapce;
}
/* be added   at 2015-06-04 17:09 by chxaitz */
/**
 * used to get the logs' num current
 */
size_t elog_get_logs_num()
{
  return elog_nodes_count;
}
/* be added   at 2015-06-04 17:09 by chxaitz */
/**
 * used to output the logs
 */
void elog_output()
{
  if( ELOG_NO_ERR == elog_port_open() )
  {
    void *p;
    while( elog_list_head )
    {
      if( elog.filter.keyword[0] )
      { /* keyword filter */
        if( elog_kw_strstr(elog_list_head->Data, elog.filter.keyword, pc_elog_kw_alpha) != -1 )
          elog_port_write( elog_list_head->Data, elog_list_head->len );
      }
      else
        elog_port_write( elog_list_head->Data, elog_list_head->len );
      p = elog_list_head;
      _elog_enter_critical();
      /* update some data */
      elog_nodes_count--;
      elog_take_sapce-=elog_list_head->len;
      /* update logs' list */
      elog_list_head = elog_list_head->pNext;
      if( NULL == elog_list_head ) elog_list_tail = NULL;
      _elog_exit_critical();
      _elog_free( p );
    }
    elog_port_close();
  }
  else  /* if failed to open the device , free all logs taked space */
  {
    void *p;
    while( elog_list_head )
    {
      p = elog_list_head;
      _elog_enter_critical();
      /* update some data */
      elog_nodes_count--;
      elog_take_sapce-=elog_list_head->len;
      /* update logs' list */
      elog_list_head = elog_list_head->pNext;
      if( NULL == elog_list_head ) elog_list_tail = NULL;
      _elog_exit_critical();
      _elog_free( p );
    }
  }
}
