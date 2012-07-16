/*
  webserver.h
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

#ifndef WEBSERVER_H_
#define WEBSERVER_H_

#ifdef __cplusplus
extern "C" {
#endif

struct web_server_t {
	char *hostname;
	int port;
	apr_socket_t *sock;
	int is_running;
};
typedef struct web_server_t web_server_t;

/*
 * HTTP messages consist of requests from client to server and responses from server to client.
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

#define HTTP_CRLF			"\r\n"

#define HTTP_METHOD_GET		"GET"
#define HTTP_METHOD_POST	"POST"
#define HTTP_METHOD_PUT		"PUT"
#define HTTP_METHOD_DELETE	"DELETE"

/**
 * Creates a webserver configuration instance
 * @param ws Non-initialized web_server_t structure
 * @param rtc Initialized runtime context
 */
status_code_t websrv_create(web_server_t **ws, runtime_context_t *rtc);

/**
 * Create web server listening socket and start listening for client connections
 * @param ws Initialized web_server context
 * @param rtc Initialized runtime context
 */
status_code_t websrv_start(web_server_t *ws, runtime_context_t *rtc);

/**
 * Set server shutdown flag
 * @param Initialized web_server context. The server must already have been started
 * with websrv_start()
 */
void websrv_stop(web_server_t *ws);

#ifdef __cplusplus
}
#endif

#endif /* WEBSERVER_H_ */
