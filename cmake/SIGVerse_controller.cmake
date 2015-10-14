#
# SIGVerse sigrunac
#

if(WIN32)
    set(sigrunac_srcs runmain.cpp ControllerLib.cpp wingetopt.cpp)
    set(sigrunac_headers ControllerLib.h wingetopt.h)
else()
    file(GLOB_RECURSE sigrunac_srcs    "${PROJECT_SOURCE_DIR}/srcs/sigverse/controller/*.cpp")
    file(GLOB_RECURSE sigrunac_headers "${PROJECT_SOURCE_DIR}/srcs/sigverse/controller/*.h")

    file(GLOB_RECURSE exclude_files "${PROJECT_SOURCE_DIR}/srcs/sigverse/controller/samples/*")
    
    list(REMOVE_ITEM sigrunac_srcs ${exclude_files})
    list(REMOVE_ITEM sigrunac_headers ${exclude_files})
    
#    message(${sigrunac_srcs})
#    message(${sigrunac_headers})
endif()


link_directories("${PROJECT_BINARY_DIR}/lib" "${ODE_ROOT_DIR}/lib/${VCVER}")

add_executable(sigrunac ${sigrunac_srcs} ${sigrunac_headers} )


set_target_properties(sigrunac PROPERTIES COMPILE_DEFINITIONS "CONTROLLER")


if(WIN32)
    set( CMAKE_EXE_LINKER_FLAGS_RELEASE  "/LTCG ${CMAKE_EXE_LINKER_FLAGS}" )
    set( CMAKE_EXE_LINKER_FLAGS_DEBUG  "${CMAKE_EXE_LINKER_FLAGS}" )
    target_link_libraries(sigrunac commonlib)

    file(TO_NATIVE_PATH ${CMAKE_INSTALL_PREFIX} CMAKE_INSTALL_PREFIX)
else()
    target_link_libraries(sigrunac commonlib ${JAVA_JVM_LIBRARY} m dl ode pthread xerces-c )
endif()


set_target_properties(sigrunac PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin" LINKER_LANGUAGE CXX)


#
#
file(GLOB sample_ctrl "${PROJECT_SOURCE_DIR}/srcs/sigverse/controller/samples/*.cpp")
install(FILES ${sample_ctrl} DESTINATION "${INSTALL_DIR}/share/samples" )

