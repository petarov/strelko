/*
  rtc.h
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

#ifndef RTC_H_
#define RTC_H_

#include "utils/conf_options.h"

/**
 * Runtime context that encapsulates all global parameters
 */
struct runtime_context_t {
	char appname[64];
	apr_pool_t *mem_pool;
	const config_option_t const *cfg;
};
typedef struct runtime_context_t runtime_context_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Allocates new runtime configuration context
 * @param rtc Ptr to memory ptr of the runtime context
 * @return TRUE if context was successfully created, FALSE otherwise
 */
int rtc_create(runtime_context_t **rtc);

/**
 * Deallocates and cleans up existing runtime context
 * @param rtc Ptr to memory ptr of the runtime context
 */
void rtc_destroy(runtime_context_t **rtc);

#ifdef __cplusplus
}
#endif

#endif /* RTC_H_ */
