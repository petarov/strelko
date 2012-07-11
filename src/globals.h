/*
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

#ifndef EAD_SERVER_H_
#define EAD_SERVER_H_

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include <syslog.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <stdint.h>
typedef uint8_t		uint8;
typedef uint32_t	uint32;
typedef uint64_t	uint64_t;
typedef int8_t		int8;
typedef int32_t		int32;
typedef int64_t		int64;

//#ifdef __off64_t
//typedef __off64_t off64_t;
//#endif

#include <assert.h>
#ifdef DEBUG
#define ASSERT(x) assert(x)
#else
#define ASSERT(x)
#endif

#ifndef PATH_MAX
#define PATH_MAX 256
#endif

#include <apr.h>
#include <apr_general.h>
#include <apr_pools.h>
#include <apr_file_io.h>
#include <apr_strings.h>
#include <apr_network_io.h>

#define APR_ERR_PRINT(rv) { char buf[512]; apr_strerror(rv, buf, sizeof(buf)); \
	log_err("APR ERROR: %s", buf); \
}

#include "status_codes.h"


#endif /* EAD_SERVER_H_ */
