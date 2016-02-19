#!/bin/bash

## Copyright (C) 2016 University of Stuttgart
## Authors: Fangli Pi

ROOT=`pwd`
BINARY_FOLDER=${ROOT}/bin
INSTALL_PATH_STARPU=${BINARY_FOLDER}/starpu
INSTALL_PATH_MF=${BINARY_FOLDER}/mf

rm -rf ${BINARY_FOLDER} ${INSTALL_PATH_STARPU} ${INSTALL_PATH_MF}

mkdir ${BINARY_FOLDER} 
mkdir ${INSTALL_PATH_STARPU} 
mkdir ${INSTALL_PATH_MF} 

# ============================================================================ #
# DOWNLOAD AND INSTALL STARPU                                                  #
# ============================================================================ #

STARPU_VERSION=1.2.0rc5
cd $ROOT
wget http://starpu.gforge.inria.fr/files/starpu-${STARPU_VERSION}.tar.gz
tar xf starpu-${STARPU_VERSION}.tar.gz
cd starpu-${STARPU_VERSION}
./configure --prefix=${INSTALL_PATH_STARPU}
make
make install
cd $ROOT
rm starpu-${STARPU_VERSION}.tar.gz

# ============================================================================ #
# DOWNLOAD AND INSTALL MF                                                      #
# ============================================================================ #
#
# REQUIRED SOFTWARE
#
echo "Checking required software ..."
echo "> git"
command -v git >/dev/null 2>&1 || { echo " git  : Not installed. Aborting." >&2; exit 1; }
echo "Done."

#
# CLONE AND INSTALL SERVER
#
echo "Downloading and installing components ..."
echo "> monitoring server"
cd ${INSTALL_PATH_MF}
git clone https://github.com/excess-project/monitoring-server
cd monitoring-server
./setup.sh

echo "> monitoring agent"
cd ${INSTALL_PATH_MF}
git clone https://github.com/excess-project/monitoring-agent
cd monitoring-agent
git checkout tags/1.0.2
git pull
./setup.sh
make
make install
echo "Done"

#
# CLONE AND PREPARE jsmn.c jsmn.h
#
cd $ROOT
git clone https://github.com/zserge/jsmn.git
mv jsmn ext