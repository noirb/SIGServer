#
# SIGVerse tools
#

# Sources
set(sendmsg_srcs    "${PROJECT_SOURCE_DIR}/srcs/sigverse/tools/sendmsg.cpp")
set(sendmsg_headers "")

set(sigstart_srcs    "${PROJECT_SOURCE_DIR}/srcs/sigverse/tools/sigstart.cpp")
set(sigstart_headers "")

set(sigend_srcs    "${PROJECT_SOURCE_DIR}/srcs/sigverse/tools/sigend.cpp")
set(sigend_headers "")


# Libraries
add_executable(sendmsg  ${sendmsg_srcs}  ${sendmsg_headers})
add_executable(sigstart ${sigstart_srcs} ${sigstart_headers})
add_executable(sigend   ${sigend_srcs}   ${sigend_headers})


# Compile options
if(WIN32)
    target_link_libraries(sendmsg  Ws2_32 commonlib_server optimized ode optimized xerces-c_3)
    target_link_libraries(sigstart Ws2_32 commonlib_server optimized ode optimized xerces-c_3)
    target_link_libraries(sigend   Ws2_32 commonlib_server optimized ode optimized xerces-c_3)
else()
    target_link_libraries(sendmsg  commonlib_server dl ode xerces-c pthread m)
    target_link_libraries(sigstart commonlib_server dl ode xerces-c pthread m)
    target_link_libraries(sigend   commonlib_server dl ode xerces-c pthread m)
endif()

set_target_properties(sendmsg  PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin" LINKER_LANGUAGE CXX)
set_target_properties(sigstart PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin" LINKER_LANGUAGE CXX)
set_target_properties(sigend   PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin" LINKER_LANGUAGE CXX)

# Compile options (for scripts)
configure_file("${PROJECT_SOURCE_DIR}/tools/X3DParser.cfg.in" "${PROJECT_BINARY_DIR}/share/etc/X3DParser.cfg")

if(WIN32)
    configure_file("${PROJECT_SOURCE_DIR}/tools/win/sigcreate.bat.in" "${PROJECT_BINARY_DIR}/bin/sigcreate.bat")
    configure_file("${PROJECT_SOURCE_DIR}/tools/win/sigserver.bat.in" "${PROJECT_BINARY_DIR}/bin/sigserver.bat")
    configure_file("${PROJECT_SOURCE_DIR}/tools/win/sigmake.bat.in"   "${PROJECT_BINARY_DIR}/bin/sigmake.bat")
    configure_file("${PROJECT_SOURCE_DIR}/tools/win/mkdll.nmake.in"   "${PROJECT_BINARY_DIR}/etc/mkdll.nmake")
else()
    configure_file("${PROJECT_SOURCE_DIR}/tools/linux/sigserver.sh.in"  "${PROJECT_BINARY_DIR}/bin/sigserver.sh")
    configure_file("${PROJECT_SOURCE_DIR}/tools/linux/siggenac.in"      "${PROJECT_BINARY_DIR}/bin/siggenac")
endif()


# Install
install(FILES "${PROJECT_BINARY_DIR}/share/etc/X3DParser.cfg"  DESTINATION "${INSTALL_DIR}/share/etc")

if(WIN32)
    install(FILES "${PROJECT_BINARY_DIR}/bin/${CMAKE_CONFIGURATION_TYPES}/sendmsg.exe"  DESTINATION "${INSTALL_DIR}/bin")
    install(FILES "${PROJECT_BINARY_DIR}/bin/${CMAKE_CONFIGURATION_TYPES}/sigstart.exe" DESTINATION "${INSTALL_DIR}/bin")
    install(FILES "${PROJECT_BINARY_DIR}/bin/${CMAKE_CONFIGURATION_TYPES}/sigend.exe"   DESTINATION "${INSTALL_DIR}/bin")
    
    install(FILES "${PROJECT_BINARY_DIR}/bin/sigcreate.bat" DESTINATION "${INSTALL_DIR}/bin")
    install(FILES "${PROJECT_BINARY_DIR}/bin/sigserver.bat" DESTINATION "${INSTALL_DIR}/bin")
    install(FILES "${PROJECT_BINARY_DIR}/bin/sigmake.bat"   DESTINATION "${INSTALL_DIR}/bin")
    install(FILES "${PROJECT_BINARY_DIR}/etc/mkdll.nmake"   DESTINATION "${INSTALL_DIR}/share/etc")
else()
    install(FILES "${PROJECT_BINARY_DIR}/bin/sendmsg"  DESTINATION "${INSTALL_DIR}/bin" PERMISSIONS ${PERMISSION755})
    install(FILES "${PROJECT_BINARY_DIR}/bin/sigstart" DESTINATION "${INSTALL_DIR}/bin" PERMISSIONS ${PERMISSION755})
    install(FILES "${PROJECT_BINARY_DIR}/bin/sigend"   DESTINATION "${INSTALL_DIR}/bin" PERMISSIONS ${PERMISSION755})
    
    install(FILES "${PROJECT_BINARY_DIR}/bin/sigserver.sh"         DESTINATION "${INSTALL_DIR}/bin"  PERMISSIONS ${PERMISSION755})
    install(FILES "${PROJECT_BINARY_DIR}/bin/siggenac"             DESTINATION "${INSTALL_DIR}/bin"  PERMISSIONS ${PERMISSION755})
    
    install(FILES "${PROJECT_SOURCE_DIR}/tools/linux/sigcreate.sh" DESTINATION "${INSTALL_DIR}/bin"  PERMISSIONS ${PERMISSION755})
    install(FILES "${PROJECT_SOURCE_DIR}/tools/linux/sigkill.sh"   DESTINATION "${INSTALL_DIR}/bin"  PERMISSIONS ${PERMISSION755})
endif()
