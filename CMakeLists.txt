#
# SIGVerse
#

cmake_minimum_required(VERSION 2.8)

PROJECT(SIGVerse CXX)

include("${PROJECT_SOURCE_DIR}/cmake/SIGVerse_Env.cmake")

include("${PROJECT_SOURCE_DIR}/cmake/SIGVerse_commonlib.cmake")
#add_subdirectory(srcs/sigverse/commonlib)
add_subdirectory(srcs/sigverse/x3d/parser/cpp/X3DParser)
add_subdirectory(srcs/sigverse/controller)
add_subdirectory(srcs/sigverse/simserver)
add_subdirectory(srcs/sigverse/tools)


install(DIRECTORY jar DESTINATION share/sigverse
   FILES_MATCHING PATTERN "*.jar"
)


message(STATUS "SIGVERSE_ROOT_DIR:" ${SIGVERSE_ROOT_DIR})
message(STATUS "ODE_ROOT_DIR:" ${ODE_ROOT_DIR})
message(STATUS "XERCES_ROOT_DIR:" ${XERCES_ROOT_DIR})

