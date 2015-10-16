#
# SIGVerse sigserver
#

file(GLOB_RECURSE sigserver_srcs    "${PROJECT_SOURCE_DIR}/srcs/sigverse/simserver/*.cpp")
file(GLOB_RECURSE sigserver_headers "${PROJECT_SOURCE_DIR}/srcs/sigverse/simserver/*.h")

if(WIN32)
    include_directories(
        "${PROJECT_SOURCE_DIR}"
        "${SIGVERSE_ROOT_DIR}/srcs/commonlib"
        "${PROJECT_SOURCE_DIR}/readconf"
        "${PROJECT_SOURCE_DIR}/../commonlib/comm"
        "${JDK_ROOT_DIR}/include/win32"
        "${JDK_ROOT_DIR}/include"
        "${XERCES_ROOT_DIR}/src"
        "${XERCES_ROOT_DIR}/include"
        "${SIGVERSE_ROOT_DIR}/srcs/x3d/parser/cpp/X3DParser"
    )
    message(STATUS "VERSION ${VCVER}")
    
    include_directories(
        "${JDK_ROOT_DIR}/include/win32"
        "${JDK_ROOT_DIR}/include"
    )
    link_directories(
        "${CMAKE_BINARY_DIR}/commonlib"
        "${ODE_ROOT_DIR}/lib/${VCVER}"
        "${XERCES_ROOT_DIR}/Build/Win32/${VCVER_S}"
        "${XERCES_ROOT_DIR}/lib"
        "${JDK_ROOT_DIR}/lib"
        "${CMAKE_BINARY_DIR}/x3d"
    )
else()
    link_directories(
        "${PROJECT_BINARY_DIR}/lib"
    )
endif()

add_executable(sigserver ${sigserver_srcs} ${sigserver_headers} ${commonlib_srcs} ${commonlib_headers})

set_target_properties(sigserver PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin" LINKER_LANGUAGE CXX)


if(WIN32)
    set( CMAKE_EXE_LINKER_FLAGS_RELEASE  "/LTCG ${CMAKE_EXE_LINKER_FLAGS}" )
    set( CMAKE_EXE_LINKER_FLAGS_DEBUG  "${CMAKE_EXE_LINKER_FLAGS}" )

    target_link_libraries(sigserver Ws2_32 jvm libx3d)
    target_link_libraries(sigserver debug oded  optimized ode)
    target_link_libraries(sigserver debug xerces-c_3D  optimized xerces-c_3)

    file(TO_NATIVE_PATH ${CMAKE_INSTALL_PREFIX} CMAKE_INSTALL_PREFIX)
    file(TO_NATIVE_PATH ${JRE_ROOT_DIR} JRE_ROOT_DIR)
    configure_file(${PROJECT_SOURCE_DIR}/sigserver.bat.in ${CMAKE_BINARY_DIR}/sigserver.bat)
 
    file(GLOB xerces_dll  "${XERCES_ROOT_DIR}/Build/Win32/${VCVER_S}/*/xerces-c*.dll")
    file(GLOB xerces_dll2 "${XERCES_ROOT_DIR}/bin/xerces-c*.dll")
    install(FILES ${xerces_dll} ${xerces_dll2} DESTINATION bin)
else()
    target_link_libraries(sigserver x3dparser ${JAVA_JVM_LIBRARY} dl ode xerces-c pthread m)
endif()



install(FILES "${PROJECT_BINARY_DIR}/bin/sigserver" DESTINATION "${INSTALL_DIR}/bin" PERMISSIONS ${PERMISSION755})
