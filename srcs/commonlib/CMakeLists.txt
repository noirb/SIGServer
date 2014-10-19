#
#

cmake_minimum_required(VERSION 2.8)

PROJECT(commonlib CXX)

include("${CMAKE_SOURCE_DIR}/cmake/SIGVerse_model_Env.cmake")


file(GLOB srcs  "*.cpp")
file(GLOB command_srcs  "command/*.cpp")
file(GLOB comm_srcs  "comm/*.cpp")
file(GLOB comm_controller_srcs  "comm/controller/*.cpp")
file(GLOB comm_encoder_srcs  "comm/encoder/*.cpp")
file(GLOB comm_event_srcs  "comm/event/*.cpp")
file(GLOB ct_srcs "ct/*.cpp")

file(GLOB headers  "*.h")
file(GLOB command_headers  "command/*.h")
file(GLOB comm_headers  "comm/*.h")
file(GLOB comm_controller_headers  "comm/controller/*.h")
file(GLOB comm_encoder_headers  "comm/encoder/*.h")
file(GLOB comm_event_headers  "comm/event/*.h")
file(GLOB ct_headers "ct/*.h")

ADD_LIBRARY(commonlib STATIC
 ${srcs}
 ${command_srcs}
 ${comm_srcs}
 ${comm_controller_srcs}
 ${comm_encoder_srcs}
 ${comm_event_srcs}
 ${ct_srcs}

 ${headers}
 ${command_headers}
 ${comm_headers}
 ${comm_controller_headers}
 ${comm_encoder_headers}
 ${comm_event_headers}
 ${ct_headers}
)

source_group("Source Files\\command"  FILES ${command_srcs})
source_group("Source Files\\comm"  FILES ${comm_srcs})
source_group("Source Files\\comm\\controller"  FILES ${comm_controller_srcs})
source_group("Source Files\\comm\\encoder"  FILES ${comm_encoder_srcs})
source_group("Source Files\\comm\\event"  FILES ${comm_event_srcs})
source_group("Source Files\\ct"  FILES ${ct_srcs})

source_group("Header Files\\command"  FILES ${command_headers})
source_group("Header Files\\comm"  FILES ${comm_headers})
source_group("Header Files\\comm\\controller"  FILES ${comm_controller_headers})
source_group("Header Files\\comm\\encoder"  FILES ${comm_encoder_headers})
source_group("Header Files\\comm\\event"  FILES ${comm_event_headers})
source_group("Header Files\\ct"  FILES ${ct_headers})



include_directories("${PROJECT_SOURCE_DIR}" "${PROJECT_SOURCE_DIR}/comm")

if(WIN32)
SET( CMAKE_STATIC_LINKER_FLAGS  "${CMAKE_SHARED_LINKER_FLAGS}  /LTCG /NODEFAULTLIB:MSVCRT" )
target_link_libraries(commonlib Ws2_32)
target_link_libraries(commonlib debug oded optimized ode)

endif()

SET_TARGET_PROPERTIES(commonlib PROPERTIES LINKER_LANGUAGE CXX)

