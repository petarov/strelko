/*
  conf_parser.h
  This file is part of e-additives.server

  Copyright (C) 2012 necroleak

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

#ifndef CONF_PARSER_H_
#define CONF_PARSER_H_

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
	};
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

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initializes configuration parameters
 * @param configs
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

#ifdef __cplusplus
}
#endif


#endif /* CONF_PARSER_H_ */
