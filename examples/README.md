# STARPU Monitoring Broker Examples

> The examples given here is based on examples from StarPU. Only changes are inserted to use StarPU Monitoring Broker to collects energy measurements from Monitoring Framework, and then to feed the measurements to StarPU task's power model.


## dgemm_history

Simple parallel double GEMM implementation: partition the output matrix in the two dimensions, and the input matrices in the corresponding dimension, and perform the output computations in parallel.

StarPU power model type is "STARPU_HISTORY_BASED".

Following functions of Monitoring Broker are used:

- mf_starpu_init
- mf_starpu_task_training

The function #mf_starpu_task_training will execute a given StarPU task for at least 2 seconds before collecting the energy values from monitoring framework. If the execution time of the task is less than 2 seconds, the task will be iterated for multiple times untill the total execution time reach 2 seconds. This implementation eases the usage of this function for no need to take the execution time into account.


## sgemm_history

Simple parallel single GEMM implementation: partition the output matrix in the two dimensions, and the input matrices in the corresponding dimension, and perform the output computations in parallel.

Other implementations is the same as dgemm_history.


## vectorscal_history

This example performs scaling an array by a factor using StarPU. Same functions of the Monitoring Broker are used as the above two examples.


## mpi_cholesky

Cholesky decomposition is distributed on two nodes of EXCESS cluster by MPI (Message Passing Interface) controlled by StarPU in this example. 

Following functions of Monitoring Broker are used:

- mf_starpu_init
- mf_starpu_time
- mf_starpu_get_energy

After the execution, the energy consumption for each cpu is reported during its execution time.