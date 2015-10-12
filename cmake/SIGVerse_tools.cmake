#
# SIGVerse tools
#

cmake_minimum_required(VERSION 2.8)

PROJECT(sendmsg CXX)
PROJECT(sigstart CXX)
PROJECT(sigend CXX)


#include("${CMAKE_SOURCE_DIR}/cmake/SIGVerse_Env.cmake")
if(WIN32)
#add_definitions(-DWIN32 -DIRWAS_SIMSERVER -DdDOUBLE -DSTRICT_INTERFACE -D_USE_MATH_DEFINES -D_CRT_SECURE_NO_WARNINGS)
add_definitions(-DWIN32 -DdDOUBLE -DSTRICT_INTERFACE -DX3DPARSER_DISABLE_DEBUG_LOG -D_USE_MATH_DEFINES -D_CRT_SECURE_NO_WARNINGS)
else()
#add_definitions(-DIRWAS_SIMSERVER -DdDOUBLE -DSTRICT_INTERFACE)
add_definitions(-DdDOUBLE -DSTRICT_INTERFACE -DX3DPARSER_DISABLE_DEBUG_LOG )
endif()

#message(STATUS "バイナリパス："  ${CMAKE_BINARY_DIR}/lib)
#message(STATUS "CMAKE_CXX_FLAGS_RELWITHDEBINFO:" "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")

  set(sendmsg_srcs    "${CMAKE_SOURCE_DIR}/srcs/sigverse/tools/sendmsg.cpp")
  set(sendmsg_headers "")

  set(sigstart_srcs    "${CMAKE_SOURCE_DIR}/srcs/sigverse/tools/sigstart.cpp")
  set(sigstart_headers "")

  set(sigend_srcs    "${CMAKE_SOURCE_DIR}/srcs/sigverse/tools/sigend.cpp")
  set(sigend_headers "")


#include_directories("${PROJECT_SOURCE_DIR}"
#  "${SIGVERSE_ROOT_DIR}/srcs/commonlib"
#  "${PROJECT_SOURCE_DIR}/../commonlib/comm"
# )

link_directories(
  "${CMAKE_BINARY_DIR}/lib"
#  "${CMAKE_BINARY_DIR}/srcs/commonlib"
  "${ODE_ROOT_DIR}/lib/${VCVER}"
)

ADD_EXECUTABLE(sendmsg  ${sendmsg_srcs}  ${sendmsg_headers} )
ADD_EXECUTABLE(sigstart ${sigstart_srcs} ${sigstart_headers} )
ADD_EXECUTABLE(sigend   ${sigend_srcs}   ${sigend_headers} )


if(WIN32)
  SET( CMAKE_EXE_LINKER_FLAGS_RELEASE  "/LTCG ${CMAKE_EXE_LINKER_FLAGS}" )
  SET( CMAKE_EXE_LINKER_FLAGS_DEBUG  "${CMAKE_EXE_LINKER_FLAGS}" )
  target_link_libraries(sendmsg  commonlib)
  target_link_libraries(sigstart commonlib)
  target_link_libraries(sigend   commonlib)

  file(TO_NATIVE_PATH ${CMAKE_INSTALL_PREFIX} CMAKE_INSTALL_PREFIX)

message(${CMAKE_INSTALL_PREFIX})
  configure_file(${PROJECT_SOURCE_DIR}/sigmake.bat.in ${CMAKE_BINARY_DIR}/sigmake.bat)
  configure_file(${PROJECT_SOURCE_DIR}/mkdll.nmake.in ${CMAKE_BINARY_DIR}/mkdll.nmake)
  configure_file(${PROJECT_SOURCE_DIR}/sigcreate.bat.in ${CMAKE_BINARY_DIR}/sigcreate.bat)
  install(FILES ${CMAKE_BINARY_DIR}/sigmake.bat ${CMAKE_BINARY_DIR}/sigcreate.bat DESTINATION bin)
  install(FILES ${CMAKE_BINARY_DIR}/mkdll.nmake DESTINATION share/sigverse/etc)

else()
  target_link_libraries(sendmsg  commonlib ${JAVA_JVM_LIBRARY} m dl ode pthread xerces-c )
  target_link_libraries(sigstart commonlib ${JAVA_JVM_LIBRARY} m dl ode pthread xerces-c )
  target_link_libraries(sigend   commonlib ${JAVA_JVM_LIBRARY} m dl ode pthread xerces-c )
endif()


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


remove_definitions()
