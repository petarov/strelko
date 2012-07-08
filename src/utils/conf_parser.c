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

static const conf_optinfo_t const *configs_template = NULL;

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

/**
 * Check if config template option is optional
 * @param key
 * @return Returns TRUE if the options is unknown or optional, FALSE otherwise
 */
int s_is_opt_optional(const char* key) {
	TRACE;
	ASSERT(key != NULL);
	for(const conf_optinfo_t *options = configs_template; options->key != NULL; options++ ) {
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

/**
 * Parse configuration option
 * @param key
 * @param value
 * @param rtc Initialized runtime context
 * @return Ptr to parsed configuration option, NULL on error
 */
conf_opt_t* s_opt_parse(const char *key, const char *value, runtime_context_t *rtc) {
	TRACE;
	ASSERT(key != NULL);
	ASSERT(value != NULL);
	for(const conf_optinfo_t *options = configs_template; options->key != NULL; options++ ) {
		if (!apr_strnatcmp(options->key, key)) {
			conf_opt_t *opt = NULL;

			// now check if the value is of the expected type
			switch(options->type) {
			case CT_INT:
				if (hlp_isnum(value, MAX_OPTION_VALUE_SIZE)) {
					opt = (conf_opt_t *)apr_pcalloc(rtc->mem_pool, sizeof(conf_opt_t));
					opt->key = key;
					opt->int_val = strtol(value, (char **)NULL, 10);
				}
				break;
			case CT_STRING:
				if (hlp_isstr(value, MAX_OPTION_VALUE_SIZE)) {
					opt = (conf_opt_t *)apr_pcalloc(rtc->mem_pool, sizeof(conf_opt_t));
					opt->key = key;
					opt->str_val = apr_pstrdup(rtc->mem_pool, value);
				}
				break;
			case CT_BOOL:
				if (hlp_isbool(value)) {
					opt = (conf_opt_t *)apr_pcalloc(rtc->mem_pool, sizeof(conf_opt_t));
					opt->key = key;
					opt->bool_val = hlp_tobool(value);
				}
				break;
			}
			return opt;
		}
	}
	// unknown option
	return NULL;
}

/**
 * Check if this option is within the list of defined.
 * @param key
 * @param value
 * @return Returns TRUE if the option is known, FALSE otherwise
 * @remark Option name check is case sensitive.
 */
int s_is_opt_valid(const char *key/*, const char *value*/) {
	TRACE;
	ASSERT(key != NULL);
//	ASSERT(value != NULL);

	if (!s_isvalid(key))
		return FALSE;

	for(const conf_optinfo_t *options = configs_template; options->key != NULL; options++ ) {
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


int conf_init(const conf_optinfo_t const *configs) {
	ASSERT(configs != NULL);
	configs_template = configs;
	return TRUE;
}

int conf_parse(const char *filename, runtime_context_t *rtc) {
	TRACE;
	ASSERT(rtc != NULL);
	apr_file_t *apr_file 	= NULL;
	int	success				= FALSE;

	apr_status_t rv = apr_file_open(&apr_file, filename, APR_FOPEN_READ | APR_FOPEN_BUFFERED,
			APR_FPROT_OS_DEFAULT, rtc->mem_pool);
	if (rv == APR_SUCCESS) {
		char line[CONF_MAX_LINE_SIZE];
		rtc->options = apr_hash_make(rtc->mem_pool);
		success = TRUE;

		while(APR_SUCCESS == apr_file_gets(line, CONF_MAX_LINE_SIZE, apr_file)) {

			if (hlp_isblank(line))
				continue;

			// remove stupid white spaces :)
			apr_collapse_spaces(line, line);

			if (!s_iscomment(line)) {
				strtokens_t *tokens = hlp_strsplit(line, CONF_OPT_SEPARATOR, rtc->mem_pool);
				if (tokens->size > 1 && s_is_opt_valid(tokens->token[0]) ) {

					char *key = tokens->token[0];
					char *value = tokens->token[1];
					conf_opt_t *opt = s_opt_parse(key, value, rtc);

					if (opt != NULL) {
						log_debug("Parsed option (%s).", key);
						apr_hash_set(rtc->options, key, APR_HASH_KEY_STRING, opt);
					} else {
						log_err("Failed to parse option (%s) !", key);
						success = FALSE;
						break;
					}
				} else {
					log_err("Invalid option line (%s) !", line);
					success = FALSE;
					break;
				}
			}
		}
		apr_file_close(apr_file);
	} else {
		log_err("Failed to load conf file - %s!", filename);
		APR_ERR_PRINT(rv);
	}

	return success;
}
