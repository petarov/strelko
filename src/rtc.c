/*
 * Strelko is a small-footprint HTTP server.
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

#include "globals.h"
#include "logger.h"
#include "rtc.h"

status_code_t rtc_create(runtime_context_t **rtc) {
	TRACE;

	runtime_context_t *runtime = (runtime_context_t *) malloc(sizeof(runtime_context_t));
	ASSERT(runtime != NULL);
	if (!runtime) {
		log_crit("Failed to allocate memory for runtime context!");
		return SC_FAILED;
	}

	memset(runtime, 0, sizeof(runtime_context_t));
	apr_pool_create(&(runtime->mem_pool), NULL);

	*rtc = runtime;

	return SC_OK;
}

void rtc_destroy(runtime_context_t **rtc) {
	TRACE;

	runtime_context_t *runtime = *rtc;
	ASSERT(runtime != NULL);

	if (runtime != NULL) {
		if (runtime->mem_pool)
			apr_pool_destroy(runtime->mem_pool);

		free(runtime);
		runtime = NULL;
	}
}
