/*
  bootstrap.c
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
#include "rtc.h"
#include "utils/conf_parser.h"
#include "webserver.h"
#include "signals.h"
#include "bootstrap.h"

static runtime_context_t *rt_ctx;
static const conf_optinfo_t const config_options[] = {
		{ "listen_address", CT_STRING, FALSE },
		{ "listen_port", CT_INT, FALSE },
		{ NULL }
};

int bs_init(int argc, char* argv[]) {
	TRACE;

	int exit_code = SC_OK;
	char *config_filepath = NULL;

	/**
	 * Check command line
	 */
	if (argc < 2) {
		fprintf(stderr, "Missing configuration file command line param!\n");
		exit_code = SC_BS_ERR_CMD;
		goto error;
	} else {
		config_filepath = argv[1];
	}

	/*
	 * Init logging system
	 */
	if (log_init(NULL)) {
        fprintf(stderr, "Could not initialize logging system!\n");
        exit_code = SC_BS_ERR_LOG;
        goto error;
	}
	log_info("Bootstrap started");

	/**
	 * Register signal handling
	 */
	signals_init();
	log_info("Signal handling registered.");

	/**
	 * Init Apache APR
	 */
    apr_status_t rv = apr_initialize();
    if (rv != APR_SUCCESS) {
        fprintf(stderr, "Could not initialize APR! (%d) \n", rv);
        APR_ERR_PRINT(rv);
        exit_code = SC_BS_ERR_APR_INIT;
        goto error;
    }
    log_info("APR initialized.");

    /**
     * Initialize runtime context and load conf options
     */
    rtc_create(&rt_ctx);
    conf_init(config_options);
    if (!conf_parse(config_filepath, rt_ctx)) {
        exit_code = SC_BS_ERR_CONF;
        goto error;
    }

error:
    return exit_code;
}

void bs_start() {
	TRACE;

	// Fire-up the web server
	ws_start(rt_ctx);
}

void bs_stop() {
	TRACE;

	rtc_destroy(&rt_ctx);
	apr_terminate();

	log_info("Bootstrap cleanup finished.");
	log_close();
}

