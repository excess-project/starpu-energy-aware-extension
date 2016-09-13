#!/bin/bash

## Copyright (C) 2016 University of Stuttgart
## Authors: Fangli Pi

module load compiler/intel/parallel_studio_2017
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
INSTALL_PATH_HWLOC=${BINARY_FOLDER}/hwloc

rm -rf ${BINARY_FOLDER}

mkdir ${BINARY_FOLDER}
mkdir ${INSTALL_PATH_STARPU} 
mkdir ${INSTALL_PATH_HWLOC}

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
wget http://gforge.inria.fr/frs/download.php/file/35412/starpu-${STARPU_VERSION}.tar.gz
tar xf starpu-${STARPU_VERSION}.tar.gz
cd starpu-${STARPU_VERSION}
./configure --prefix=${INSTALL_PATH_STARPU} --with-cuda-dir=${CUDA_DIR} --with-cuda-lib-dir=${CUDA_LIB}
make -j 5
make install

#clean
cd $ROOT
rm hwloc-1.11.2.tar.gz
rm starpu-${STARPU_VERSION}.tar.gz
rm -rf hwloc-1.11.2
rm -rf starpu-${STARPU_VERSION}

# ============================================================================ #
# CLONE AND PREPARE jsmn.c jsmn.h                                              #
# ============================================================================ #

cd $ROOT
git clone https://github.com/zserge/jsmn.git
rm -rf ext
mv jsmn ext
