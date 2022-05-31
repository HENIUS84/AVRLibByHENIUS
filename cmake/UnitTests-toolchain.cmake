cmake_minimum_required(VERSION 3.16)

include(FetchContent)
set(FETCHCONTENT_QUIET off)
include(${CMAKE_LIB_DIR}/Utils.cmake)

################################
# Project settings
################################

enable_testing()

################################
# Test coverage tool
################################

find_coverage_tool()

################################
# Compiler settings
################################

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
    -g \
    -DGTEST_USE_OWN_TR1_TUPLE=1 \
    -DGTEST_HAS_TR1_TUPLE=1 \
    -std=gnu++14 \
    -coverage -fprofile-arcs -ftest-coverage")

################################
# GTest
################################

option(CMAKE_USE_WIN32_THREADS_INIT "using WIN32 threads" ON)
option(gtest_disable_pthreads "Disable uses of pthreads in gtest." ON)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        master
)

FetchContent_GetProperties(googletest)
if(NOT googletest_POPULATED)
  message(STATUS "Fetching Google Test...")
  FetchContent_Populate(googletest)
  add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})
endif()

# ******************************************************************************
# Configures the project.
# ******************************************************************************
function(configure_unit_tests ENABLE_POST)
    add_executable(${CMAKE_PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp ${TEST_SRC_FILES})
    target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC gmock gmock_main)
	if (${ENABLE_POST} EQUAL 1)
		add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
						   COMMAND ${CMAKE_COMMAND}
						   -DPROJECT_NAME=${CMAKE_PROJECT_NAME}
						   -DTESTED_SOURCE_DIR=${TESTED_SOURCE_DIR}
						   -DCMAKE_LIB_DIR=${CMAKE_LIB_DIR}
						   -P ${CMAKE_LIB_DIR}/UnitTests-POST.cmake)
	endif()
endfunction()
