/*
  http.h
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

#ifndef HTTP_H_
#define HTTP_H_

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

#endif /* HTTP_H_ */