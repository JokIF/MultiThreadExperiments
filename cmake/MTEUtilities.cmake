function(add_mte_example_with_settings TARGET_NAME)
    add_executable(${TARGET_NAME} ${ARGN})
    target_link_libraries(${TARGET_NAME} PRIVATE MteCompileOptions)
endfunction()

function(add_mte_interface_library_with_settings TARGET_NAME)
    add_library(${TARGET_NAME} INTERFACE)
    
    target_include_directories(${TARGET_NAME}
        INTERFACE
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    )

    target_link_libraries(${TARGET_NAME} 
        INTERFACE 
            MteCompileOptions
    )
endfunction()

function(add_mte_public_library_with_settings TARGET_NAME)
    add_library(${TARGET_NAME} ${ARGN})
    
    target_include_directories(${TARGET_NAME}
        PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    )

    target_link_libraries(${TARGET_NAME} 
        PUBLIC 
            MteCompileOptions
    )
endfunction()

function(add_mte_gtest_executable TARGET_NAME LIBS_NAMES)
    add_executable(${TARGET_NAME} ${ARGN})
    target_link_libraries(${TARGET_NAME} 
        PRIVATE 
            GTest::gtest_main
            MteCompileOptions
            ${LIBS_NAMES}
    )
    target_compile_definitions(${TARGET_NAME} PRIVATE BUILD_TESTING)
    add_test(NAME ${TARGET_NAME} COMMAND ${TARGET_NAME})
endfunction()