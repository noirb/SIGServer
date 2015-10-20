#
# SIGVerse sigrunac
#

file(GLOB_RECURSE sigrunac_srcs    "${PROJECT_SOURCE_DIR}/srcs/sigverse/controller/*.cpp")
file(GLOB_RECURSE sigrunac_headers "${PROJECT_SOURCE_DIR}/srcs/sigverse/controller/*.h")

file(GLOB_RECURSE exclude_files "${PROJECT_SOURCE_DIR}/srcs/sigverse/controller/samples/*")
    
list(REMOVE_ITEM sigrunac_srcs ${exclude_files})
list(REMOVE_ITEM sigrunac_headers ${exclude_files})


if(WIN32)
    include_directories(
        "${XERCES_ROOT_DIR}/src"
        "${XERCES_ROOT_DIR}/include"
        "${JDK_ROOT_DIR}/include/win32"
        "${JDK_ROOT_DIR}/include"
    )
#    message(STATUS "VERSION ${VCVER}")
    
    link_directories(
        "${PROJECT_BINARY_DIR}/lib"
        "${ODE_ROOT_DIR}/lib/Release"
        "${XERCES_ROOT_DIR}/lib/Release"
        "${JDK_ROOT_DIR}/lib"
    )
else()
    link_directories(
        "${PROJECT_BINARY_DIR}/lib"
    )
endif()

add_executable(sigrunac ${sigrunac_srcs} ${sigrunac_headers})

set_target_properties(sigrunac PROPERTIES COMPILE_DEFINITIONS "CONTROLLER")
set_target_properties(sigrunac PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin" LINKER_LANGUAGE CXX)


if(WIN32)
    message(STATUS "CMAKE_EXE_LINKER_FLAGS:" ${CMAKE_EXE_LINKER_FLAGS})
    set( CMAKE_EXE_LINKER_FLAGS_RELEASE  "/LTCG ${CMAKE_EXE_LINKER_FLAGS}" )
    set( CMAKE_EXE_LINKER_FLAGS_DEBUG  "${CMAKE_EXE_LINKER_FLAGS}" )

    target_link_libraries(sigrunac Ws2_32 x3dparser jvm)
    target_link_libraries(sigrunac debug oded optimized ode)
    target_link_libraries(sigrunac debug xerces-c_3D  optimized xerces-c_3)

#    file(TO_NATIVE_PATH ${CMAKE_INSTALL_PREFIX} CMAKE_INSTALL_PREFIX)
else()
    target_link_libraries(sigrunac commonlib_controller x3dparser ${JAVA_JVM_LIBRARY} dl ode xerces-c pthread m)
endif()



#
#
install(DIRECTORY "${PROJECT_SOURCE_DIR}/srcs/sigverse/controller/samples" DESTINATION "${INSTALL_DIR}/share" FILES_MATCHING PATTERN "*")

if(WIN32)
    install(FILES "${PROJECT_BINARY_DIR}/bin/${CMAKE_CONFIGURATION_TYPES}/sigrunac.exe" DESTINATION "${INSTALL_DIR}/bin")
else()
    install(FILES "${PROJECT_BINARY_DIR}/bin/sigrunac" DESTINATION "${INSTALL_DIR}/bin" PERMISSIONS ${PERMISSION755})
endif()
