#
# SIGVerse X3DParser
#

cmake_minimum_required(VERSION 2.8)

PROJECT(x3dparser CXX)

#include("${CMAKE_SOURCE_DIR}/cmake/SIGVerse_model_Env.cmake")
if(WIN32)
#add_definitions(-DWIN32 -DIRWAS_SIMSERVER -DdDOUBLE -DSTRICT_INTERFACE -D_USE_MATH_DEFINES -D_CRT_SECURE_NO_WARNINGS)
add_definitions(-DWIN32 -DdDOUBLE -DSTRICT_INTERFACE -DX3DPARSER_DISABLE_DEBUG_LOG -D_USE_MATH_DEFINES -D_CRT_SECURE_NO_WARNINGS)
else()
#add_definitions(-DIRWAS_SIMSERVER -DdDOUBLE -DSTRICT_INTERFACE)
add_definitions(-DdDOUBLE -DSTRICT_INTERFACE -DX3DPARSER_DISABLE_DEBUG_LOG )
endif()


file(GLOB_RECURSE srcs     "${CMAKE_SOURCE_DIR}/srcs/sigverse/x3d/parser/cpp/X3DParser/*.cpp")
file(GLOB_RECURSE headers  "${CMAKE_SOURCE_DIR}/srcs/sigverse/x3d/parser/cpp/X3DParser/*.h")


ADD_LIBRARY(x3dparser STATIC
 ${srcs}
 ${headers}
)

#source_group("Source Files\\command"  FILES ${command_srcs})
#source_group("Header Files\\command"  FILES ${command_headers})

#if(WIN32)
#SET( CMAKE_STATIC_LINKER_FLAGS  "${CMAKE_SHARED_LINKER_FLAGS}  /LTCG /NODEFAULTLIB:MSVCRT" )
#    target_link_libraries(commonlib Ws2_32)
#    target_link_libraries(commonlib debug oded optimized ode)
#endif()

SET_TARGET_PROPERTIES(x3dparser
  PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
  ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
  LINKER_LANGUAGE CXX)

install(FILES  ${headers} DESTINATION include/sigverse)


#
#  ODE
#
#file(GLOB ode_headers "${ODE_ROOT_DIR}/include/ode/*.h")
#
#install(FILES ${ode_headers} DESTINATION include/ode)
#
#install(FILES "${ODE_ROOT_DIR}/lib/${VCVER}/Release/ode.lib" DESTINATION lib)

remove_definitions()

