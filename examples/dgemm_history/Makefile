CC       = icc
CFLAGS   = -O3 -g -qopenmp $(INC_MF) $(INC_STARPU) ${INC_STARPUEXT}
LFLAGS   = $(MF_LIB) $(PARSER_LIB) $(PUBLISHER_LIB) ${MF_STARPU_LIB} -Wl,-rpath-link,lib
CFLAGS  += $(shell pkg-config --cflags starpu-1.2)
LDFLAGS += $(shell pkg-config --libs starpu-1.2)
LDFLAGS += -lmkl_intel_lp64 -lmkl_core -lmkl_intel_thread -lpthread -lm

INC_STARPUEXT=-I../../src

INC_MF=-I../../bin/mf/monitoring-agent/dist/include
INC_STARPU=-I../../bin/starpu/include/starpu/1.2
CORE_LIBS=../../bin/mf/monitoring-agent/lib

HOST=$(shell hostname)

ifneq (,$(findstring excess,$(HOST)))
	INC_MF=-I/opt/mf/stable/16.6/include
	CORE_LIBS=/opt/mf/stable/16.6/lib
endif

MF_LIB=-L$(CORE_LIBS) -lmf
PARSER_LIB=-L$(CORE_LIBS) -lparser
PUBLISHER_LIB=-L$(CORE_LIBS) -lpublisher
MF_STARPU_LIB=-L../../src -lmfstarpu

all: dgemm_mf_starpu

%.o: %.c
	$(CC) -c $(CFLAGS) $+ -o $@

blas.o: common/blas.c
	$(CC) -c $(CFLAGS) $+ -o $@

dgemm_mf_starpu: dgemm_mf_starpu.o blas.o
	$(CC) $(CFLAGS) $+ -o $@ $(LDFLAGS) $(LFLAGS)

clean:
	rm -f *.o dgemm_mf_starpu
