#!/bin/sh
#
# Author: p.petrov
# Desc: Static code analysis based on clang-analyzer
# Changes:
#
########################################################

### Define vars

DISTRO=`uname -r | cut -d- -f3`
CCC_PATH=/usr/lib/clang-analyzer/scan-build/ccc-analyzer
BUILD=$1
CORES=2

### Validations

if [ "$DISTRO" != "ARCH" ]; then
	echo "WARN: This script is adapted to run on ArchLinux systems!"
fi

if [ ! -e $CCC_PATH ]; then
	echo "Cound not find ccc-analyzer at $CCC_PATH !"
	exit;
fi

if [ "$BUILD" = "" ]; then
	echo "Build mode not set! Use [Debug|Release] as command line param."
	exit;
fi

### Generate

cmake -DCMAKE_BUILD_TYPE="$BUILD" -DCMAKE_C_COMPILER=$CCC_PATH

scan-build -V --use-cc=gcc make -j$CORES

