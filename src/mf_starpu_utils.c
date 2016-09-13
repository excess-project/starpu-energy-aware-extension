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

#include <unistd.h>
#include <starpu.h>
#include <starpu_scheduler.h>
#include <starpu_sched_ctx.h>
#include <starpu_perfmodel.h>
#include <starpu_worker.h>
#include <starpu_profiling.h>
#include <starpu_task.h>
#include <starpu_util.h>
#include <hwloc.h>
#include <mf_api.h>
#include <jsmn.h>
#include <math.h>
#include "mf_starpu_utils.h"

#define MAX_METRICS_NUM 6

static int num_cpus = 0;
static int num_gpus = 0;

/***************************************************************************
*
*  INTERNAL FUNCTIONS
*
***************************************************************************/

/* Counts the number of CPU sockets based on hwloc library */
int hardware_sockets_count(void);

/* Sets the metrics based on hardward (CPUs, GPUs) */
void set_all_metrics(char **metrics, int *metrics_num);

/* Sets the nth metric in the metrics array */
void set_the_nth_metric(char **metrics, int n, char *name);

/* Gets the sum of all power based on the given metrics during the given time interval */
float get_mf_power_data(double start_time, double end_time, char **metrics, int metrics_num);


/* Initializes Monitoring Framework API */
int mf_starpu_init(void)
{
	char *user = getenv("MF_USER");
	char *task  = getenv("MF_TASKID");
	char *exp_id = getenv("MF_EXPID");

	if(user == NULL || task == NULL || exp_id == NULL) {
		printf("ERROR: power monitoring could not be initialized because: \n");
		printf("\t\tmf runtime environments are not set.\n");
		return -1;
	}
	char *starpu_ncuda = getenv("STARPU_NCUDA");
	num_gpus = atoi(starpu_ncuda);
	num_cpus = hardware_sockets_count();
	
	/* initialize mf api */
	mf_api_initialize(SERVER, user, exp_id, task);
	
	/* enable starpu profiling */
	if(starpu_profiling_status_set(STARPU_PROFILING_ENABLE) < 0) {
    	printf("ERROR: starpu_profiling_status_set failed.\n");
    	return -1;
	}
	return 0;
}

/* Gets the current timestamps in seconds */
double mf_starpu_time()
{
	struct timespec mf_date;
	clock_gettime(CLOCK_REALTIME, &mf_date);
	double mf_time = mf_date.tv_sec + (double) (mf_date.tv_nsec / 10e8);
	return mf_time;
}

/* Gets the energy value during the time interval */
double mf_starpu_get_energy(double start_t, double end_t)
{
	int i;
	double power = 0.0;
	double energy = 0.0;
	char **Metrics = (char**) 0;
	Metrics = (char **) malloc (MAX_METRICS_NUM * sizeof(char *));
	for(i=0; i<MAX_METRICS_NUM; i++) {
		Metrics[i] = (char *)0;
	}
	int Metrics_num = 0;
	set_all_metrics(Metrics, &Metrics_num);
	if((end_t -start_t) <= 0.5) {
		sleep(1);
	}
	power = get_mf_power_data(start_t, end_t, Metrics, Metrics_num);
	for(i=0; i<MAX_METRICS_NUM; i++) {
		free(Metrics[i]);
	}
	free(Metrics);
	energy = power * (end_t - start_t);
	return energy;
}

/* Trains StarPU task with energy monitoring data */
int mf_starpu_task_training(struct starpu_task *task, unsigned nimpl)
{
	struct timespec task_start_date;
	struct timespec task_end_date;
	double task_start_time, task_end_time, task_duration;
	int loops = 1;
	int ret, i;
	task->synchronous = 1; //important for mf_starpu
	task->destroy = 0; //task will be destroyed by hand, important for mf_starpu
	
	clock_gettime(CLOCK_REALTIME, &task_start_date);

	ret = starpu_task_submit(task);
	if(ret == -ENODEV){
		printf("\n[ERROR] starpu_task_submit failed.\n");
		return -1;
	}
	clock_gettime(CLOCK_REALTIME, &task_end_date);

	task_start_time = task_start_date.tv_sec + (double) (task_start_date.tv_nsec / 10e8);
	task_end_time = task_end_date.tv_sec + (double) (task_end_date.tv_nsec / 10e8);
	task_duration = (double) (task_end_time - task_start_time)/ loops;

	while ((task_end_time - task_start_time) < 2.0 ) {
		/*if task execution time is less than 2 seconds, repeat the task execution till 2 seconds*/
		loops = (int) 2 / task_duration + 1;
		clock_gettime(CLOCK_REALTIME, &task_start_date);

		for (i=0; i<loops; i++) {
			ret = starpu_task_submit(task);
			if(ret == -ENODEV){
				printf("\n[ERROR] starpu_task_submit failed.\n");
				return -1;
			}
		}

		clock_gettime(CLOCK_REALTIME, &task_end_date);
		task_start_time = task_start_date.tv_sec + (double) (task_start_date.tv_nsec / 10e8);
		task_end_time = task_end_date.tv_sec + (double) (task_end_date.tv_nsec / 10e8);
		task_duration = (double) (task_end_time - task_start_time)/ loops;
		//printf("\nloops is %d\n", loops);
		//printf("\ntask_duration is :%Lf\n", task_duration);
	}

	double energy = (double) mf_starpu_get_energy(task_start_time, task_end_time) / loops;

	mf_starpu_metrics_feed(task, nimpl, energy);

    starpu_task_destroy(task);
    return 0;
}

/* Feeds StarPU task power model with the measured energy value */
void mf_starpu_metrics_feed(struct starpu_task *task, unsigned nimpl, double energy)
{
	struct starpu_perfmodel *Power_model = task->cl->power_model;
	unsigned workerid = task->profiling_info->workerid;
	unsigned sched_ctx_id = starpu_sched_ctx_get_context();
	struct starpu_perfmodel_arch *arch = starpu_worker_get_perf_archtype(workerid, sched_ctx_id);

	starpu_perfmodel_update_history(Power_model, task, arch, workerid, nimpl, energy);
}

int hardware_sockets_count(void)
{
	int depth;
	int skts_num = 0;
	hwloc_topology_t topology;
	hwloc_topology_init(&topology);
	hwloc_topology_load(topology);
	depth = hwloc_get_type_depth(topology, HWLOC_OBJ_SOCKET);
	if (depth == HWLOC_TYPE_DEPTH_UNKNOWN) {
		printf("*** The number of sockets is unknown\n");
		return -1;
	}
	skts_num=hwloc_get_nbobjs_by_depth(topology, depth);
	return skts_num;
}

void set_all_metrics(char **metrics, int *metrics_num)
{
	int i, j;
	for (i=0, j=0; i < num_cpus; i++){
		char metric_package[32]={'\0'};
		char metric_dram[32]={'\0'};
		sprintf(metric_package, "PACKAGE_POWER:PACKAGE%d", i);
		sprintf(metric_dram, "DRAM_POWER:PACKAGE%d", i);
		set_the_nth_metric(metrics, j, metric_package);
		set_the_nth_metric(metrics, j+1, metric_dram);
		j = j+2;
	}
	for (i=0; i < num_gpus; i++){
		char metric_gpu[32]={'\0'};
		sprintf(metric_gpu, "GPU%d:power", i);
		set_the_nth_metric(metrics, j, metric_gpu);
		j++;
	}
	*metrics_num = j;
}

void set_the_nth_metric(char **metrics, int n, char *name)
{
	metrics[n] = malloc(32 * sizeof(char));
	strcpy(metrics[n], name);
}

float get_mf_power_data(double start_time, double end_time, char **metrics, int metrics_num)
{
	int i, r;
	float avg = 0.0;
	float avg_sum = 0.0;
	char tmp_string[2048]={'\0'};
	char tmp_avg[50]={'\0'};
	jsmn_parser p;
	jsmntok_t t[640];	// We expect no more than 640 tokens

	//printf("\nGet power data from MF:\n");
	char *response = (char *)0;
   	response = malloc(10000 * sizeof(char));
   	if(response == NULL){
   		printf("\nERROR: get_mf_power_data(), response malloc failed.\n");
   		return -1;
   	}
   	//memset(response, 0, 10000);
   	mf_api_stats_metrics_by_interval(metrics, metrics_num, start_time, end_time, response);
   	if(strlen(response) == 0 || strstr(response, "null") != NULL || strstr(response, "error") != NULL){
   		printf("\nERROR: Get energy data from MF failed.\n");
   		return -1;
   	}
   	//get the float avg value
	jsmn_init(&p);
	r = jsmn_parse(&p, response, strlen(response), t, sizeof(t)/sizeof(t[0]));
	for (i = 1; i < r; i++) {
		memset(tmp_string, 0, 2048);
		memset(tmp_avg, 0, 50);
		strncpy(tmp_string, response+t[i].start, t[i].end-t[i].start);
		if(strcmp(tmp_string, "avg") == 0) {
			avg = 0.0;
			strncpy(tmp_avg, response+t[i+1].start, t[i+1].end-t[i+1].start);
			sscanf(tmp_avg, "%f", &avg);
			i++;
			if(avg <= 0) {
				free(response);
				return -1;
			}
			avg_sum += avg;
			//printf("\n[DEBUG] tmp_avg is : %s\n", tmp_avg);
		}
	}
	free(response);
	return avg_sum;
}