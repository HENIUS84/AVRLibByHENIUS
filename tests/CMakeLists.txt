cmake_minimum_required(VERSION 3.16)

################################
# Paths
################################

string(REPLACE "\\" "/" C_LIB_BY_HENIUS_DIR ${C_LIB_BY_HENIUS_DIR})

# cmake libs directory resolving
set (CMAKE_LIB_DIR "${C_LIB_BY_HENIUS_DIR}/cmake")
include(${CMAKE_LIB_DIR}/Utils.cmake)

# Tested source files directory resolving
set(TESTED_SOURCE_DIR ${C_LIB_BY_HENIUS_DIR}/source)

################################
# Project settings
################################

set(PROJECT_NAME "CLibByHenius_Tests")
project(${PROJECT_NAME})
add_compile_definitions(I2C_DEBUG_ENABLED)
include(${CMAKE_LIB_DIR}/UnitTests-toolchain.cmake)

################################
# Tests
################################

include_directories(
    mocks
	${C_LIB_BY_HENIUS_DIR}/include
	${C_LIB_BY_HENIUS_DIR}/include/gtest
	${TESTED_SOURCE_DIR}/utils
	${C_LIB_BY_HENIUS_DIR}/include/utils
	${TESTED_SOURCE_DIR}/avr/drivers
	${C_LIB_BY_HENIUS_DIR}/include/avr/drivers
	${googletest_SOURCE_DIR}/googlemock/include)

file (GLOB_RECURSE TEST_SRC_FILES REC
      ${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/mocks/*.cpp)
configure_unit_tests(${DISABLE_POST})
