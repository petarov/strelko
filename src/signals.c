/*
  signals.c
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

#include <signal.h>
#include <sys/signal.h>
#include <unistd.h>

#include "globals.h"
#include "utils/logger.h"
#include "bootstrap.h"
#include "signals.h"

static signals_t signals[] = {
		// This signal indicates that someone has killed the controlling terminal.
		{ SIGHUP, "SIGHUP", signals_handler },
		// Application is running in a foreground in a terminal and someone presses CTRL-C.
		{ SIGINT, "SIGINT", signals_handler },
		// Signal means “Quit from keyboard”. Sent explicitly ?
		{ SIGQUIT, "SIGQUIT", signals_handler },
		// Abort signal means you used used abort() API inside of your program.
		{ SIGABRT, "SIGABRT", signals_handler },
		// Operating system sends a program this signal when it tries to access memory
		// that does not belong to it.
		/*{ SIGSEGV, "SIGSEGV", signals_handler },*/
		// Signal tells your program to terminate itself. Cleanly shut down.
		{ SIGTERM, "SIGTERM", signals_handler },
		// Abnormal termination signal.
		/*{ SIGKILL, "SIGKILL", signals_handler },*/
		// Child process of your program has stopped or terminated.
		{ SIGCHLD, "SIGCHLD", signals_handler },
		// NULL
		{ 0, NULL, 0 }
};

void signals_init() {
	TRACE;

	struct sigaction sigact;
	for (signals_t *sig = signals; sig->signum != 0; sig++) {
		memset(&sigact, 0, sizeof(struct sigaction));
		sigact.sa_flags = sig->signum;
		sigact.sa_handler = signals_handler;
		sigaction(sig->signum, &sigact, NULL);

	}
}

void signals_handler(int signum) {
    TRACE;

	log_notice("Got signal %d", signum);
    for (signals_t *sig = signals; sig->signum != 0; sig++) {
    	if (sig->signum == signum) {
    		log_info("(%d) is a known signal %s", signum, sig->signame);
    		break;
    	}
    }

    int stop_ship = 0;

    switch(signum) {
    case SIGHUP:
    	//TODO Restart;
    	break;
    case SIGTERM:
    case SIGINT:
    case SIGQUIT:
    case SIGABRT:
    	log_notice("Goodbye!");
    	stop_ship = 1;
    	break;
    }

    // clean shutdown
    if (stop_ship) {
    	bs_stop();
    	exit(SC_TERMINATED);
    }
}

/*

void signals_handler(int signum) {
	TRACE;

	 Since this handler is established for more than one kind of signal,
	     it might still get invoked recursively by delivery of some other kind
	     of signal.  Use a static variable to keep track of that.
	if (recieved_cleanup_sig)
		raise(signum);
	recieved_cleanup_sig = 1;

    printf("Received signal %d\n", signum);
    printf("Signal originates from process %lu\n", getpid());

    bs_stop();

     Now reraise the signal.  Since the signal is blocked,
       it will receive its default handling, which is
       to terminate the process.  We could just call
       exit or abort, but reraising the signal
       sets the return status from the process correctly.
    raise(signum);
}
*/
