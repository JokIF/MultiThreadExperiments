project(ThreadSafeMap VERSION 0.1.0 LANGUAGES CXX)

set(ThreadSafeHeaders
    ${CMAKE_CURRENT_SOURCE_DIR}/include/SimpleThreadSafeMap.h
)

add_library(ThreadSafeMapLib INTERFACE)
target_include_directories(ThreadSafeMapLib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_sources(ThreadSafeMapLib INTERFACE ${ThreadSafeHeaders})
target_compile_features(ThreadSafeMapLib INTERFACE cxx_std_23)

if (BUILD_TESTING)
    file(GLOB TEST_SOURCES tests/*.cpp)
    if (TEST_SOURCES)
        add_executable(ThreadSafeMapTests ${TEST_SOURCES})
        target_link_libraries(ThreadSafeMapTests
            PRIVATE
                ThreadSafeMapLib
                gtest_main
        )
        add_test(NAME ThreadSafeMapTests COMMAND ThreadSafeMapTests)
    endif()
endif()