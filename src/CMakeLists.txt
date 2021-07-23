TARGET_SOURCES(vcgra_only
    PRIVATE
        ./vcgra_only.cpp
        ./testbench_vcgra_only.cpp
        ./VCGRA.cpp
)
TARGET_INCLUDE_DIRECTORIES (vcgra_only
  PRIVATE ${CMAKE_SOURCE_DIR}/header/
  PRIVATE ${SYSTEMC_INCLUDE_DIRS}
)
TARGET_COMPILE_FEATURES (vcgra_only PRIVATE cxx_std_14)
TARGET_LINK_LIBRARIES (vcgra_only
  PRIVATE "${SYSTEMC_LIBRARIES}"
)
IF (MCPAT_ENABLED)
    TARGET_COMPILE_DEFINITIONS(vcgra_only
        PRIVATE
            MCPAT)
ENDIF (MCPAT_ENABLED)
IF (GSYSC_ENABLED)
    TARGET_INCLUDE_DIRECTORIES (vcgra_only
        PRIVATE ${GSYSC_INCLUDE_DIRS}
        PRIVATE Qt5Widget_INCLUDE_DIR)
    TARGET_LINK_LIBRARIES (vcgra_only
        PRIVATE ${GSYSC_LIBRARY}
        PRIVATE Threads::Threads
        PRIVATE Qt5::Widgets)
    TARGET_COMPILE_DEFINITIONS(vcgra_only
        PRIVATE
            GSYSC)
    TARGET_COMPILE_DEFINITIONS(vcgra_only
        PRIVATE SC_USE_SC_STRING_OLD)
ENDIF(GSYSC_ENABLED)


TARGET_SOURCES(vcgra_prefetchers
    PRIVATE
        ./TopLevel_VcgraPreloaders.cpp
        ./Testbench_VcgraPreloaders.cpp
        ./vcgra_prefetchers.cpp
        ./VCGRA.cpp
)
TARGET_INCLUDE_DIRECTORIES (vcgra_prefetchers
  PRIVATE ${CMAKE_SOURCE_DIR}/header/
  PRIVATE ${SYSTEMC_INCLUDE_DIRS}
)
TARGET_COMPILE_FEATURES (vcgra_prefetchers PUBLIC cxx_std_14)
TARGET_LINK_LIBRARIES (vcgra_prefetchers
  PRIVATE "${SYSTEMC_LIBRARIES}"
)
IF (MCPAT_ENABLED)
    TARGET_COMPILE_DEFINITIONS(vcgra_prefetchers
        PRIVATE
            MCPAT)
ENDIF (MCPAT_ENABLED)
IF (GSYSC_ENABLED)
    TARGET_INCLUDE_DIRECTORIES (vcgra_prefetchers
        PRIVATE ${GSYSC_INCLUDE_DIRS}
        PRIVATE Qt5Widget_INCLUDE_DIR)
    TARGET_LINK_LIBRARIES (vcgra_prefetchers
        PRIVATE ${GSYSC_LIBRARY}
        PRIVATE Threads::Threads
        PRIVATE Qt5::Widgets)
    TARGET_COMPILE_DEFINITIONS(vcgra_prefetchers
        PRIVATE
            GSYSC)
    TARGET_COMPILE_DEFINITIONS(vcgra_prefetchers
        PRIVATE SC_USE_SC_STRING_OLD)
ENDIF(GSYSC_ENABLED)


TARGET_SOURCES(full_architecture
    PRIVATE
        ./sc_main.cpp
        ./TopLevel.cpp
        ./Testbench_TopLevel.cpp
        ./VCGRA.cpp
        ./Management_Unit.cpp
        ./MMU.cpp
        ./CommandInterpreter.cpp
)
TARGET_INCLUDE_DIRECTORIES (full_architecture
  PRIVATE ${CMAKE_SOURCE_DIR}/header/
  PRIVATE ${SYSTEMC_INCLUDE_DIRS}
)
TARGET_COMPILE_FEATURES (full_architecture PUBLIC cxx_std_14)
TARGET_LINK_LIBRARIES (full_architecture
  PRIVATE "${SYSTEMC_LIBRARIES}"
)
IF (MCPAT_ENABLED)
    TARGET_COMPILE_DEFINITIONS(full_architecture
        PRIVATE
            MCPAT)
ENDIF (MCPAT_ENABLED)
IF (GSYSC_ENABLED)
    TARGET_INCLUDE_DIRECTORIES (full_architecture
        PRIVATE ${GSYSC_INCLUDE_DIRS}
        PRIVATE Qt5Widget_INCLUDE_DIR)
    TARGET_LINK_LIBRARIES (full_architecture
        PRIVATE ${GSYSC_LIBRARY}
        PRIVATE Threads::Threads
        PRIVATE Qt5::Widgets)
    TARGET_COMPILE_DEFINITIONS(full_architecture
        PRIVATE
            GSYSC)
    TARGET_COMPILE_DEFINITIONS(full_architecture
        PRIVATE SC_USE_SC_STRING_OLD)
ENDIF(GSYSC_ENABLED)