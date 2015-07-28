#
#

cmake_minimum_required(VERSION 2.8)

PROJECT(SIGVerse CXX)

set(SIGV_MAJOR_VERSION 2) 
set(SIGV_MINOR_VERSION 3)
set(SIGV_PATCH_VERSION 1)
set(SIGV_VERSION ${SIGV_MAJOR_VERSION}.${SIGV_MINOR_VERSION})
set(SIGV_FULL_VERSION ${SIGV_MAJOR_VERSION}.${SIGV_MINOR_VERSION}.${SIGV_PATCH_VERSION})

add_subdirectory(srcs/commonlib)
add_subdirectory(srcs/simserver)
add_subdirectory(srcs/x3d)
add_subdirectory(srcs/model)

include("${PROJECT_SOURCE_DIR}/cmake/SIGVerse_Env.cmake")

install(DIRECTORY jar DESTINATION share/sigverse
   FILES_MATCHING PATTERN "*.jar"
)
