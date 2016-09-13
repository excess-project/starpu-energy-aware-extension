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

int main(int argc, char **argv)
{
	int i, NX;

/* get NX */
	NX = 0;
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
	float factor = 3.14;
	float *vector = (float *)malloc(NX * sizeof(float));

/*mf_starpu_init*/
	if (starpu_init(NULL) == -ENODEV)
		return -1;

/* mf_starpu_init initializes Monitoring Framework API */
	if (mf_starpu_init() == -1)
		return -1;

/*load starpu opencl*/
#ifdef STARPU_USE_OPENCL
	char opencl_file[] = "/nas_home/hpcfapix/starpu-energy-extension/examples/vectorscal_history/vector_scal_opencl_kernel.cl";
	if (starpu_opencl_load_opencl_from_file(opencl_file, &opencl_program, NULL) == -1)
		return -1;
#endif

/*starpu data register*/
	starpu_data_handle_t vector_handle;
	starpu_vector_data_register(&vector_handle, 0, (uintptr_t)vector, NX, sizeof(vector[0]));

/*starpu_task creat*/
	struct starpu_task *task = starpu_task_create();
	task->cl = &cl;
	task->handles[0] = vector_handle;
	task->cl_arg = &factor;
	task->cl_arg_size = sizeof(factor);

/* Trains the StarPU task with energy monitoring data 
 * The task may be executed for multiple times for getting more accurate energy values */
	unsigned nimpl=0;
	if(mf_starpu_task_training(task, nimpl)== -1) 
		return -1;

/*starpu_data_unregister*/
	starpu_data_unregister(vector_handle);

/* end of the testing program */
	free(vector);

/*unload starpu opencl*/
#ifdef STARPU_USE_OPENCL
	if (starpu_opencl_unload_opencl(&opencl_program) == -1) 
		return -1;
#endif
	starpu_shutdown();
	return 0;
}
