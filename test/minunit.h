/* file: minunit.h
 * http://www.jera.com/techinfo/jtns/jtn002.html#Setting_Up_A_Test_Case
 */
#ifndef MINUNIT_H_
#define MINUNIT_H_

extern int tests_run;

#define mu_assert(message, test) do { if (!(test)) return message; } while (0)
#define mu_run_test(test) do { char *message = test(); tests_run++; \
                                if (message) return message; } while (0)

// *** added by p.petrov
#define mu_trace(NAME) do { printf("*** Running %s - %s:%d \n", #NAME, __FILE__, __LINE__); \
						fflush(stdout); all_tests++; } while (0)
#define mu_run_testsuite(ts) do { char *message = ts(); \
                                if (message) {fprintf(stderr, "\n[FAILED] %s | %s", #ts, message); \
								return -1;} } while (0)

#endif /* MINUNIT_H_ */
