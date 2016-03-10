#!/bin/bash

## Copyright (C) 2016 University of Stuttgart
## Authors: Fangli Pi

module load compiler/intel/14.0.2
module load compiler/gnu/4.9.2
module load amd/app-sdk/3.0.124.132-GA
module load mpi/mpich/3.1-gnu-4.9.2
# CUDA DIRECTORY ON CLUSTER
module load compiler/cuda/7.0
CUDA_DIR=/opt/compiler/cuda/7.0/
CUDA_LIB=/opt/compiler/cuda/7.0/lib64

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
./configure --prefix=${INSTALL_PATH_STARPU} --with-cuda-dir=${CUDA_DIR} --with-cuda-lib-dir=${CUDA_LIB}
make
make install

#clean
cd $ROOT
rm starpu-${STARPU_VERSION}.tar.gz
rm  -rf starpu-${STARPU_VERSION}

# ============================================================================ #
# CLONE AND PREPARE jsmn.c jsmn.h                                              #
# ============================================================================ #

cd $ROOT
git clone https://github.com/zserge/jsmn.git
mv jsmn ext