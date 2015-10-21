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


# Set Install directory path
if(WIN32)
    set(CMAKE_INSTALL_PREFIX "C:/SIGVerse")
else()
    set(CMAKE_INSTALL_PREFIX $ENV{HOME})
endif()

set(INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/sigverse-${SIGSERVER_FULL_VERSION})

message(STATUS "CMAKE_INSTALL_PREFIX:" ${CMAKE_INSTALL_PREFIX})
message(STATUS "INSTALL_DIR:" ${INSTALL_DIR})


# Find library path
include("${PROJECT_SOURCE_DIR}/cmake/SIGVerse_find_libs.cmake")


# Set Common definitions
if(WIN32)
    if(MSVC10)
        set(VCVER "vs2010")
        set(VCVER_S "VC10")
    elseif(MSVC11)
        set(VCVER "vs2012")
        set(VCVER_S "VC11")
    elseif(MSVC12)
        set(VCVER "vs2013")
        set(VCVER_S "VC12")
    endif()
    
    set(CMAKE_STATIC_LINKER_FLAGS      "${CMAKE_SHARED_LINKER_FLAGS} /LTCG /NODEFAULTLIB:MSVCRT" )
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "/LTCG ${CMAKE_EXE_LINKER_FLAGS}" )
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG   "${CMAKE_EXE_LINKER_FLAGS}" )
#    file(TO_NATIVE_PATH ${CMAKE_INSTALL_PREFIX} CMAKE_INSTALL_PREFIX)
#    file(TO_NATIVE_PATH ${JRE_ROOT_DIR} JRE_ROOT_DIR)

    add_definitions(-DWIN32 -DdDOUBLE -DSTRICT_INTERFACE -DUSE_ODE -DUSE_XERCES -DX3DPARSER_DISABLE_DEBUG_LOG -D_USE_MATH_DEFINES -D_CRT_SECURE_NO_WARNINGS)
else()
    list(APPEND PERMISSION755 "OWNER_READ" "OWNER_WRITE" "OWNER_EXECUTE" "GROUP_READ" "GROUP_EXECUTE" "WORLD_READ" "WORLD_EXECUTE")
    add_compile_options(-std=c++11 -D__cplusplus=201103L)
    add_definitions(-DdDOUBLE -DSTRICT_INTERFACE -DUSE_ODE -DUSE_XERCES -DX3DPARSER_DISABLE_DEBUG_LOG)
endif()


# Set Common include directories
include_directories("${PROJECT_SOURCE_DIR}/srcs" "${ODE_ROOT_DIR}/include")

if(WIN32)
    include_directories(
        "${XERCES_ROOT_DIR}/src"
        "${JDK_ROOT_DIR}/include/win32"
        "${JDK_ROOT_DIR}/include"
    )
endif()

# Set Common link directories
link_directories("${PROJECT_BINARY_DIR}/lib")
    
if(WIN32)
    link_directories(
        "${ODE_ROOT_DIR}/lib/Release"
        "${XERCES_ROOT_DIR}/lib/Release"
        "${JDK_ROOT_DIR}/lib"
    )
endif()


# Execute Sub CMake files
include("${PROJECT_SOURCE_DIR}/cmake/SIGVerse_commonlib.cmake")
include("${PROJECT_SOURCE_DIR}/cmake/SIGVerse_x3dparser.cmake")
include("${PROJECT_SOURCE_DIR}/cmake/SIGVerse_controller.cmake")
include("${PROJECT_SOURCE_DIR}/cmake/SIGVerse_simserver.cmake")
include("${PROJECT_SOURCE_DIR}/cmake/SIGVerse_tools.cmake")


# Install Resources
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

# Install libraries
if(WIN32)
    # Install ODE
    file(GLOB ode_headers "${ODE_ROOT_DIR}/include/ode/*.h")
    install(FILES "${ode_headers}"                      DESTINATION "${INSTALL_DIR}/include/ode")
    install(FILES "${ODE_ROOT_DIR}/lib/Release/ode.lib" DESTINATION "${INSTALL_DIR}/lib")
    
    # Install Xerces
    file(GLOB xerces_dll  "${XERCES_ROOT_DIR}/lib/Release/xerces-c*.dll")
    install(FILES ${xerces_dll} DESTINATION "${INSTALL_DIR}/bin")
endif()

