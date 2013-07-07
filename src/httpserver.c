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
#include "http_parser.h"

#define BUFSIZE			4096

static apr_status_t s_listen(web_server_t *ws, apr_pool_t *mp) {
	TRACE;

	apr_status_t rv;
    apr_socket_t *s;
    apr_sockaddr_t *sa;

    log_debug("Starting http server on %s:%d", ws->hostname, ws->port);

    if (APR_SUCCESS != (rv = apr_sockaddr_info_get(&sa, NULL, APR_INET,
    		ws->port, 0, mp)))
    	goto error;

    if (APR_SUCCESS != (rv = apr_socket_create(&s, sa->family, SOCK_STREAM,
    		APR_PROTO_TCP, mp)))
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

static int request_uri_cb(http_parser *parser, const char *p, size_t len) {
	web_client_t *client = (web_client_t *) parser->data;
	client->req->uri = apr_pstrdup(client->rtc->mem_pool, p);

	//TODO: split CRLF
	log_debug("URI: %s", client->req->uri);
	return 0;
}

static int http_header_cb(http_parser *parser, const char *p, size_t length) {
//	printf("HDR: %s", p);
	return 0;
}

static int http_body_cb(http_parser *parser, const char *p, size_t length) {
	printf("BODY: %s", p);
	return 0;
}

static void s_process_client(void *clientptr) {
	TRACE;
	ASSERT(clientptr != NULL);

	web_client_t *client = (web_client_t *) clientptr;
	ASSERT(client != NULL);

	http_request_t *req = (http_request_t *) apr_palloc(client->rtc->mem_pool,
			sizeof(http_request_t));
	ASSERT(req != NULL);
	client->req = req;

	char buf[BUFSIZE];
	http_parser_settings settings;
	settings.on_header_field = http_header_cb;
	settings.on_header_value = http_header_cb;
	settings.on_url = request_uri_cb;
	settings.on_body = http_body_cb;

	http_parser *parser = apr_palloc(client->rtc->mem_pool,
			sizeof(http_parser));
	parser->data = clientptr;
	http_parser_init(parser, HTTP_REQUEST);

	int server_err = FALSE;
	while (1) {
		apr_size_t recv_len = sizeof(buf) - 1; // -1 for a null-terminated
		apr_status_t rv = apr_socket_recv(client->sock, buf, &recv_len);
		if (rv == APR_EOF || recv_len == 0)
			break;

		// null-terminate the string buffer
		buf[recv_len] = '\0';
		apr_size_t parsed_len = http_parser_execute(
				parser, &settings, buf, recv_len);

		if (parsed_len != recv_len) {
			log_err("Request failed! Parsed length=%d, recieved=%d",
					parsed_len, recv_len);
			server_err = TRUE;
			break;
		}

		/*
		 * The request line and headers must all end with <CR><LF>
		 * Note that this is a nasty hack! Improper GET requests would
		 * block the thread as the while(1) loop will continue forever because
		 * the socket_recv will keep blocking - waiting for more data.
		 */
		if (strstr(buf, HTTP_CRLF HTTP_CRLF)) {
			break;
		}
	}

	log_debug("Locating resource %s", client->req->uri);

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

	if (server_err) {
		const char *resp_hdr = "HTTP/1.0 500 Internal Server Error" \
				HTTP_CRLF HTTP_CRLF;
		apr_size_t len = strlen(resp_hdr);
		apr_socket_send(client->sock, resp_hdr, &len);
	}

	apr_socket_close(client->sock);
	pthread_exit(0);
}

status_code_t httpsrv_create(web_server_t **ws, runtime_context_t *rtc) {
	TRACE;

	web_server_t *websrv = (web_server_t *)apr_palloc(rtc->mem_pool,
			sizeof(web_server_t));
	websrv->hostname = apr_pstrdup(rtc->mem_pool, CF_STR(CF_LISTEN_ADDRESS));
	websrv->port = CF_INT(CF_LISTEN_PORT);

	*ws = websrv;

	return SC_OK;
}

status_code_t httpsrv_start(web_server_t *ws, runtime_context_t *rtc) {
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

		web_client_t *client = (web_client_t *)apr_palloc(rtc->mem_pool,
				sizeof(web_client_t));
		client->sock = client_sock;
		client->connected = TRUE;
		client->rtc = rtc;

		int rc = pthread_create(&client->thread, NULL,
				(void *)s_process_client, (void *)client);

		// TODO: remove
		pthread_join(client->thread, NULL);
	}

	return SC_OK;
}

void httpsrv_stop(web_server_t *ws) {
	TRACE;
//	ASSERT(ws != NULL);
	if (ws != NULL) {
		log_info("Shutting down http server %s:%d ...", ws->hostname, ws->port);
		ws->is_running = FALSE;
		// TODO: block while all threads are stopped?
	}
}
