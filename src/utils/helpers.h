/*
  helpers.h
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

#ifndef HELPERS_H_
#define HELPERS_H_

// Fix this stupid issue with apr.h
#ifndef PATH_MAX
#define PATH_MAX 256
#endif

#include <apr_strings.h>
#include <apr_lib.h>


/**
 * Check if string value consists only of numeric values
 * @param str
 * @param size
 */
int hlp_isnum(const char *str, int size);

/**
 * Check if string value consists only of ASCII 7-bit values
 * @param str
 * @param size
 */
int hlp_isstr(const char *str, int size);

/**
 * Check if string value is of boollean type. Either 'true' or 'false' or '1' or '0'.
 * @param str
 * @param size
 */
int hlp_isbool(const char *str);

#endif /* HELPERS_H_ */