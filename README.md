# STARPU Monitoring Broker

> The design is also named as starpu-energy-aware-extension. It implements functions to train StarPU tasks with energy consumption data, which is fetched from Monitoring Framework by MF APIs. 


## Motivation
StarPU benefits the application in runtime with data management and scheduling policies based on the application's execution time. Now with input from our Monitoring Broker, StarPU is capable to schedule applications based on not only performance but also power. 


## Prerequisites

The StarPU Monitoring Broker requires first a running monitoring framework (with both server and agent). In order to install this requirements, please checkout the associated [monitoring server][server] and [monitoring agent][agent].Please note that the installation and setup steps mentioned below assume that you are running a current Linux as operating system.

It should be noticed that the following mf-agent plugins and metrics need to be turned on for the StarPU Monitoring Broker.

| Plugins         | Metrics                                         | 
|-----------------|-------------------------------------------------| 
| RAPL            |   PACKAGE_ENERGY:PACKAGE0                       |
| RAPL            |   PACKAGE_ENERGY:PACKAGE1                       |
| RAPL            |   DRAM_ENERGY:PACKAGE0                          |
| RAPL            |   DRAM_ENERGY:PACKAGE1                          |
| NVIDIA          |   power_usage                                   |


Secondly a working StarPU is required. User can easily get the newest version of StarPU in the homepage http://starpu.gforge.inria.fr/.  

To ease the process of setting up a development enviroment, we provide a basic script that downloads all dependencies, installs them locally in the project directory, and then performs some clean-up operations.

Executing the following script locally 

```bash
./setup.sh
```

, or the following script on EXCESS cluster

```bash
./setup_on_cluster.sh
```

results in a new directory named `bin`, which holds the required dependencies for compiling the project. And StarPU source directory is also kept with some important files, like the "config.log", which tells the system's hardware and driver status detected by StarPU.


## Source Building and library create

This section assumes that you've successfully installed all required dependencies as described in the previous paragraphs. 

Executing the following commands locally

```bash
source setenv.sh
make clean all
```

, or the following script on EXCESS cluster

```bash
source setenv_on_cluster.sh
make clean all
```

compiles and builds the StarPU Monitoring Broker libraries, which can be reused directly in a StarPU application.


## Examples

All experiments are stored in the directory "examples", and are modified based on the associated StarPU examples to use library generated above. 

Following is a list of all experiments

- dgemm_history
- sgemm_history
- vectorscal_history
- vectorscal_regression

For more details please refer to the README file in the "examples" directory.


## Tutorial

The StarPU Monitoring Broker works like a normal library including the following functions (version 1.1):

- int mf_starpu_get_user(int argc, char **argv, char *user);
- int mf_starpu_get_task(int argc, char **argv, char *task);
- int mf_starpu_get_experiment_id(int argc, char **argv, char *exp_id);
- int mf_starpu_get_train_loops(int argc, char **argv);
- int mf_starpu_init(struct starpu_conf *conf, char *user, char *task, char *exp_id);
- int mf_starpu_task_training(struct starpu_task *task, unsigned nimpl);
- double mf_starpu_get_energy(long double start_t, long double end_t, unsigned ENERGY_TYPE);
- void mf_starpu_metrics_feed(struct starpu_task *task, unsigned nimpl, double energy);
- void mf_starpu_show_expectation(struct starpu_task *task, unsigned nimpl);
- long double mf_starpu_time(void);

Following is a modified simple example based on StarPU "mult" example, illustrating how to use the StarPU Monitoring Broker library.

```bash
#include <starpu.h>
#include "mf_starpu_utils.h"
 
/* skip unmodified source code */
  
/* data structure to hold the power model */
static struct starpu_perfmodel dgemm_power_model = {
	.type = STARPU_HISTORY_BASED,
	.symbol = dgemm_history_power
};
  
static struct starpu_codelet cl = {
	.type = STARPU_SEQ,
	.max_parallelism = INT_MAX,
	.cpu_funcs = {cpu_mult},
#ifdef STARPU_USE_CUDA
	.cuda_funcs = {cublas_mult},
#endif
	.nbuffers = 3,
	.modes = {STARPU_R, STARPU_R, STARPU_RW},
	.model = &dgemm_perf_model,
	.power_model = &dgemm_power_model /* link power model to codelet */
};
 
int main(int argc, char **argv) 
{
    double start, end;
    int ret;
    
    char user[40] = {'\0'};
    char task[40] = {'\0'};
    char exp_id[40] = {'\0'};

    if(mf_starpu_get_user(argc, argv, user) == -1)
      return -1;
    if(mf_starpu_get_task(argc, argv, task) == -1)
      return -1;
    if(mf_starpu_get_experiment_id(argc, argv, exp_id)==-1)
      return -1;

    parse_args(argc, argv);
    
    if (mf_starpu_init(NULL, user, task, exp_id) == -1)
      return -1;

    /* skipped unmodified source code */

    unsigned x, y, iter;
    for (iter = 0; iter < niter; iter++) {
      for (x = 0; x < nslicesx; x++)
      for (y = 0; y < nslicesy; y++) {
        struct starpu_task *task = starpu_task_create();
  
        /* skipped unmodified source code */
        
        /* starpu_task_submit() 
        0 is the nimpl */
        ret = mf_starpu_task_training(task, 0); 
        if(ret == -1)
          ret = 77;
          goto enodev;
      }
  
      starpu_task_wait_for_all();
    }

    /* skipped unmodified source code */

    starpu_cublas_shutdown();
    starpu_shutdown();
  
    return ret;
}
```

## Acknowledgment

This project is realized through [EXCESS][excess]. EXCESS is funded by the EU 7th Framework Programme (FP7/2013-2016) under grant agreement number 611183.


## Contributing
Find a bug? Have a feature request?
Please [create](https://github.com/excess-project/starpu-energy-aware-extension/website/issues) an issue.


## Main Contributors

**Dennis Hoppe, HLRS**
+ [github/hopped](https://github.com/hopped)

**Fangli Pi, HLRS**
+ [github/hpcfapix](https://github.com/hpcfapix)

**Dmitry Khabi, HLRS**


## Release History

| Date        | Version | Comment          |
| ----------- | ------- | ---------------- |
| 2016-02-20  | 1.0     | Public release.  |
| 2016-04-05  | 1.1     | Public release.  |


## License
Copyright (C) 2016 University of Stuttgart

[GNU LESSER GENERAL PUBLIC LICENSE Version 2.1](LICENSE).


[server]: https://github.com/excess-project/monitoring-server
[agent]:  https://github.com/excess-project/monitoring-agent
[excess]: http://www.excess-project.eu
[dreamcloud]: http://www.dreamcloud-project.eu
[plugin-tutorial]: src/plugins/README.md
