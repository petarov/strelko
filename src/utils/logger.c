/*
  logger.c
  This file is part of e-additives.server

  Copyright (C) 2012 necroleak

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

#include <stdio.h>
#include <stdarg.h>
#include "logger.h"

logger_output_types_e local_log_type;


int log_init_ex(const char *application_name, logger_output_types_e log_type) {

	if ((log_type & L_SYSLOG) == L_SYSLOG) {

		setlogmask(LOG_UPTO (LOG_DEBUG));

		// open sys log with defaults
		openlog(application_name, LOG_CONS | LOG_PID | LOG_NDELAY | LOG_PERROR, LOG_LOCAL1);

	} else {
		fprintf(stderr, "Invalid logger type %d !", log_type);
		return -1;
	}

	local_log_type = log_type;

	return 0;
}

void log_write(int level, char *format, ...) {
	va_list ap;
	va_start(ap, format);
	char buf[256];

	vsnprintf(buf, 256, format, ap);
	syslog(level, buf);
	va_end(ap);

}

void log_close() {
	if ((local_log_type & L_SYSLOG) == L_SYSLOG) {
		closelog();
	}
}
