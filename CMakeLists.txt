# ---------------------Project settings------------------------
cmake_minimum_required(VERSION 3.15)
project(MultiThreadExperiments VERSION 0.0.3 LANGUAGES CXX)

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(BUILD_TESTING "Build all tests" ON)
option(BUILD_EXAMPLES "Build eny examples" ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# -----------------------dependencies--------------------------

add_subdirectory(libs/MHope)

# Testing (GoogleTest)
if (BUILD_TESTING)
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY  https://github.com/google/googletest.git
        GIT_TAG         v1.14.0
    )
    FetchContent_MakeAvailable(googletest)
    
    enable_testing()

    add_definitions(-DBUILD_TESTING)
endif()

# --------------------Include All Modules----------------------

file(GLOB MODULES_DIRS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/core ${CMAKE_CURRENT_SOURCE_DIR}/core/*)
foreach(MODULE_DIR ${MODULES_DIRS})
    if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/core/${MODULE_DIR})
        if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/core/${MODULE_DIR}/CMakeLists.txt)
            add_subdirectory(core/${MODULE_DIR})
        endif()
    endif()
endforeach()
