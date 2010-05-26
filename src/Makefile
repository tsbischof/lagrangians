CC := cc
CCFLAGS := -Wall -fopenmp
LD := cc
LIBS := -lm 
OBJS := double_pendulum.o double_well.o grapher.o harmonic_oscillator.o \
		image_funcs.o installed.o integrators.o main.o parse_utils.o rules.o

all: lagrangians

lagrangians: $(OBJS)
	$(LD) $(CCFLAGS) $(LIBS) -o $@ $(OBJS) 

$%.o: %.c
	$(CC) $(CCFLAGS) -o $@ $<

clean:
	rm -f $(OBJS) $(TARGETS)
