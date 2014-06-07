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

#ifndef RTC_H_
#define RTC_H_

#include <apr_hash.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Runtime context that encapsulates all global parameters
 */
struct runtime_context_t {
	char appname[64];
	apr_pool_t *mem_pool;
	apr_hash_t *options;
};
typedef struct runtime_context_t runtime_context_t;


/**
 * Allocates new runtime configuration context
 * @param rtc Ptr to memory ptr of the runtime context
 * @return SC_OK if context was successfully created, SC_FAILED otherwise
 */
status_code_t rtc_create(runtime_context_t **rtc);

/**
 * Deallocates and cleans up existing runtime context
 * @param rtc Ptr to memory ptr of the runtime context
 */
void rtc_destroy(runtime_context_t **rtc);

#ifdef __cplusplus
}
#endif

#endif /* RTC_H_ */
