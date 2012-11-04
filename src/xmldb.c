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
#include <apr_xml.h>

static int s_load_xml(apr_file_t *apr_file, runtime_context_t *rtc) {
	TRACE;

	apr_xml_parser *parser = 0;
	apr_xml_doc *doc;

	apr_status_t rv = apr_xml_parse_file(rtc->mem_pool, &parser, &doc, apr_file, 2000);
	if (rv == APR_SUCCESS) {
		//TODO:
	}

	return rv;
}

int xmldb_init(const char *dirpath, runtime_context_t *rtc) {
	TRACE;
	ASSERT(rtc != NULL);

	apr_status_t rv			= APR_SUCCESS;
	apr_dir_t *apr_dir		= NULL;
	apr_file_t *apr_file 	= NULL;
	int	success				= TRUE;

	rv = apr_dir_open(&apr_dir, dirpath, rtc->mem_pool);
	if (rv != APR_SUCCESS) {
		log_err("Failed to open directory - %s!", dirpath);
		APR_ERR_PRINT(rv);
		return FALSE;
	}

	rv = APR_ENOENT;
	do {
		// read next file in data directory
		apr_finfo_t fileinfo;
		rv = apr_dir_read(&fileinfo, APR_FINFO_MIN, apr_dir);
		if (rv != APR_ENOENT && apr_strnatcmp(fileinfo.name, ".")
				&& apr_strnatcmp(fileinfo.name, "..")) {

			// get full file path
			char *filepath = apr_pstrcat(rtc->mem_pool, dirpath, "/", fileinfo.name, NULL);
			log_debug("Loading xml database %s ...", filepath);

			int xml_rv = APR_SUCCESS;
			apr_status_t fp_rv = apr_file_open(&apr_file, filepath, APR_FOPEN_READ | APR_FOPEN_BUFFERED,
					APR_FPROT_OS_DEFAULT, rtc->mem_pool);

			if (fp_rv == APR_SUCCESS) {
				xml_rv = s_load_xml(apr_file, rtc);
				if (xml_rv != APR_SUCCESS) {
					log_err("Failed to parse xml file - %s!", filepath);
					APR_ERR_PRINT(xml_rv);
					success = FALSE;
					apr_file_close(apr_file);
					break;
				}
				apr_file_close(apr_file);
			} else {
				log_err("Failed to load xml file - %s!", filepath);
				APR_ERR_PRINT(fp_rv);
				success = FALSE;
				break;
			}
		}
	} while (rv != APR_ENOENT);

	if (apr_dir != NULL) {
		apr_dir_close(apr_dir);
	}

	return success;
}
