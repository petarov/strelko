/*
  xmldb.c
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

#include "globals.h"
#include "logger.h"
#include "utils.h"
#include "rtc.h"
#include "xmldb.h"

#include <apr_lib.h>

static void s_dump_xml(apr_xml_elem *elem, runtime_context_t *rtc) {
	TRACE;

    apr_xml_elem *child_element;
    const char *text;

	if (elem->attr) {
		fprintf(stdout, "This tag has an attribute : <%s %s=%s>\n",
				elem->name, elem->attr->name, elem->attr->value );
	}

	if (elem->first_child) {
        child_element = elem->first_child;

        while (child_element)
        {
            printf( "<%s> --> ", child_element->name );
            if( !APR_XML_ELEM_IS_EMPTY( child_element ) )
            {
                apr_xml_to_text( rtc->mem_pool, child_element, APR_XML_X2T_INNER,
                		NULL, NULL, &text, NULL);
                fprintf(stdout, "%s\n", text );
            }
            s_dump_xml( child_element, rtc);
            child_element = child_element->next;
        }
	}
}

static status_code_t s_load_xml(xmldb_entry_t **db_entry, apr_file_t *apr_file,
		runtime_context_t *rtc) {
	TRACE;

	apr_xml_parser *parser = NULL;
	apr_xml_doc *doc = NULL;
	char errbuf[255];

	apr_status_t rv = apr_xml_parse_file(rtc->mem_pool, &parser, &doc, apr_file, 2000);
	if (rv != APR_SUCCESS) {
		APR_ERR_PRINT(rv);

		if (parser != NULL) {
			log_err("XML Error: %s", apr_xml_parser_geterror(parser, errbuf, 255));
		}

		return SC_XMLDB_ERR_FILE;
	} else {
		// create new database entry

		*db_entry = (xmldb_entry_t *)apr_palloc(rtc->mem_pool, sizeof(xmldb_entry_t));
		xmldb_entry_t *db = *db_entry;
		db->xmlparser = parser;
		db->xmldoc = doc;

		apr_xml_attr *attr = doc->root->attr;
		if (!attr) {
			log_err("XML database contains no header attributes!");
			return SC_XMLDB_ERR_HEADER;
		} else {
			while (attr != NULL) {
				if (!apr_strnatcmp(attr->name, XML_ATTRIB_LANGUAGE)) {
					db->language = apr_pstrdup(rtc->mem_pool, attr->value);
				} else if (!apr_strnatcmp(attr->name, XML_ATTRIB_AUTHOR)) {
					db->author = apr_pstrdup(rtc->mem_pool, attr->value);
				} else if (!apr_strnatcmp(attr->name, XML_ATTRIB_CREATED)) {
					db->date_created = apr_pstrdup(rtc->mem_pool, attr->value);
				} else if (!apr_strnatcmp(attr->name, XML_ATTRIB_LASTCHANGE)) {
					db->date_lastchange = apr_pstrdup(rtc->mem_pool, attr->value);
				}
				attr = attr->next;
			}

			if (db->language == NULL) {
				log_err("XML database does not specify language!");
				return SC_XMLDB_ERR_LANGUAGE;
			}
		}
		//s_dump_xml(doc->root, rtc);
	}

	return SC_OK;
}

status_code_t xmldb_init(const char *dirpath, runtime_context_t *rtc) {
	TRACE;
	ASSERT(rtc != NULL);

	apr_status_t rv			= APR_SUCCESS;
	apr_dir_t *apr_dir		= NULL;
	apr_file_t *apr_file	= NULL;
	status_code_t ret		= SC_OK;

	rv = apr_dir_open(&apr_dir, dirpath, rtc->mem_pool);
	if (rv != APR_SUCCESS) {
		log_err("Failed to open directory - %s!", dirpath);
		APR_ERR_PRINT(rv);
		return SC_FAILED;
	}

	// create storage for databases and save it into the runtime context
	xmldb_t	*xmldb = (xmldb_t *)apr_palloc(rtc->mem_pool, sizeof(xmldb_t));
	xmldb->count = 0;
	xmldb->db = apr_hash_make(rtc->mem_pool);
	rtc->xmldb = xmldb;

	do {
		// read next file in data directory
		apr_finfo_t fileinfo;
		rv = apr_dir_read(&fileinfo, APR_FINFO_MIN, apr_dir);
		if (rv != APR_ENOENT && apr_strnatcmp(fileinfo.name, ".")
				&& apr_strnatcmp(fileinfo.name, "..")) {

			char *filepath = apr_pstrcat(rtc->mem_pool, dirpath, "/", fileinfo.name, NULL);
			log_debug("Loading xml database %s ...", filepath);

			apr_status_t fp_rv = apr_file_open(&apr_file, filepath,
					APR_FOPEN_READ | APR_FOPEN_BUFFERED, APR_FPROT_OS_DEFAULT, rtc->mem_pool);
			if (fp_rv == APR_SUCCESS) {
				xmldb_entry_t *xmldb_entry = NULL;

				if (SC_OK != s_load_xml(&xmldb_entry, apr_file, rtc)) {
					log_err("Failed to load xml file - %s!", filepath);
					apr_file_close(apr_file);
					continue;
				}

				// store loaded xml database entry
				xmldb->count++;
				apr_hash_set(xmldb->db, xmldb_entry->language, APR_HASH_KEY_STRING, xmldb_entry);
				apr_file_close(apr_file);

			} else {
				log_err("Failed to load xml file - %s!", filepath);
				APR_ERR_PRINT(fp_rv);
				//ret = SC_XMLDB_ERR_FILE;
				continue;
			}
		}
	} while (rv != APR_ENOENT);

	if (xmldb->count == 0) {
		// no databases were loaded, therefore nothing can be served back
		ret = SC_XMLDB_ERR_NO_DATABASES;
	}

	apr_dir_close(apr_dir);

	return ret;
}

int xmldb_get(const char *lang, const char *additive_code, runtime_context_t *rtc) {
	TRACE;
	ASSERT(rtc != NULL);

	xmldb_entry_t *db_entry = apr_hash_get(rtc->xmldb->db, lang, APR_HASH_KEY_STRING);
	if (db_entry) {
		// get <locale> <items> <item>
		apr_xml_elem *elem = db_entry->xmldoc->root->first_child->first_child;
		while (elem != NULL) {
			log_debug("next elem ...");
			int found = FALSE;

			// search for additive with given code
			apr_xml_attr *attr = elem->attr;
			while (attr != NULL) {
				log_debug("next attrib %s", attr->name);
				if (!apr_strnatcmp(attr->name, XML_ATTRIB_KEY)
						&& !apr_strnatcmp(attr->value, additive_code)) {
					found = TRUE;
					break;
				}
				attr = attr->next;
			}

			if (found) {
				log_debug("Found item %s!", elem->attr->name);
				return TRUE; //apr_pstrdup(rtc->mem_pool, elem->attr->value);
				//break;
			}

			// TODO
			elem = elem->next;
		}

	} else {
		log_err("Lang not found!");
	}

	return FALSE;
}
