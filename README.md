# STARPU Monitoring Broker

> The design is also named as starpu-energy-aware-extension. It implements functions to train StarPU tasks with energy consumption data, which is fetched from Monitoring Framework by MF APIs. 


## Motivation
StarPU benefits the application in runtime with data management and scheduling policies based on the application's execution time. Now with input from our Monitoring Broker, StarPU is capable to schedule applications based on not only performance but also power. 


## Prerequisites

The StarPU Monitoring Broker requires first a running monitoring framework (with both server and agent). In order to install this requirements, please checkout the associated [monitoring server][server] and [monitoring agent][agent].Please note that the installation and setup steps mentioned below assume that you are running a current Linux as operating system.

Before you can proceed, please clone these repositories:

```bash
git clone git://github.com/excess-project/monitoring-server.git
```

```bash
git clone git://github.com/excess-project/monitoring-agent.git
```
It should be noticed that the following mf-agent plugins and metrics need to be turned on for the StarPU Monitoring Broker.

| Plugins         | Metrics                                         | 
|-----------------|-------------------------------------------------| 
| RAPL            |   PACKAGE_ENERGY:PACKAGE0                       |
| RAPL            |   PACKAGE_ENERGY:PACKAGE1                       |
| RAPL            |   DRAM_ENERGY:PACKAGE0                          |
| RAPL            |   DRAM_ENERGY:PACKAGE1                          |
| NVIDIA          |   power_usage = on                              |
|-----------------|-------------------------------------------------| 

Secondly a working StarPU is required. User can easily get the newest version of StarPU in the homepage http://starpu.gforge.inria.fr/.  

To ease the process of setting up a development enviroment, we provide a basic script that downloads all dependencies, installs them locally in the project directory, and then performs some clean-up operations.

Executing the following script

```bash
./setup.sh
```

results in a new directory named `bin`, which holds the required dependencies for compiling the project. And StarPU source directory is also kept with some important files, like the "config.log", which tells the system's hardware and driver status detected by StarPU.


## Source Building and library create

This section assumes that you've successfully installed all required dependencies as described in the previous paragraphs. 

Executing the following commands 

```bash
source setenv.sh
make clean all
```

compiles and builds the StarPU Monitoring Broker libraries, which can be reused directly in a StarPU application.


## Experiments

All experiments are stored in the directory "examples", and are modified based on the associated StarPU examples to use library generated above. 

Following is a list of all experiments

- dgemm_history
- sgemm_history
- vectorscal_history
- vectorscal_regression


## Acknowledgment

This project is realized through [EXCESS][excess]. EXCESS is funded by the EU 7th Framework Programme (FP7/2013-2016) under grant agreement number 611183. We are also collaborating with the European project [DreamCloud][dreamcloud].


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
| 2016-02-20  | 0.1     | Public release.  |


## License
Copyright (C) 2016 University of Stuttgart

[GNU LESSER GENERAL PUBLIC LICENSE Version 2.1](LICENSE).


[server]: https://github.com/excess-project/monitoring-server
[agent]:  https://github.com/excess-project/monitoring-agent
[excess]: http://www.excess-project.eu
[dreamcloud]: http://www.dreamcloud-project.eu
[plugin-tutorial]: src/plugins/README.md
