/*
 * Copyright (C) 2016 University of Stuttgart
 *
 * mf_starpu_utils is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or (at
 * your option) any later version.
 *
 * See the GNU Lesser General Public License in LICENSE for more details.
 */

/** @file mf_starpu_utils.h
 *  @brief Interface to train StarPU tasks with respect to energy consumption.
 *
 *  This interface declares a means to retrieve energy measurements from
 *  monitoring framework and then feed the values to StarPU power model.
 *  Please see the {@link } for a
 *  usage example. Per default, a developer should first determine if the
 *  library is installed (#libmfstarpu.a), then initialize StarPU and 
 *  Monitoring Framework API via calling #mf_starpu_init, finally submitting 
 *  the training tasks with #mf_starpu_task_training.
 *
 *  @author Fangli Pi
 */

#ifndef __UTILS_H_
#define __UTILS_H_

//#include <stddef.h>

/** @brief Macros for various ENERGY_TYPE of Monitoring Framework
 *  CPU_ENERGY: only CPU related metrics are collected
 *  GPU_ENERGY: only GPU related metrics are collected
 *  ALL_ENERGY: both CPU and GPU metrics are collected
 */
#define CPU_ENERGY 1
#define GPU_ENERGY 2
#define ALL_ENERGY 3

static const char SERVER[]= "http://192.168.0.160:3000";

/** @brief Train StarPU task with power model
 *
 *  This function should be called after #mf_starpu_init, 
 *  in this function timestamps are kept before and after tasks execution,
 *  then energy measurements is retrieved and feed into the task's
 *  power model. 
 *
 *  @return 0 on success; -1 otherwise
 */
int mf_starpu_task_training(struct starpu_task *task, unsigned nimpl);


/** @brief Show StarPU prediction of the task's execution time and energy 
 *
 *  This function shows the predicted time and energy of a trained StarPU
 *  task.
 *
 */
void mf_starpu_show_expectation(struct starpu_task *task, unsigned nimpl);


/** @brief Feed StarPU task power model with the measured energy
 * 
 *  This function is called inside #mf_starpu_task_train, but can also be called 
 *  by general StarPU programms.
 *
 */
void mf_starpu_metrics_feed(struct starpu_task *task, unsigned nimpl, double energy);


/** @brief Collects the avergae value of a given metric during the time interval.
 *
 *  This function using mf_api.h and jsmn.h to get the average value of a given metric,
 *  which the monitoring framework measures.
 *
 *  @return average value of the metric
 */
float get_mf_power_data(long double start_time, long double end_time, char *Metric);


/** @brief Collects energy consumption of GPU during the time interval.
 *
 *  This function collects "GPU0:power" and "GPU1:power" based on nvidia plug-in
 *  of the monitoring framework.
 *
 *  @return sum of energy of GPU
 */
double get_gpu_energy(long double start_time, long double end_time);


/** @brief Collects energy consumption of CPU during the time interval.
 *
 *  This function collects "PACKAGE_POWER:PACKAGE0", "PACKAGE_POWER:PACKAGE1" 
 *  "DRAM_POWER:PACKAGE0", and "DRAM_POWER:PACKAGE1" based on rapl plug-in
 *  of the monitoring framework.
 *
 *  @return sum of energy of CPU
 */
double get_cpu_energy(long double start_time, long double end_time);


/** @brief Get the energy value during the time interval according to different
 *  energy types.
 *
 *  When ENERGY_TYPE = CPU_ENERGY: only CPU related metrics are collected
 *  When ENERGY_TYPE = GPU_ENERGY: only GPU related metrics are collected
 *  When ENERGY_TYPE = ALL_ENERGY: both CPU and GPU metrics are collected
 *
 *  @return energy value
 */
double mf_starpu_get_energy(long double start_t, long double end_t, unsigned ENERGY_TYPE);


/** @brief Get execution id for accessing monitoring framework
 *
 *  This function reads the input arguments after "-dbkey" as the execution id
 *  
 *  @return 0 on success; -1 otherwise
 */
int mf_starpu_get_execution_id(int argc, char **argv, char *exe_id);


/** @brief Get train loops for the StarPU task
 *
 *  This function reads the input arguments after "-train" as the number of
 *  train loops.
 *
 *  @return 0 on success; -1 otherwise
 */
int mf_starpu_get_train_loops(int argc, char **argv);


/** @brief Initialize StarPU and Monitoring Framework API
 *
 *  This function initialize StarPU with given #conf, or default configuration
 *  if #conf=NULL, then mf_api is initialized with given execution id.
 * 
 *  @return 0 on success; -1 otherwise
 */
int mf_starpu_init(struct starpu_conf *conf, char *execution_id);


/** @brief Get the current timestamps in seconds
 *
 *  @return current timestamp
 */
long double mf_starpu_time(void);

#endif