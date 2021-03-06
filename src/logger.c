/*
 * Strelko is a small footprint HTTP server.
 * Copyright (C) 2013 Petar Petrov
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
*/

#include <stdio.h>
#include <stdarg.h>
#include "logger.h"

logger_output_types_e local_log_type = L_NONE;


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
	if (local_log_type == L_NONE)
		return;

	va_list ap;
	va_start(ap, format);
	char buf[256];

	if ((local_log_type & L_SYSLOG) == L_SYSLOG) {
		vsnprintf(buf, 256, format, ap);
		syslog(level, buf);
	}

	va_end(ap);

}

void log_close() {
	if ((local_log_type & L_SYSLOG) == L_SYSLOG) {
		closelog();
	}
}
