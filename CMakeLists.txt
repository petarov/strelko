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
project(strelko)

set(APP_DEBUG ON)

### Misc macros
macro(APP_DEBUG_MESSAGE _message)
  if (APP_DEBUG)
    message(STATUS "(DEBUG) ${_message}")
  endif (APP_DEBUG)
endmacro(APP_DEBUG_MESSAGE _message)

### Setup #################################################################
APP_DEBUG_MESSAGE("Build type is ${CMAKE_BUILD_TYPE}" )

### includes
include_directories(src test ext/http-parser)

# Setup configuration

set (VERSION_MAJ 1)
set (VERSION_MIN 0)

### configure a header file to pass some of the CMake settings to the source code
configure_file (
  "${PROJECT_SOURCE_DIR}/Config.h.in"
  "${PROJECT_SOURCE_DIR}/src/Config.h"
  )
  
### requirements
# PThread
#find_package(pthread REQUIRED)

# APR lib
set(APRUTIL_LIB_NAME "libaprutil-1.so")
set(APR_LIB_NAME "libapr-1.so")
find_library( 
    APR_LIBRARY
    NAMES ${APR_LIB_NAME}
    PATHS "/usr/lib/" "/usr/local/lib/")
    
if (NOT APR_LIBRARY)
    message( FATAL_ERROR "APR library (${APR_LIB_NAME}) not found!")
endif (NOT APR_LIBRARY)

include_directories(/usr/include/apr-1)

### Debug or Release build
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-g)
    add_definitions(-Wall)
    add_definitions(-D_DEBUG)
    add_definitions(-DDEBUG)
else (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-O3 -DNDEBUG)
endif (CMAKE_BUILD_TYPE STREQUAL "Debug")

if (UNIX AND NOT WIN32)
    add_definitions(-DLINUX_BUILD)
    
    if (CMAKE_SIZEOF_VOID_P MATCHES "8")
    	set(CPU_ARCH 64)
		add_definitions(-DARCH_X64)
    else (CMAKE_SIZEOF_VOID_P MATCHES "8")
		add_definitions(-DARCH_X86)
		set(CPU_ARCH 86)
    endif (CMAKE_SIZEOF_VOID_P MATCHES "8")
endif (UNIX AND NOT WIN32)

add_definitions(-D_LARGEFILE_SOURCE=1)
add_definitions(-D_LARGEFILE64_SOURCE)
add_definitions(-D_FILE_OFFSET_BITS=64)
# We compile in C99 mode
add_definitions(-std=c99)
# wtf -> http://pubs.opengroup.org/onlinepubs/9699919799/xrat/V4_xsh_chap02.html#tag_22_02_02
add_definitions(-D_XOPEN_SOURCE)

### build subdirs
add_subdirectory(src)

