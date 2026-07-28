# ---------------------Project settings------------------------
cmake_minimum_required(VERSION 3.20)
project(MultiThreadExperiments VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(BUILD_TESTING "Build all tests" ON)
option(BUILD_EXAMPLES "Build eny examples" ON)
option(USE_MHOPE "Use MHope library (submodule)" ON)

include(cmake/MTECompilerOptions.cmake)
include(cmake/MTEUtilities.cmake)

# -----------------------dependencies--------------------------
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
endif()

# --------------------Include All Modules----------------------

add_subdirectory(core/Atomic)
add_subdirectory(core/PrioretyMutex)
add_subdirectory(core/ThreadSafeMap)
add_subdirectory(core/ThreadSafeQueue)
add_subdirectory(core/ThreadSafeStack)

if (USE_MHOPE)
    add_subdirectory(libs/MHope)
endif()
