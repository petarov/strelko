/*
  bootstrap.c
  This file is part of e-additives.server

  Copyright (C) 2012 necroleak

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

#include "ead_server.h"
#include "bootstrap.h"

int bs_init() {

	// Init Apache APR
    apr_status_t rv;

    /* per-process initialization */
    rv = apr_initialize();
    if (rv != APR_SUCCESS) {
        ASSERT(0);
        return E_BS_APR_INIT_FAILED;
    }

    return E_BS_OK;
}

void bs_cleanup() {

	apr_terminate();

}

