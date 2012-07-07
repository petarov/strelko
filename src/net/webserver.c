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
#include "bootstrap.h"
#include "utils/logger.h"
#include "rtc.h"
#include "webserver.h"

#define LISTEN_PORT		8081
#define BUFSIZE			4096
#define CRLF_STR		"\r\n"

static apr_status_t do_listen(apr_socket_t **sock, apr_pool_t *mp);
static int do_serv_task(apr_socket_t *serv_sock, apr_pool_t *mp);

void ws_start(runtime_context_t *rt_ctx) {
	TRACE;

	apr_pool_t *mp = rt_ctx->mem_pool;
	apr_socket_t *s;/* listening socket */

	int rv = do_listen(&s, mp);
	if (rv != APR_SUCCESS) {
		goto error;
	}

	while (1) {
		apr_socket_t *ns;

		rv = apr_socket_accept(&ns, s, mp);
		if (rv != APR_SUCCESS) {
			goto error;
		}

		/* it is a good idea to specify socket options for the newly accepted socket explicitly */
		apr_socket_opt_set(ns, APR_SO_NONBLOCK, 0);
		apr_socket_timeout_set(ns, -1);

		if (!do_serv_task(ns, mp)) {
			goto error;
		}
		apr_socket_close(ns);
	}

error:
	return;
}

/**
 * Create a listening socket, and listen it.
 */
static apr_status_t do_listen(apr_socket_t **sock, apr_pool_t *mp)
{
    apr_status_t rv;
    apr_socket_t *s;
    apr_sockaddr_t *sa;

    rv = apr_sockaddr_info_get(&sa, NULL, APR_INET, LISTEN_PORT, 0, mp);
    if (rv != APR_SUCCESS) {
        return rv;
    }

    rv = apr_socket_create(&s, sa->family, SOCK_STREAM, APR_PROTO_TCP, mp);
    if (rv != APR_SUCCESS) {
        return rv;
    }

    /* it is a good idea to specify socket options explicitly.
     * in this case, we make a blocking socket as the listening socket */
    apr_socket_opt_set(s, APR_SO_NONBLOCK, 0);
    apr_socket_timeout_set(s, -1);
    apr_socket_opt_set(s, APR_SO_REUSEADDR, 1);/* this is useful for a server(socket listening) process */

    rv = apr_socket_bind(s, sa);
    if (rv != APR_SUCCESS) {
        return rv;
    }
    rv = apr_socket_listen(s, SOMAXCONN);
    if (rv != APR_SUCCESS) {
        return rv;
    }

    *sock = s;
    return rv;
}

static int do_serv_task(apr_socket_t *sock, apr_pool_t *mp)
{
    int is_firstline = TRUE;
    const char *filepath = NULL;

    while (1) {
    	char buf[BUFSIZE];
		apr_size_t len = sizeof(buf) - 1; // -1 for a null-terminated
		apr_status_t rv = apr_socket_recv(sock, buf, &len);
		if (rv == APR_EOF || len == 0)
			break;

		// null-terminate the string buffer
		buf[len] = '\0';

		if (is_firstline) {
			char **tokens;
			apr_tokenize_to_argv(buf, &tokens, mp);
			if (tokens[0] && tokens[1] && strcmp(tokens[0], "GET") == 0) {
				filepath = tokens[1];
			}
			is_firstline = FALSE;
		}
		if (strstr(buf, CRLF_STR CRLF_STR)) {/* expect the end of the request. no guarantee */
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
        const char *resp_hdr = "HTTP/1.0 404 Not Found" CRLF_STR CRLF_STR;
        apr_size_t len = strlen(resp_hdr);
        apr_socket_send(sock, resp_hdr, &len);
        return TRUE;
    }
}
