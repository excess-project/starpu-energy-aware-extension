/* StarPU --- Runtime system for heterogeneous multicore architectures.
 *
 * Copyright (C) 2010, 2011, 2012, 2013  Centre National de la Recherche Scientifique
 * Copyright (C) 2010-2012  Université de Bordeaux
 * Copyright (C) 2016 University of Stuttgart
 *
 * StarPU is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or (at
 * your option) any later version.
 *
 * StarPU is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU Lesser General Public License in COPYING.LGPL for more details.
 */

/*
 * This example demonstrates how to use StarPU to scale an array by a factor.
 * It shows how to manipulate data with StarPU's data management library.
 *  1- how to declare a piece of data to StarPU (starpu_vector_data_register)
 *  2- how to describe which data are accessed by a task (task->handles[0])
 *  3- how a kernel can manipulate the data (buffers[0].vector.ptr)
 */

#include <starpu.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <mf_api.h>
#include "mf_starpu_utils.h"

#define FPRINTF(ofile, fmt, ...) do { if (!getenv("STARPU_SSILENT")) {fprintf(ofile, fmt, ## __VA_ARGS__); }} while(0)

extern void scal_cpu_func(void *buffers[], void *_args);
extern void scal_cuda_func(void *buffers[], void *_args);
extern void scal_opencl_func(void *buffers[], void *_args);

static struct starpu_perfmodel vector_scal_model =
{
	.type = STARPU_HISTORY_BASED,
	.symbol = "vector_scal_history"
};

static struct starpu_perfmodel vector_scal_power_model =
{
	.type = STARPU_HISTORY_BASED,
	.symbol = "vector_scal_power_history"
};

static struct starpu_codelet cl =
{
	.where = STARPU_CPU | STARPU_CUDA | STARPU_OPENCL,
	/* CPU implementation of the codelet */
	.cpu_funcs = {
		scal_cpu_func
	},
#ifdef STARPU_USE_CUDA
	/* CUDA implementation of the codelet */
	.cuda_funcs = {scal_cuda_func},
#endif
#ifdef STARPU_USE_OPENCL
	/* OpenCL implementation of the codelet */
	.opencl_funcs = {scal_opencl_func},
#endif
	.nbuffers = 1,
	.modes = {STARPU_RW},
	.model = &vector_scal_model,
	.power_model = &vector_scal_power_model
};

#ifdef STARPU_USE_OPENCL
struct starpu_opencl_program opencl_program;
#endif

/* global variables */
int NX, TRAIN_LOOPS;
float factor;
float *vector;

int mf_starpu_test(struct starpu_conf conf, char *user, char *task, char *exp_id);

int main(int argc, char **argv)
{
	int i;
	struct starpu_conf conf;

/*get from input arguments user, task, and exp_id */
	char user[40] = {'\0'};
	char task[40] = {'\0'};
	char exp_id[40] = {'\0'};

	if(mf_starpu_get_user(argc, argv, user) == -1)
		return -1;
	if(mf_starpu_get_task(argc, argv, task) == -1)
		return -1;
	if(mf_starpu_get_experiment_id(argc, argv, exp_id)==-1)
		return -1;

/*get train loops*/
	TRAIN_LOOPS = mf_starpu_get_train_loops(argc, argv);
	if(TRAIN_LOOPS <= 0) {
		printf("ERROR: mf_starpu_get_train_loops failed.\n");
		return -1;
	}
/* get NX */
	for (i = 1; i < argc; i++)
	{
		if (strcmp(argv[i], "-NX") == 0)
		{
			char *argptr;
			NX = strtol(argv[++i], &argptr, 10);
			break;
		}
	}
	if(NX == 0) {
		printf("ERROR: no NX is given.\n");
		return -1;
	}

/* initialize data */
	factor = 3.14;
	vector = (float *)malloc(NX * sizeof(float));

/*creat configure data for starpu using cpu*/
	starpu_conf_init(&conf);
	conf.sched_policy_name = "dmda";
	conf.ncpus = 1;
	conf.ncuda = 0;
	conf.nopencl = 0;
	conf.calibrate = 1;
	printf("TRAIN_LOOPS is: %d\n", TRAIN_LOOPS);
    if(mf_starpu_test(conf, user, task, exp_id) == -1){
    	printf("\nERROR: mf_starpu_test for cpu failed.\n");
    	return -1;
    }

/*creat configure data for starpu using gpu*/
    starpu_conf_init(&conf);
    conf.sched_policy_name = "dmda";
	conf.ncpus = 0;
	conf.ncuda = 1;
	conf.nopencl = 0;
	conf.calibrate = 1;
	printf("TRAIN_LOOPS is: %d\n", TRAIN_LOOPS);
	if(mf_starpu_test(conf, user, task, exp_id) == -1){
    	printf("\nERROR: mf_starpu_test for gpu failed.\n");
    	return -1;
    }

/* end of the testing program */
	free(vector);
	return 0;
}

int mf_starpu_test(struct starpu_conf conf, char *user, char *task, char *exp_id)
{
	int i;
	double energy;

	if (mf_starpu_init(&conf, user, task, exp_id) == -1)
		return -1;

	struct starpu_task **training_tasks = malloc(TRAIN_LOOPS * sizeof(struct starpu_task *));
	
	long double start_time = mf_starpu_time();

	for (i = 0; i < TRAIN_LOOPS; i++) {
	
		starpu_data_handle_t vector_handle;
		starpu_vector_data_register(&vector_handle, 0, (uintptr_t)vector, NX, sizeof(vector[0]));
		struct starpu_task *task = starpu_task_create();
		task->synchronous = 1; //important for mf_starpu
		task->destroy = 0; //task will be destroyed by hand, important for mf_starpu
		task->cl = &cl;

		/* the codelet manipulates one buffer in RW mode */
		task->handles[0] = vector_handle;

		/* an argument is passed to the codelet, beware that this is a
	 	* READ-ONLY buffer and that the codelet may be given a pointer to a
	 	* COPY of the argument */
		task->cl_arg = &factor;
		task->cl_arg_size = sizeof(factor);

		training_tasks[i] = task;
		/* execute the task on any eligible computational ressource */
		int ret = starpu_task_submit(task);
		if(ret == -ENODEV){
			printf("\n[ERROR] starpu_task_submit failed.\n");
			return -1;
		}
		//STARPU_CHECK_RETURN_VALUE(ret, "starpu_task_submit");
		/* StarPU does not need to manipulate the array anymore so we can stop
 	 * monitoring it */
		starpu_data_unregister(vector_handle);
	}

	long double end_time = mf_starpu_time();

	energy = (double) mf_starpu_get_energy(start_time, end_time, ALL_ENERGY) / TRAIN_LOOPS;
	if(energy <= 0) {
		printf("ERROR: mf_starpu_get_energy failed.\n");
		return -1;
	}
	
	unsigned nimpl = 0;
	for (i = 0; i < TRAIN_LOOPS; i++) {
		mf_starpu_metrics_feed(training_tasks[i], nimpl, energy);
		starpu_task_destroy(training_tasks[i]);
	}
	
	free(training_tasks);
	starpu_shutdown();
	return 0;
}
