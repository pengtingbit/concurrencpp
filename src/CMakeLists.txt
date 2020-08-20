set(SOURCE
	executors/executor.cpp
	runtime/runtime.cpp
	threads/thread.cpp
	results/result_core.cpp
	executors/executor.cpp
	executors/manual_executor.cpp
	executors/thread_executor.cpp
	executors/thread_pool_executor.cpp
	executors/worker_thread_executor.cpp
	timers/timer_queue.cpp
	timers/timer.cpp
)

include_directories(
    ../include
    ../include/executors
    ../include/results
    ../include/runtime
    ../include/threads
    ../include/timers
    ../include/utils
)

#set(HEADERS
#   ${CMAKE_CURRENT_SOURCE_DIR}/../include/*
#   ${CMAKE_CURRENT_SOURCE_DIR}/../include/executors/*
#   ${CMAKE_CURRENT_SOURCE_DIR}/../include/results/*
#   ${CMAKE_CURRENT_SOURCE_DIR}/../include/runtime/*
#   ${CMAKE_CURRENT_SOURCE_DIR}/../include/threads/*
#   ${CMAKE_CURRENT_SOURCE_DIR}/../include/imters/*
#   ${CMAKE_CURRENT_SOURCE_DIR}/../include/utils/*
#)

add_library(${PROJECTNAME} STATIC
        ${SOURCE}
        ${HEADERS}
	)

find_library(LIBRT rt)                                                                                                                                                                                                                                                                                        
	if(LIBRT)
		target_link_libraries(${PROJECTNAME} ${LIBRT})
	endif()
target_link_libraries(${PROJECTNAME} pthread)
