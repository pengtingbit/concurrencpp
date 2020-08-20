# Configuration script for the folder.
# Execute cmake one level up to (re)generate the
# Projectfiles/Makefiles


include_directories(${source_dir}/include)
set(GTEST_PATH ${PROJECT_SOURCE_DIR}/contrib)
set(GTEST_SRC ${GTEST_PATH}/gtest/gtest-all.cc)
set(GTEST_MAIN ${GTEST_PATH}/gtest/gtest_main.cc)
include_directories(
	../include
	#../src
	./
	${GTEST_PATH}
)

link_directories( 
	${CMAKE_CURRENT_BINARY_DIR}/src
)

add_executable( tests
	#${GTEST_SRC}
	#${GTEST_MAIN}
	main.cpp
	helpers/assertions.cpp
	helpers/object_observer.cpp

	tester/tester.cpp
	
	tests/all_tests.cpp
	tests/timer_tests.cpp
	tests/runtime_tests.cpp

	tests/executor_tests/inline_executor_tests.cpp
	tests/executor_tests/manual_executor_tests.cpp
	tests/executor_tests/thread_executor_tests.cpp
	tests/executor_tests/thread_pool_executor_tests.cpp
	tests/executor_tests/worker_thread_executor_tests.cpp

	tests/result_tests/result_tests.cpp
	tests/result_tests/result_await_tests.cpp
	tests/result_tests/result_resolve_tests.cpp
	tests/result_tests/result_promise_tests.cpp
	tests/result_tests/coroutine_adapters_tests.cpp
)


find_library(LIBRT rt)                                                                                                                                                                                                                                                                                        
	if(LIBRT)
		target_link_libraries(tests ${LIBRT})
	endif()
target_link_libraries(tests concurrencpp pthread)

add_test(tests-${PROJECT_NAME} tests --gtest_output=xml:${CMAKE_CURRENT_BINARY_DIR}/xunit.xml)
# Adding custom test target "check" because it is the only way to show the full output of test errors on console
add_custom_target(check-${PROJECT_NAME} COMMAND ${CMAKE_CTEST_COMMAND} --force-new-ctest-process --output-on-failure)
