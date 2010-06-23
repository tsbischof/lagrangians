CC = cc
CFLAGS = -Wall -fopenmp
LD = cc
INCLUDES = -I/usr/local/include
LIBS = -lm -liniparser
OBJS =  image_funcs.o integrators.o main.o parse_utils.o rules.o installed.o \
		grapher.o  \
		systems/double_pendulum.o systems/dangling_stick.o \
		systems/harmonic_oscillator.o systems/double_well.o systems/trig.o \
		systems/springy_pendulum.o
TARGETS = lagrangians
USE_GPU = False

all: lagrangians

lagrangians: $(OBJS)
	$(LD) $(CFLAGS) $(INCLUDES) $(LIBS) -o $@ $(OBJS) 

$%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) $(LIBS) -o $@ $<

clean:
	rm -f $(OBJS) $(TARGETS)
