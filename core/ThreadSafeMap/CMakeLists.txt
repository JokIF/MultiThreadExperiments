add_mte_interface_library_with_settings(ThreadSafeMapLib)

if (BUILD_TESTING)
    add_mte_gtest_executable(ThreadSafeMapTests
        "ThreadSafeMapLib"
        tests/SimpleThreadSafeMapTests.cpp
    )
endif()