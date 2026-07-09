project(ThreadSafeQueue VERSION 0.0.5 LANGUAGES CXX)

set(QueueIncludes 
    ${CMAKE_CURRENT_SOURCE_DIR}/include/SimpleThreadSafeQueue.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/NonThreadSafeQueue.h
)
add_library(ThreadSafeQueueLib INTERFACE)
target_include_directories(ThreadSafeQueueLib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_sources(ThreadSafeQueueLib INTERFACE ${QueueIncludes})
target_compile_features(ThreadSafeQueueLib INTERFACE cxx_std_23)

if (BUILD_TESTING)
    file(GLOB TEST_SOURCES tests/*.cpp)
    if (TEST_SOURCES)
        add_executable(ThreadSafeQueueTests ${TEST_SOURCES})
        target_link_libraries(ThreadSafeQueueTests
            PRIVATE
                ThreadSafeQueueLib
                gtest_main
        )
        add_test(NAME ThreadSafeQueueTests COMMAND ThreadSafeQueueTests)
    endif()
endif()
