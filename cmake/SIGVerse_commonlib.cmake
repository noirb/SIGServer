#
# SIGVerse commonlib
#

file(GLOB_RECURSE commonlib_srcs    "${CMAKE_SOURCE_DIR}/srcs/sigverse/commonlib/*.cpp")
file(GLOB_RECURSE commonlib_headers "${CMAKE_SOURCE_DIR}/srcs/sigverse/commonlib/*.h")

add_library(commonlib STATIC ${commonlib_srcs} ${commonlib_headers})

if(WIN32)
    set( CMAKE_STATIC_LINKER_FLAGS  "${CMAKE_SHARED_LINKER_FLAGS}  /LTCG /NODEFAULTLIB:MSVCRT" )
    target_link_libraries(commonlib Ws2_32)
    target_link_libraries(commonlib debug oded optimized ode)
endif()

set_target_properties(commonlib PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib" LINKER_LANGUAGE CXX)

install(FILES  ${headers} DESTINATION include/sigverse)

