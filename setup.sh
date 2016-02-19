#!/bin/bash

## Copyright (C) 2016 University of Stuttgart
## Authors: Fangli Pi

ROOT=`pwd`
BINARY_FOLDER=${ROOT}/bin
INSTALL_PATH_HWLOC=${BINARY_FOLDER}/hwloc
INSTALL_PATH_STARPU=${BINARY_FOLDER}/starpu
INSTALL_PATH_MF=${BINARY_FOLDER}/mf

rm -rf ${BINARY_FOLDER}

mkdir ${BINARY_FOLDER}
mkdir ${INSTALL_PATH_HWLOC}
mkdir ${INSTALL_PATH_STARPU} 
mkdir ${INSTALL_PATH_MF} 

# ============================================================================ #
# DOWNLOAD AND INSTALL STARPU                                                  #
# ============================================================================ #

# get hwloc
cd $ROOT
wget https://www.open-mpi.org/software/hwloc/v1.11/downloads/hwloc-1.11.2.tar.gz
tar xf hwloc-1.11.2.tar.gz
cd hwloc-1.11.2
./configure --prefix=${INSTALL_PATH_HWLOC}
make
make install

# get starpu
STARPU_VERSION=1.2.0rc5
cd $ROOT
wget http://starpu.gforge.inria.fr/files/starpu-${STARPU_VERSION}.tar.gz
tar xf starpu-${STARPU_VERSION}.tar.gz
cd starpu-${STARPU_VERSION}
./configure --prefix=${INSTALL_PATH_STARPU} --with-hwloc=${INSTALL_PATH_HWLOC}
make
make install

#clean
cd $ROOT
rm hwloc-1.11.2.tar.gz
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
git checkout tags/1.0.1 -b 1.0.1
./setup.sh

echo "> monitoring agent"
cd ${INSTALL_PATH_MF}
git clone https://github.com/excess-project/monitoring-agent
cd monitoring-agent
git checkout tags/1.0.1 -b 1.0.1
./setup.sh
make
make install
echo "Done"


# ============================================================================ #
# CLONE AND PREPARE jsmn.c jsmn.h                                              #
# ============================================================================ #

cd $ROOT
git clone https://github.com/zserge/jsmn.git
mv jsmn ext