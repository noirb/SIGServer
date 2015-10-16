#
# SIGVerse sigrunac
#

file(GLOB_RECURSE sigrunac_srcs    "${PROJECT_SOURCE_DIR}/srcs/sigverse/controller/*.cpp")
file(GLOB_RECURSE sigrunac_headers "${PROJECT_SOURCE_DIR}/srcs/sigverse/controller/*.h")

file(GLOB_RECURSE exclude_files "${PROJECT_SOURCE_DIR}/srcs/sigverse/controller/samples/*")
    
list(REMOVE_ITEM sigrunac_srcs ${exclude_files})
list(REMOVE_ITEM sigrunac_headers ${exclude_files})


link_directories("${PROJECT_BINARY_DIR}/lib" "${ODE_ROOT_DIR}/lib/${VCVER}")

add_executable(sigrunac ${sigrunac_srcs} ${sigrunac_headers} ${commonlib_srcs} ${commonlib_headers})

set_target_properties(sigrunac PROPERTIES COMPILE_DEFINITIONS "CONTROLLER")
set_target_properties(sigrunac PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin" LINKER_LANGUAGE CXX)


if(WIN32)
    set( CMAKE_EXE_LINKER_FLAGS_RELEASE  "/LTCG ${CMAKE_EXE_LINKER_FLAGS}" )
    set( CMAKE_EXE_LINKER_FLAGS_DEBUG  "${CMAKE_EXE_LINKER_FLAGS}" )
    target_link_libraries(sigrunac x3dparser ${JAVA_JVM_LIBRARY} dl ode xerces-c pthread m)

    file(TO_NATIVE_PATH ${CMAKE_INSTALL_PREFIX} CMAKE_INSTALL_PREFIX)
else()
    target_link_libraries(sigrunac x3dparser ${JAVA_JVM_LIBRARY} dl ode xerces-c pthread m)
endif()



#
#
install(DIRECTORY "${PROJECT_SOURCE_DIR}/srcs/sigverse/controller/samples" DESTINATION "${INSTALL_DIR}/share" FILES_MATCHING PATTERN "*.cpp")

install(FILES "${PROJECT_BINARY_DIR}/bin/sigrunac" DESTINATION "${INSTALL_DIR}/bin" PERMISSIONS ${PERMISSION755})

