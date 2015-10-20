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

if(WIN32)
    set(CMAKE_INSTALL_PREFIX "C:/SIGVerse")
else()
    list(APPEND PERMISSION755 "OWNER_READ" "OWNER_WRITE" "OWNER_EXECUTE" "GROUP_READ" "GROUP_EXECUTE" "WORLD_READ" "WORLD_EXECUTE")
    set(CMAKE_INSTALL_PREFIX $ENV{HOME})
endif()

set(INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/sigverse-${SIGSERVER_FULL_VERSION})

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



if(WIN32)
    add_definitions(-DWIN32 -DdDOUBLE -DSTRICT_INTERFACE -DUSE_ODE -DUSE_XERCES -DX3DPARSER_DISABLE_DEBUG_LOG -D_USE_MATH_DEFINES -D_CRT_SECURE_NO_WARNINGS)
else()
    add_compile_options(-std=c++11 -D__cplusplus=201103L)
    add_definitions(-DdDOUBLE -DSTRICT_INTERFACE -DUSE_ODE -DUSE_XERCES -DX3DPARSER_DISABLE_DEBUG_LOG)
endif()

# find library
include("${PROJECT_SOURCE_DIR}/cmake/SIGVerse_find_libs.cmake")

include_directories("${PROJECT_SOURCE_DIR}/srcs" "${ODE_ROOT_DIR}/include")


include("${PROJECT_SOURCE_DIR}/cmake/SIGVerse_commonlib.cmake")
include("${PROJECT_SOURCE_DIR}/cmake/SIGVerse_x3dparser.cmake")
include("${PROJECT_SOURCE_DIR}/cmake/SIGVerse_controller.cmake")
include("${PROJECT_SOURCE_DIR}/cmake/SIGVerse_simserver.cmake")
include("${PROJECT_SOURCE_DIR}/cmake/SIGVerse_tools.cmake")

# Install Models
file(GLOB data_xml "${PROJECT_SOURCE_DIR}/model/shapes/*.xml")
file(GLOB data_wrl "${PROJECT_SOURCE_DIR}/model/shapes/*.wrl")
file(GLOB data_x3d "${PROJECT_SOURCE_DIR}/model/shapes/*.x3d")
install(FILES ${data_xml}             DESTINATION "${INSTALL_DIR}/share/data/xml")
install(FILES ${data_wrl} ${data_x3d} DESTINATION "${INSTALL_DIR}/share/data/shape")

# Install Headers
file(GLOB header_commonlib "${PROJECT_SOURCE_DIR}/srcs/sigverse/commonlib/*.h")
file(GLOB header_controller "${PROJECT_SOURCE_DIR}/srcs/sigverse/commonlib/comm/controller/*.h")
install(FILES ${header_commonlib}  DESTINATION "${INSTALL_DIR}/include/sigverse/commonlib/")
install(FILES ${header_controller} DESTINATION "${INSTALL_DIR}/include/sigverse/commonlib/comm/controller/")


#
#  ODE
#
if(WIN32)
    file(GLOB ode_headers "${ODE_ROOT_DIR}/include/ode/*.h")
    
    install(FILES ${ode_headers} DESTINATION include/ode)
    install(FILES "${ODE_ROOT_DIR}/lib/Release/ode.lib" DESTINATION lib)
endif()

message(STATUS "CMAKE_INSTALL_PREFIX:" ${CMAKE_INSTALL_PREFIX})
message(STATUS "INSTALL_DIR:" ${INSTALL_DIR})

if(WIN32)
    message(STATUS "SIGVERSE_ROOT_DIR:" ${SIGVERSE_ROOT_DIR})
    message(STATUS "ODE_ROOT_DIR:" ${ODE_ROOT_DIR})
    message(STATUS "XERCES_ROOT_DIR:" ${XERCES_ROOT_DIR})
    message(STATUS "JDK_ROOT_DIR:" ${JDK_ROOT_DIR})
    message(STATUS "JRE_ROOT_DIR:" ${JRE_ROOT_DIR})
endif()
