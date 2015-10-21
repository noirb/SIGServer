#
# SIGVerse sigserver
#

# Sources
file(GLOB_RECURSE sigserver_srcs    "${PROJECT_SOURCE_DIR}/srcs/sigverse/simserver/*.cpp")
file(GLOB_RECURSE sigserver_headers "${PROJECT_SOURCE_DIR}/srcs/sigverse/simserver/*.h")


# Libraries
add_executable(sigserver ${sigserver_srcs} ${sigserver_headers})


# Compile options
set_target_properties(sigserver PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin" LINKER_LANGUAGE CXX)

if(WIN32)
    target_link_libraries(sigserver Ws2_32 commonlib_server x3dparser jvm optimized ode optimized xerces-c_3)
else()
    target_link_libraries(sigserver commonlib_server x3dparser ${JAVA_JVM_LIBRARY} dl ode xerces-c pthread m)
endif()


# Install
if(WIN32) 
    install(FILES "${PROJECT_BINARY_DIR}/bin/${CMAKE_CONFIGURATION_TYPES}/sigserver.exe" DESTINATION "${INSTALL_DIR}/bin")
else()
    install(FILES "${PROJECT_BINARY_DIR}/bin/sigserver" DESTINATION "${INSTALL_DIR}/bin" PERMISSIONS ${PERMISSION755})
endif()
