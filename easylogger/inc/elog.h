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
 * Function: Is is an head file for this library. You can see all be called functions.
 * Created on: 2015-04-28
 */

#ifndef __ELOG_H__
#define __ELOG_H__

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "coll_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* output log's level */
#define ELOG_LVL_ASSERT                      0
#define ELOG_LVL_ERROR                       1
#define ELOG_LVL_WARN                        2
#define ELOG_LVL_INFO                        3
#define ELOG_LVL_DEBUG                       4
#define ELOG_LVL_VERBOSE                     5
/* setting static output log level. default is verbose */
#define ELOG_OUTPUT_LVL                      ELOG_LVL_VERBOSE
/* enable log output. default open this macro */
#define ELOG_OUTPUT_ENABLE
/* using output to file mode */
#define ELOG_USING_OUTPUT_FILE
/* output line number max length */
#define ELOG_LINE_NUM_MAX_LEN                5
/* log buffer size */
#define ELOG_BUF_SIZE                        256
#define ELOG_BUF_SIZE_BASE                   128
/* output filter's tag max length */
#define ELOG_FILTER_TAG_MAX_LEN              16
/* output filter's keyword max length */
#define ELOG_FILTER_KW_MAX_LEN               16
/* keyword alpha len max */
#define ELOG_TG_ALPHA_MAX_LEN                96 /* do not exchange this two define*/
#define ELOG_KW_ALPHA_MAX_LEN                96
#define ELOG_KW_ALPHA_BASE                   32
/* EasyLogger software version number */
#define ELOG_SW_VERSION                      "0.05.25"

/* EasyLogger assert for developer. */
#define ELOG_ASSERT(EXPR)                                                   \
if (!(EXPR))                                                                \
{                                                                           \
    elog_a("ELOG", "(%s) has assert failed at %s.\n", #EXPR, __FUNCTION__); \
    while (1);                                                              \
}

/* all formats index */
typedef enum {
    ELOG_FMT_LVL    = 1 << 0, /**< level */
    ELOG_FMT_TAG    = 1 << 1, /**< tag */
    ELOG_FMT_TIME   = 1 << 2, /**< current time */
    ELOG_FMT_P_INFO = 1 << 3, /**< process info */
    ELOG_FMT_T_INFO = 1 << 4, /**< thread info */
    ELOG_FMT_DIR    = 1 << 5, /**< file directory and name */
    ELOG_FMT_FUNC   = 1 << 6, /**< function name */
    ELOG_FMT_LINE   = 1 << 7, /**< line number */
} ElogFmtIndex;

/* EasyLogger error code */
typedef enum {
    ELOG_NO_ERR,
    ELOG_ERR,
} ElogErrCode;

/* elog.c */
ElogErrCode elog_init(void);
void elog_set_output_enabled(bool enabled);
bool elog_get_output_enabled(void);
void elog_set_fmt(uint8_t level, uint8_t set);
void elog_set_filter(uint8_t level, const char *tag, const char *keyword);
void elog_set_filter_lvl(uint8_t level);
void elog_set_filter_tag(const char *tag);
void elog_set_filter_kw(const char *keyword);
void elog_raw(const char *format, ...);
void elog_add(uint8_t level, const char *tag, const char *file, const char *func,
        const long line, const char *format, ...);

void        elog_output();      /* used to output the logs */
size_t elog_get_take_mem();     /* used to get the space logs takes current */
size_t elog_get_logs_num();     /* used to get the logs' num current */

#ifndef ELOG_OUTPUT_ENABLE

#define elog_a(tag, ...)
#define elog_e(tag, ...)
#define elog_w(tag, ...)
#define elog_i(tag, ...)
#define elog_d(tag, ...)
#define elog_v(tag, ...)

#else /* ELOG_OUTPUT_ENABLE */

#if ELOG_OUTPUT_LVL >= ELOG_LVL_ASSERT
#define elog_a(tag, ...) \
        elog_add(ELOG_LVL_ASSERT, tag, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
#else
#define elog_a(tag, ...)
#endif

#if ELOG_OUTPUT_LVL >= ELOG_LVL_ERROR
#define elog_e(tag, ...) \
        elog_add(ELOG_LVL_ERROR, tag, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
#else
#define elog_e(tag, ...)
#endif

#if ELOG_OUTPUT_LVL >= ELOG_LVL_WARN
#define elog_w(tag, ...) \
        elog_add(ELOG_LVL_WARN, tag, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
#else
#define elog_w(tag, ...)
#endif

#if ELOG_OUTPUT_LVL >= ELOG_LVL_INFO
#define elog_i(tag, ...) \
        elog_add(ELOG_LVL_INFO, tag, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
#else
#define elog_i(tag, ...)
#endif

#if ELOG_OUTPUT_LVL >= ELOG_LVL_DEBUG
#define elog_d(tag, ...) \
        elog_add(ELOG_LVL_DEBUG, tag, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
#else
#define elog_d(tag, ...)
#endif

#if ELOG_OUTPUT_LVL == ELOG_LVL_VERBOSE
#define elog_v(tag, ...) \
        elog_add(ELOG_LVL_VERBOSE, tag, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
#endif

#endif /* ELOG_OUTPUT_ENABLE */

/* elog_utils.c */
size_t elog_strcpy(size_t cur_len, char *dst, const char *src);
size_t elog_u32_to_str(char *buf, size_t val);
int elog_kw_alpha_set( char *src, const char *des );
int elog_kw_strstr(const char *src, const char *des, const char *alpha);

/* elog_port.c */
#define	_ELOG_SYNC_t			      xSemaphoreHandle
ElogErrCode elog_port_init(void);
void elog_port_output(const char *output, size_t size);
const char *elog_port_get_time(void);
const char *elog_port_get_p_info(void);
const char *elog_port_get_t_info(void);
int  elog_port_cre_syncobj (_ELOG_SYNC_t* sobj);	                        /* Create a sync object */
int  elog_port_req_grant   (_ELOG_SYNC_t sobj);				        /* Lock sync object */
void elog_port_rel_grant   (_ELOG_SYNC_t sobj);				        /* Unlock sync object */
int  elog_port_del_syncobj (_ELOG_SYNC_t sobj);				        /* Delete a sync object */

ElogErrCode elog_port_open();
ElogErrCode elog_port_close();
ElogErrCode elog_port_write( char *buf, size_t len );

#define _elog_malloc(a)         pvPortMalloc(a)
#define _elog_free(a)           vPortFree(a)
#define _elog_enter_critical()  taskENTER_CRITICAL()
#define _elog_exit_critical()   taskEXIT_CRITICAL()

#ifdef __cplusplus
}
#endif

#endif /* __ELOG_H__ */
