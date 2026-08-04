add_library(MteCompileOptions INTERFACE)

target_compile_options(MteCompileOptions INTERFACE
    $<$<CXX_COMPILER_ID:MSVC>:
        /W4
        /permissive-
        /Zc:preprocessor
        /Zc:__cplusplus
        /EHsc
        /MP
        /sdl
        /volatile:iso
        /diagnostics:caret
        /utf-8
    >

    $<$<CXX_COMPILER_ID:GNU>:
        -Wall -Wextra -Wpedantic
        -Wshadow -Wnon-virtual-dtor -Wold-style-cast
        -Wcast-align -Wunused -Woverloaded-virtual
        -Wconversion -Wsign-conversion
        -Wdouble-promotion -Wformat=2 -Wmissing-declarations
        -Wredundant-decls -Wundef -Wstrict-aliasing
        -Wlogical-op
        -fconcepts-diagnostics-depth=0
    >

    $<$<CXX_COMPILER_ID:Clang,AppleClang>:
        -Wall -Wextra -Wpedantic
        -Wshadow -Wnon-virtual-dtor -Wold-style-cast
        -Wcast-align -Wunused -Woverloaded-virtual
        -Wconversion -Wsign-conversion
        -Wdouble-promotion -Wformat=2 -Wmissing-declarations
        -Wredundant-decls -Wundef -Wstrict-aliasing
        -Wthread-safety -Wthread-safety-beta -Wmove
    >
)

target_link_options(MteCompileOptions INTERFACE
    $<$<CXX_COMPILER_ID:MSVC>:/guard:cf>
)