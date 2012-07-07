/*
  config.c
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
#include "utils/helpers.h"
#include "utils/logger.h"
#include "conf_options.h"

static const config_option_t const *config_options = NULL;

int cfg_init(const config_option_t const *configs) {
	ASSERT(configs != NULL);
	config_options = configs;
	return TRUE;
}

int cfg_is_valid(const char *key, const char *value) {
	TRACE;
	ASSERT(key != NULL);
	ASSERT(value != NULL);
	for(const config_option_t *options = config_options; options != NULL; options++ ) {
		if (!apr_strnatcmp(options->key, key)) {
			// now check if the value is of the expected type
			switch(options->type) {
			case CT_INT:
				return hlp_isnum(value, MAX_OPTION_VALUE_SIZE);
				break;
			case CT_STRING:
				return hlp_isstr(value, MAX_OPTION_VALUE_SIZE);
			case CT_BOOL:
				return hlp_isbool(value);
			default:
				return FALSE; // we don't need any surprises
			}
		}
	}
	return FALSE;
}

int cfg_is_optional(const char* key) {
	TRACE;
	ASSERT(key != NULL);
	for(const config_option_t *options = config_options; options != NULL; options++ ) {
		if (!options->optional)
			return FALSE;
	}
	return TRUE;
}

const config_option_t* cfg_get_option(const char *key) {
	TRACE;
	ASSERT(key != NULL);
	for(const config_option_t *options = config_options; options != NULL; options++ ) {
		if (!apr_strnatcmp(options->key, key))
			return options;
	}
	return NULL;
}
