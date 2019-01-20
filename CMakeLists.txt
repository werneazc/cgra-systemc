cmake_minimum_required(VERSION 2.6)

project(cgra_systemc)

#Bring the headers from project include folder and systemc include dir to project
include_directories(header "$ENV{SYSTEMC_ROOT}/include")

#However, the file(GLOB...) allows for wildcard additions:
file(GLOB SOURCES LIST_DIRECTORIES FALSE "src/*.cpp")

#Add SystemC library to dependencies of project
find_library(SYSTEMC systemc PATHS "$ENV{SYSTEMC_ROOT}/lib64")

#Create project executable and link SystemC library 
add_executable(cgra_systemc ${SOURCES})
target_link_libraries(cgra_systemc ${SYSTEMC})

find_package(Doxygen)
set(DOXFILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/doc/cgra-systemc)
# set(DOXFILE ${CMAKE_CURRENT_BINARY_DIR}/cgra_systemc)
# configure_file(${DOXFILE_IN} ${DOXFILE} @ONLY)

add_custom_target(
doc ALL
COMMAND ${DOXYGEN_EXECUTABLE} ${DOXFILE_IN}
WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/doc
COMMENT "Generating doxygen documentation"
VERBATIM
)

#install(TARGETS cgra_systemc RUNTIME DESTINATION bin)
