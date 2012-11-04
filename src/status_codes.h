/*
  exit_codes.h
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

#ifndef EXIT_CODES_H_
#define EXIT_CODES_H_

#ifdef __cplusplus
extern "C" {
#endif

enum status_code_e {
	SC_OK = 0,
	SC_FAILED = 1,

	// bootstrap errors
	SC_BS_ERR_CMD = 201,
	SC_BS_ERR_LOG,
	SC_BS_ERR_APR_INIT,
	SC_BS_ERR_CONF,

	// web server errors
	SC_WS_LISTEN_FAILED,

	// xml database errors
	SC_XMLDB_ERR_FILE = 801,
	SC_XMLDB_ERR_HEADER = 802,
	SC_XMLDB_ERR_LANGUAGE = 803,
	SC_XMLDB_ERR_NO_DATABASES = 810,

	// others
	SC_TERMINATED = 0x0face0ff
};
typedef enum status_code_e status_code_e;

#ifdef __cplusplus
}
#endif

#endif /* EXIT_CODES_H_ */
