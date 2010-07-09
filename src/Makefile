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

LDFLAGS := -lm -liniparser 
CPPFLAGS =
OBJS :=  image_funcs.o integrators.o main.o parse_utils.o rules.o installed.o \
        grapher.o 
SYSTEMS := harmonic_oscillator trig double_well \
		double_pendulum dangling_stick springy_pendulum double_spring \
		platform_with_springs
OBJS += ${addsuffix .o, ${addprefix systems/, ${SYSTEMS}}}
TARGET = lagrangians

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

all: ${TARGET}

${TARGET}: ${BROBJS} ${OBJS}
	${LD} ${LDFLAGS} ${CFLAGS} ${CPPFLAGS} -o $@ ${OBJS} ${BROBJS}

$%.o: %.c
	${CC} ${LDFLAGS} ${CFLAGS} ${CPPFLAGS} -c -o $@ $<

${BROBJS}: ${BRKERNS} 
	${CC} ${LDFLAGS} ${CFLAGS} ${CPPFLAGS} -c -o $@ ${@:.o=.cpp}

${BRKERNS}: 
	${BRCC} ${@:.brook=.br}

install: ${TARGET}
	mv ${TARGET} ~tsbischof/bin/.

clean:
	rm -f ${OBJS} ${TARGETS} ${BRGEN} ${BROBJS} 
