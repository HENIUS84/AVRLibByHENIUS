# ******************************************************************************
# Executes Python function.
# retval Result of function execution
# ******************************************************************************
function(run_python_function FUNCTION_NAME RET_VALUE)
	set(SCRIPT_PATH ${C_LIB_BY_HENIUS_DIR}\\cmake)
	string(REPLACE "\\" "\\\\" SCRIPT_PATH ${SCRIPT_PATH})
	execute_process(
		COMMAND python -c
       "import sys; \
        sys.path.append('${SCRIPT_PATH}'); \
        from Utils import *; \
        ${FUNCTION_NAME}()"
	    OUTPUT_VARIABLE RET_VALUE_TMP)
	string(REGEX REPLACE "\n$" "" RET_VALUE_TMP "${RET_VALUE_TMP}")
	set(${RET_VALUE} ${RET_VALUE_TMP} PARENT_SCOPE)
endfunction()

function(get_opencppcoverage_version)
endfunction()

# ******************************************************************************
# Searches the gcovr progarm.
# ******************************************************************************
function(find_coverage_tool)
	if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
		run_python_function(get_opencppcoverage_version TOOL_VERSION)
	else()
		run_python_function(get_gcovr_version TOOL_VERSION)	
	endif()
	message("Found test coverage tool: " ${TOOL_VERSION})
endfunction()

# ******************************************************************************
# Replaces specified text in file.
# ******************************************************************************
function(replace_text_in_file INPUT_FILE SEARCHED_TEXT REPLACED_TEXT)
	file(READ ${INPUT_FILE} filedata)
	string(REGEX REPLACE
	       "${SEARCHED_TEXT}"
		   "${REPLACED_TEXT}"
		   filedata
		   "${filedata}")
	file(WRITE ${INPUT_FILE} "${filedata}")
endfunction()

# ******************************************************************************
# Generates test coverage report for MSVC compiler.
# ******************************************************************************
function(generate_coverage_report_MSVC FIRMWARE_DIR HTML_REPORT_DIR EXECUTABLE_FILE)
	message("Coverage report generating for MSVC compiler")
	file(TO_NATIVE_PATH "${FIRMWARE_DIR}" FIRMWARE_DIR)
	file(TO_NATIVE_PATH "${HTML_REPORT_DIR}" HTML_REPORT_DIR)
	file(TO_NATIVE_PATH "${EXECUTABLE_FILE}" EXECUTABLE_FILE)
	execute_process(
			COMMAND OpenCppCoverage 
			--verbose
			--sources ${FIRMWARE_DIR}
			--excluded_sources googletest			
			--excluded_sources main.cpp		
			--excluded_sources tests
			--export_type html:${HTML_REPORT_DIR}
			${EXECUTABLE_FILE})
endfunction()

# ******************************************************************************
# Generates test coverage report for GCC compiler.
# ******************************************************************************
function(generate_coverage_report_GCC FIRMWARE_DIR HTML_REPORT_DIR EXECUTABLE_FILE)
	message("Coverage report generating for GCC compiler")
	file(MAKE_DIRECTORY ${HTML_REPORT_DIR})
	execute_process(COMMAND ${EXECUTABLE_FILE})
	set(REPORT_FILE_PATH ${HTML_REPORT_DIR}/index.html)
	execute_process(
			COMMAND gcovr 
			--html
			--html-details
			--sort-percentage
			--exclude ".*googletest.*"
			--exclude ".*main.cpp.*"
			--exclude ".*base_mock.h.*"
			--exclude ".*tests.*"
			--exclude ".*mocks.*"
			-r ${FIRMWARE_DIR}
			-o ${REPORT_FILE_PATH})
			
	# Workarround for not woking --html-title in gcovr
	file (GLOB_RECURSE HTML_FILES REC ${HTML_REPORT_DIR}/*.html)
	foreach(file ${HTML_FILES})
		replace_text_in_file(${file}
						     "GCC Code Coverage Report"
						     "${SOLUTION_NAME} Code Coverage Report")
	endforeach()	
endfunction()

# ******************************************************************************
# Generates test coverage report for current compiler.
# ******************************************************************************
function(generate_coverage_report FIRMWARE_DIR HTML_REPORT_DIR EXECUTABLE_FILE)
	if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
		generate_coverage_report_MSVC(
			${FIRMWARE_DIR}
			${HTML_REPORT_DIR}
			${EXECUTABLE_FILE})
	else()
		generate_coverage_report_GCC(
			${FIRMWARE_DIR}
			${HTML_REPORT_DIR}
			${EXECUTABLE_FILE})			
	endif()
	message("Report generated")	
endfunction()

# ******************************************************************************
# Converts a camel case string to lower case and underscores.
# :param str: the string
# :type str: string
# :param var: the output variable name
# :type var: string
# ******************************************************************************
function(string_camel_case_to_lower_case_underscore str var)
  # insert an underscore before any upper case letter
  # which is not followed by another upper case letter
  string(REGEX REPLACE "(.)([A-Z][a-z]+)" "\\1_\\2" value "${str}")
  # insert an underscore before any upper case letter
  # which is preseded by a lower case letter or number
  string(REGEX REPLACE "([a-z0-9])([A-Z])" "\\1_\\2" value "${value}")
  string(TOLOWER "${value}" value)
  set(${var} "${value}" PARENT_SCOPE)
endfunction()