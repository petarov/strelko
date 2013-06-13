/*
  bootstrap.h
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

#ifndef BOOTSTRAP_H_
#define BOOTSTRAP_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialize system variables and modules
 * @param argc Command line arguments count
 * @param argv Command line arguments
 * @return Initialization status code. SC_OK means all was successful.
 */
status_code_t bs_init(int argc, char* argv[]);

/**
 * Application entry point
 */
void bs_start();

/**
 * Stop all threads and processes
 */
void bs_stop();

/**
 * Destroy acquired resources
 */
void bs_destroy();

#ifdef __cplusplus
}
#endif

#endif /* BOOTSTRAP_H_ */
