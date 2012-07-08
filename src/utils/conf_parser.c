/*
  conf_parser.c
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
#include "logger.h"
#include "helpers.h"
#include "rtc.h"
#include "conf_parser.h"

#include <apr_lib.h>

static const conf_optinfo_t const *config_options = NULL;


static int s_iscomment(char *line) {
	return *line == '#' || *line == '/';
}

static int s_isvalid(char *line) {
	char *p = line;

//	// most likely a comment ?
//	if (*p == '#')
//		return FALSE;

	// is it in range [A-Za-z0-9_]
	while (*p != '\0') {
		if (!apr_isalnum(*p) && *p != '_')
			return FALSE;
		p++;
	}
	return TRUE;
}

int conf_init(const conf_optinfo_t const *configs) {
	ASSERT(configs != NULL);
	config_options = configs;
	return TRUE;
}

conf_opt_t* conf_parse(const char *filename, runtime_context_t *rtc) {
	TRACE;
	ASSERT(rtc != NULL);

	conf_opt_t *cfile 		= NULL;
	apr_file_t *apr_file 	= NULL;
	apr_status_t rv			= APR_SUCCESS;

	rv = apr_file_open(&apr_file, filename, APR_FOPEN_READ | APR_FOPEN_BUFFERED,
			APR_FPROT_OS_DEFAULT, rtc->mem_pool);

	if (rv == APR_SUCCESS) {
		char line[CONF_MAX_LINE_SIZE];
		while(APR_SUCCESS == apr_file_gets(line, CONF_MAX_LINE_SIZE, apr_file)) {

			// remove stupid white spaces :)
			apr_collapse_spaces(line, line);

			if (!s_iscomment(line)) {
				strtokens_t *tokens = hlp_strsplit(line, CONF_OPT_SEPARATOR, rtc->mem_pool);
				if (tokens->size > 1 && conf_is_opt_valid(tokens->token[0]) ) {
					char *key = tokens->token[0];
					char *value = tokens->token[1];

					conf_opt_t *opt = conf_opt_parse(key, value, rtc);
					if (opt != NULL) {
						log_info("Parsed option (%s)", key);
					} else {
						log_err("Failed to parse option (%s) !", key);
						//TODO err

					}
				} else {
					log_err("Invalid option line (%s) !", line);
					//TODO err
				}
			}
		}
		apr_file_close(apr_file);
	} else {
		char buf[512];
		apr_strerror(rv, buf, sizeof(buf));
		log_err("Failed to load %s conf file! Error: %s", filename, buf);
	}

	return cfile;
}

conf_opt_t* conf_opt_parse(const char *key, const char *value, runtime_context_t *rtc) {
	TRACE;
	ASSERT(key != NULL);
	ASSERT(value != NULL);
	for(const conf_optinfo_t *options = config_options; options->key != NULL; options++ ) {
		if (!apr_strnatcmp(options->key, key)) {
			conf_opt_t *opt = NULL;

			// now check if the value is of the expected type
			switch(options->type) {
			case CT_INT:
				if (hlp_isnum(value, MAX_OPTION_VALUE_SIZE)) {
					opt = (conf_opt_t *)apr_pcalloc(rtc->mem_pool, sizeof(conf_opt_t));
					opt->key = key;
					opt->int_value = strtol(value, (char **)NULL, 10);
				}
				break;
			case CT_STRING:
				if (hlp_isstr(value, MAX_OPTION_VALUE_SIZE)) {
					opt = (conf_opt_t *)apr_pcalloc(rtc->mem_pool, sizeof(conf_opt_t));
					opt->key = key;
					opt->str_value = apr_pstrdup(rtc->mem_pool, value);
				}
				break;
			case CT_BOOL:
				if (hlp_isbool(value)) {
					opt = (conf_opt_t *)apr_pcalloc(rtc->mem_pool, sizeof(conf_opt_t));
					opt->key = key;
					opt->bool_value = hlp_tobool(value);
				}
				break;
			}
			return opt;
		}
	}
	// unknown option
	return NULL;
}

int conf_is_opt_valid(const char *key/*, const char *value*/) {
	TRACE;
	ASSERT(key != NULL);
//	ASSERT(value != NULL);

	if (!s_isvalid(key))
		return FALSE;

	for(const conf_optinfo_t *options = config_options; options->key != NULL; options++ ) {
		if (!apr_strnatcmp(options->key, key)) {
			return TRUE;
//			// now check if the value is of the expected type
//			switch(options->type) {
//			case CT_INT:
//				return hlp_isnum(value, MAX_OPTION_VALUE_SIZE);
//				break;
//			case CT_STRING:
//				return hlp_isstr(value, MAX_OPTION_VALUE_SIZE);
//			case CT_BOOL:
//				return hlp_isbool(value);
//			default:
//				return FALSE; // we don't need any surprises
//			}
		}
	}
	return FALSE;
}

int conf_is_opt_optional(const char* key) {
	TRACE;
	ASSERT(key != NULL);
	for(const conf_optinfo_t *options = config_options; options->key != NULL; options++ ) {
		if (!options->optional)
			return FALSE;
	}
	return TRUE;
}

//const conf_optinfo_t* conf_get_optinfo(const char *key) {
//	TRACE;
//	ASSERT(key != NULL);
//	for(const conf_optinfo_t *options = config_options; options->key != NULL; options++ ) {
//		if (!apr_strnatcmp(options->key, key))
//			return options;
//	}
//	return NULL;
//}


