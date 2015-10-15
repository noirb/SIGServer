#
# SIGVerse commonlib
#

file(GLOB_RECURSE commonlib_srcs    "${PROJECT_SOURCE_DIR}/srcs/sigverse/commonlib/*.cpp")
file(GLOB_RECURSE commonlib_headers "${PROJECT_SOURCE_DIR}/srcs/sigverse/commonlib/*.h")

add_library(commonlib STATIC ${commonlib_srcs} ${commonlib_headers})

if(WIN32)
    set( CMAKE_STATIC_LINKER_FLAGS  "${CMAKE_SHARED_LINKER_FLAGS}  /LTCG /NODEFAULTLIB:MSVCRT" )
    target_link_libraries(commonlib Ws2_32)
    target_link_libraries(commonlib debug oded optimized ode)
endif()

set_target_properties(commonlib PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib" LINKER_LANGUAGE CXX)

