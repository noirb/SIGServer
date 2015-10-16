#
# SIGVerse tools
#

set(sendmsg_srcs    "${PROJECT_SOURCE_DIR}/srcs/sigverse/tools/sendmsg.cpp")
set(sendmsg_headers "")

set(sigstart_srcs    "${PROJECT_SOURCE_DIR}/srcs/sigverse/tools/sigstart.cpp")
set(sigstart_headers "")

set(sigend_srcs    "${PROJECT_SOURCE_DIR}/srcs/sigverse/tools/sigend.cpp")
set(sigend_headers "")


link_directories("${PROJECT_BINARY_DIR}/lib" "${ODE_ROOT_DIR}/lib/${VCVER}")

add_executable(sendmsg  ${sendmsg_srcs}  ${sendmsg_headers}  ${commonlib_srcs} ${commonlib_headers})
add_executable(sigstart ${sigstart_srcs} ${sigstart_headers} ${commonlib_srcs} ${commonlib_headers})
add_executable(sigend   ${sigend_srcs}   ${sigend_headers}   ${commonlib_srcs} ${commonlib_headers})


target_link_libraries(sendmsg  ${JAVA_JVM_LIBRARY} m dl ode pthread xerces-c)
target_link_libraries(sigstart ${JAVA_JVM_LIBRARY} m dl ode pthread xerces-c)
target_link_libraries(sigend   ${JAVA_JVM_LIBRARY} m dl ode pthread xerces-c)
    
if(WIN32)
    set( CMAKE_EXE_LINKER_FLAGS_RELEASE  "/LTCG ${CMAKE_EXE_LINKER_FLAGS}" )
    set( CMAKE_EXE_LINKER_FLAGS_DEBUG  "${CMAKE_EXE_LINKER_FLAGS}" )
    file(TO_NATIVE_PATH ${CMAKE_INSTALL_PREFIX} CMAKE_INSTALL_PREFIX)
    
    message(${CMAKE_INSTALL_PREFIX})
    
    configure_file(${PROJECT_SOURCE_DIR}/tools/sigserver.bat.in ${PROJECT_BINARY_DIR}/bin/sigserver.bat)
    configure_file(${PROJECT_SOURCE_DIR}/tools/sigmake.bat.in   ${PROJECT_BINARY_DIR}/bin/sigmake.bat)
    configure_file(${PROJECT_SOURCE_DIR}/tools/mkdll.nmake.in   ${PROJECT_BINARY_DIR}/bin/mkdll.nmake)
    configure_file(${PROJECT_SOURCE_DIR}/tools/sigcreate.bat.in ${PROJECT_BINARY_DIR}/bin/sigcreate.bat)
else()
    configure_file(${PROJECT_SOURCE_DIR}/tools/sigserver.sh.in  ${PROJECT_BINARY_DIR}/bin/sigserver.sh)
    configure_file(${PROJECT_SOURCE_DIR}/tools/siggenac.in      ${PROJECT_BINARY_DIR}/bin/siggenac)
    configure_file(${PROJECT_SOURCE_DIR}/tools/X3DParser.cfg.in ${PROJECT_BINARY_DIR}/share/etc/X3DParser.cfg)
endif()


set_target_properties(sendmsg  PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin" LINKER_LANGUAGE CXX)
set_target_properties(sigstart PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin" LINKER_LANGUAGE CXX)
set_target_properties(sigend   PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin" LINKER_LANGUAGE CXX)

install(FILES "${PROJECT_BINARY_DIR}/bin/sendmsg"  DESTINATION "${INSTALL_DIR}/bin" PERMISSIONS ${PERMISSION755})
install(FILES "${PROJECT_BINARY_DIR}/bin/sigstart" DESTINATION "${INSTALL_DIR}/bin" PERMISSIONS ${PERMISSION755})
install(FILES "${PROJECT_BINARY_DIR}/bin/sigend"   DESTINATION "${INSTALL_DIR}/bin" PERMISSIONS ${PERMISSION755})

if(WIN32)
    install(FILES ${PROJECT_BINARY_DIR}/bin/sigmake.bat   DESTINATION "${INSTALL_DIR}/bin")
    install(FILES ${PROJECT_BINARY_DIR}/bin/mkdll.nmake   DESTINATION "${INSTALL_DIR}/share/sigverse/etc")
    install(FILES ${PROJECT_BINARY_DIR}/bin/sigcreate.bat DESTINATION "${INSTALL_DIR}/bin")
else()
    install(FILES ${PROJECT_BINARY_DIR}/bin/sigserver.sh         DESTINATION "${INSTALL_DIR}/bin"       PERMISSIONS ${PERMISSION755})
    install(FILES ${PROJECT_BINARY_DIR}/bin/siggenac             DESTINATION "${INSTALL_DIR}/bin"       PERMISSIONS ${PERMISSION755})
    install(FILES ${PROJECT_BINARY_DIR}/share/etc/X3DParser.cfg  DESTINATION "${INSTALL_DIR}/share/etc")
    install(FILES ${PROJECT_SOURCE_DIR}/tools/sigcreate.sh       DESTINATION "${INSTALL_DIR}/bin"       PERMISSIONS ${PERMISSION755})
    install(FILES ${PROJECT_SOURCE_DIR}/tools/sigkill.sh         DESTINATION "${INSTALL_DIR}/bin"       PERMISSIONS ${PERMISSION755})
endif()
