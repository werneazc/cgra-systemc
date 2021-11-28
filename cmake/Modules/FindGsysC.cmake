# * Find GsysC Find the gSysC libraries
#
# This module defines the following variables: GSYSC_FOUND       - True if
# GSYSC_INCLUDE_DIR & GSYSC_LIBRARY are found GSYSC_LIBRARIES   - Set when
# GSYSC_LIBRARY is found GSYSC_INCLUDE_DIRS - Set when GSYSC_INCLUDE_DIR is
# found
#
# GSYSC_INCLUDE_DIR - where to find gsysc.h, etc. GSYSC_LIBRARY     - the gSysC
# library
#

# =============================================================================
# Copyright 2020 André Werner andre.werner@b-tu.de
#
# Distributed under the OSI-approved BSD License (the "License");
#
# This software is distributed WITHOUT ANY WARRANTY; without even the implied
# warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# License for more information.
# =============================================================================

find_path(
  GSYSC_INCLUDE_DIR
  NAMES gsysc.h gsysc
  HINTS $ENV{GSYSC_BASE} $ENV{GSYSC} $ENV{GSYSC_ROOT}
  PATH_SUFFIXES include
  DOC "The gSysC include directory")

find_library(
  GSYSC_LIBRARY
  NAMES gsysc
  HINTS $ENV{GSYSC_BASE} $ENV{GSYSC} $ENV{GSYSC_ROOT}
  PATH_SUFFIXES
    lib
    lib-linux
    lib-linux64
    lib-macosx
    lib-macosx64
    lib-darwin
    lib64
    build
  DOC "The gSysC library")

# handle the QUIETLY and REQUIRED arguments and set SYSTEMC_FOUND to TRUE if 
# all listed variables are TRUE
include(${CMAKE_CURRENT_LIST_DIR}/FindPackageHandleStandardArgs.cmake)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GsysC
                                  DEFAULT_MSG
                                  GSYSC_LIBRARY GSYSC_INCLUDE_DIR
                                  )

if(GSYSC_FOUND)
  set(GSYSC_LIBRARIES ${GSYSC_LIBRARY})
  set(GSYSC_INCLUDE_DIRS ${GSYSC_INCLUDE_DIR})
endif()
