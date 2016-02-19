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

#include <unistd.h>
#include <starpu.h>
#include <starpu_scheduler.h>
#include <starpu_sched_ctx.h>
#include <starpu_perfmodel.h>
#include <starpu_worker.h>
#include <starpu_profiling.h>
#include <starpu_task.h>
#include <starpu_util.h>
#include <mf_api.h>
#include <jsmn.h>
#include "mf_starpu_utils.h"

int mf_starpu_get_execution_id(int argc, char **argv, char *exe_id)
{
	int i;
	for (i = 1; i < argc; i++)
	{
		if (strcmp(argv[i], "-dbkey") == 0)
		{
			strcpy(exe_id, argv[++i]);
			break;
		}
	}
	if(strlen(exe_id) == 0) {
		printf("ERROR: no mf execution_id is given.\n");
		return -1;
	}
	return 0;
}

int mf_starpu_get_train_loops(int argc, char **argv)
{
	int i, loops;
	loops = 0;
	for (i = 1; i < argc; i++)
	{
		if (strcmp(argv[i], "-train") == 0)
		{
			char *argptr;
			loops = strtol(argv[++i], &argptr, 10);
			break;
		}
	}
	if(loops == 0) {
		printf("ERROR: no train loops is given.\n");
		return -1;
	}
	return loops;
}

int mf_starpu_init(struct starpu_conf *conf, char *execution_id)
{
	int ret = starpu_init(conf);
	if (ret == -ENODEV) {
		printf("ERROR: starpu_init failed.\n");
		return -1;
	}
	mf_api_initialize(SERVER, execution_id);
	if(starpu_profiling_status_set(STARPU_PROFILING_ENABLE) < 0) {
     //enable profiling failed
    	printf("ERROR: starpu_profiling_status_set failed.\n");
    	return -1;
	}
	return 0;
}

long double mf_starpu_time()
{
	struct timespec mf_date;
	clock_gettime(CLOCK_REALTIME, &mf_date);
	long double mf_time = mf_date.tv_sec + (long double) (mf_date.tv_nsec / 10e8);
	return mf_time;
}

double mf_starpu_get_energy(long double start_t, long double end_t, unsigned ENERGY_TYPE)
{
	double energy;
	if(ENERGY_TYPE == CPU_ENERGY) {
		energy = get_cpu_energy(start_t, end_t);
	}
	else if(ENERGY_TYPE == GPU_ENERGY) {
		energy = get_gpu_energy(start_t, end_t);
	}
			
	else if(ENERGY_TYPE == ALL_ENERGY) {
		energy =  get_cpu_energy(start_t, end_t);
		energy += get_gpu_energy(start_t, end_t);
	}
	else {
		printf("ERROR: mf_starpu_get_energy unknown ENERGY_TYPE %d.\n", ENERGY_TYPE);
		return -1;
	}
	if(energy > 0){
		return energy;	
	}
	else {
		printf("ERROR: mf_starpu_get_energy failed.\n");
		return -1;
	}	
}

double get_cpu_energy(long double start_time, long double end_time)
{
	int i;
	double package0, package1, dram0, dram1, energy;
	for (i = 0; i < 10; i++) {
		package0 = (double) get_mf_power_data(start_time, end_time, "PACKAGE_POWER:PACKAGE0");
		package1 = (double) get_mf_power_data(start_time, end_time, "PACKAGE_POWER:PACKAGE1");
		dram0 = (double) get_mf_power_data(start_time, end_time, "DRAM_POWER:PACKAGE0");
		dram1 = (double) get_mf_power_data(start_time, end_time, "DRAM_POWER:PACKAGE1");
 		if(package0<0 || package1<0 || dram0<0 || dram1<0) {
 			sleep(10);
 		}
 		else {
 			energy = (double) (package0 + package1 + dram0 + dram1) * (end_time - start_time);
 			//printf("\nget_cpu_energy is : %f.\n", energy);
 			return energy;		
 		}
	}
	return -1;
}

double get_gpu_energy(long double start_time, long double end_time)
{
	int i;
	double gpu0, gpu1, energy;
	for (i = 0; i < 10; i++) {
		gpu0 = (double) get_mf_power_data(start_time, end_time, "GPU0:power");
		gpu1 = (double) get_mf_power_data(start_time, end_time, "GPU1:power");
		if(gpu0<0 || gpu1<0 ) {
	 		sleep(10);
	 	}
	 	else {
 			energy = (double) (gpu0 + gpu1) * (end_time - start_time);
 			//printf("\nget_gpu_energy is : %f.\n", energy);
 			return energy;		
 		}
	}
	return -1;
}

//get from mf_agent the power data, using mf_api.h and jsmn.h
float get_mf_power_data(long double start_time, long double end_time, char *Metric) 
{
	int i, r;
	float avg = -1.0;
	char tmp_string[100]={'\0'};
	char tmp_avg[50]={'\0'};
	jsmn_parser p;
	jsmntok_t t[128]; /* We expect no more than 128 tokens */

	printf("\nGet energy data from MF:\n");
   	char *response = malloc(10000 * sizeof(char));
   	if(response == NULL){
   		printf("\nERROR: get_mf_power_data(), response malloc failed.\n");
   		return -1;
   	}
   	memset(response, 0, 10000);
   	stats_data_by_metric(Metric, start_time, end_time, response);
   	
   	if(strlen(response) == 0){
   		printf("\nERROR: Get energy data from MF failed.\n");
   		return -1;
   	}
   	//get the float avg value
	jsmn_init(&p);
	r = jsmn_parse(&p, response, strlen(response), t, sizeof(t)/sizeof(t[0]));
	
	for (i = 1; i < r; i++) {
		memset(tmp_string, 0, 100);
		memset(tmp_avg, 0, 50);
		strncpy(tmp_string, response+t[i].start, t[i].end-t[i].start);
		if(strcmp(tmp_string, "avg") == 0) {
			strncpy(tmp_avg, response+t[i+1].start, t[i+1].end-t[i+1].start);
			//printf("\n[DEBUG] tmp_average value is : %s\n", tmp_average);
			sscanf(tmp_avg, "%f", &avg);
			i++;
			break;
			//printf("\nmax_string is :%s\nmax_value is: %f\n", tmp_max, max);
		}
	}
	//printf("\n[DEBUG] average %s: %.3f\n", Metric, average);
	if(response != NULL && strlen(response) != 0) {
		//printf("\nresponse length is %d\n", strlen(response));
		free(response);
	}
    return avg;
}

void mf_starpu_metrics_feed(struct starpu_task *task, unsigned nimpl, double energy)
{
	//printf("\n[INFO] starpu_perfmodel_update_history...\n");
	struct starpu_perfmodel *Power_model = task->cl->power_model;
	unsigned workerid = task->profiling_info->workerid;
	unsigned sched_ctx_id = starpu_sched_ctx_get_context();
	struct starpu_perfmodel_arch *arch = starpu_worker_get_perf_archtype(workerid, sched_ctx_id);

	starpu_perfmodel_update_history(Power_model, task, arch, workerid, nimpl, energy);
}

void mf_starpu_show_expectation(struct starpu_task *task, unsigned nimpl)
{
	//printf("\nstart of mf_starpu_show_task_t....\n");
	unsigned workerid = task->profiling_info->workerid;
	unsigned sched_ctx_id = starpu_sched_ctx_get_context();
	struct starpu_perfmodel_arch *arch = starpu_worker_get_perf_archtype(workerid, sched_ctx_id);

	double T_execution = starpu_task_expected_length(task, arch, nimpl);
	double Consumption = starpu_task_expected_power(task, arch, nimpl);
	printf("\n[INFO] T_execution = %f\n", T_execution);
	printf("\n[INFO] Consumption = %f\n", Consumption);
}

int mf_starpu_task_training(struct starpu_task *task, unsigned nimpl)
{
	struct timespec task_start_date;
	struct timespec task_end_date;
	task->synchronous = 1; //important for mf_starpu
	task->destroy = 0; //task will be destroyed by hand, important for mf_starpu
	
	clock_gettime(CLOCK_REALTIME, &task_start_date);

	int ret = starpu_task_submit(task);
	if(ret == -ENODEV){
		printf("\n[ERROR] starpu_task_submit failed.\n");
		return -1;
	}
	
	clock_gettime(CLOCK_REALTIME, &task_end_date);

	long double task_start_time = task_start_date.tv_sec + (long double) (task_start_date.tv_nsec / 10e8);
	long double task_end_time = task_end_date.tv_sec + (long double) (task_end_date.tv_nsec / 10e8);

	double energy = mf_starpu_get_energy(task_start_time, task_end_time, ALL_ENERGY);

	mf_starpu_metrics_feed(task, nimpl, energy);

    starpu_task_destroy(task);
    return 0;
}