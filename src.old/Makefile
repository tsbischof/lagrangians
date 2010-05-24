CXX = c++
CXX_FLAGS = -Wall -fopenmp
LD = c++
LD_FLAGS =
DEBUG = 
LIBS = -lboost_regex -lboost_program_options 
TARGETS = lagrangians$(EXEEXT)
OBJS = double_pendulum.o double_well.o grapher.o harmonic_oscillator.o \
		image_funcs.o installed.o integrators.o main.o parse_utils.o rules.o
INCLUDES = 

all: $(TARGETS)
	mv ${TARGETS} ..

lagrangians: $(OBJS)
	$(LD) $(DEBUG) $(CXX_FLAGS) $(LD_FLAGS) $(INCLUDE) $(LIBS) -o $@ $(OBJS) 
	
$%.o: %.cpp
	$(CXX) $(DEBUG) $(CXX_FLAGS) $(LIBS) -o $@ $<

clean:
	rm -f $(OBJS) $(TARGETS)
