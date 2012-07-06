/*
  helpers.c
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

#include <ctype.h>
#include <string.h>
#include "helpers.h"

int hlp_isnum(const char *str, int size) {
	const char *p = str;
	int i = size;

	while(i-- > 0 && *p != '\0') {
		if (!apr_isdigit(*p))
			return FALSE;
		p++;
	}
	return TRUE;
}

int hlp_isstr(const char *str, int size) {
	const char *p = str;
	int i = size;

	while(i-- > 0 && *p != '\0') {
		if (!apr_isascii(*p))
			return FALSE;
		p++;
	}
	return TRUE;
}

int hlp_isbool(const char *str) {
	return (!strncmp("true", str, 4) || !strncmp("false", str, 5) ||
			!strncmp("TRUE", str, 4) || !strncmp("FALSE", str, 5) ||
			!strncmp("1", str, 1) || !strncmp("0", str, 1) );
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
