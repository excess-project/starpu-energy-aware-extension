## Copyright (C) 2016 University of Stuttgart
## Authors: Fangli Pi

CC       = gcc
CFLAGS   = -Wall $(INC_MF) $(INC_STARPU) ${INC_EXT} -O3 -g
LFLAGS   = $(MF_LIB) $(PARSER_LIB) $(PUBLISHER_LIB) -Wl,-rpath-link,lib
CFLAGS  += $(shell pkg-config --cflags starpu-1.2)
LDFLAGS += $(shell pkg-config --libs starpu-1.2)

ifeq ($HOSTNAME, "fe.excess-project.eu")
	echo "on cluster"
	INC_MF=-I/opt/mf/stable/16.2/include
	INC_STARPU=-Ibin/starpu/include/starpu/1.2/
	INC_EXT=-Iext/
	CORE_LIBS=/opt/mf/stable/16.2/lib
else
	echo "NOT on cluster"
	INC_MF=-Ibin/mf/monitoring-agent/dist/include
	INC_STARPU=-Ibin/starpu/include/starpu/1.2/
	INC_EXT=-Iext/
	CORE_LIBS=bin/mf/monitoring-agent/lib
endif

MF_LIB=-L$(CORE_LIBS) -lmf
PARSER_LIB=-L$(CORE_LIBS) -lparser
PUBLISHER_LIB=-L$(CORE_LIBS) -lpublisher

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
