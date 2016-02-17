#!/bin/bash

ROOT=`pwd`
LIB=${ROOT}/src
LIB_MF=${ROOT}/bin/mf/monitoring-agent/lib
PKG_CONFIG_PATH=$PKG_CONFIG_PATH:${ROOT}/bin/starpu/lib/pkgconfig
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:${ROOT}/bin/starpu/lib:$LIB:${LIB_MF}
export PKG_CONFIG_PATH
export LD_LIBRARY_PATH
