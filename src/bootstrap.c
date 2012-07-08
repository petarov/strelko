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
#include "net/webserver.h"
#include "bootstrap.h"

static runtime_context_t *rt_ctx;
static int recieved_cleanup_sig = 0;

static const conf_optinfo_t const config_options[] = {
		{ "listen_address", CT_STRING, FALSE },
		{ "listen_port", CT_INT, FALSE },
		{ NULL }
};

/**
 * Handles Unix signals
 * @remark http://www.cs.utah.edu/dept/old/texinfo/glibc-manual-0.02/library_21.html
 */
static void sighandler(int signum) {
	TRACE;

	/* Since this handler is established for more than one kind of signal,
	     it might still get invoked recursively by delivery of some other kind
	     of signal.  Use a static variable to keep track of that. */
	if (recieved_cleanup_sig)
		raise(signum);
	recieved_cleanup_sig = 1;

    printf("Received signal %d\n", signum);
    printf("Signal originates from process %lu\n", getpid());

    bs_stop();

    /* Now reraise the signal.  Since the signal is blocked,
       it will receive its default handling, which is
       to terminate the process.  We could just call
       exit or abort, but reraising the signal
       sets the return status from the process correctly. */
    raise(signum);
}


int bs_init(int argc, char* argv[]) {
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
	log_info("Bootstrap started");

	/**
	 * Register signal handling
	 */
	signal(SIGTERM, sighandler);
	log_info("Signal handling registered.");

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
    log_info("APR initialized.");

    /**
     * Runtime context initialization
     */
    rtc_create(&rt_ctx);

    conf_init(config_options);
    if (!conf_parse("ead.conf", rt_ctx)) {
        rv_init = E_BS_CONF_FAILED;
        goto error;
    }

error:
    return rv_init;
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

