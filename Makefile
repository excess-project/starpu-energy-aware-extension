## Copyright (C) 2016 University of Stuttgart
## Authors: Fangli Pi

CC       = gcc
CFLAGS   = -Wall $(INC_MF) $(INC_STARPU) ${INC_EXT} ${INC_HWLOC} -O3 -g
LFLAGS   = $(MF_LIB) $(PARSER_LIB) $(PUBLISHER_LIB) ${HWLOC_LIB} -Wl,-rpath-link,lib
CFLAGS  += $(shell pkg-config --cflags starpu-1.2)
LDFLAGS += $(shell pkg-config --libs starpu-1.2)

INC_MF=-Ibin/mf/monitoring-agent/dist/include
INC_STARPU=-Ibin/starpu/include/starpu/1.2
INC_EXT=-Iext/
INC_HWLOC=-Ibin/hwloc/include

CORE_LIBS=bin/mf/monitoring-agent/lib
MF_LIB=-L$(CORE_LIBS) -lmf
PARSER_LIB=-L$(CORE_LIBS) -lparser
PUBLISHER_LIB=-L$(CORE_LIBS) -lpublisher
HWLOC_LIB=-Lbin/hwloc/lib -lhwloc

HOST=$(shell hostname)
ifneq (,$(findstring excess,$(HOST)))
	INC_MF=-I/opt/mf/stable/16.6/include
	CORE_LIBS=/opt/mf/stable/16.6/lib
	INC_HWLOC=-I/usr/include
	HWLOC_LIB=-L/usr/lib64
endif

all: ext/libjsmn.a src/libmfstarpu.a

src/mf_starpu_utils.o: src/mf_starpu_utils.c
	$(CC) -c $(CFLAGS) $+ -o $@

ext/jsmn.o: ext/jsmn.c
	$(CC) -c $(CFLAGS) $+ -o $@

ext/libjsmn.a: ext/jsmn.o
	$(AR) rc $@ $^

src/libmfstarpu.a: src/mf_starpu_utils.o ext/jsmn.o
	$(AR) rc $@ $^

clean:
	rm -f src/*.o 
	rm -f ext/*.o
	rm -f src/libmfstarpu.a
	rm -f ext/libjsmn.a 
