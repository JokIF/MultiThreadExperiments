project(atomic VERSION 1.0.0 LANGUAGES CXX)

add_executable(AtomicRelaxedExample examples/example_memory_order_relaxed.cpp)
target_compile_features(AtomicRelaxedExample PUBLIC cxx_std_23)
target_compile_options(AtomicRelaxedExample PRIVATE "/Zc:preprocessor")

add_executable(AtomicConsumeExample examples/example_memory_order_consume.cpp)
target_compile_features(AtomicConsumeExample PUBLIC cxx_std_23)
target_compile_options(AtomicConsumeExample PRIVATE "/Zc:preprocessor")

add_executable(AtomicThreeThreadsSyncingExample examples/example_three_threads_syncing.cpp)
target_compile_features(AtomicThreeThreadsSyncingExample PUBLIC cxx_std_23)
target_compile_options(AtomicThreeThreadsSyncingExample PRIVATE "/Zc:preprocessor")


add_executable(AtomicBarrierExample examples/example_barrier.cpp)
target_compile_features(AtomicBarrierExample PUBLIC cxx_std_23)
target_compile_options(AtomicBarrierExample PRIVATE "/Zc:preprocessor")

add_executable(AtomicPopulateQueueExample examples/example_populate_queue.cpp)
target_compile_features(AtomicPopulateQueueExample PUBLIC cxx_std_23)
target_compile_options(AtomicPopulateQueueExample PRIVATE "/Zc:preprocessor")

if (BUILD_TESTING)
    file(GLOB ATOMIC_TEST_SOURCES tests/*.cpp)
    if (ATOMIC_TEST_SOURCES)
        add_executable(AtomicTests ${ATOMIC_TEST_SOURCES})
        target_link_libraries(AtomicTests PRIVATE gtest_main)
        target_compile_features(AtomicTests PUBLIC cxx_std_23)
        target_compile_options(AtomicTests PRIVATE "/Zc:preprocessor")
        add_test(NAME AtomicTests COMMAND AtomicTests)
    endif()
endif()
