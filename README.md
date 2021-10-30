# cgra-systemc
SystemC implementation of a parameterizable CGRA

[![Build Status](https://travis-ci.org/werneazc/cgra-systemc.svg?branch=master)](https://travis-ci.org/werneazc/cgra-systemc)

## Build

The architecture is designed in [SystemC](https://accellera.org/downloads/standards/systemc). It is compiled with different
Linux distributions like Debian, Ubuntu or OpenSuse. The CMakeLists.txt contains an automatic mechanism to find the
SystemC dependencies. Therefore, please provide `SYSTEMC_ROOT` environment variable to point to the root directory of your SystemC
installation.
## Architecture variants

- VCGRA: A processing system acts directly with an accelerator (VCGRA). The virtual CGRA consists of processing elements
and virtual channels. The processing system needs to control the calculation the whole runtime.
- VCGRA & Pre-Fetchers: A processing systems has access to configuration pre-fetchers for processing elements and
virtual channels to prepare runtime configurations for the VCGRA before switching.
- Full Architecture: The full architecture is an accelerator with a Central Control Unit, a Memory Management Unit,
pre-fetchers for configurations, input and output data and a shared scratchpad memory between the accelerator and
the processing system. The Central Control Unit is programmable with a special assembler and can work independently with
data stored in shared memory. The processing system does not need to perform any control during  processing of
data of the VCGRA accelerator architecture. The VCGRA assembler is available [here](https://github.com/werneazc/cgra_assembler.git).

## Evaluation

### Demo

Evaluation is performed by processing an edge detection of an image of Lena. The configuration files for
[McPAT](https://github.com/werneazc/mcpat.git) are stored in the repository in the `mcpat` directory.
An adapted version of McPAT is necessary to overcome the limitations of McPAT that is meanly created to
simulate big multicore systems. Because of this limitations this is a very rough estimation for area
and power.

### Evaluation Results

|                                       |   VCGRA   | VCGRA<br>&<br>Pre-Fetchers | Full<br>Architecture |
|---------------------------------------|:---------:|:--------------------------:|:--------------------:|
| Simulation<br>Time [ms]               |       784 |                        446 |                  532 |
| Execution Cycles                      |   3920881 |                    2229665 |              2660805 |
| Area Chip [mm^2]                      |  0.634446 |                   0.650449 |             0.722449 |
| Area Processing Elements<br>[mm^2]    |  0.560303 |                   0.560303 |             0.560303 |
| Area Virtual Channels<br>[mm^2]       | 0.0391092 |                  0.0391092 |            0.0391092 |
| Area Synchronization Unit<br>[mm^2]   |  0.035034 |                   0.035034 |             0.035034 |
| Area Pre-Fetchers [mm^2]              |         - |                   0.016003 |             0.015885 |
| Area Central Control Unit<br>[mm^2]   |         - |                          - |             0.035019 |
| Area Memory Management Unit<br>[mm^2] |         - |                          - |            0.0370991 |
| Peak Power Consumption Chip [mW]      |       342 |                        345 |                  371 |

## Visualization

### Requirements

- [gSysC](https://github.com/werneazc/gsysc.git) library
- Qt5 libraries installed at the development host-PC
- Linux CMD-line variable `GSYSC_ROOT` pointing to the root directory of your gSysC clone and
  `SYSTEMC_LIBRARIES` pointing to library file of SystemC. 

### Usability

Please add the following option to your CMake configuration command: `DGSYSC_ENABLED=ON`.
The project will be compiled with GUI support. If one of the compile [targets](#architecture-variants) is started
from Linux CMD, the GUI opens automatically. One can browse the module hierarchy as well as
performing a step-by-step simulation and observe the state of individual signals in a
signal browser.