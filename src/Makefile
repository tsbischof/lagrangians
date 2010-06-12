CC = cc
CFLAGS = -Wall -fopenmp
LD = cc
LIBS = -lm 
OBJS =  systems/harmonic_oscillator.o systems/double_well.o \
		systems/double_pendulum.o systems/trig.o systems/trig2.o \
		systems/trig3.o systems/dangling_stick.o \
		image_funcs.o installed.o integrators.o main.o parse_utils.o rules.o \
		grapher.o \
		iniparser/dictionary.o iniparser/iniparser.o 
TARGETS = lagrangians

all: lagrangians

lagrangians: $(OBJS)
	$(LD) $(CFLAGS) $(LIBS) -o $@ $(OBJS) 

$%.o: %.c
	$(CC) $(CFLAGS) $(LIBS) -o $@ $<

clean:
	rm -f $(OBJS) $(TARGETS)
