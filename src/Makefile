CC = cc
CFLAGS = -Wall -fopenmp
LD = cc
LIBS = -lm 
OBJS =  image_funcs.o integrators.o main.o parse_utils.o rules.o installed.o \
		grapher.o iniparser/dictionary.o iniparser/iniparser.o \
		systems/double_pendulum.o systems/dangling_stick.o \
		systems/harmonic_oscillator.o systems/double_well.o systems/trig.o
TARGETS = lagrangians

all: lagrangians

lagrangians: $(OBJS)
	$(LD) $(CFLAGS) $(LIBS) -o $@ $(OBJS) 

$%.o: %.c
	$(CC) $(CFLAGS) $(LIBS) -o $@ $<

clean:
	rm -f $(OBJS) $(TARGETS)
