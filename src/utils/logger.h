/*
  logger.h
  This file is part of e-additives.server

  Copyright (C) 2012 Petar Petrov

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

  For more information please contact:
  petar.petrov.georgiev(at)gmail[dot]com

*/

#ifndef LOGGER_H_
#define LOGGER_H_

#include <syslog.h>

enum logger_output_types_e {
	L_NONE = 0,
	L_SYSLOG = 0x00000001
};
typedef enum logger_output_types_e logger_output_types_e;

/**
 * Initilize logging system
 * @param application_name Name of application that logs the message. If NULL is passed, the executable name from
 * argv[0] will be used.
 * @param log_type Type of logging output.
 * @remark Currently only SYSLOG logging supported.
 */
int log_init_ex(const char *application_name, logger_output_types_e log_type);

/**
 * Default logger initialization uses Unix syslog
 */
#define log_init(name) log_init_ex(name, L_SYSLOG)

/**
 * @param level
 * @format
 */
void log_write(int level, char *format, ...);

#define log_debug(...) log_write(LOG_DEBUG, __VA_ARGS__)  /* debug-level messages */
#define log_info(...) log_write(LOG_INFO, __VA_ARGS__)  /* informational */
#define log_notice(...) log_write(LOG_NOTICE, __VA_ARGS__)  /* normal but significant condition */
#define log_warn(...) log_write(LOG_WARNING, __VA_ARGS__)  /* warning conditions */
#define log_err(...) log_write(LOG_ERR, __VA_ARGS__)  /* error conditions */
#define log_crit(...) log_write(LOG_CRIT, __VA_ARGS__)  /* critical conditions */

#ifdef DEBUG
#define TRACE log_debug("%s %d", __FILE__, __LINE__)
#else
#define TRACE
#endif

/**
 * Cleanup logging resources
 */
void log_close();


#endif /* LOGGER_H_ */
