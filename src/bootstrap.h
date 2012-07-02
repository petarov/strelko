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

/**
 * Error codes the bootstrap process returns
 */
enum bs_errors_e {
	E_BS_OK,
	E_BS_LOG_FAILED,
	E_BS_APR_FAILED
};
typedef enum bs_errors_e bs_errors_e;


/**
 * Initialize system variables and modules
 * @return Error code during initialization. E_BS_OK means all is successful.
 */
int bs_init();

/**
 * Application entry point
 */
void bs_start();

/**
 * Frees all resources before closing the application
 */
void bs_cleanup();

#endif /* BOOTSTRAP_H_ */
