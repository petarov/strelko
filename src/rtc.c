/*
  rtc.c
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

#include "globals.h"
#include "utils/logger.h"
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
	if (runtime->mem_pool)
		apr_pool_destroy(runtime->mem_pool);

	free(runtime);
}
