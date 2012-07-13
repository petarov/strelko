/*
  tests.c
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
#include "minunit.h"
#include "tests.h"

#include <string.h>

int tests_run = 0, all_tests = 0;
static apr_pool_t *test_pool = NULL;

static s_pre() {
	// APR
	apr_initialize();
	apr_pool_initialize();
	apr_pool_create(&test_pool, NULL);
	ASSERT(test_pool != NULL);
}

static s_post() {
	// APR
	ASSERT(test_pool != NULL);
	apr_pool_destroy(test_pool);
	apr_terminate();
}


/************************ UTILS ****************************************************************/

char* tc_utils_1() {
	mu_trace(tc_utils_1);

	char *name = "Firstname=Middlename=Lastname Tartd";
	strtokens_t *t = utils_strsplit(strdup(name), "=", test_pool);
	printf("size: %d\n", t->size);
	char **p = t->token;
	while(*p != NULL) {
		printf("Token = %s\n", *p++);
	}

	mu_assert("Invalid token!", 0 == strncmp(t->token[0], "Firstname", 9));
	mu_assert("Invalid token!", 0 == strncmp(t->token[1], "Middlename", 10));
	mu_assert("Invalid token!", 0 == strncmp(t->token[2], "Lastname Tartd", 14));

	// part 2
	name = "Brothers in )(*)(%)(@#)@(#";
	t = utils_strsplit(strdup(name), " ", test_pool);
	printf("size: %d\n", t->size);
	p = t->token;
	while(*p != NULL) {
		printf("Token = %s\n", *p++);
	}

	mu_assert("Invalid token!", 0 == strcmp(t->token[0], "Brothers"));
	mu_assert("Invalid token!", 0 == strcmp(t->token[1], "in"));
	mu_assert("Invalid token!", 0 == strcmp(t->token[2], ")(*)(%)(@#)@(#"));
	return NULL;
}

char* tc_utils_2() {
	mu_trace(tc_utils_2);

	mu_assert("isbool(1)", utils_isbool("1") );
	mu_assert("isbool(0)", utils_isbool("0") );
	mu_assert("isbool(alfa)", !utils_isbool("alfa") );
	mu_assert("isbool(TRUE)", utils_isbool("TRUE") );
	mu_assert("isbool(true)", utils_isbool("true") );
	mu_assert("isbool(FALSE)", utils_isbool("FALSE") );
	mu_assert("isbool(false)", utils_isbool("false") );
	mu_assert("isbool(fAlse)", !utils_isbool("fAlse") );

	mu_assert("_tobool(TRUE)", utils_tobool("TRUE") == 1);
	mu_assert("_tobool(false)", utils_tobool("false") == 0);
	mu_assert("_tobool(test)", utils_tobool("test") == 1);
	mu_assert("_tobool(falSe)", utils_tobool("falSe") == 1); // ?!
	mu_assert("_tobool(trUe)", utils_tobool("trUe") == 1);
	mu_assert("_tobool(0)", utils_tobool("0") == 0);
	mu_assert("_tobool(1)", utils_tobool("1") == 1);

	mu_assert("_isblank(   BLA $§ BLA)", !utils_isblank("   BLA $§ BLA") );
	mu_assert("_isblank()", utils_isblank("") );
	mu_assert("_isblank( 	)", utils_isblank(" 	") );

	mu_assert("_isnum(1234567890)", utils_isnum("1234567890", 10) );
	mu_assert("_isnum(QWERTZUIOPASDFGHJKLÖ!\"§$%&/()=1234567890ß)", !utils_isnum("QWERTZUIOPASDFGHJKLÖ!\"§$%&/()=1234567890ß", 42) );

	mu_assert("_isstr(YXCVBNMQWERTZUIOPsadfghjklö1234567890%/&()=?$(!)", utils_isstr("YXCVBNMQWERTZUIOPsadfghjkl1234567890%/&()=?$(!", 47) );
	mu_assert("_isstr(1234567890)", utils_isstr("1234567890", 10) );
	mu_assert("_isstr(spaces,tabs)", utils_isstr("\n\r\t", 3) );
	mu_assert("_isstr(öüä)", !utils_isstr("öüä", 3) ); // not ASCII

	return NULL;
}

/************************ LOGGING ****************************************************************/

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

/************************ Test Suits *************************************************************/

char* test_suit_utils() {
	s_pre();

    mu_run_test(tc_utils_1);
    mu_run_test(tc_utils_2);

	s_post();
	return NULL;
}

int run_all_test_suits() {

	// all utils tests
	mu_run_testsuite(test_suit_utils);

	printf("\n Successful tests: %d/%d \n", tests_run, all_tests);
	return 0;
}
