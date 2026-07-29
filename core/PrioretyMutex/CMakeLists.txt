add_mte_public_library_with_settings(PriotexLib src/Priotex.cpp)

if (BUILD_EXAMPLES)
    add_mte_example_with_settings(PriotexExample examples/testExample.cpp)
    target_link_libraries(PriotexExample PRIVATE PriotexLib)
endif()

if (BUILD_TESTING)
    target_compile_definitions(PriotexLib PRIVATE BUILD_TESTING)

    add_mte_gtest_executable(PriotexTests
        "PriotexLib"
        tests/CoupleThreadsTests.cpp
        tests/OneThreadTests.cpp
    )
endif()