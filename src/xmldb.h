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

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Xml database file
 */
struct xmldb_entry_t {
	apr_xml_parser *xmlparser;
	apr_xml_doc	*xmldoc;
	char *language;
	char *author;
	char *date_created;
	char *date_lastchange;
};
typedef struct xmldb_entry_t xmldb_entry_t;

/**
 * Collection of Xml databases
 */
struct xmldb_t {
	int count;
	apr_hash_t *db;
};
//typedef struct xmldb_t xmldb_t;

/**
 * Tags and attributes contained in Xml databases
 */
#define XML_TAG_LOCALE 		"locale"
#define XML_TAG_ITEMS 		"items"
#define XML_TAG_ITEM 		"item"
#define XML_TAG_DATA 		"data"
#define XML_ATTRIB_LANGUAGE	"language"
#define XML_ATTRIB_AUTHOR	"author"
#define XML_ATTRIB_CREATED	"created"
#define XML_ATTRIB_LASTCHANGE	"lastchange"
#define XML_ATTRIB_KEY		"key"
#define XML_ATTRIB_NAME		"name"
#define XML_ATTRIB_STATUS	"status"
#define XML_ATTRIB_VEGAN	"veg"

/**
 * Load Xml files with additives info from given directory.
 * @param dirpath
 * @param rtc Runtime context with initialized memory pool
 * @return status_code_t
 */
status_code_t xmldb_init(const char *dirpath, runtime_context_t *rtc);

/**
 * Get addtive entry from database
 * @param lang
 * @param additive_code
 * @return
 */
int xmldb_get(const char *lang, const char *additive_code, runtime_context_t *rtc);

/**
 *
 * @param lang
 * @param search_string
 */
//void xmldb_query(const char *lang, const char *search_string);


#ifdef __cplusplus
}
#endif

#endif /* XMLDB_H_ */
