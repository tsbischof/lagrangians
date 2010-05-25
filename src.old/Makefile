CXX := c++
CXXFLAGS := -Wall -fopenmp -g
LD := c++
LIBS := -lboost_regex -lboost_program_options 
OBJS := double_pendulum.o double_well.o grapher.o harmonic_oscillator.o \
		image_funcs.o installed.o integrators.o main.o parse_utils.o rules.o

all: lagrangians

lagrangians: $(OBJS)
	$(LD) $(CXXFLAGS) $(LIBS) -o $@ $(OBJS) 

$%.o: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

clean:
	rm -f $(OBJS) $(TARGETS)
