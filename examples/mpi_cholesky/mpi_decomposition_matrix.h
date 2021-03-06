/* StarPU --- Runtime system for heterogeneous multicore architectures.
 *
 * Copyright (C) 2009-2012  Université de Bordeaux
 * Copyright (C) 2010  Mehdi Juhoor <mjuhoor@gmail.com>
 * Copyright (C) 2010, 2011, 2012, 2013  CNRS
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

#ifndef __MPI_CHOLESKY_MATRIX_H__
#define __MPI_CHOLESKY_MATRIX_H__

/* Returns the MPI node number where data indexes index is */
int my_distrib(int x, int y, int nb_nodes);

void matrix_display(float ***bmat, int rank);
void matrix_init(float ****bmat, int rank, int nodes, int alloc_everywhere);
void matrix_free(float ****bmat, int rank, int nodes, int alloc_everywhere);

#endif /* __MPI_CHOLESKY_MATRIX_H__ */

