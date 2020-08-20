include_directories(../include)

add_executable(
    sandbox
    main.cpp
)
find_library(LIBRT rt)                                                                                                                                                                                                                                                                                        
	if(LIBRT)
		target_link_libraries(sandbox ${LIBRT})
	endif()
target_link_libraries(sandbox concurrencpp pthread)

