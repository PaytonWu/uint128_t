find_package(GTest REQUIRED)
include(GoogleTest)

if (BUILD_SHARED_LIBS)
    add_definitions(-DUINT128_T_DLL)
endif()

aux_source_directory(testcases UINT128_TEST_SOURCES)

add_executable(tests ${UINT128_TEST_SOURCES})

target_include_directories(tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)
target_link_directories(tests PRIVATE ${PROJECT_BINARY_DIR})
target_link_libraries(tests PRIVATE ${UINT128_LIBRARY} GTest::GTest GTest::Main)
gtest_discover_tests(tests)
