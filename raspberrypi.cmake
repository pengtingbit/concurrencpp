set(toolchain_dir /usr/arm-linux-gnueabihf )
set(toolchain_bin_dir ${toolchain_dir}/bin)
set(toolchain_inc_dir ${toolchain_dir}/include) # was /include
set(toolchain_lib_dir ${toolchain_dir}/lib)

set(CMAKE_SYSTEM_NAME Linux CACHE INTERNAL "system name")
set(CMAKE_SYSTEM_PROCESSOR arm CACHE INTERNAL "processor")
set(CMAKE_C_COMPILER /usr/bin/arm-linux-gnueabihf-gcc)
set(CMAKE_CXX_COMPILER /usr/bin/arm-linux-gnueabihf-g++)
#set(CMAKE_C_FLAGS "-O2 -integrated-as -target armv6-linux-gnueabihf -mfloat-abi=hard --sysroot=${toolchain_dir}" CACHE INTERNAL "c compiler flags")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 --target=armv6-linux-gnueabihf -mfloat-abi=hard --sysroot=${toolchain_dir}" CACHE INTERNAL "cxx compiler flags")

set(link_flags "-L${toolchain_lib_dir} -ldl")
SET (CMAKE_LINKER  "usr/bin/arm-linux-gnueabihf-ld" CACHE FILEPATH "Linker")

set(CMAKE_EXE_LINKER_FLAGS ${link_flags} CACHE INTERNAL "exe link flags")
set(CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_LINK_EXECUTABLE} ${CMAKE_EXE_LINKER_FLAGS} ${CMAKE_LINKER}")
set(CMAKE_MODULE_LINKER_FLAGS ${link_flags} CACHE INTERNAL "module link flags")
set(CMAKE_SHARED_LINKER_FLAGS ${link_flags} CACHE INTERNAL "shared link flags")
set(CMAKE_FIND_ROOT_PATH ${toolchain_lib_dir} CACHE INTERNAL "cross root directory")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM BOTH CACHE INTERNAL "")
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY CACHE INTERNAL "")
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY CACHE INTERNAL "")
