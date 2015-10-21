#
# SIGVerse sigrunac
#

# Sources
file(GLOB_RECURSE sigrunac_srcs    "${PROJECT_SOURCE_DIR}/srcs/sigverse/controller/*.cpp")
file(GLOB_RECURSE sigrunac_headers "${PROJECT_SOURCE_DIR}/srcs/sigverse/controller/*.h")

file(GLOB_RECURSE exclude_files "${PROJECT_SOURCE_DIR}/srcs/sigverse/controller/samples/*")
    
list(REMOVE_ITEM sigrunac_srcs    ${exclude_files})
list(REMOVE_ITEM sigrunac_headers ${exclude_files})


# Libraries
add_executable(sigrunac ${sigrunac_srcs} ${sigrunac_headers})


# Compile options
set_target_properties(sigrunac PROPERTIES COMPILE_DEFINITIONS "CONTROLLER")
set_target_properties(sigrunac PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin" LINKER_LANGUAGE CXX)

if(WIN32)
    target_link_libraries(sigrunac Ws2_32 commonlib_controller x3dparser jvm optimized ode optimized xerces-c_3)
else()
    target_link_libraries(sigrunac commonlib_controller x3dparser ${JAVA_JVM_LIBRARY} dl ode xerces-c pthread m)
endif()


# Install
install(DIRECTORY "${PROJECT_SOURCE_DIR}/srcs/sigverse/controller/samples" DESTINATION "${INSTALL_DIR}/share" FILES_MATCHING PATTERN "*")

if(WIN32)
    install(FILES "${PROJECT_BINARY_DIR}/bin/${CMAKE_CONFIGURATION_TYPES}/sigrunac.exe" DESTINATION "${INSTALL_DIR}/bin")
else()
    install(FILES "${PROJECT_BINARY_DIR}/bin/sigrunac" DESTINATION "${INSTALL_DIR}/bin" PERMISSIONS ${PERMISSION755})
endif()
