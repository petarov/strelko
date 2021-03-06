/*
 * Strelko is a small footprint HTTP server.
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

#include <ctype.h>
#include <string.h>
#include "utils.h"

#include <apr_strings.h>
#include <apr_lib.h>
#include <apr_tables.h>

int utils_isnum(const char *str, int size) {
	const char *p = str;
	int i = size;

	while(i-- > 0 && *p != '\0') {
		if (!ISNUM(*p))
			return FALSE;
		p++;
	}
	return TRUE;
}

int utils_isstr(const char *str, int size) {
	const char *p = str;
	int i = size;

	while(i-- > 0 && *p != '\0') {
		if (!apr_isascii(*p))
			return FALSE;
		p++;
	}
	return TRUE;
}

int utils_isbool(const char *str) {
	return (!strncmp("true", str, 4) || !strncmp("false", str, 5) ||
			!strncmp("1", str, 1) || !strncmp("0", str, 1) ||
			!strncmp("TRUE", str, 4) || !strncmp("FALSE", str, 5) );
//	const char *p = str;
//	int i = size;
//
//	while(i-- > 0 && *p != '\0') {
//		if (*p != '1' && *p != '0')
//			return FALSE;
//		p++;
//	}
//	return TRUE;
}

int utils_tobool(const char *str) {
	return (!strncmp("false", str, 5) || !strncmp("FALSE", str, 5) ||
			!strncmp("0", str, 1)) ? FALSE : TRUE;
}

int utils_isblank(const char *str) {
	const char *p = str;

	while(*p != '\0') {
		if (!ISWHITESPACE(*p))
			return FALSE;
		p++;
	}
	return TRUE;
}

strtokens_t* utils_strsplit(char *str, const char *sep, apr_pool_t *mp) {

	strtokens_t 		*tokens = apr_palloc(mp, sizeof(strtokens_t));
    tokens->size = 0;
    tokens->token = NULL;
    
	apr_array_header_t 	*arr 	= apr_array_make(mp, TOKEN_ARRAY_INIT_SIZE, sizeof(const char *));
	char *state 	= NULL;
	char *next 		= apr_strtok(str, sep, &state);

	while (next != NULL ) {
		*(char **)apr_array_push(arr) = next; // apr_pstrdup(mp, next);
		next = apr_strtok(NULL, sep, &state);
	}
	// copy once again the values into strtokens_st
	// ...hey, I know it's slow, but I don't want to deal with APR arrays :(
	tokens->size = arr->nelts;
	if (arr->nelts) {
		tokens->token = (char **)apr_palloc(mp, sizeof(char **) * arr->nelts);
		for (int i = 0; i < arr->nelts; i++) {
			const char *s = ((const char**)arr->elts)[i];
			tokens->token[i] = apr_pstrdup(mp, s);
		}
	}
	return tokens;
}

