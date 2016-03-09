#!/bin/bash

## Copyright (C) 2016 University of Stuttgart
## Authors: Fangli Pi

# CUDA DIRECTORY ON CLUSTER
CUDA_DIR=/opt/compiler/cuda/6.5/
CUDA_LIB=/opt/compiler/cuda/6.5/lib64

ROOT=`pwd`
BINARY_FOLDER=${ROOT}/bin
INSTALL_PATH_STARPU=${BINARY_FOLDER}/starpu

rm -rf ${BINARY_FOLDER}

mkdir ${BINARY_FOLDER}
mkdir ${INSTALL_PATH_STARPU} 

# ============================================================================ #
# DOWNLOAD AND INSTALL STARPU                                                  #
# ============================================================================ #

# get starpu
STARPU_VERSION=1.2.0rc5
cd $ROOT
wget http://starpu.gforge.inria.fr/files/starpu-${STARPU_VERSION}.tar.gz
tar xf starpu-${STARPU_VERSION}.tar.gz
cd starpu-${STARPU_VERSION}
./configure --prefix=${INSTALL_PATH_STARPU} --with-hwloc=${INSTALL_PATH_HWLOC} --with-cuda-dir=${CUDA_DIR} --with-cuda-lib-dir=${CUDA_LIB}
make
make install

#clean
cd $ROOT
rm starpu-${STARPU_VERSION}.tar.gz


# ============================================================================ #
# CLONE AND PREPARE jsmn.c jsmn.h                                              #
# ============================================================================ #

cd $ROOT
git clone https://github.com/zserge/jsmn.git
mv jsmn ext