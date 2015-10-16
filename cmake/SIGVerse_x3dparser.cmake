#
# SIGVerse X3DParser
#

file(GLOB_RECURSE x3dparser_srcs    "${PROJECT_SOURCE_DIR}/srcs/sigverse/x3d/parser/cpp/X3DParser/*.cpp")
file(GLOB_RECURSE x3dparser_headers "${PROJECT_SOURCE_DIR}/srcs/sigverse/x3d/parser/cpp/X3DParser/*.h")

add_library(x3dparser STATIC ${x3dparser_srcs} ${x3dparser_headers})

set_target_properties(x3dparser PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib" LINKER_LANGUAGE CXX)

install(DIRECTORY "${PROJECT_SOURCE_DIR}/srcs/sigverse/x3d/jars" DESTINATION "${INSTALL_DIR}/share" FILES_MATCHING PATTERN "*.jar")

