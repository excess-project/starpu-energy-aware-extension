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
 *  This interface declares means to retrieve energy measurements from
 *  monitoring framework and then feed the values to StarPU power model.
 *  Please see the {@link } for a
 *  usage example. Per default, a developer should first determine if the
 *  library is installed (#libmfstarpu.a), then initialize StarPU and initialize
 *  Monitoring Framework API via calling #mf_starpu_init, finally submitting 
 *  the training tasks with #mf_starpu_task_training.
 *
 *  @author Fangli Pi
 */

#ifndef __UTILS_H_
#define __UTILS_H_

static const char SERVER[]= "http://192.168.0.160:3030";

/** @brief Initializes Monitoring Framework API
 *
 *  This function initializes the mf_api with user, task and exp_id.
 *  The three variables are set through associated system environments.
 *  During the initialization phase, also number of cpu sockets and gpu devices
 *  are counted, for preparing the metrics for data retrieving
 * 
 *  @return 0 on success; -1 otherwise
 */
int mf_starpu_init(void);


/** @brief Gets the current timestamps in seconds
 *
 *  @return current timestamp
 */
double mf_starpu_time(void);


/** @brief Gets the energy value during the time interval.
 *
 *  The function prepares the metrics for CPUs and GPUs power measurements,
 *  then the average power values are accumulated and energy is calculated by
 *  power times time.
 *
 *  @return energy value
 */
double mf_starpu_get_energy(double start_t, double end_t);


/** @brief Trains StarPU task with energy monitoring data
 *
 *  This function should be called after #mf_starpu_init, 
 *  in this function timestamps are kept before and after tasks execution,
 *  then the energy value is retrieved and feed into the task's
 *  power model.
 *
 *  If a task's execution time is less than 2 seconds, the task will be 
 *  executed for 2 seconds in a loop
 *
 *  @return 0 on success; -1 otherwise
 */
int mf_starpu_task_training(struct starpu_task *task, unsigned nimpl);


/** @brief Feeds StarPU task power model with the measured energy value
 * 
 *  This function is called inside #mf_starpu_task_train, but can also be called 
 *  by general StarPU programms.
 *
 */
void mf_starpu_metrics_feed(struct starpu_task *task, unsigned nimpl, double energy);

#endif