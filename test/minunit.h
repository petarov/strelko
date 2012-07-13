/* file: minunit.h
 * http://www.jera.com/techinfo/jtns/jtn002.html#Setting_Up_A_Test_Case
 */
#ifndef MINUNIT_H_
#define MINUNIT_H_

// Modifications by by p.petrov

#define mu_assert(message, test) do { if (!(test)) { \
									char buf[1024]; sprintf(buf, "%s (--> %s:%d)", message, __FILE__, __LINE__); \
									return apr_pstrdup(test_pool, buf); } \
								} while (0)
#define mu_run_test(test) do { char *message = test(); tests_run++; \
                                if (message) return message; } while (0)

#define mu_trace(NAME) do { printf("*** Running %s - %s:%d \n", #NAME, __FILE__, __LINE__); \
						fflush(stdout); all_tests++; } while (0)
#define mu_run_testsuite(ts) do { char *message = ts(); \
                                if (message) {fprintf(stderr, "\n[FAILED] %s | %s", #ts, message); \
								fflush(stderr); return -1;} } while (0)

#endif /* MINUNIT_H_ */
