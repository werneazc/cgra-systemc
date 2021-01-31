CMAKE_MINIMUM_REQUIRED (VERSION 3.14)
PROJECT (
  cgra_systemc
  VERSION 0.1.1
  DESCRIPTION "Virtual Coarse Grain Reconfigurable Array Simulator"
  LANGUAGES CXX
)

# Enable/disable McPAT statistic outputs to mcpat_stats.log file
OPTION (
  MCPAT_ENABLED
  "Enable statistic outputs for McPAT"
  OFF
)

SET (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
                       ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules
)
FIND_PACKAGE (SystemC REQUIRED)

ADD_SUBDIRECTORY (src/)

# Create full_architecture with all its components
ADD_EXECUTABLE (
  full_architecture
  src/sc_main.cpp
  src/TopLevel.cpp
  src/Testbench_TopLevel.cpp
)
TARGET_INCLUDE_DIRECTORIES (
  full_architecture
  PUBLIC header/
  PRIVATE ${SYSTEMC_INCLUDE_DIRS}
)
TARGET_COMPILE_FEATURES (full_architecture PUBLIC cxx_std_14)
TARGET_LINK_LIBRARIES (
  full_architecture
  PRIVATE COMPONENTS
  PRIVATE "${SYSTEMC_LIBRARIES}"
)
IF (MCPAT_ENABLED)
  ADD_COMPILE_DEFINITIONS (MCPAT)
ENDIF (MCPAT_ENABLED)

# Create VCGRA with all its components
ADD_EXECUTABLE (vcgra_only src/vcgra_only.cpp src/testbench_vcgra_only.cpp)
TARGET_INCLUDE_DIRECTORIES (
  vcgra_only
  PUBLIC header/
  PRIVATE ${SYSTEMC_INCLUDE_DIRS}
)
TARGET_COMPILE_FEATURES (vcgra_only PUBLIC cxx_std_14)
TARGET_LINK_LIBRARIES (
  vcgra_only
  PRIVATE COMPONENTS
  PRIVATE "${SYSTEMC_LIBRARIES}"
)
IF (MCPAT_ENABLED)
  ADD_COMPILE_DEFINITIONS (MCPAT)
ENDIF (MCPAT_ENABLED)

# Add doxygen documentation
FIND_PACKAGE (Doxygen)
SET (DOXFILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/doc/cgra-systemc)
# set(DOXFILE ${CMAKE_CURRENT_BINARY_DIR}/cgra_systemc)
# configure_file(${DOXFILE_IN} ${DOXFILE} @ONLY)

ADD_CUSTOM_TARGET (
  doc ALL
  COMMAND ${DOXYGEN_EXECUTABLE} ${DOXFILE_IN}
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/doc
  COMMENT "Generating doxygen documentation"
  VERBATIM
)
