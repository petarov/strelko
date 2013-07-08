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
#include "conf_parser.h"
#include "http.h"

status_code_t http_create(http_request_t **http, apr_pool_t *parent_pool) {
	TRACE;

	http_request_t *ht = (http_request_t *)apr_palloc(parent_pool,
			sizeof(http_request_t));
	apr_pool_create(&(ht->mem_pool), parent_pool);

	*http = ht;

	return SC_OK;
}

void http_destory(http_request_t *http) {
	TRACE;

	if (http) {
		apr_pool_destroy(http->mem_pool);
	}
}

status_code_t http_prase_uri(http_request_t *req, char *uri,
		runtime_context_t *rtc) {

	//

	return SC_OK;
}


