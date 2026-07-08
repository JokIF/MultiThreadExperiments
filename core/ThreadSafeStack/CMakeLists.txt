project(ThreadSafeStack VERSION 1.0.0 LANGUAGES CXX)

set(SOURCES_THREAD_SAFE_STACK_LIB src/SimpleThreadSafeStack.cpp)
add_library(ThreadSafeStackLib ${SOURCES_THREAD_SAFE_STACK_LIB})
target_include_directories(ThreadSafeStackLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_features(ThreadSafeStackLib PUBLIC cxx_std_23)
target_compile_options(ThreadSafeStackLib PRIVATE "/Zc:preprocessor")

if (BUILD_TESTING)
    file(GLOB TEST_SOURCES tests/*.cpp)
    if (TEST_SOURCES)
        add_executable(ThreadSafeStackTests ${TEST_SOURCES})
        target_link_libraries(ThreadSafeStackTests
            PRIVATE
                ThreadSafeStackLib
                gtest_main
        )
        add_test(NAME ThreadSafeStackTests COMMAND ThreadSafeStackTests)
    endif()
endif()