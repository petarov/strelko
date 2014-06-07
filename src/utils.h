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

#ifndef UTILS_H_
#define UTILS_H_

// Fix this stupid issue with apr.h
#ifndef PATH_MAX
#define PATH_MAX 256
#endif

#include <apr_pools.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ISNUM(c) 	(c >= '0' && c <= '9')
#define ISWHITESPACE(c) (c == '\t' || c == ' ' || c == '\n' || c == '\r')

/**
 * Check if string value consists only of numeric values
 * @param str
 * @param size
 */
int utils_isnum(const char *str, int size);

/**
 * Check if string value consists only of ASCII 7-bit values
 * @param str
 * @param size
 */
int utils_isstr(const char *str, int size);

/**
 * Check if string value is of boollean type. Either 'true' or 'false' or '1' or '0'.
 * @param str
 * @param size
 */
int utils_isbool(const char *str);

/**
 * Convert string to boolean value
 * @param str String must be one of the following: TRUE, true, FALSE, false, 1 or 0
 * @Return 0 if string is FALSE, false or 0, otherwise returns 1
 */
int utils_tobool(const char *str);

/**
 * Check if line is blank
 * @param str
 */
int utils_isblank(const char *str);

/**
 * Holds string tokens created from the hlp_strsplit() function
 */
struct strtokens_t {
	int size;
	char **token;
};
typedef struct strtokens_t strtokens_t;

#define TOKEN_ARRAY_INIT_SIZE	10

/**
 * Split a string into tokens given the selected separators
 * @param str String to split
 * @param sep List of separator characters (no delimiters)
 * @param mp APR memory pool
 * @return Returns tokens strtokens_t structure which holds the tokens and the total size of
 * tokens found
 * @remark If no string could not be split, the .size field will be set to 1
 */
strtokens_t* utils_strsplit(char *str, const char *sep, apr_pool_t *mp);

#ifdef __cplusplus
}
#endif

#endif /* UTILS_H_ */
