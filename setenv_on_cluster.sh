#!/bin/bash

module load amd/app-sdk/3.0.124.132-GA
module load mpi/mpich/3.1-gnu-4.9.2
module load compiler/cuda/7.0
module load numlib/intel/mkl/11.1
module load compiler/intel/14.0.2
module load compiler/gnu/4.9.2

ROOT=`pwd`
LIB=${ROOT}/src
LIB_MF=/opt/mf/stable/16.2/lib/

PKG_CONFIG_PATH=$PKG_CONFIG_PATH:${ROOT}/bin/starpu/lib/pkgconfig:${PKG_HWLOC}
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:${ROOT}/bin/starpu/lib:$LIB:${LIB_MF}
export PKG_CONFIG_PATH
export LD_LIBRARY_PATH
