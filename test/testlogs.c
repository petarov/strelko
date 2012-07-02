/*
  testlogs.c
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

#include "tests.h"
#include "utils/logger.h"

void test_logs_1() {

	int rv = log_init("Test app");
	if (rv != 0) {
		fprintf(stderr, "Failed to init log system err = %d", rv);
		assert( 0);
	}

	log_debug("This is a debug message 1.");
	log_debug("This is a debug message with ID=%d and ID=%d", 2, 3);
	log_debug("This is a debug message with STR=%s and ID=%d", "Test string", 3);

	log_notice("This is a notice message.");
	log_info("This is a info message.");
	log_warn("This is a warning message.");
	log_err("This is an error message!");
	log_crit("This is a CRITICAL error message!!");

	log_close();
}

void test_logs_2() {

	int rv = log_init_ex("Test app", 2);
	if (rv != 0) {
		fprintf(stderr, "Failed to init log system err = %d", rv);
		assert(0);
	}

	log_close();
}