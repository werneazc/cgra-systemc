add_library(COMPONENTS OBJECT CommandInterpreter.cpp Management_Unit.cpp
                              MMU.cpp VCGRA.cpp)
target_include_directories(COMPONENTS PUBLIC ${PROJECT_SOURCE_DIR}/header)
target_compile_features(COMPONENTS PRIVATE cxx_std_14)
target_link_libraries(COMPONENTS PRIVATE SystemC::systemc)
if(MCPAT_ENABLED)
  add_compile_definitions(MCPAT)
endif(MCPAT_ENABLED)