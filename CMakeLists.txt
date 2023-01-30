cmake_minimum_required(VERSION 3.22)

project(
    UINT128_T
    VERSION 0.1.0
    DESCRIPTION "An unsigned 128 bit integer type for C++"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(UINT128_LIBRARY uint128)

if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "MSVC")
    add_compile_options(
        /Zc:__cplusplus       # https://learn.microsoft.com/en-us/cpp/build/reference/zc-cplusplus
        /Zc:enumTypes         # https://learn.microsoft.com/en-us/cpp/build/reference/zc-enumtypes
        /Zc:externC           # https://learn.microsoft.com/en-us/cpp/build/reference/zc-externc
        /Zc:externConstexpr   # https://learn.microsoft.com/en-us/cpp/build/reference/zc-externconstexpr
        /Zc:inline            # https://learn.microsoft.com/en-us/cpp/build/reference/zc-inline-remove-unreferenced-comdat
        /Zc:preprocessor      # https://learn.microsoft.com/en-us/cpp/build/reference/zc-preprocessor
        /Zc:referenceBinding  # https://learn.microsoft.com/en-us/cpp/build/reference/zc-referencebinding-enforce-reference-binding-rules
        /Zc:rvalueCast        # https://learn.microsoft.com/en-us/cpp/build/reference/zc-rvaluecast-enforce-type-conversion-rules
        /Zc:strictStrings     # https://learn.microsoft.com/en-us/cpp/build/reference/zc-strictstrings-disable-string-literal-type-conversion
        /Zc:templateScope     # https://learn.microsoft.com/en-us/cpp/build/reference/zc-templatescope
        /Zc:ternary           # https://learn.microsoft.com/en-us/cpp/build/reference/zc-ternary
        /Zc:throwingNew       # https://learn.microsoft.com/en-us/cpp/build/reference/zc-throwingnew-assume-operator-new-throws
        /permissive-          # https://blogs.msdn.microsoft.com/vcblog/2016/11/16/permissive-switch/
                              # https://blogs.msdn.microsoft.com/vcblog/2017/09/11/two-phase-name-lookup-support-comes-to-msvc/
                              # https://learn.microsoft.com/en-us/cpp/build/reference/permissive-standards-conformance
        /volatile:iso         # https://learn.microsoft.com/en-us/cpp/build/reference/volatile-volatile-keyword-interpretation
        /GS
        /Zi
        /EHsc
        /fp:precise
        /errorReport:prompt
        /diagnostics:caret    # https://learn.microsoft.com/en-us/cpp/build/reference/diagnostics-compiler-diagnostic-options
        /utf-8                # https://learn.microsoft.com/en-us/cpp/build/reference/utf-8-set-source-and-executable-character-sets-to-utf-8
        /WL                   # https://learn.microsoft.com/en-us/cpp/build/reference/wl-enable-one-line-diagnostics
    )
endif()

aux_source_directory(src SRCS)
add_library(${UINT128_LIBRARY} ${SRCS})

if (BUILD_SHARED_LIBS)
    target_compile_definitions(${UINT128_LIBRARY} PRIVATE UINT128_T_DLL UINT128_T_EXPORT)
endif()

if (WITH_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

if (BUILD_SHARED_LIBS)
    install(
        TARGETS ${UINT128_LIBRARY}
        COMPONENT runtime
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)
else()
    install(
        TARGETS ${UINT128_LIBRARY}
        COMPONENT devel
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)
endif()

install(
    DIRECTORY include/uint128
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    COMPONENT devel
)
