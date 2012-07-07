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

struct conf_file_t {
	char *key;
	union {
		char *str_value;
		int int_value;
		int bool_value;
	};
};
typedef struct conf_file_t conf_file_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Load an INI style configuration file
 * @param filename Full file path to conf file
 * @param rtc Runtime context with initialized memory pool
 * @remark Sections are not supported !
 */
conf_file_t* conf_parse(const char *filename, runtime_context_t *rtc);

#ifdef __cplusplus
}
#endif


#endif /* CONF_PARSER_H_ */
