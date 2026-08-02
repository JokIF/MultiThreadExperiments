add_mte_interface_library_with_settings(ThreadSafeStackLib)

if (BUILD_TESTING)
    add_mte_gtest_executable(ThreadSafeStackTests
        "ThreadSafeStackLib"
        tests/SimpleThreadSafeStackTests.cpp)
endif()