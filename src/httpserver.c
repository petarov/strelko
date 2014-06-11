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

#include "globals.h"
#include "logger.h"
#include "conf_parser.h"
#include "httpserver.h"
#include "http_parser.h"

#define BUFSIZE			4096
#define POLL_CONN_SIZE  100
#define POLL_TIMEOUT	(APR_USEC_PER_SEC * 30)

static apr_status_t s_listen(web_server_t *ws, apr_pool_t *mp) {
	TRACE;

    apr_status_t rv;
    apr_socket_t *s;
    apr_sockaddr_t *soaddr;

    log_debug("Starting http server on %s:%d", ws->hostname, ws->port);

    if (APR_SUCCESS != (rv = apr_sockaddr_info_get(&soaddr, NULL, APR_INET,
    		ws->port, 0, mp)))
    	goto error;

    if (APR_SUCCESS != (rv = apr_socket_create(&s, soaddr->family, SOCK_STREAM,
    		APR_PROTO_TCP, mp)))
    	goto error;

    /* 
     * Specify socket options explicitly.
     * Listening socket is a non-blocking socket.
     */
    apr_socket_opt_set(s, APR_SO_NONBLOCK, 1);
    /**
     * [a] When you want a non-blocking socket, set it to 
     * 'APR_SO_NONBLOCK==1(on) and timeout==0'. Refer to:
     * dev.ariel-networks.com/apr/apr-tutorial/html/apr-tutorial-13.html#ss13.4
     */
    apr_socket_timeout_set(s, 0);
    
    apr_socket_opt_set(s, APR_SO_REUSEADDR, 1);

    if (APR_SUCCESS != (rv = apr_socket_bind(s, soaddr)))
    	goto error;

    if (APR_SUCCESS != (rv = apr_socket_listen(s, SOMAXCONN)))
    	goto error;

    // assign successfully created socket
    ASSERT(s);
    ws->sock = s;

error:
    return rv;
}

static status_code_t s_wc_create(web_client_t **wc, runtime_context_t *rtc) {
    
    web_client_t *client = (web_client_t *)apr_palloc(rtc->mem_pool, 
            sizeof(web_client_t));
    
    apr_pool_create(&(client->mem_pool), rtc->mem_pool);
    
    client->done = FALSE;
    client->bucket = (char *)apr_palloc(client->mem_pool, BUFSIZE * 2);
    client->blen = 0;
    
    *wc = client;
    
    return SC_OK;
} 

static int request_uri_cb(http_parser *parser, const char *p, size_t len) {
	log_debug("Request: %s", p);
    
    web_client_t *client = (web_client_t *) parser->data;
    strtokens_t *tokens = utils_strsplit((char *)p, " ", client->mem_pool);
    log_debug("tokens: %s", tokens->token[0]);
    if (tokens->size > 0) {
        client->req.uri = apr_pstrdup(client->mem_pool, tokens->token[0]);
    }
	
	return 0;
}

static int http_header_field_cb(http_parser *parser, const char *p, size_t length) {
//	printf("HDR: %s", p);
	// TODO: save last header key/val
	// TODO: add to new header key
	return 0;
}

static int http_header_value_cb(http_parser *parser, const char *p, size_t length) {
//	printf("HDR: %s", p);
	// TODO: save to current hdr value
	// TODO: set flag that we have value
	return 0;
}

static int http_header_done_cb(http_parser *parser) {
	// TODO: save last header key/val
	return 0;
}

static int http_body_cb(http_parser *parser, const char *p, size_t length) {
//	printf("BODY: %s", p);
	// TODO: add to body data
	return 0;
}

static int http_message_complete_cb(http_parser *parser) {
	web_client_t *client = (web_client_t *) parser->data;
	client->done = TRUE;
	// TODO: finalize request parsing
	return 0;
}

static void s_process_client(void *clientptr, apr_pollset_t *pollset) {
	TRACE;
	ASSERT(clientptr != NULL);
    
	web_client_t *client = (web_client_t *) clientptr;
	char buf[BUFSIZE];

	int server_err = FALSE;
	while (!client->done) {
		apr_size_t recv_len = sizeof(buf) - 1; // -1 for a null-terminated
		apr_status_t rv = apr_socket_recv(client->sock, buf, &recv_len);
		if (rv == APR_EOF || recv_len == 0)
			break;

		// null-terminate the string buffer
		buf[recv_len] = '\0';
        
        memcpy(client->bucket + client->blen, buf, recv_len);
        client->blen += recv_len;
        
    	/*
		 * The request line and headers must all end with <CR><LF>
		 * Note that this is a nasty hack! Improper GET requests would
		 * block the thread as the while(1) loop will continue forever because
		 * the socket_recv will keep blocking - waiting for more data.
		 */
		if (strstr(buf, HTTP_CRLF HTTP_CRLF)) {
            client->done = TRUE;
            
            log_debug("Bucket: %s", client->bucket);
            
            // change socket status
            apr_pollfd_t pfd = { client->mem_pool, APR_POLL_SOCKET, APR_POLLIN, 
                0, { NULL }, client };
            pfd.desc.s = client->sock;
            apr_pollset_remove(pollset, &pfd);
            
            pfd.reqevents = APR_POLLOUT;
            apr_pollset_add(pollset, &pfd);
            
            // parse HTTP request
            http_parser parser;
            parser.data = clientptr;
            http_parser_init(&parser, HTTP_REQUEST);
            
            http_parser_settings settings = {
                NULL, request_uri_cb, NULL, http_header_field_cb, 
                http_header_value_cb, http_header_done_cb, http_body_cb, 
                http_message_complete_cb
            };
            
            apr_size_t parsed_len = http_parser_execute(&parser, &settings, 
                    client->bucket, client->blen);

            if (parsed_len != client->blen) {
                log_err("Request failed! Parsed length=%d, received=%d",
                        parsed_len, client->blen);
                server_err = TRUE;
                break;
            } else {
                // DUMMY RESPONSE -> TODO
                const char *resp_hdr = "HTTP/1.0 501 Internal Server Error" \
                        HTTP_CRLF HTTP_CRLF;
                apr_size_t len = strlen(resp_hdr);
                apr_socket_send(client->sock, resp_hdr, &len);
                
            }
		}
	}

//    apr_file_t *apr_file = NULL;
//    const conf_opt_t *opt = conf_get_opt(CF_DOCUMENT_ROOT, client->rtc);
//    char filepath[PATH_MAX];
//    
//    snprintf(filepath, PATH_MAX, "%s%s", opt->u.str_val, client->req->uri);
//    
//    log_debug("Serving from %s ", filepath);
//    
//	apr_status_t rv = apr_file_open(&apr_file, filepath, 
//            APR_FOPEN_READ | APR_FOPEN_BUFFERED | APR_FOPEN_SENDFILE_ENABLED, 
//            APR_FPROT_OS_DEFAULT, client->mem_pool);
//	if (rv == APR_SUCCESS) {
//        
//        apr_finfo_t finfo;
//        rv = apr_file_info_get(&finfo, APR_FINFO_NORM, apr_file);
//        if (rv != APR_SUCCESS) {
//            server_err = TRUE;
//            APR_ERR_PRINT(rv);
//            goto cleanup;
//        }
//        
//        // set response headers
//        char rfc822[APR_RFC822_DATE_LEN + 1];
//        apr_rfc822_date(rfc822, apr_time_now());
//        
//        struct iovec headers[5];
//        headers[0].iov_base = "HTTP/1.1 200 OK" HTTP_CRLF;
//        headers[0].iov_len = strlen(headers[0].iov_base);
//        headers[1].iov_base = apr_psprintf(client->mem_pool, 
//                "Date: %s" HTTP_CRLF, rfc822);
//        headers[1].iov_len = strlen(headers[1].iov_base);
//        headers[2].iov_base = apr_psprintf(client->mem_pool, 
//                "Content-Length: %" APR_OFF_T_FMT HTTP_CRLF, finfo.size);
//        headers[2].iov_len = strlen(headers[2].iov_base);
//        headers[3].iov_base = "Content-Type: text/html" HTTP_CRLF;
//        headers[3].iov_len = strlen(headers[3].iov_base);
//        headers[4].iov_base = HTTP_CRLF;
//        headers[4].iov_len = strlen(headers[4].iov_base);
//        //        headers[1].iov_base = "Connection: Close" HTTP_CRLF;
////        headers[1].iov_len = strlen(headers[1].iov_base);
//        
//        apr_hdtr_t hdr;
//        hdr.headers = headers;
//        hdr.numheaders = 5;
//        hdr.numtrailers = 0;
//        
//        apr_off_t offset = 0;
//        apr_size_t len = finfo.size;
//        apr_size_t expected_len = 0;
//        for (int i = 0; i < hdr.numheaders; i++) {
//            log_debug("HDR: %s", headers[i].iov_base);
//            expected_len += headers[i].iov_len;
//        }
//        expected_len += finfo.size;
//        
//        rv = apr_socket_sendfile(client->sock, apr_file, &hdr, &offset, &len, 0);
//        if (rv != APR_SUCCESS) {
//            server_err = TRUE;
//        } else if (len != expected_len) {
//            log_warn("Sent %" APR_SIZE_T_FMT " of %" APR_SIZE_T_FMT " bytes", 
//                    len, expected_len);
//        }
//    } else {
//		log_err("File not found - %s", filepath);
//		APR_ERR_PRINT(rv);
//		const char *resp_hdr = "HTTP/1.0 404 Not Found" \
//				HTTP_CRLF HTTP_CRLF;
//		apr_size_t len = strlen(resp_hdr);
//		apr_socket_send(client->sock, resp_hdr, &len);        
//    }

cleanup:
    
	if (server_err) {
		const char *resp_hdr = "HTTP/1.0 500 Internal Server Error" \
				HTTP_CRLF HTTP_CRLF;
		apr_size_t len = strlen(resp_hdr);
		apr_socket_send(client->sock, resp_hdr, &len);
	}

    // remove socket
    apr_pollfd_t pfd = { client->mem_pool, APR_POLL_SOCKET, APR_POLLOUT, 0, 
        { NULL }, client };
    pfd.desc.s = client->sock;
    apr_pollset_remove(pollset, &pfd);
    
	apr_socket_close(client->sock);
	/*
	 * Destroy memory pool and sub-pools
	 */
	apr_pool_destroy(client->mem_pool);
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

	apr_status_t rv = APR_SUCCESS;
    apr_pollset_t *pollset;
    apr_int32_t num;
    const apr_pollfd_t *ret_pfd;    
    
	// start listening on host:port
	if (APR_SUCCESS != (rv = s_listen(ws, rtc->mem_pool)))
		return SC_WS_LISTEN_FAILED;

    // set up a pollset object
    apr_pollset_create(&pollset, POLL_CONN_SIZE, rtc->mem_pool, 0);
    {
        apr_pollfd_t pfd = { rtc->mem_pool, APR_POLL_SOCKET, APR_POLLIN, 
            0, { NULL }, NULL };
        pfd.desc.s = ws->sock;
        apr_pollset_add(pollset, &pfd);
    }

	ws->is_running = TRUE;

	while (ws->is_running) {
        rv = apr_pollset_poll(pollset, POLL_TIMEOUT, &num, &ret_pfd);
        if (APR_SUCCESS != rv)
            continue;
        
        ASSERT(num > 0);
        for (int i = 0; i < num; i++) {
            if (ret_pfd[i].desc.s == ws->sock) {
                // listening socket is readable -> new connection
                apr_socket_t *client_sock;

                rv = apr_socket_accept(&client_sock, ws->sock, rtc->mem_pool);
                if (rv != APR_SUCCESS) {
                    // just notify
                    log_err("Socket accept failed!");
                    APR_ERR_PRINT(rv);
                    continue;
                }

                // specify client socket options
                apr_socket_opt_set(client_sock, APR_SO_NONBLOCK, 1);
                apr_socket_timeout_set(client_sock, 0);

                // create new client
                web_client_t *client = NULL;
                if (SC_OK != s_wc_create(&client, rtc)) {
                    log_warn("Failed to create client!");
                    continue;
                }
                client->sock = client_sock;
                
                apr_pollfd_t pfd = { client->mem_pool, APR_POLL_SOCKET, 
                    APR_POLLIN, 0, { NULL }, client };
                pfd.desc.s = client->sock;

                apr_pollset_add(pollset, &pfd);
            } else {
                // incoming socket data
                log_debug("SOCKET[%d]: data has arrived.", i);
                s_process_client(ret_pfd[i].client_data, pollset);
            }
        }
	}

	return SC_OK;
}

void httpsrv_stop(web_server_t *ws) {
	TRACE;

	if (ws != NULL) {
		log_info("Shutting down http server %s:%d ...", ws->hostname, ws->port);
		ws->is_running = FALSE;
	}
}
