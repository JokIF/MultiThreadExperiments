add_mte_public_library_with_settings(ThreadSafeStackLib src/SimpleThreadSafeStack.cpp)

if (BUILD_TESTING)
    add_mte_gtest_executable(ThreadSafeStackTests
        "ThreadSafeStackLib"
        tests/SimpleThreadSafeStackTests.cpp)
endif()