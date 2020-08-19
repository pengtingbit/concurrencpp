# Configuration script for the project.
# Execute cmake on this directory to (re)generate the
# Projectfiles/Makefiles
cmake_minimum_required(VERSION 3.18.0)

set(PROJECTNAME "concurrencpp")
project("concurrencpp" C CXX)
message(STATUS "### ${PROJECT_NAME} ###")

set(CMAKE_CXX_COMPILER /usr/bin/clang++)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build Type: Debug or Flight")
    
# detect if tasking is build standalone
if (CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    set(GCOVR_SCRIPT gcovr CACHE STRING "Path to the gcovr script")
    set(GCOVR_OPTIONS --object-directory="${CMAKE_BINARY_DIR}/src" -r ${CMAKE_SOURCE_DIR} --xml -o ${CMAKE_BINARY_DIR}/coverage.xml CACHE STRING "String gcovr options")
    set(CPPCHECK_OPTIONS src include -I include -I include/executors -I include/results -I include/runtime -I include/threads -I include/timers -I include/utils -I src/executors -I src/results -I src/runtime -I src/threads -I src/timers -I src/utils --enable=all --inconclusive -i contrib --inline-suppr --xml --xml-version=2  2> ${CMAKE_BINARY_DIR}/cppcheck.xml CACHE STRING "cppcheck options")

    set(STANDALONE_BUILD TRUE)
     
else()
    set(STANDALONE_BUILD FALSE)
    
endif()
        
set(BARE_METAL "linux" CACHE STRING "Select the bare metal code")
set(USED_CORES 1 CACHE STRING "Number of cores used by the scheduler")
set(BOARD_DRIVER "board" CACHE STRING "Specific hardware driver for communication")

#set(NODE_LOGIC_ID 10)

find_package(Threads REQUIRED)

set(CLANG_COROUTINES_COMPILE_FLAGS "-Wall -Wextra -stdlib=libc++ -fcoroutines-ts")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CLANG_COROUTINES_COMPILE_FLAGS}")

#if( CMAKE_BUILD_TYPE MATCHES Debug AND STANDALONE_BUILD )
if( CMAKE_BUILD_TYPE MATCHES Debug )
  message (STATUS "Coverage support enabled for targets: ${ARG_TARGETS}")
  # Debugging information enabled
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
  # Coverage analysis
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fprofile-arcs -ftest-coverage -lgcov")

  # Add optional target "coverage" to genereate code coverage report (console / XML)
  if (GCOVR_SCRIPT)
      add_custom_target(coverage
      ${GCOVR_SCRIPT} ${GCOVR_OPTIONS}
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
  endif()
endif()

set(CPPCHECK_EXECUTABLE cppcheck CACHE STRING "Path to the cppcheck executable")


add_subdirectory(src)

# build test
if(CMAKE_BUILD_TYPE MATCHES Debug)
  enable_testing()
  add_subdirectory(tests)
  add_dependencies(tests ${PROJECT_NAME})
else()
  #add_subdirectory(examples EXCLUDE_FROM_ALL)
  add_subdirectory(examples)
  add_subdirectory(sandbox)
  add_subdirectory(thread_sanitizers)
endif()



if (CPPCHECK_EXECUTABLE AND STANDALONE_BUILD)
  add_custom_target(cppcheck
	${CPPCHECK_EXECUTABLE} ${CPPCHECK_OPTIONS}
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}	
  )
endif()

# build documentation
find_package(Doxygen)
if (DOXYGEN_FOUND)
	if (NOT EXISTS ${CMAKE_BINARY_DIR}/doc)
        file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/doc)
    endif()

    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
		${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY
	)
	add_custom_target (doc-${PROJECT_NAME} 
  		COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
  		SOURCES ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
	)
endif()
