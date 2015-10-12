#
# SIGVerse sigrunac
#

cmake_minimum_required(VERSION 2.8)

PROJECT(sigrunac CXX)


include("${CMAKE_SOURCE_DIR}/cmake/SIGVerse_Env.cmake")

if(WIN32)
  set(sigrunac_srcs runmain.cpp ControllerLib.cpp wingetopt.cpp)
  set(sigrunac_headers ControllerLib.h wingetopt.h)

else()
  set(sigrunac_srcs runmain.cpp ControllerLib.cpp)
  set(sigrunac_headers ControllerLib.h)
endif()


#include_directories("${PROJECT_SOURCE_DIR}"
#  "${SIGVERSE_ROOT_DIR}/srcs/commonlib"
#  "${PROJECT_SOURCE_DIR}/../commonlib/comm"
# )

link_directories(
  "${CMAKE_BINARY_DIR}/lib"
#  "${CMAKE_BINARY_DIR}/srcs/commonlib"
  "${ODE_ROOT_DIR}/lib/${VCVER}"
)

ADD_EXECUTABLE(sigrunac ${sigrunac_srcs}  ${sigrunac_headers} )


if(WIN32)
  SET( CMAKE_EXE_LINKER_FLAGS_RELEASE  "/LTCG ${CMAKE_EXE_LINKER_FLAGS}" )
  SET( CMAKE_EXE_LINKER_FLAGS_DEBUG  "${CMAKE_EXE_LINKER_FLAGS}" )
  target_link_libraries(sigrunac commonlib)

  file(TO_NATIVE_PATH ${CMAKE_INSTALL_PREFIX} CMAKE_INSTALL_PREFIX)
else()
#    target_link_libraries(${_TARGET} ${JAVA_JVM_LIBRARY} ${MATLAB_LIBS})
#    target_link_libraries(sigrunac commonlib jvm m dl ode pthread xerces-c )
    target_link_libraries(sigrunac commonlib ${JAVA_JVM_LIBRARY} m dl ode pthread xerces-c )
endif()


SET_TARGET_PROPERTIES(sigrunac
 PROPERTIES
 RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
 LINKER_LANGUAGE CXX)


#
#
file(GLOB sample_ctrl "${PROJECT_SOURCE_DIR}/samples/export/*.cpp")
install(FILES ${sample_ctrl} DESTINATION share/sigverse/samples )
