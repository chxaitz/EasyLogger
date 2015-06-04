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
 * Function: Portable interface for RT-Thread.
 * Created on: 2015-04-28
 */

#include "elog.h"
#include "time.h"
#include "usart.h"
#include "rtc.h"

/**
 * EasyLogger port initialize
 *
 * @return result
 */
ElogErrCode elog_port_init(void) {
    ElogErrCode result = ELOG_NO_ERR;

    return result;
}

/**
 * output log port interface
 */
void elog_port_output(const char *output, size_t size) {
    /* output to terminal */
    usart_write( 0, output, size);
    //TODO output to flash
}

/**
 * output lock
 */
void elog_port_output_lock(void) {
}

/**
 * output unlock
 */
void elog_port_output_unlock(void) {
}
/* be modified   at 2015-06-04 17:09 by chxaitz */
/**
 * get current time interface
 *
 * @return current time
 */
const char *elog_port_get_time(void) {
  time_t tm_cur;
  struct tm *px_tm;
  static char cur_system_time[21] = { 0 };
  tm_cur = rtc_get_time_t();
  px_tm = localtime( &tm_cur );
  sprintf(cur_system_time,"%4d-%02d-%02d %02d:%02d:%02d", (1900+px_tm->tm_year), (1+px_tm->tm_mon), px_tm->tm_mday, px_tm->tm_hour, px_tm->tm_min, px_tm->tm_sec);
  return cur_system_time;
}

/**
 * get current process name interface
 *
 * @return current process name
 */
const char *elog_port_get_p_info(void) {
    return "";
}

/**
 * get current thread name interface
 */
const char *elog_port_get_t_info(void) {
    return "";
}
/* be added   at 2015-06-04 17:09 by chxaitz */
/**
 * open then output stream device
 */
ElogErrCode elog_port_open()
{
  ElogErrCode res = ELOG_NO_ERR;
  return res;
}
/* be added   at 2015-06-04 17:09 by chxaitz */
/**
 * write data to the stream device
 */
ElogErrCode elog_port_write( char *buf, size_t len )
{
  ElogErrCode res = ELOG_NO_ERR;
  usart_write( 0, buf, len );
  return res;
}
/* be added   at 2015-06-04 17:09 by chxaitz */
/**
 * close then output stream device
 */
ElogErrCode elog_port_close()
{
  ElogErrCode res = ELOG_NO_ERR;
  return res;
}
