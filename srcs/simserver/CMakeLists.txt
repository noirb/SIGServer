#
#

cmake_minimum_required(VERSION 2.8)

PROJECT(simserver CXX)

include("${CMAKE_SOURCE_DIR}/cmake/SIGVerse_Env.cmake")


file(GLOB srcs "*.cpp")
file(GLOB readconf_srcs "readconf/*.cpp")
file(GLOB commonlib_srcs  "../commonlib/*.cpp")
file(GLOB commonlib_command_srcs  "../commonlib/command/*.cpp")
file(GLOB commonlib_comm_srcs  "../commonlib/comm/*.cpp")
file(GLOB commonlib_comm_controller_srcs  "../commonlib/comm/controller/*.cpp")
file(GLOB commonlib_comm_encoder_srcs  "../commonlib/comm/encoder/*.cpp")
file(GLOB commonlib_comm_event_srcs  "../commonlib/comm/event/*.cpp")
file(GLOB commonlib_ct_srcs "../commonlib/ct/*.cpp")

file(GLOB headers "*.h")
file(GLOB readconf_headers "readconf/*.h")
file(GLOB commonlib_headers  "../commonlib/*.h")
file(GLOB commonlib_command_headers  "../commonlib/command/*.h")
file(GLOB commonlib_comm_headers  "../commonlib/comm/*.h")
file(GLOB commonlib_comm_controller_headers  "../commonlib/comm/controller/*.h")
file(GLOB commonlib_comm_encoder_headers  "../commonlib/comm/encoder/*.h")
file(GLOB commonlib_comm_event_headers  "../commonlib/comm/event/*.h")
file(GLOB commonlib_ct_headers "../commonlib/ct/*.h")



include_directories("${PROJECT_SOURCE_DIR}"
  "${SIGVERSE_ROOT_DIR}/srcs/commonlib"
  "${PROJECT_SOURCE_DIR}/readconf"
  "${PROJECT_SOURCE_DIR}/../commonlib/comm"
  "${JDK_ROOT_DIR}/include/win32"
  "${JDK_ROOT_DIR}/include"
  "${XERCES_ROOT_DIR}/src"
  "${XERCES_ROOT_DIR}/include"
  "${SIGVERSE_ROOT_DIR}/srcs/x3d/parser/cpp/X3DParser"
 )

message(STATUS "VERSION ${VCVER}")

include_directories(
      "${JDK_ROOT_DIR}/include/win32"
      "${JDK_ROOT_DIR}/include"
)

link_directories("${CMAKE_BINARY_DIR}/commonlib"
       "${ODE_ROOT_DIR}/lib/${VCVER}"
       "${XERCES_ROOT_DIR}/Build/Win32/${VCVER_S}"
	"${XERCES_ROOT_DIR}/lib"
      "${JDK_ROOT_DIR}/lib"
      "${CMAKE_BINARY_DIR}/x3d"
)


ADD_EXECUTABLE(simserver
 ${srcs} ${readconf_srcs}
 ${commonlib_srcs}
 ${commonlib_command_srcs}
 ${commonlib_comm_srcs}
 ${commonlib_comm_controller_srcs}
 ${commonlib_comm_encoder_srcs}
 ${commonlib_comm_event_srcs}
 ${commonlib_ct_srcs}

 ${headers}  ${readconf_headers}
 ${commonlib_headers}
 ${commonlib_command_headers}
 ${commonlib_comm_headers}
 ${commonlib_comm_controller_headers}
 ${commonlib_comm_encoder_headers}
 ${commonlib_comm_event_headers}
 ${commonlib_ct_headers}
)

source_group("Source Files\\readconf"  FILES ${readconf_srcs})
source_group("Source Files\\commonlib"  FILES ${commonlib_srcs})
source_group("Source Files\\commonlib\\command"  FILES ${commonlib_command_srcs})
source_group("Source Files\\commonlib\\comm"  FILES ${commonlib_comm_srcs})
source_group("Source Files\\commonlib\\comm\\controller"  FILES ${commonlib_comm_controller_srcs})
source_group("Source Files\\commonlib\\comm\\encoder"  FILES ${commonlib_comm_encoder_srcs})
source_group("Source Files\\commonlib\\comm\\event"  FILES ${commonlib_comm_event_srcs})
source_group("Source Files\\commonlib\\ct"  FILES ${commonlib_ct_srcs})

source_group("Header Files\\readconf"  FILES ${readconf_headers})
source_group("Header Files\\commonlib"  FILES ${commonlib_headers})
source_group("Header Files\\commonlib\\command"  FILES ${commonlib_command_headers})
source_group("Header Files\\commonlib\\comm"  FILES ${commonlib_comm_headers})
source_group("Header Files\\commonlib\\comm\\controller"  FILES ${commonlib_comm_controller_headers})
source_group("Header Files\\commonlib\\comm\\encoder"  FILES ${commonlib_comm_encoder_headers})
source_group("Header Files\\commonlib\\comm\\event"  FILES ${commonlib_comm_event_headers})
source_group("Header Files\\commonlib\\ct"  FILES ${commonlib_ct_headers})



if(WIN32)
  SET( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS}" )
  target_link_libraries(simserver Ws2_32 jvm libx3d)
  target_link_libraries(simserver debug oded  optimized ode)
  target_link_libraries(simserver debug xerces-c_3D  optimized xerces-c_3)
endif()


SET_TARGET_PROPERTIES(simserver PROPERTIES LINKER_LANGUAGE CXX)

