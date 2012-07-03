/*
  config.h
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

#ifndef CONFIG_H_
#define CONFIG_H_

struct config_t {
	char srv_bind_address[16];
	int	 srv_bind_port;
};
typedef struct config_t config_t;

/**
 * Initialize configuration given command line arguments
 * @param Ptr to uninitialized config_t structure
 * @param argc
 * @param argv
 */
int cfg_create(config_t **cfg, int argc, char* argv[]);

/**
 * Clean up configuration structure and release memory
 */
void cfg_destroy(config_t **cfg);

#endif /* CONFIG_H_ */
