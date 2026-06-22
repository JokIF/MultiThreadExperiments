cmake_minimum_required(VERSION 3.15)
project(MultiThreadExperiments VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SOURCES
    main.cpp    
)

add_executable(MTE_APP ${SOURCES})

set_target_properties(MTE_APP PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

target_compile_features(MTE_APP PUBLIC cxx_std_23)
target_compile_options(MTE_APP PRIVATE "/Zc:preprocessor")

add_subdirectory(libs/MHope)
target_link_libraries(MTE_APP PRIVATE MHopeLib)