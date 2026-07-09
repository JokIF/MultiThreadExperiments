project(ThreadSafeQueue VERSION 0.0.2 LANGUAGES CXX)

set(QueueIncludes 
    ${CMAKE_CURRENT_SOURCE_DIR}/include/SimpleThreadSafeQueue.h
)
add_library(ThreadSafeQueueLib INTERFACE)
target_include_directories(ThreadSafeQueueLib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_sources(ThreadSafeQueueLib INTERFACE ${QueueIncludes})
target_compile_features(ThreadSafeQueueLib INTERFACE cxx_std_23)
