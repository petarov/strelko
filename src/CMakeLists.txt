#-------------------------------------------------------------------------------
# CMakeLists.txt
# 
#  Copyright (C) 2013  Petar Petrov
# 
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
# 
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
# 
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>.
#-------------------------------------------------------------------------------
cmake_minimum_required (VERSION 2.6)

APP_DEBUG_MESSAGE("Building src/ ..." )

set(SOURCES 
    bootstrap.c
    conf_parser.c
    main.c
    logger.c
    rtc.c
    signals.c
    utils.c
    httpserver.c
    http.c
    ../ext/http-parser/http_parser.c
)

APP_DEBUG_MESSAGE("Sources: ${SOURCES}")

### Build #################################################################
add_executable(strelko_srv ${SOURCES})

target_link_libraries(strelko_srv ${APR_LIB_NAME} ${APRUTIL_LIB_NAME} 
    ${CMAKE_THREAD_LIBS_INIT})
