CC = cc
LD = cc
CFLAGS = -Wall -O3 -fopenmp -fPIC -shared
LDFLAGS := -lm

COLORMAP = colormap
LAGRANGIANOBJS := integrators.o rules.o
LAGRANGIAN = liblagrangians.so
SYSTEMS = dangling_stick double_pendulum double_spring double_well \
		driven_damped_pendulum harmonic_oscillator platform_with_springs \
		springy_pendulum trig
LAGRANGIANOBJS += ${addsuffix .o, ${addprefix systems/, ${SYSTEMS}}}

TARGETS = ${COLORMAP} ${LAGRANGIAN} 

all: ${TARGETS}

${COLORMAP}: 
	${CC} -Wall -O3 -o $@ colormap.c ${LDFLAGS}

${LAGRANGIAN}: ${LAGRANGIANOBJS}
	${CC} ${CFLAGS} -o $@ ${LAGRANGIANOBJS} ${LDFLAGS}

$%.o: %.c
	${CC} ${CFLAGS} -c -o $@ $< ${LDFLAGS}

install: ${COLORMAP} ${LAGRANGIAN}
	cp ${LAGRANGIAN} ${HOME}/lib
	cp ${COLORMAP} ${HOME}/bin

clean:
	rm -f ${TARGETS} ${COLORMAP} ${LAGRANGIANOBJS} 
