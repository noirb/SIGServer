#
# SIGVerse
#

cmake_minimum_required(VERSION 2.8)

project(SIGVerse CXX)

set(SIGSERVER_MAJOR_VERSION 2) 
set(SIGSERVER_MINOR_VERSION 3)
set(SIGSERVER_PATCH_VERSION 2)
set(SIGSERVER_VERSION ${SIGSERVER_MAJOR_VERSION}.${SIGSERVER_MINOR_VERSION})
set(SIGSERVER_FULL_VERSION ${SIGSERVER_MAJOR_VERSION}.${SIGSERVER_MINOR_VERSION}.${SIGSERVER_PATCH_VERSION})

set(CMAKE_INSTALL_PREFIX "~")
set(INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/sigverse-${SIGSERVER_FULL_VERSION})

message(STATUS "CMAKE_INSTALL_PREFIX:" ${CMAKE_INSTALL_PREFIX})
message(STATUS "INSTALL_DIR:" ${INSTALL_DIR})

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
    set(VCVER_s "")
endif()


add_compile_options(-std=c++11 -D__cplusplus=201103L)

if(WIN32)
    add_definitions(-DWIN32 -DdDOUBLE -DSTRICT_INTERFACE -DX3DPARSER_DISABLE_DEBUG_LOG -D_USE_MATH_DEFINES -D_CRT_SECURE_NO_WARNINGS)
else()
    add_definitions(-DdDOUBLE -DSTRICT_INTERFACE -DX3DPARSER_DISABLE_DEBUG_LOG)
endif()

include("${PROJECT_SOURCE_DIR}/cmake/SIGVerse_find_libs.cmake")

include_directories("${PROJECT_SOURCE_DIR}/srcs" "${ODE_ROOT_DIR}/include")


include("${PROJECT_SOURCE_DIR}/cmake/SIGVerse_commonlib.cmake")
include("${PROJECT_SOURCE_DIR}/cmake/SIGVerse_x3dparser.cmake")
include("${PROJECT_SOURCE_DIR}/cmake/SIGVerse_controller.cmake")
include("${PROJECT_SOURCE_DIR}/cmake/SIGVerse_simserver.cmake")
include("${PROJECT_SOURCE_DIR}/cmake/SIGVerse_tools.cmake")



install(DIRECTORY "${PROJECT_SOURCE_DIR}/srcs/sigverse/x3d/jars" DESTINATION share/sigverse FILES_MATCHING PATTERN "*.jar")

#
#  ODE
#
if(WIN32)
    file(GLOB ode_headers "${ODE_ROOT_DIR}/include/ode/*.h")
    
    install(FILES ${ode_headers} DESTINATION include/ode)
    install(FILES "${ODE_ROOT_DIR}/lib/${VCVER}/Release/ode.lib" DESTINATION lib)
endif()

file(GLOB_RECURSE sigverse_headers "${PROJECT_SOURCE_DIR}/srcs/sigverse/*.h")
install(FILES ${sigverse_headers} DESTINATION "${INSTALL_DIR}/include/sigverse")


message(STATUS "SIGVERSE_ROOT_DIR:" ${SIGVERSE_ROOT_DIR})
message(STATUS "ODE_ROOT_DIR:" ${ODE_ROOT_DIR})
message(STATUS "XERCES_ROOT_DIR:" ${XERCES_ROOT_DIR})

message(STATUS "CMAKE_INSTALL_PREFIX:" ${CMAKE_INSTALL_PREFIX})
message(STATUS "INSTALL_DIR:" ${INSTALL_DIR})