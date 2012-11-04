/*
  xmldb.h
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

#ifndef XMLDB_H_
#define XMLDB_H_

struct xmldb_t {
	char *key;
	int optional;
};
typedef struct xmldb_t xmldb_t;

/**
 * Load Xml files with additives info from given directory.
 * @param dirpath
 * @param rtc Runtime context with initialized memory pool
 */
int xmldb_init(const char *dirpath, runtime_context_t *rtc);

void xmldb_query(const char *lang, const char *additive);


#endif /* XMLDB_H_ */