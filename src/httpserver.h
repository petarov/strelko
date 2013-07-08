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
#ifndef HTTPSERVER_H_
#define HTTPSERVER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "http.h"

struct web_server_t {
	char *hostname;
	int port;
	apr_socket_t *sock;
	int is_running;
};
typedef struct web_server_t web_server_t;

struct web_client_t {
	pthread_t thread; // TODO: remove
	apr_socket_t *sock;
	int connected;
	int done;
	http_request_t *req;
	runtime_context_t *rtc;
};
typedef struct web_client_t web_client_t;

/**
 * Thread pool control structure that manages a queue of created threads.
 */
struct thread_pool_t {
	web_client_t *head;	// queue head
	web_client_t *tail;	// queue tail
	int size;	// size of items in queue
	int num_active;	// size of occupied threads
	pthread_t *threads;
	pthread_mutex_t tlock;
	int stop;
};
typedef struct thread_pool_t thread_pool_t;

/**
 * Client thread pool limit
 */
#define MAX_CLIENTS_IN_POOL 100

/**
 * Creates a webserver configuration instance
 * @param ws Non-initialized web_server_t structure
 * @param rtc Initialized runtime context
 */
status_code_t httpsrv_create(web_server_t **ws, runtime_context_t *rtc);

/**
 * Create web server listening socket and start listening for client connections
 * @param ws Initialized web_server context
 * @param rtc Initialized runtime context
 */
status_code_t httpsrv_start(web_server_t *ws, runtime_context_t *rtc);

/**
 * Set server shutdown flag
 * @param Initialized web_server context. The server must already have been started
 * with websrv_start()
 */
void httpsrv_stop(web_server_t *ws);

#ifdef __cplusplus
}
#endif

#endif /* HTTPSERVER_H_ */
