#
# SIGVerse
#

cmake_minimum_required(VERSION 2.8)

PROJECT(SIGVerse CXX)

#include("${PROJECT_SOURCE_DIR}/cmake/SIGVerse_Env.cmake")

set(SIGSERVER_MAJOR_VERSION 2) 
set(SIGSERVER_MINOR_VERSION 3)
set(SIGSERVER_PATCH_VERSION 1)
set(SIGSERVER_VERSION ${SIGSERVER_MAJOR_VERSION}.${SIGSERVER_MINOR_VERSION})
set(SIGSERVER_FULL_VERSION ${SIGSERVER_MAJOR_VERSION}.${SIGSERVER_MINOR_VERSION}.${SIGSERVER_PATCH_VERSION})

if(MSVC10)
  set(VCVER "vs2010")
  set(VCVER_S "VC10")
elseif(MSVC11)
  set(VCVER "vs2012")
  set(VCVER_S "VC11")
elseif(MSVC12)
  set(VCVER "vs2013")
  set(VCVER_S "VC12")
else()
  set(VCVER "")
  set(VCVER "")
endif()

add_definitions( -std=c++11 -D__cplusplus=201103L )

include("${CMAKE_SOURCE_DIR}/cmake/SIGVerse_find_libs.cmake")

include_directories("${CMAKE_SOURCE_DIR}/srcs" "${ODE_ROOT_DIR}/include")


include("${PROJECT_SOURCE_DIR}/cmake/SIGVerse_commonlib.cmake")
include("${PROJECT_SOURCE_DIR}/cmake/SIGVerse_x3dparser.cmake")
include("${PROJECT_SOURCE_DIR}/cmake/SIGVerse_controller.cmake")
include("${PROJECT_SOURCE_DIR}/cmake/SIGVerse_simserver.cmake")
include("${PROJECT_SOURCE_DIR}/cmake/SIGVerse_tools.cmake")



install(DIRECTORY jar DESTINATION share/sigverse
   FILES_MATCHING PATTERN "*.jar"
)


message(STATUS "SIGVERSE_ROOT_DIR:" ${SIGVERSE_ROOT_DIR})
message(STATUS "ODE_ROOT_DIR:" ${ODE_ROOT_DIR})
message(STATUS "XERCES_ROOT_DIR:" ${XERCES_ROOT_DIR})

