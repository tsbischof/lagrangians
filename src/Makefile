CC = cc
CFLAGS = -Wall 
LD = cc
LIBS = -lm 
OBJS = double_pendulum.o double_well.o grapher.o harmonic_oscillator.o \
		image_funcs.o installed.o integrators.o main.o parse_utils.o rules.o \
		trig.o trig2.o  \
		iniparser/dictionary.o iniparser/iniparser.o 
TARGETS = lagrangians

all: lagrangians

lagrangians: $(OBJS)
	$(LD) $(CFLAGS) $(LIBS) -o $@ $(OBJS) 

$%.o: %.c
	$(CC) $(CFLAGS) $(LIBS) -o $@ $<

clean:
	rm -f $(OBJS) $(TARGETS)
