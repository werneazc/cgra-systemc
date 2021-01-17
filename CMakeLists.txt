cmake_minimum_required(VERSION 3.14)
project(
  cgra_systemc
  VERSION 0.1.1
  DESCRIPTION "Virtual Coarse Grain Reconfigurable Array Simulator"
  LANGUAGES CXX)

# Enable/disable McPAT statistic outputs to mcpat_stats.log file
option(MCPAT_ENABLED "Enable statistic outputs for McPAT" OFF)

# Prepare SystemC environment Please add environment variable to SystemC root
# directory to your OS environment.
set(CMAKE_PREFIX_PATH $ENV{SYSTEMC_ROOT})
find_package(SystemCLanguage REQUIRED CONFIG)

add_subdirectory(src/)

# Create full_architecture with all its components
add_executable(full_architecture src/sc_main.cpp src/TopLevel.cpp
                                 src/Testbench_TopLevel.cpp)
target_include_directories(full_architecture PUBLIC header/)
target_compile_features(full_architecture PUBLIC cxx_std_14)
target_link_libraries(full_architecture SystemC::systemc COMPONENTS)
if(MCPAT_ENABLED)
  add_compile_definitions(MCPAT)
endif(MCPAT_ENABLED)

# Create full_architecture with all its components
add_executable(vcgra_only src/vcgra_only.cpp src/testbench_vcgra_only.cpp)
target_include_directories(vcgra_only PUBLIC header/)
target_compile_features(vcgra_only PUBLIC cxx_std_14)
target_link_libraries(vcgra_only SystemC::systemc COMPONENTS)
if(MCPAT_ENABLED)
  add_compile_definitions(MCPAT)
endif(MCPAT_ENABLED)

# Add doxygen documentation
find_package(Doxygen)
set(DOXFILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/doc/cgra-systemc)
# set(DOXFILE ${CMAKE_CURRENT_BINARY_DIR}/cgra_systemc)
# configure_file(${DOXFILE_IN} ${DOXFILE} @ONLY)

add_custom_target(
  doc ALL
  COMMAND ${DOXYGEN_EXECUTABLE} ${DOXFILE_IN}
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/doc
  COMMENT "Generating doxygen documentation"
  VERBATIM)
