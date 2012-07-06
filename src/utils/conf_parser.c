/*
  conf_parser.c
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
#include "utils/logger.h"
#include "config/rtc.h"
#include "config/rtc.h"
#include "conf_parser.h"

conf_file_t* cnfp_load(const char *filename, runtime_context_t *rtc) {
	TRACE;

	ASSERT(rtc != NULL);
	conf_file_t *cfile 		= NULL;
	apr_file_t *apr_file 	= NULL;
	apr_status_t rv			= APR_SUCCESS;

	rv = apr_file_open(&apr_file, filename, APR_FOPEN_READ | APR_FOPEN_BUFFERED, APR_FPROT_OS_DEFAULT,
			rtc->mem_pool);
	if (rv == APR_SUCCESS) {
		//TODO

		apr_file_close(apr_file);
	} else {
		char buf[512];
		apr_strerror(rv, buf, sizeof(buf));
		log_err("Failed to load %s conf file! Error: %s", filename, buf);
	}

	return cfile;
}
