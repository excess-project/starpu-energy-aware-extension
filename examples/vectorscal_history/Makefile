CC       = gcc
CFLAGS   = -Wall -funroll-loops -fstrict-aliasing $(INC_MF) $(INC_STARPU) ${INC_STARPUEXT} -O3 -g
LFLAGS   = $(MF_LIB) $(PARSER_LIB) $(PUBLISHER_LIB) ${MF_STARPU_LIB} -Wl,-rpath-link,lib
CFLAGS  += $(shell pkg-config --cflags starpu-1.2)
LDFLAGS += $(shell pkg-config --libs starpu-1.2)

INC_CUDA=-I/opt/compiler/cuda/7.0/include

INC_STARPUEXT=-I../../src

INC_MF=-I../../bin/mf/monitoring-agent/dist/include
INC_STARPU=-I../../bin/starpu/include/starpu/1.2
CORE_LIBS=../../bin/mf/monitoring-agent/lib

HOST=$(shell hostname)

ifneq (,$(findstring excess,$(HOST)))
	INC_MF=-I/opt/mf/stable/16.2/include
	INC_STARPU=-Ibin/starpu/include/starpu/1.2/
	CORE_LIBS=/opt/mf/stable/16.2/lib
endif

MF_LIB=-L$(CORE_LIBS) -lmf
PARSER_LIB=-L$(CORE_LIBS) -lparser
PUBLISHER_LIB=-L$(CORE_LIBS) -lpublisher
MF_STARPU_LIB=-L../../src -lmfstarpu

all: vector_scal
	
vector_scal: vector_scal.o vector_scal_cpu.o vector_scal_cuda.o vector_scal_opencl.o
	$(CC) $(CFLAGS) $+ -o $@ $(LDFLAGS) $(LFLAGS)

%.o: %.c
	$(CC) -c $(CFLAGS) $+ -o $@

%.o: %.cu
	nvcc -arch=compute_35 $(INC_STARPU) ${INC_CUDA}  -I/usr/include/libxml2  $< -c $@

clean:
	rm -f vector_scal *.o
