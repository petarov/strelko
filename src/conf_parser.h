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

#ifndef CONF_PARSER_H_
#define CONF_PARSER_H_

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Maximum size of a configuration line
 */
#define CONF_MAX_LINE_SIZE	512
#define CONF_OPT_SEPARATOR	"="

struct conf_opt_t {
	const char *key;
	union {
		char *str_val;
		int int_val;
		int bool_val;
	} u;
};
typedef struct conf_opt_t conf_opt_t;

/**
 * Maximum size of any option value
 */
#define MAX_OPTION_VALUE_SIZE	100

/**
 * Type of configuration values
 * Supported: Integer, String, Boolean
 */
enum conf_opttype_e {
	CT_INT,
	CT_STRING,
	CT_BOOL
};
typedef enum conf_opttype_e conf_opttype_e;

struct conf_optinfo_t {
	char *key;
	conf_opttype_e type;
	int optional;
};
typedef struct conf_optinfo_t conf_optinfo_t;

/**
 * Initializes configuration parameters.
 * @param configs Configuration template of what options are expected to be loaded
 * @return Currently always returns TRUE
 */
int conf_init(const conf_optinfo_t const *configs);

/**
 * Load an INI style configuration file
 * @param filename Full file path to conf file
 * @param rtc Runtime context with initialized memory pool
 * @return TRUE on success, FALSE otherwise
 * @remark Sections are not supported !
 */
int conf_parse(const char *filename, runtime_context_t *rtc);

/**
 * Load configurations from command line parameters
 */
int conf_parse_arg(int argc, char *argv[], runtime_context_t *rtc);

/**
 * Get parsed option
 * @param key Option name
 * @param rtc Initialized runtime context
 * @return Ptr to loaded conf_opt_t structure if key was found, NULL otherwise
 */
const conf_opt_t* const conf_get_opt(const char *key, runtime_context_t *rtc);

/**
 * Get string from configuration macro. Requires valid runtime context.
 */
#define CF_STR(key) conf_get_opt(key, rtc)->u.str_val
/**
 * Get integer from configuration macro. Requires valid runtime context.
 */
#define CF_INT(key) conf_get_opt(key, rtc)->u.int_val
/**
 * Get boolean from configuration macro. Requires valid runtime context.
 */
#define CF_BOOL(key) conf_get_opt(key, rtc)->u.bool_val

#ifdef __cplusplus
}
#endif


#endif /* CONF_PARSER_H_ */
