/*
  testhelpers.c
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
#include "utils/helpers.h"
#include "tests.h"

void testtokens_1() {
	apr_pool_initialize();
	apr_pool_t *mp = NULL;
	apr_pool_create(&mp, NULL);
	char *name = "Firstname=Middlename=Lastname Tartd";
	printf("starting... \n");

	strtokens_t *t = hlp_strsplit(strdup(name), ";", mp);
	printf("size: %d\n", t->size);
	char **p = t->token;
	while(*p != NULL) {
		printf("Token = %s\n", *p++);
	}

	apr_pool_destroy(mp);
}

