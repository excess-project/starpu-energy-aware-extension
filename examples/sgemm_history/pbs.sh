#!/bin/sh -f
#the job name is "sgemm_mf_starpu"
#PBS -N sgemm_mf_starpu
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

echo "$( date +'%c' ) DBKEY is : ${DBKEY}"
#start testing on cpu
declare -a SIZE_ARRAY=(8 40 72 104 136 168 200 232)
N=1
ITER=10
EXECUTABLE=${ROOT}/examples/sgemm_history/sgemm_mf_starpu

export STARPU_NCPU=1
export STARPU_NCUDA=0
export STARPU_NOPENCL=0
export STARPU_SCHED=dmda
export STARPU_CALIBRATE=1
sleep 5
for SIZE in "${SIZE_ARRAY[@]}"; do
	echo "$( date +'%c' ) [CPU] start sgemm_mf_starpu ..."
	echo "$( date +'%c' ) ${EXECUTABLE} -x ${SIZE} -y ${SIZE} -z ${SIZE} -nblocks ${N} -iter ${ITER} -user ${PBS_USER} -task ${PBS_JOBNAME} -exp ${DBKEY}"
	${EXECUTABLE} -x ${SIZE} -y ${SIZE} -z ${SIZE} -nblocks ${N} -iter ${ITER} -user hpcfapix -task ${PBS_JOBNAME} -exp ${DBKEY}
	echo "$( date +'%c' ): ending-------------------------------------------------------------------------------"
done

#start testing on gpu
export STARPU_NCPU=0
export STARPU_NCUDA=1
export STARPU_NOPENCL=0
export STARPU_SCHED=dmda
export STARPU_CALIBRATE=1
sleep 5
for SIZE in "${SIZE_ARRAY[@]}"; do
	echo "$( date +'%c' ): [GPU] start sgemm_mf_starpu ..."
	echo "$( date +'%c' ) ${EXECUTABLE} -x ${SIZE} -y ${SIZE} -z ${SIZE} -nblocks ${N} -iter ${ITER} -user ${PBS_USER} -task ${PBS_JOBNAME} -exp ${DBKEY}"
	${EXECUTABLE} -x ${SIZE} -y ${SIZE} -z ${SIZE} -nblocks ${N} -iter ${ITER} -user hpcfapix -task ${PBS_JOBNAME} -exp ${DBKEY}
	echo "$( date +'%c' ): ending-------------------------------------------------------------------------------"
done
