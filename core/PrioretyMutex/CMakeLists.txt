project(Priotex VERSION 1.0.1 LANGUAGES CXX)

set(SOURCES src/Priotex.cpp)
add_library(PriotexLib ${SOURCES})
target_include_directories(PriotexLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_features(PriotexLib PUBLIC cxx_std_23)
target_compile_options(PriotexLib PRIVATE "/Zc:preprocessor")

add_executable(PriotexExample examples/testExample.cpp)
target_link_libraries(PriotexExample PRIVATE PriotexLib)
target_compile_features(PriotexExample PUBLIC cxx_std_23)
target_compile_options(PriotexExample PRIVATE "/Zc:preprocessor")

if (BUILD_TESTING)
    file(GLOB TEST_SOURCES tests/*.cpp)
    if (TEST_SOURCES)
        add_executable(PriotexTests ${TEST_SOURCES})
        target_link_libraries(PriotexTests
            PRIVATE
                PriotexLib
                gtest_main
                MHopeLib
        )
        add_test(NAME PriotexTests COMMAND PriotexTests)
    endif()
endif()