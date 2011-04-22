CC = cc
LD = cc
CFLAGS = -Wall -fPIC -O3 -shared -fopenmp
LDFLAGS := -lm

COLORMAPOBJS = colormap.o
COLORMAP = libcolormap.so
LAGRANGIANOBJS := integrators.o rules.o
LAGRANGIAN = liblagrangians.so
SYSTEMS = dangling_stick double_pendulum double_spring double_well \
		driven_damped_pendulum harmonic_oscillator platform_with_springs \
		springy_pendulum trig
LAGRANGIANOBJS += ${addsuffix .o, ${addprefix systems/, ${SYSTEMS}}}

TARGETS = ${COLORMAP} ${LAGRANGIAN} 

all: ${TARGETS}

${COLORMAP}: ${COLORMAPOBJS}
	${CC} ${CFLAGS} -o $@ ${COLORMAPOBJS} ${LDFLAGS}

${LAGRANGIAN}: ${LAGRANGIANOBJS}
	${CC} ${CFLAGS} -o $@ ${LAGRANGIANOBJS} ${LDFLAGS}

$%.o: %.c
	${CC} ${CFLAGS} -c -o $@ $< ${LDFLAGS}

install: ${COLORMAP} ${LAGRANGIAN}
	cp ${COLORMAP} ~tsbischof/lib
	cp ${LAGRANGIAN} ~tsbischof/lib

clean:
	rm -f ${TARGETS} ${COLORMAPOBJS} ${LAGRANGIANOBJS} *.pyc
