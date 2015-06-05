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
 * Function: Some utils for this library.
 * Created on: 2015-04-28
 */

#include "elog.h"
#include <string.h>

/**
 * another copy string function
 *
 * @param cur_len current copied log length, max size is ELOG_BUF_SIZE
 * @param dst destination
 * @param src source
 *
 * @return copied length
 */
size_t elog_strcpy(size_t cur_len, char *dst, const char *src) {
    const char *src_old = src;
    while (*src != 0) {
        /* make sure destination has enough space */
        if (cur_len++ <= ELOG_BUF_SIZE) {
            *dst++ = *src++;
        } else {
            break;
        }
    }
    return src - src_old;
}
/* be added   at 2015-06-04 17:09 by chxaitz */
/**
 * u32 to string
 *
 * @param buf the log str buffer
 * @param val the val to be trans
 *
 * @return copied length
 */
size_t elog_u32_to_str(char *buf, size_t val)  
{  
  uint8 len,pos;
  const size_t com[]={0,10,100,1000,10000};
  len = 1;
  while( val>com[len] && len < 5)       len++;
  pos = len;
  while(pos){ buf[--pos] = (val%10)+'0';val /= 10;}
  return len;
}
/* be added   at 2015-06-04 17:09 by chxaitz */
/* it will be fault when have unshowed ascii */
/**
 * the alpha calculater func
 *
 * @param src   the alpha str buffer
 * @param des   the key  str buffer
 *
 * @return copied length
 */
int elog_kw_alpha_set( char *src,  const char *des )
{
  int i = 0;
  int len_d;

  if(src == NULL || des == NULL) return -1;
  len_d = strlen(des);
  for(i = 0; i < ELOG_KW_ALPHA_MAX_LEN; i++) src[i] = len_d;
  for(i = 0; i < len_d; i++) src[des[i]-ELOG_KW_ALOHA_BASE] = len_d-i-1;
  
  return 0;
}
/* be added   at 2015-06-04 17:09 by chxaitz */
/**
 * an string compare function
 *
 * @param src   the src  str buffer
 * @param des   the key  str buffer
 * @param alpha the alpha str buffer
 *
 * @return copied length
 */
int elog_kw_strstr(const char *src, const char *des, const char *alpha)
{
  int i, pos = 0;
  int len, len_d;

  if(src == NULL || des == NULL || alpha == NULL) return -1;
  len = strlen(src);
  len_d = strlen(des);
  len = len - len_d;
  for(pos = 1; pos <= len; ) {
    for(i = pos - 1; i - pos + 1 < len_d; i++) {
        if(src[i] != des[i - pos + 1]) 
            break;
    }
    
    if((i - pos + 1) == len_d) return pos;
    else pos += alpha[src[pos + len_d - 1]-ELOG_KW_ALOHA_BASE] + 1;
  }
  return -1;
}

/*
#define _elog_set_sptr(x)       {x->string = x->data + 1;}
#define _elog_set_null(x)       {*(x->data) = '\0';*(x->data + x->data_len - 1) = '\0';}
void* _elog_str_resize(cstr_t *ts, cstr_len_t size)
{
  void *p;
  if ((p = _elog_malloc(size)) != NULL) //apply success
  {
    void *ptr = ts->data;
    memcpy( p, ptr, size+2 );
    _elog_free( ptr );
    
    ts->data_len = size + 2;
    ts->data = p;
    _elog_set_sptr(ts);
    _elog_set_null(ts);
    return ts;
  }
  else
    return NULL;
}
*/
