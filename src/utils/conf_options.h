/*
  config.h
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

#ifndef CONFIG_H_
#define CONFIG_H_

/**
 * Maximum size of any option value
 */
#define MAX_OPTION_VALUE_SIZE	100

enum config_type_e {
	CT_INT,
	CT_STRING,
	CT_BOOL
};
typedef enum config_type_e config_type_e;

struct config_option_t {
	char *key;
	config_type_e type;
	int optional;
};
typedef struct config_option_t config_option_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initializes configuration parameters
 * @param configs
 * @return Currently always returns TRUE
 */
int cfg_init(const config_option_t const *configs);

/**
 * Check if this option is within the list of defined. Additionally checks if the value matches
 * the type specified in the option defition.
 * @param option Option to look for
 * @return Returns TRUE if the option is known and value is of the expected type, FALSE otherwise
 * @remark Option name check is case sensitive.
 */
int cfg_is_valid(const char *key, const char *value);

/**
 * Check if this option is optional
 * @param option Option name
 * @return Returns TRUE if the options is unknown or optional, FALSE otherwise
 */
int cfg_is_optional(const char* key);

/**
 * Get a structure of all option details
 * @param option
 * @return Const pointer to option structure
 */
const config_option_t* cfg_get_option(const char *key);

#ifdef __cplusplus
}
#endif

#endif /* CONFIG_H_ */
