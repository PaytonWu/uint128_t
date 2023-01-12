find_package(GTest REQUIRED)
include(GoogleTest)

aux_source_directory(testcases UINT128_TEST_SOURCES)

add_executable(tests ${UINT128_TEST_SOURCES})

target_include_directories(tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}../../include)
target_link_libraries(tests PRIVATE ${UINT128_LIB} GTest::GTest GTest::Main)
gtest_discover_tests(tests)

