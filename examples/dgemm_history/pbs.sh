#!/bin/sh -f
#the job name is "dgemm_mf_starpu"
#PBS -N dgemm_mf_starpu
#PBS -q batch

#use the complite path to the standard output files
#PBS -o /nas_home/hpcfapix/$PBS_JOBID.out
#PBS -e /nas_home/hpcfapix/$PBS_JOBID.err
#PBS -l walltime=00:10:00

#PBS -l nodes=1:node01:ppn=20
ROOT=/nas_home/hpcfapix/starpu-energy-aware-extension/examples/dgemm_history
DBKEY_FILE=/nas_home/hpcfapix/.mf/dbkey/${PBS_JOBID}
DBKEY=$(cat ${DBKEY_FILE})

echo "$( date +'%c' ) DBKEY is : ${DBKEY}"
#start testing on cpu
declare -a SIZE_ARRAY=(8 40 72 104 136 168 200 232)
N=1
ITER=10
EXECUTABLE=${ROOT}/dgemm_mf_starpu

export STARPU_NCPU=1
export STARPU_NCUDA=0
export STARPU_NOPENCL=0
export STARPU_SCHED=dmda
export STARPU_CALIBRATE=1
sleep 5
for SIZE in "${SIZE_ARRAY[@]}"; do
	echo "$( date +'%c' ) [CPU] start dgemm_mf_starpu ..."
	echo "$( date +'%c' ) ${EXECUTABLE} -x ${SIZE} -y ${SIZE} -z ${SIZE} -nblocks ${N} -iter ${ITER} -user ${PBS_USER} -task ${PBS_JOBNAME} -dbkey ${DBKEY}"
	${EXECUTABLE} -x ${SIZE} -y ${SIZE} -z ${SIZE} -nblocks ${N} -iter ${ITER} -user ${PBS_USER} -task ${PBS_JOBNAME} -dbkey ${DBKEY}
	cp ${ROOT}/Metrics.data ${ROOT}/Metrics_cpu_${SIZE}.data
	rm ${ROOT}/Metrics.data
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
	echo "$( date +'%c' ): [GPU] start dgemm_mf_starpu ..."
	echo "$( date +'%c' ) ${EXECUTABLE} -x ${SIZE} -y ${SIZE} -z ${SIZE} -nblocks ${N} -iter ${ITER} -user ${PBS_USER} -task ${PBS_JOBNAME} -dbkey ${DBKEY}"
	${EXECUTABLE} -x ${SIZE} -y ${SIZE} -z ${SIZE} -nblocks ${N} -iter ${ITER} -user ${PBS_USER} -task ${PBS_JOBNAME} -dbkey ${DBKEY}
	cp ${ROOT}/Metrics.data ${ROOT}/Metrics_gpu_${SIZE}.data
	rm ${ROOT}/Metrics.data
	echo "$( date +'%c' ): ending-------------------------------------------------------------------------------"
done