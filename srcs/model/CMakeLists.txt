#
#

cmake_minimum_required(VERSION 2.8)

PROJECT(sigrun CXX)

include("${CMAKE_SOURCE_DIR}/cmake/SIGVerse_Env.cmake")

if(WIN32)
set(srcs runmain.cpp ControllerLib.cpp wingetopt.cpp)
set(headers ControllerLib.h wingetopt.h)
else()
set(srcs runmain.cpp ControllerLib.cpp)
set(headers ControllerLib.h)
endif()

include_directories("${PROJECT_SOURCE_DIR}"
  "${SIGVERSE_ROOT_DIR}/srcs/commonlib"
  "${PROJECT_SOURCE_DIR}/../commonlib/comm"
 )

message(STATUS "VERSION ${VCVER}")


link_directories("${CMAKE_BINARY_DIR}/srcs/commonlib"
       "${ODE_ROOT_DIR}/lib/${VCVER}"
)


ADD_EXECUTABLE(sigrun
 ${srcs} 
 ${headers} 
)


if(WIN32)
#  SET( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS}" )
  SET( CMAKE_EXE_LINKER_FLAGS_RELEASE  "/LTCG ${CMAKE_EXE_LINKER_FLAGS}" )
  SET( CMAKE_EXE_LINKER_FLAGS_DEBUG  "${CMAKE_EXE_LINKER_FLAGS}" )
  target_link_libraries(sigrun commonlib)
endif()


SET_TARGET_PROPERTIES(sigrun PROPERTIES LINKER_LANGUAGE CXX)

