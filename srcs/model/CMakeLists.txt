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

  configure_file(${PROJECT_SOURCE_DIR}/sigmake.bat.in ${CMAKE_BINARY_DIR}/sigmake.bat)
  configure_file(${PROJECT_SOURCE_DIR}/mkdll.nmake.in ${CMAKE_BINARY_DIR}/mkdll.nmake)
  install(FILES ${CMAKE_BINARY_DIR}/sigmake.bat DESTINATION bin)
  install(FILES ${CMAKE_BINARY_DIR}/mkdll.nmake DESTINATION share/sigverse/etc)


endif()


SET_TARGET_PROPERTIES(sigrun
 PROPERTIES
 RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
 LINKER_LANGUAGE CXX)
SET_TARGET_PROPERTIES(sendmsg
 PROPERTIES
 RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
 LINKER_LANGUAGE CXX)
SET_TARGET_PROPERTIES(sigstart
 PROPERTIES
 RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
 LINKER_LANGUAGE CXX)
SET_TARGET_PROPERTIES(sigend
 PROPERTIES
 RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
 LINKER_LANGUAGE CXX)

#
#
install(TARGETS sigrun sendmsg sigstart sigend
  RUNTIME DESTINATION bin
)

#
#
file(GLOB sample_ctrl "${PROJECT_SOURCE_DIR}/samples/export/*.cpp")
install(FILES ${sample_ctrl} DESTINATION share/sigverse/samples )
