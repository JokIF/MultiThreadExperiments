add_mte_interface_library_with_settings(ThreadSafeQueueLib)

if (BUILD_TESTING)
    add_mte_gtest_executable(ThreadSafeQueueTests
        "ThreadSafeQueueLib"
        tests/NonThreadSafeQueueTests.cpp
        tests/SimpleThreadSafeQueueTests.cpp
        tests/ThreadSafeQueueTests.cpp
    )
endif()
