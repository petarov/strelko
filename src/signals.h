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

#ifndef SIGNALS_H_
#define SIGNALS_H_

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Custom Unix signals structure. It describes what kind of signals are recognized.
 */
struct signals_t {
	int signum;
	char *signame;
	void (*signalhandler)(int signum);
};
typedef struct signals_t signals_t;

/**
 * Register signal handling
 */
void signals_init();

/**
 * Handle system signals
 * @param signum
 * @remark http://www.cs.utah.edu/dept/old/texinfo/glibc-manual-0.02/library_21.html
 */
void signals_handler(int signum);

#ifdef __cplusplus
}
#endif

#endif /* SIGNALS_H_ */
