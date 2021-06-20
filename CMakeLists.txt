CMAKE_MINIMUM_REQUIRED (VERSION 3.14)

INCLUDE(cmake/Modules/PreventInSourceBuilds.cmake)
INCLUDE(cmake/Modules/CompilerWarnings.cmake)

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

# Enable/disable GUI using gSysC
OPTION (
  GSYSC_ENABLED
  "Enable gSysC GUI. Requires gSysC libary."
  OFF
)

#Enable/disable warnings
OPTION (
  WARNINGS_ENABLED
  "Enable/disable warnings during compiling"
  OFF
)

# Create new compile target with small optimizations
SET (CMAKE_CXX_FLAGS_OPTIMIZED -O1 -DNGEBUG)
SET(CMAKE_CXX_FLAGS_OPTIMIZED "-O1 -DNDEBUG" CACHE STRING
 "Flags used by the C++ compiler during maintainer builds."
  FORCE)
SET(CMAKE_C_FLAGS_OPTIMIZED "" CACHE STRING
  "Flags used by the C compiler during maintainer builds."
  FORCE)
SET(CMAKE_EXE_LINKER_FLAGS_OPTIMIZED
  "" CACHE STRING
  "Flags used for linking binaries during maintainer builds."
  FORCE)
SET(CMAKE_SHARED_LINKER_FLAGS_OPTIMIZED
  "" CACHE STRING
  "Flags used by the shared libraries linker during maintainer builds."
  FORCE)
MARK_AS_ADVANCED(
  CMAKE_CXX_FLAGS_OPTIMIZED
  CMAKE_C_FLAGS_OPTIMIZED
  CMAKE_EXE_LINKER_FLAGS_OPTIMIZED
  CMAKE_SHARED_LINKER_FLAGS_OPTIMIZED)
# Update the documentation string of CMAKE_BUILD_TYPE for GUIs
SET(CMAKE_BUILD_TYPE "${CMAKE_BUILD_TYPE}" CACHE STRING
  "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel Optimized."
  FORCE)

IF(CMAKE_CONFIGURATION_TYPES)
  LIST(APPEND CMAKE_CONFIGURATION_TYPES Optimized)
  LIST(REMOVE_DUPLICATES CMAKE_CONFIGURATION_TYPES)
  SET(CMAKE_CONFIGURATION_TYPES "${CMAKE_CONFIGURATION_TYPES}" CACHE STRING
    "Add the configurations that we need"
    FORCE)
ENDIF()


SET (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
                       ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules
)

FIND_PACKAGE (SystemC REQUIRED)

IF(GSYSC_ENABLED)
    FIND_PACKAGE (Threads REQUIRED)
    FIND_PACKAGE(GsysC)
    SET(CMAKE_AUTOMOC ON)
    FIND_PACKAGE(Qt5 COMPONENTS Core Gui Widgets REQUIRED )
ENDIF(GSYSC_ENABLED)

# Link this 'library' to use the warnings specified in CompilerWarnings.cmake
ADD_LIBRARY(project_warnings INTERFACE)
# Standard compiler warnings
set_project_warnings(project_warnings)


# Create VCGRA with all its components
ADD_EXECUTABLE (vcgra_only
    ""
)
IF(WARNINGS_ENABLED)
TARGET_LINK_LIBRARIES(vcgra_only
    PRIVATE
        project_warnings
)
ENDIF()

# Create VCGRA and pre-fetchers with all its components
ADD_EXECUTABLE (
  vcgra_prefetchers
  ""
)
IF(WARNINGS_ENABLED)
TARGET_LINK_LIBRARIES(vcgra_prefetchers
    PRIVATE
        project_warnings
)
ENDIF()

# Create full_architecture with all its components
ADD_EXECUTABLE (
  full_architecture
  ""
)
IF(WARNINGS_ENABLED)
TARGET_LINK_LIBRARIES(full_architecture
    PRIVATE
        project_warnings
)
ENDIF()

ADD_SUBDIRECTORY (src/)

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
