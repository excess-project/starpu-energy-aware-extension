#!/bin/sh -f
#the job name is "vector_regression_mf_starpu"
#PBS -N vector_regression_mf_starpu
#PBS -q night

#use the complite path to the standard output files
#PBS -o /nas_home/hpcfapix/$PBS_JOBID.out
#PBS -e /nas_home/hpcfapix/$PBS_JOBID.err
#PBS -l walltime=00:30:00
#PBS -l nodes=1:node03:ppn=24

module load amd/app-sdk/3.0.124.132-GA
module load mpi/mpich/3.1-gnu-4.9.2
module load compiler/cuda/7.0
module load numlib/intel/mkl/11.1
module load compiler/intel/14.0.2
module load compiler/gnu/4.9.2

ROOT=/nas_home/hpcfapix/starpu-energy-aware-extension

LIB=${ROOT}/src
LIB_MF=/opt/mf/stable/16.2/lib/
PKG_CONFIG_PATH=$PKG_CONFIG_PATH:${ROOT}/bin/starpu/lib/pkgconfig
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:${ROOT}/bin/starpu/lib:$LIB:${LIB_MF}
export PKG_CONFIG_PATH
export LD_LIBRARY_PATH

DBKEY_FILE=/nas_home/hpcfapix/.mf/dbkey/${PBS_JOBID}
DBKEY=$(cat ${DBKEY_FILE})
PBS_USER=hpcfapix

echo "$( date +'%c' ) DBKEY is : ${DBKEY}"
#start testing on cpu
declare -a SIZE_ARRAY=(1200 1400 1600 2000 2400 3200 4800 9600 19200 24000)
TRAIN_LOOPS=100000
EXECUTABLE=${ROOT}/examples/vectorscal_regression/vector_scal

sleep 5
for SIZE in "${SIZE_ARRAY[@]}"; do
	echo "$( date +'%c' ): start vector_scal, size = ${SIZE}, user = ${PBS_USER}, task = ${PBS_JOBNAME}, exp = ${DBKEY}, TRAIN_LOOPS = ${TRAIN_LOOPS}"
	${EXECUTABLE} -NX ${SIZE} -user ${PBS_USER} -task ${PBS_JOBNAME} -exp ${DBKEY} -train ${TRAIN_LOOPS}
	echo "$( date +'%c' ): ending-------------------------------------------------------------------------------"
done