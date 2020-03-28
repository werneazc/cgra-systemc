cmake_minimum_required(VERSION 3.10)
project(cgra_systemc
    VERSION 0.1.0
    DESCRIPTION "Virtual Coarse Grain Reconfigurable Array Simulator"
    LANGUAGES CXX)
    
set(CMAKE_PREFIX_PATH /opt/systemc)
find_package(SystemCLanguage REQUIRED CONFIG)

#However, the file(GLOB...) allows for wildcard additions:
file(GLOB SOURCES LIST_DIRECTORIES FALSE "src/*.cpp")

#Create project executable and link SystemC library 
add_executable(cgra_systemc ${SOURCES})
target_include_directories(cgra_systemc
    PUBLIC
        header/
    )
target_compile_features(cgra_systemc
    PUBLIC
        cxx_std_14
    )
target_link_libraries(cgra_systemc SystemC::systemc)

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
