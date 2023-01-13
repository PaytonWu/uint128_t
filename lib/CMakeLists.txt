if (BUILD_SHARED_LIBS)
    add_definitions(-DUINT128_T_DLL -DUINT128_T_EXPORT)
endif()

set(SRCS uint128_t.cpp)

add_library(${UINT128_LIBRARY} ${SRCS})
set_property(
    TARGET uint128
    PROPERTY POSITION_INDEPENDENT_CODE ON)
