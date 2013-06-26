/*
 * Strelko is a small-footprint HTTP server.
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

#include "globals.h"
#include "logger.h"
#include "conf_parser.h"
#include "httpserver.h"
#include "signals.h"
#include "bootstrap.h"

static runtime_context_t *rtctx;
static const conf_optinfo_t const config_options[] = {
		{ "listen_address", CT_STRING, FALSE },
		{ "listen_port", CT_INT, FALSE },
		{ NULL }
};
static web_server_t *websrv = NULL;

status_code_t bs_init(int argc, char* argv[]) {
	TRACE;

	int exit_code = SC_OK;
//	char *config_filepath = NULL;

	/**
	 * Check command line
	 */
	if (argc < 2) {
		fprintf(stderr, "Missing command line parameters!\n");
		exit_code = SC_BS_ERR_CMD;
		goto error;
//	} else {
//		config_filepath = argv[1];
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
    rtc_create(&rtctx);

    conf_init(config_options);
    if (!conf_parse_arg(argc, argv, rtctx)) {
        exit_code = SC_BS_ERR_CONF;
        goto error;
    }

error:
    return exit_code;
}

void bs_start() {
	TRACE;

	// Fire-up the web server
	httpsrv_create(&websrv, rtctx);
	httpsrv_start(websrv, rtctx);
}

void bs_stop() {
	TRACE;

	httpsrv_stop(websrv);
}

void bs_destroy() {
	TRACE;

	rtc_destroy(&rtctx);
	apr_terminate();

	log_info("Bootstrap cleanup finished.");
	log_close();
}

