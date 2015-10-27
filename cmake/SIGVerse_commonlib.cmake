#
# SIGVerse commonlib
#

# Sources
file(GLOB_RECURSE commonlib_srcs    "${PROJECT_SOURCE_DIR}/srcs/sigverse/commonlib/*.cpp")
file(GLOB_RECURSE commonlib_headers "${PROJECT_SOURCE_DIR}/srcs/sigverse/commonlib/*.h")

# Libraries
add_library(commonlib_controller STATIC ${commonlib_srcs} ${commonlib_headers})
add_library(commonlib_server     STATIC ${commonlib_srcs} ${commonlib_headers})

# Compile options
if(WIN32)
    target_link_libraries(commonlib_controller Ws2_32 optimized ode)
    target_link_libraries(commonlib_server     Ws2_32 optimized ode)
endif()

set_target_properties(commonlib_controller PROPERTIES COMPILE_DEFINITIONS "CONTROLLER")

set_target_properties(commonlib_controller PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib" LINKER_LANGUAGE CXX)
set_target_properties(commonlib_server     PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib" LINKER_LANGUAGE CXX)

# Install
if(WIN32)
    install(FILES "${PROJECT_BINARY_DIR}/lib/${CMAKE_CONFIGURATION_TYPES}/commonlib_controller.lib" DESTINATION "${INSTALL_DIR}/lib")
endif()
