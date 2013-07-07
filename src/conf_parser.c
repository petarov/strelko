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
#include "utils.h"
#include "rtc.h"
#include "conf_parser.h"
#include "confs.h"

#include <getopt.h>
#include <dirent.h>
#include <apr_lib.h>
#include <apr_strings.h>

// @see http://www.ibm.com/developerworks/aix/library/au-unix-getopt.html
static const char *opt_string = "vh:p:r:?";

static struct option long_options[] = {
		{ "verbose", no_argument, NULL, 'v' },
		{ "help", no_argument, NULL, 0 },
		{ "host", required_argument, NULL, 'h' },
		{ "port", required_argument, NULL, 'p' },
		{ "root", required_argument, NULL, 'r' },
		{NULL, 0, NULL, 0}
};

static const conf_optinfo_t const *configs_template = NULL;

static int s_iscomment(char *line) {
	return *line == '#' || *line == '/';
}

static int s_isvalid(const char *line) {
	const char *p = line;

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

static void s_opt_add(const char* key, conf_opt_t* opt, runtime_context_t *rtc) {
	TRACE;
	ASSERT(key != NULL);
	if (!rtc->options) {
		rtc->options = apr_hash_make(rtc->mem_pool);
	}
	apr_hash_set(rtc->options, key, APR_HASH_KEY_STRING, opt);
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

void s_opt_addnew(const char *key, const char *value, conf_opttype_e type,
		runtime_context_t *rtc) {
	TRACE;
	ASSERT(key != NULL);
	ASSERT(value != NULL);

	conf_opt_t *opt = (conf_opt_t *) apr_pcalloc(rtc->mem_pool,
			sizeof(conf_opt_t));
	opt->key = apr_pstrdup(rtc->mem_pool, key);

	// now check if the value is of the expected type
	switch(type) {
	case CT_INT:
		opt->u.int_val = strtol(value, (char **)NULL, 10);
		break;
	case CT_BOOL:
		opt->u.bool_val = utils_tobool(value);
		break;
	case CT_STRING:
	default:
		opt->u.str_val = apr_pstrdup(rtc->mem_pool, value);
		break;
	}

	s_opt_add(key, opt, rtc);
}

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
				if (utils_isnum(value, MAX_OPTION_VALUE_SIZE)) {
					opt = (conf_opt_t *)apr_pcalloc(rtc->mem_pool, sizeof(conf_opt_t));
					opt->key = key;
					opt->u.int_val = strtol(value, (char **)NULL, 10);
				}
				break;
			case CT_STRING:
				if (utils_isstr(value, MAX_OPTION_VALUE_SIZE)) {
					opt = (conf_opt_t *)apr_pcalloc(rtc->mem_pool, sizeof(conf_opt_t));
					opt->key = key;
					opt->u.str_val = apr_pstrdup(rtc->mem_pool, value);
				}
				break;
			case CT_BOOL:
				if (utils_isbool(value)) {
					opt = (conf_opt_t *)apr_pcalloc(rtc->mem_pool, sizeof(conf_opt_t));
					opt->key = key;
					opt->u.bool_val = utils_tobool(value);
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
	apr_file_t *apr_file = NULL;
	int	success	= FALSE;

	apr_status_t rv = apr_file_open(&apr_file, filename, APR_FOPEN_READ | APR_FOPEN_BUFFERED,
			APR_FPROT_OS_DEFAULT, rtc->mem_pool);
	if (rv == APR_SUCCESS) {
		char line[CONF_MAX_LINE_SIZE];
//		rtc->options = apr_hash_make(rtc->mem_pool);
		success = TRUE;

		while(APR_SUCCESS == apr_file_gets(line, CONF_MAX_LINE_SIZE, apr_file)) {

			if (utils_isblank(line))
				continue;

			// remove stupid white spaces :)
			apr_collapse_spaces(line, line);

			if (!s_iscomment(line)) {
				strtokens_t *tokens = utils_strsplit(line, CONF_OPT_SEPARATOR, rtc->mem_pool);
				if (tokens->size > 1 && s_is_opt_valid(tokens->token[0]) ) {

					char *key = tokens->token[0];
					char *value = tokens->token[1];
					conf_opt_t *opt = s_opt_parse(key, value, rtc);

					if (opt != NULL) {
						log_debug("Parsed option (%s).", key);
//						apr_hash_set(rtc->options, key, APR_HASH_KEY_STRING, opt);
						s_opt_add(key, opt, rtc);
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

int conf_parse_arg(int argc, char *argv[], runtime_context_t *rtc) {
	TRACE;
	ASSERT(rtc != NULL);
	int	success = FALSE;

	int next = -1;
	int opt_idx;
	conf_opt_t *opt = NULL;

	do {
		next = getopt_long(argc, argv, opt_string, long_options, &opt_idx);
		if (next == -1)
			break;

		switch(next) {
		// verbose
		case 'v':
			break;
		// help
		case '?':
			break;
		// host
		case 'h':
			s_opt_addnew(CF_LISTEN_ADDRESS, optarg, CT_STRING, rtc);
			break;
		// port
		case 'p':
			s_opt_addnew(CF_LISTEN_PORT, optarg, CT_INT, rtc);
			break;
		case 'r':
			s_opt_addnew(CF_DOCUMENT_ROOT, optarg, CT_STRING, rtc);
			break;
		default:
			// unreachable
			break;
		}

	} while (next != -1);

	/*
	 * Set Default options
	 */

	if (!conf_get_opt(CF_DOCUMENT_ROOT, rtc)) {
		char cwd[1024];
		if (getcwd(cwd, sizeof(cwd)) == NULL) {
			log_err("getcwd failed(%d)!", errno);
			return FALSE;

			// TODO: respect ERANGE return code
		}

		s_opt_addnew(CF_DOCUMENT_ROOT, cwd, CT_STRING, rtc);
	}

	char *docroot = conf_get_opt(CF_DOCUMENT_ROOT, rtc)->u.str_val;
	DIR *dir = opendir(docroot);
	if (!dir) {
		if (ENOENT == errno) {
			log_err("Document root directory (%s) does not exist!", docroot);
		} else {
			log_err("opendir failed(%d)!", errno);
		}
		return FALSE;
	}
	closedir(dir);

	log_info("Document root is %s", docroot);

	success = conf_get_opt(CF_LISTEN_ADDRESS, rtc) && conf_get_opt(CF_LISTEN_PORT, rtc);
	return success;
}

const conf_opt_t* const conf_get_opt(const char *key, runtime_context_t *rtc) {
	conf_opt_t *opt = apr_hash_get(rtc->options, key, APR_HASH_KEY_STRING);
	ASSERT(opt != NULL);
	return opt;
}
