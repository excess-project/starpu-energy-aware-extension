CC       = mpicc
CFLAGS   = -O3 -g -Wall -funroll-loops -fstrict-aliasing $(INC_STARPU) ${INC_STARPUEXT} ${INC_STARPU_MPI} ${INC_COMMON}
LFLAGS   = $(MF_LIB) $(PARSER_LIB) $(PUBLISHER_LIB) ${STARPUEXT_LIB} -Wl,-rpath-link,lib
CFLAGS  += $(shell pkg-config --cflags starpu-1.2)
LDFLAGS += $(shell pkg-config --libs starpu-1.2)
CFLAGS  += $(shell pkg-config --cflags starpumpi-1.2)
LDFLAGS += $(shell pkg-config --libs starpumpi-1.2)
LDFLAGS += -lmkl_intel_lp64 -lblas -lmkl_core -lmkl_intel_thread -liomp5 -lpthread -lm

INC_STARPUEXT=-I../../src
INC_STARPU_MPI=-I./include
INC_COMMON=-I./common
INC_STARPU=-I../../bin/starpu/include/starpu/1.2
CORE_LIBS=../../bin/mf/monitoring-agent/lib

HOST=$(shell hostname)

ifneq (,$(findstring excess,$(HOST)))
	CORE_LIBS=/opt/mf/stable/16.6/lib
endif

MF_LIB=-L$(CORE_LIBS) -lmf
PARSER_LIB=-L$(CORE_LIBS) -lparser
PUBLISHER_LIB=-L$(CORE_LIBS) -lpublisher
STARPUEXT_LIB=-L../../src -lmfstarpu

all: mpi_cholesky

%.o: %.c
	$(CC) -c $(CFLAGS) $+ -o $@

blas.o: common/blas.c
	$(CC) -c $(CFLAGS) $+ -o $@

mpi_cholesky: mpi_cholesky.o mpi_cholesky_codelets.o mpi_cholesky_kernels.o mpi_cholesky_models.o mpi_decomposition_matrix.o mpi_decomposition_params.o blas.o
	$(CC) $(CFLAGS) $+ -o $@ $(LDFLAGS) $(LFLAGS)

clean:
	rm -f *.o mpi_cholesky
