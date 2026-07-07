project(SimpleThreadSafeStack VERSION 1.0.0 LANGUAGES CXX)

set(SOURCES_SIMPLE_STACK_LIB src/ThreadSafeStack.cpp)
add_library(SimpleStackLib ${SOURCES_SIMPLE_STACK_LIB})
target_include_directories(SimpleStackLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_features(SimpleStackLib PUBLIC cxx_std_23)
target_compile_options(SimpleStackLib PRIVATE "/Zc:preprocessor")

