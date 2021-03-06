/* StarPU --- Runtime system for heterogeneous multicore architectures.
 *
 * Copyright (C) 2009-2012, 2015  Université de Bordeaux
 * Copyright (C) 2010  Mehdi Juhoor <mjuhoor@gmail.com>
 * Copyright (C) 2010, 2011, 2012, 2013, 2015  CNRS
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

#include "mpi_cholesky.h"
#include <mf_starpu_utils.h>

int main(int argc, char **argv)
{
	/* create a simple definite positive symetric matrix example
	 *
	 *	Hilbert matrix : h(i,j) = 1/(i+j+1)
	 * */

	float ***bmat;
	int rank, nodes, ret;
	double timing, flops;
	int correctness;

	ret = starpu_init(NULL);
	STARPU_CHECK_RETURN_VALUE(ret, "starpu_init");

	/* mf_starpu_init initializes Monitoring Framework API */
	if (mf_starpu_init() == -1)
		return -1;

	ret = starpu_mpi_init(&argc, &argv, 1);
	STARPU_CHECK_RETURN_VALUE(ret, "starpu_mpi_init");
	starpu_mpi_comm_rank(MPI_COMM_WORLD, &rank);
	starpu_mpi_comm_size(MPI_COMM_WORLD, &nodes);
	starpu_cublas_init();

	parse_args(argc, argv, nodes);

	matrix_init(&bmat, rank, nodes, 1);
	matrix_display(bmat, rank);

	/*get start time for mf_starpu*/
	double mf_start_time = mf_starpu_time();
	printf("\n[MF_STARPU] rank %d start time is :%f\n", rank, mf_start_time);
	
	dw_cholesky(bmat, size/nblocks, rank, nodes, &timing, &flops);

	/*get end time of mf_starpu*/
	double mf_end_time = mf_starpu_time();
	printf("\n[MF_STARPU] rank %d end time is :%f\n", rank, mf_end_time);

	starpu_mpi_shutdown();

#ifndef STARPU_SIMGRID
	matrix_display(bmat, rank);

	dw_cholesky_check_computation(bmat, rank, nodes, &correctness, &flops);
#endif

	matrix_free(&bmat, rank, nodes, 1);
	starpu_cublas_shutdown();
	starpu_shutdown();

#ifndef STARPU_SIMGRID
	assert(correctness);
#endif

	if (rank == 0)
	{
		FPRINTF(stdout, "Computation time (in ms): %2.2f\n", timing/1000);
		FPRINTF(stdout, "Synthetic GFlops : %2.2f\n", (flops/timing/1000.0f));
	}

	/*get cpu energy from mf*/
	double cpu_energy = mf_starpu_get_energy(mf_start_time, mf_end_time);
	printf("\n[MF_STARPU] rank %d cpu energy is: %f\n", rank, cpu_energy);

	return 0;
}
