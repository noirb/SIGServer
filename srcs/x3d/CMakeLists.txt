#
#

cmake_minimum_required(VERSION 2.8)

PROJECT(libx3d CXX)

include("${CMAKE_SOURCE_DIR}/cmake/SIGVerse_Env.cmake")


file(GLOB srcs "parser/cpp/X3Dparser/*.cpp")

file(GLOB headers "parser/cpp/X3Dparser/*.h")

ADD_LIBRARY(libx3d STATIC
 ${srcs} ${headers}
)

#include_directories("${PROJECT_SOURCE_DIR}")

include_directories(
  "${PROJECT_SOURCE_DIR}"
  "${JDK_ROOT_DIR}/include/win32"
  "${JDK_ROOT_DIR}/include"
)

if(WIN32)
target_link_libraries(libx3d Ws2_32.lib jvm.lib)

endif()

SET_TARGET_PROPERTIES(libx3d PROPERTIES LINKER_LANGUAGE CXX)

