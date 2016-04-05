# STARPU Monitoring Broker Examples

> The examples given here is based on examples from StarPU. Only changes are inserted to use StarPU Monitoring Broker to collects energy measurements from Monitoring Framework, and then to feed the measurements to StarPU task's power model.


## dgemm_history

Simple parallel double GEMM implementation: partition the output matrix in the two dimensions, and the input matrices in the corresponding dimension, and perform the output computations in parallel.

StarPU power model type is "STARPU_HISTORY_BASED".

Following functions of Monitoring Broker are used:

- mf_starpu_get_user
- mf_starpu_get_task
- mf_starpu_get_experiment_id
- mf_starpu_init
- mf_starpu_task_training

The function #mf_starpu_task_training will execute a given StarPU task for at least 2 seconds before collecting the energy values from monitoring framework. If the execution time of the task is less than 2 seconds, the task will be iterated for multiple times untill the total execution time reach 2 seconds. This implementation eases the usage of this function for no need to take the execution time into account.


## sgemm_history

Simple parallel single GEMM implementation: partition the output matrix in the two dimensions, and the input matrices in the corresponding dimension, and perform the output computations in parallel.

Other implementations is the same as dgemm_history.


## vectorscal_history

This example performs scaling an array by a factor using StarPU. 

StarPU power model type is "STARPU_HISTORY_BASED".

Following functions of Monitoring Broker are used:

- mf_starpu_get_user
- mf_starpu_get_task
- mf_starpu_get_experiment_id
- mf_starpu_get_train_loops
- mf_starpu_init
- mf_starpu_time
- mf_starpu_get_energy
- mf_starpu_metrics_feed

After #mf_starpu_init, StarPU task create, handle the registered data set, and task submission are done inside a train loop, while timestamps are captured around the loop. Number of train loops equals to the number of tasks, which StarPU created. Before the tasks are destroyed by hand, energy data is fetched and feed for each trained task.

To enable real data calculation and data transfers, all values of the array are generated inside the task codelet. 


## vectorscal_regression

This example performs scaling an array by a factor using StarPU. 

Only difference with "vectorscal_history" is that the StarPU power model type is "STARPU_REGRESSION_BASED".