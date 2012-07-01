/*
  bootstrap.c
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

#include "ead_server.h"
#include "bootstrap.h"
#include "utils/logger.h"
#include "net/webserver.h"

apr_pool_t *mem_pool = NULL;

int bs_init() {
	TRACE;

	int rv_init = E_BS_OK;

	/*
	 * Init logging system
	 */
	if (log_init(NULL)) {
    	ASSERT(0);
        fprintf(stderr, "Could not initialize logging system!\n");
        rv_init = E_BS_LOG_FAILED;
        goto error;
	}

	log_info("Bootstrap starting ...");

	/**
	 * Init Apache APR
	 */
    apr_status_t rv;

    rv = apr_initialize();
    if (rv != APR_SUCCESS) {
    	ASSERT(0);
        fprintf(stderr, "Could not initialize APR! (%d) \n", rv);
        rv_init = E_BS_APR_FAILED;
        goto error;
    }

    // Memory pool
	apr_pool_create(&mem_pool, NULL);

	log_info("APR initialized.");

error:
    return rv_init;
}

void bs_start() {
	TRACE;

	ws_start(mem_pool);
}

void bs_cleanup() {
	TRACE;

	if (mem_pool) {
		apr_pool_destroy(mem_pool);
	}
	apr_terminate();

	log_info("Bootstrap cleanup finished.");
	log_close();
}

