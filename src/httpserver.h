/*
 * Strelko is a small footprint HTTP server.
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

#define HTTP_CRLF		"\r\n"
#define HTTP_METHOD_GET		"GET"
#define HTTP_METHOD_POST	"POST"
#define HTTP_METHOD_PUT		"PUT"
#define HTTP_METHOD_DELETE	"DELETE"    
    
/*
 * HTTP messages consist of requests from client to server and responses from 
 * server to client.
 * 
 * HTTP-message   = Request | Response     ; HTTP/1.1 messages
 *
 * generic-message 	= start-line
 * 					*(message-header CRLF)
 * 					CRLF
 * 					[ message-body ]
 * start-line		= Request-Line | Status-Line
*/

/*
 * Request-Line   = Method SP Request-URI SP HTTP-Version CRLF
 */
struct http_request_t {
	const char *method;
	char *uri;
	char *http_version;

	char *message_body;
	char *raw_body;

	char *cookies;

	const char *content_type;
	long content_len;
};
typedef struct http_request_t http_request_t;

struct web_server_t {
	char *hostname;
	int port;
	apr_socket_t *sock;
	int is_running;
};
typedef struct web_server_t web_server_t;

struct web_client_t {
	apr_socket_t *sock;
        /**
         * Store incoming data into byte bucket
         */
        char *bucket;
        int blen;
        /**
         * Store parsed http request
         */
	http_request_t req;
        /**
         * Flags
         */
	int done;
	/*
	 * Each client should maintain it's own memory pool.
	 * When request is parsed and served the pool will be destroyed.
	 * Thus, freeing all request allocated memory.
	 */
	apr_pool_t *mem_pool;
};
typedef struct web_client_t web_client_t;

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
