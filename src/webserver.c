/*
  webserver.c
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
#include "logger.h"
#include "conf_parser.h"
#include "webserver.h"
#include "http_parser.h"

#define BUFSIZE			4096

static apr_status_t s_listen(web_server_t *ws, apr_pool_t *mp) {
	TRACE;

	apr_status_t rv;
    apr_socket_t *s;
    apr_sockaddr_t *sa;

    log_debug("Starting http server on %s:%d", ws->hostname, ws->port);

    if (APR_SUCCESS != (rv = apr_sockaddr_info_get(&sa, NULL, APR_INET, ws->port, 0, mp)))
    	goto error;

    if (APR_SUCCESS != (rv = apr_socket_create(&s, sa->family, SOCK_STREAM, APR_PROTO_TCP, mp)))
    	goto error;

    /* it is a good idea to specify socket options explicitly.
     * in this case, we make a blocking socket as the listening socket
     */
    apr_socket_opt_set(s, APR_SO_NONBLOCK, 0);
    apr_socket_timeout_set(s, -1);
    apr_socket_opt_set(s, APR_SO_REUSEADDR, 1);

    if (APR_SUCCESS != (rv = apr_socket_bind(s, sa)))
    	goto error;

    if (APR_SUCCESS != (rv = apr_socket_listen(s, SOMAXCONN)))
    	goto error;

    // assign the created socket
    ws->sock = s;

error:
    return rv;
}

static int http_header_cb(http_parser *parser, const char *at, size_t length) {
	printf("HDR: %s", at);
	return 0;
}

static int http_body_cb(http_parser *parser, const char *at, size_t length) {
	printf("BODY: %s", at);
	return 0;
}

static int s_process_packet(apr_socket_t *sock, apr_pool_t *mp) {
	TRACE;

	char buf[BUFSIZE];
	http_parser_settings settings;
	settings.on_header_field = http_header_cb;
	settings.on_header_value = http_header_cb;
	settings.on_body = http_body_cb;

	http_parser *parser = apr_palloc(mp, sizeof(http_parser));
	http_parser_init(parser, HTTP_REQUEST);

	while (1) {
		apr_size_t len = sizeof(buf) - 1; // -1 for a null-terminated
		apr_status_t rv = apr_socket_recv(sock, buf, &len);
		if (rv == APR_EOF || len == 0)
			break;

		// null-terminate the string buffer
		buf[len] = '\0';
		http_parser_execute(parser, &settings, buf, len);

		// TODO client struct needed ?!
		if (strstr(buf, HTTP_CRLF HTTP_CRLF)) {
			break;
		}
    }

//    if (filepath) {
//        apr_status_t rv;
//        apr_file_t *fp;
//
//        if ((rv = apr_file_open(&fp, filepath, APR_READ, APR_OS_DEFAULT, mp)) == APR_SUCCESS) {
//            const char *resp_hdr;
//            apr_size_t len;
//            const char *resp_body;
//
//            apr_finfo_t finfo;
//            apr_file_info_get(&finfo, APR_FINFO_SIZE, fp);
//
//            resp_hdr = apr_psprintf(mp, "HTTP/1.0 200 OK" CRLF_STR "Content-Length: %" APR_OFF_T_FMT CRLF_STR CRLF_STR, finfo.size);
//            len = strlen(resp_hdr);
//            apr_socket_send(sock, resp_hdr, &len);
//
//            resp_body = apr_palloc(mp, finfo.size);
//            len = finfo.size;
//            apr_file_read(fp, (void*)resp_body, &len);
//            apr_socket_send(sock, resp_body, &len);
//
//            return TRUE;
//        }
//    }

    /* error case */
    {
        const char *resp_hdr = "HTTP/1.0 404 Not Found" HTTP_CRLF HTTP_CRLF;
        apr_size_t len = strlen(resp_hdr);
        apr_socket_send(sock, resp_hdr, &len);
        return TRUE;
    }
}

status_code_t websrv_create(web_server_t **ws, runtime_context_t *rtc) {
	TRACE;

	web_server_t *websrv = (web_server_t *)apr_palloc(rtc->mem_pool, sizeof(web_server_t));
	websrv->hostname = apr_pstrdup(rtc->mem_pool, CF_STR(CF_LISTEN_ADDRESS));
	websrv->port = CF_INT(CF_LISTEN_PORT);

	*ws = websrv;

	return SC_OK;
}

status_code_t websrv_start(web_server_t *ws, runtime_context_t *rtc) {
	TRACE;
	ASSERT(ws != NULL);

	apr_pool_t *mp 	= rtc->mem_pool;
	apr_status_t rv = APR_SUCCESS;

	// start listening on host:port
	if (APR_SUCCESS != (rv = s_listen(ws, mp)))
		return SC_WS_LISTEN_FAILED;

	ws->is_running = TRUE;

	while (ws->is_running) {
		apr_socket_t *client_sock;

		rv = apr_socket_accept(&client_sock, ws->sock, mp);
		if (rv != APR_SUCCESS) {
			// just notify
			log_err("Client socket failed !");
//			goto error;
		}

		// specify client socket options
		apr_socket_opt_set(client_sock, APR_SO_NONBLOCK, 0);
		apr_socket_timeout_set(client_sock, -1);

		if (!s_process_packet(client_sock, mp)) {
			log_err("Failed to serve client!");
			//goto error;
		}
		apr_socket_close(client_sock);
	}

	return SC_OK;
}

void websrv_stop(web_server_t *ws) {
	TRACE;
	ASSERT(ws != NULL);

	log_info("Shutting down http server %s:%d ...", ws->hostname, ws->port);
	ws->is_running = FALSE;
}
