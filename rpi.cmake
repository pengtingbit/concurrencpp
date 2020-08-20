#set (ARCH raspberrypi)
# this one is important
SET(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)
#this one not so much
#SET(CMAKE_SYSTEM_VERSION 1)
INCLUDE(CMakeForceCompiler)
# specify the cross compiler
CMAKE_FORCE_C_COMPILER(arm-linux-gnueabihf-gcc GNU)
CMAKE_FORCE_CXX_COMPILER(arm-linux-gnueabihf-g++ GNU)

SET(TARGET_TRIPLE arm-linux-gnueabihf)
# Specify raspberry triple
set(CROSS_FLAGS "--target=${TARGET_TRIPLE}")

#specify the cross compiler
SET(CMAKE_C_COMPILER /usr/bin/arm-linux-gnueabihf-gcc)
SET(CMAKE_CXX_COMPILER /usr/bin/arm-linux-gnueabihf-g++)

SET(CMAKE_FIND_ROOT_PATH /usr/arm-linux-gnueabihf)

# search for programs in the build host directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# for libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

SET(CFALGS_BASICS=" -02 -Wall -g0 ")
SET(CMAKE_C_FLAGS " ${CFLAGS_BASICS} -pthread")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

SET(CMAKE_THREAD_LIBS_INIT "-lpthread")
SET(CMAKE_HAVE_PTHREAD_H "/usr/arm-linux-gnueabihf/include/")


