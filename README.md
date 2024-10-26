# Lagrangians
This package implements a set of differential equation systems, the tools needed to integrate those equations to a desired endpoint, and tools for data visualization. The integrators are intentionally semi-stable to enable exploration of both well-behaved systems (where numerical stability is not too hard to achieve) and poorly-behaved systems (where numerical instability leads to its own interesting behavior).

The short version of the mechanics is that we define a system of equations with some number of parameters. We then pick an interesting 2D slice through that phase space to form some set of initial conditions for the system, and rules for what we want to visualize as these initial conditions are advanced. Finally we advance the initial conditions through time to arrive at our interesting trajectories.

## Build
The C++ code has been most extensively used on x86-64 Linux. I am happy to support anyone wanting to build it elsewhere, there is (hopefully) little in the way of portability.
```
./bootstrap
./configure
make install
```
### Dependencies
* autotools (automake, autoconf)
* make
* C++ compiler (e.g. clang++, g++)
* boost (datetime, filesystem, program_options, regex, system)
* openmp

## Usage
The general workflow is:
1. Prepare input file `foo.inp`
2. Process input file using `lagrangians foo.inp`
3. Visualize output using `colormap.py foo.inp`

[Several example input files](run.v2?raw=True) exist for the current version of the code. [Older input files](run.v1?raw=True) can provide inspiration but may require some work to execute properly (reversion to older commit, etc).

### Input file syntax
Input files are ini-style text files with the following sections:
* config: global configuration
  * system
    * dangling_stick: a weighted rigid rod suspended from a spring
    * double_pendulum: a pair of rave never built it on Windows, but igid rods with weights
    * double_spring: a double pendulum with springs instead of rigid rods
    * springy_pendulum: a pendulum with a weight on a spring attached to the bottom
    * trig: various sinusoids
  * run_type
    * image: integrate over all points in phase space until a given endpoint and report the final value
    * video: integrate over all points in phase space, recording at each timepoint
  * endpoint: varies with each system, but examples include:
    * lower_flip: bottom half of the double pendulum-like variable does a flip
    * upper_flip: same, for the upper half
    * lower_turnaround: stop when the velocity of the lower half has inverted
  * height, width: pixel dimensions for the file
  * t: time steps for the integrator, as a triplet of floats: start, stop, step
* defaults: for each system parameter, you can specify the value here if it does not vary over the width or height
* horizontal/vertical: for each parameter to be varied, provide the lower and upper limits. Based on the specified width and height the points in phase space will be interpolated

## Examples
[My tumblr](https://differentialart.tumblr.com) contains a series of images generated using this software.

### Dangling stick
![sharp orange](https://66.media.tumblr.com/e56d5520a6bff5c107a46472e6fc26bd/tumblr_oprg3ctfxl1w5gu6mo1_1280.png)

### Double pendulum
![strokes](https://66.media.tumblr.com/445da890191dd152cb4d68a02c687ba0/tumblr_on2dx7qyB61w5gu6mo1_1280.png)

## Prints
[See my Etsy page](https://bischofart.etsy.com/) or [contact the author](https://github.com/tsbischof) for a custom request.
