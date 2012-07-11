/*
  exit_codes.h
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

#ifndef EXIT_CODES_H_
#define EXIT_CODES_H_

enum status_codes_e {
	SC_OK = 0,

	// Bootstrap errors
	SC_BS_ERR_CMD,
	SC_BS_ERR_LOG,
	SC_BS_ERR_APR_INIT,
	SC_BS_ERR_CONF,

	// Others
	SC_TERMINATED = 0x0face0ff
};
typedef enum status_codes_e status_codes;

#endif /* EXIT_CODES_H_ */
