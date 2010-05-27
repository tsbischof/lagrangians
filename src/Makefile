CC := cc
CCFLAGS := -Wall -fopenmp -g
LD := cc
LIBS := -lm 
LIBDIRS := 
INCLUDES := 
OBJS := double_pendulum.o double_well.o grapher.o harmonic_oscillator.o \
		image_funcs.o installed.o integrators.o main.o parse_utils.o rules.o \
		iniparser/dictionary.o iniparser/iniparser.o

all: lagrangians

lagrangians: $(OBJS)
	$(LD) $(CCFLAGS) $(LIBDIRS) $(INCLUDES) $(LIBS) -o $@ $(OBJS) 

$%.o: %.c
	$(CC) $(CCFLAGS) $(LIBDIRS) $(INCLUDES) $(LIBS) -o $@ $<

clean:
	rm -f $(OBJS) $(TARGETS)
