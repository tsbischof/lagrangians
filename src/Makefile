OPENMP = -fopenmp

USE_GPU = #-DUSE_GPU
OPENCL = 
BROOK = #true

CFLAGS = -Wall -O3 ${OPENMP} ${USE_GPU}

ifdef USE_GPU
CC = c++
CFLAGS += -Wno-write-strings
LD = c++
else
CC = cc
LD = cc
endif

LIBS = -lm -liniparser
OBJS :=  image_funcs.o integrators.o main.o parse_utils.o rules.o installed.o \
        grapher.o  
SYSTEMS := harmonic_oscillator trig double_well \
		double_pendulum dangling_stick springy_pendulum double_spring
OBJS += ${addsuffix .o, ${addprefix systems/, ${SYSTEMS}}}
TARGETS = lagrangians

ifeq (${BROOK},true)
BRCC = brcc
BRKERNS = systems/gpu/harmonic_oscillator.brook
BRCPP = systems/gpu/gpu_brook.cpp
BRGEN := ${BRKERNS:.brook=.h} ${BRKERNS:.brook=_gpu.h} ${BRKERNS:.brook=.cpp}
BROBJS := ${BRCPP:.cpp=.o} ${BRKERNS:.brook=.o}
BRLIBS := -I/usr/local/atibrook/sdk/include -L/usr/local/atibrook/sdk/lib -lbrook -lbrook_cal
LIBS += ${BRLIBS}
else
endif

all: lagrangians

lagrangians: ${BROBJS} ${OBJS}
	${LD} ${CFLAGS} ${LIBS} -o $@ ${OBJS} ${BROBJS}

$%.o: %.c
	${CC} ${CFLAGS} ${LIBS} -o $@ $<

${BROBJS}: ${BRKERNS} 
	${CC} ${CFLAGS} ${LIBS} -c -o $@ ${@:.o=.cpp}

${BRKERNS}: 
	${BRCC} ${@:.brook=.br}

clean:
	rm -f ${OBJS} ${TARGETS} ${BRGEN} ${BROBJS} 
