#
#

cmake_minimum_required(VERSION 2.8)

PROJECT(sigrun CXX)
PROJECT(sendmsg CXX)
PROJECT(sigstart CXX)
PROJECT(sigend CXX)


include("${CMAKE_SOURCE_DIR}/cmake/SIGVerse_Env.cmake")

if(WIN32)
  set(sigrun_srcs runmain.cpp ControllerLib.cpp wingetopt.cpp)
  set(sigrun_headers ControllerLib.h wingetopt.h)

else()
  set(sigrun_srcs runmain.cpp ControllerLib.cpp)
  set(sigrun_headers ControllerLib.h)

endif()

  set(sendmsg_srcs sendmsg.cpp)
  set(sendmsg_headers "")

  set(sigstart_srcs sigstart.cpp)
  set(sigstart_headers "")

  set(sigend_srcs sigend.cpp)
  set(sigend_headers "")


include_directories("${PROJECT_SOURCE_DIR}"
  "${SIGVERSE_ROOT_DIR}/srcs/commonlib"
  "${PROJECT_SOURCE_DIR}/../commonlib/comm"

 )

link_directories(
  "${CMAKE_BINARY_DIR}/srcs/commonlib"
  "${ODE_ROOT_DIR}/lib/${VCVER}"
)

ADD_EXECUTABLE(sigrun ${sigrun_srcs}  ${sigrun_headers} )
ADD_EXECUTABLE(sendmsg ${sendmsg_srcs}  ${sendmsg_headers} )
ADD_EXECUTABLE(sigstart ${sigstart_srcs}  ${sigstart_headers} )
ADD_EXECUTABLE(sigend ${sigend_srcs}  ${sigend_headers} )


if(WIN32)
#  SET( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS}" )
  SET( CMAKE_EXE_LINKER_FLAGS_RELEASE  "/LTCG ${CMAKE_EXE_LINKER_FLAGS}" )
  SET( CMAKE_EXE_LINKER_FLAGS_DEBUG  "${CMAKE_EXE_LINKER_FLAGS}" )
  target_link_libraries(sigrun commonlib)
  target_link_libraries(sendmsg commonlib)
  target_link_libraries(sigstart commonlib)
  target_link_libraries(sigend commonlib)
endif()


SET_TARGET_PROPERTIES(sigrun PROPERTIES LINKER_LANGUAGE CXX)
SET_TARGET_PROPERTIES(sendmsg PROPERTIES LINKER_LANGUAGE CXX)
SET_TARGET_PROPERTIES(sigstart PROPERTIES LINKER_LANGUAGE CXX)
SET_TARGET_PROPERTIES(sigend PROPERTIES LINKER_LANGUAGE CXX)
