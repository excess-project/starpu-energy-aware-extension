#!/bin/bash

ROOT=`pwd`
LIB=${ROOT}/src
LIB_MF=${ROOT}/bin/mf/monitoring-agent/lib
LIB_HWLOC=${ROOT}/bin/hwloc/lib
PKG_HWLOC=${ROOT}/bin/hwloc/lib/pkgconfig
PKG_CONFIG_PATH=$PKG_CONFIG_PATH:${ROOT}/bin/starpu/lib/pkgconfig:${PKG_HWLOC}
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:${ROOT}/bin/starpu/lib:$LIB:${LIB_MF}:${LIB_HWLOC}
export PKG_CONFIG_PATH
export LD_LIBRARY_PATH
