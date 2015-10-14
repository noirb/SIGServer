#
# SIGVerse tools
#

set(sendmsg_srcs    "${CMAKE_SOURCE_DIR}/srcs/sigverse/tools/sendmsg.cpp")
set(sendmsg_headers "")

set(sigstart_srcs    "${CMAKE_SOURCE_DIR}/srcs/sigverse/tools/sigstart.cpp")
set(sigstart_headers "")

set(sigend_srcs    "${CMAKE_SOURCE_DIR}/srcs/sigverse/tools/sigend.cpp")
set(sigend_headers "")


link_directories("${CMAKE_BINARY_DIR}/lib" "${ODE_ROOT_DIR}/lib/${VCVER}")

add_executable(sendmsg  ${sendmsg_srcs}  ${sendmsg_headers} )
add_executable(sigstart ${sigstart_srcs} ${sigstart_headers} )
add_executable(sigend   ${sigend_srcs}   ${sigend_headers} )


if(WIN32)
    set( CMAKE_EXE_LINKER_FLAGS_RELEASE  "/LTCG ${CMAKE_EXE_LINKER_FLAGS}" )
    set( CMAKE_EXE_LINKER_FLAGS_DEBUG  "${CMAKE_EXE_LINKER_FLAGS}" )
    target_link_libraries(sendmsg  commonlib)
    target_link_libraries(sigstart commonlib)
    target_link_libraries(sigend   commonlib)
    
    file(TO_NATIVE_PATH ${CMAKE_INSTALL_PREFIX} CMAKE_INSTALL_PREFIX)
    
    message(${CMAKE_INSTALL_PREFIX})
    configure_file(${PROJECT_SOURCE_DIR}/sigmake.bat.in ${CMAKE_BINARY_DIR}/sigmake.bat)
    configure_file(${PROJECT_SOURCE_DIR}/mkdll.nmake.in ${CMAKE_BINARY_DIR}/mkdll.nmake)
    configure_file(${PROJECT_SOURCE_DIR}/sigcreate.bat.in ${CMAKE_BINARY_DIR}/sigcreate.bat)
    install(FILES ${CMAKE_BINARY_DIR}/sigmake.bat ${CMAKE_BINARY_DIR}/sigcreate.bat DESTINATION bin)
    install(FILES ${CMAKE_BINARY_DIR}/mkdll.nmake DESTINATION share/sigverse/etc)
else()
    target_link_libraries(sendmsg  commonlib ${JAVA_JVM_LIBRARY} m dl ode pthread xerces-c )
    target_link_libraries(sigstart commonlib ${JAVA_JVM_LIBRARY} m dl ode pthread xerces-c )
    target_link_libraries(sigend   commonlib ${JAVA_JVM_LIBRARY} m dl ode pthread xerces-c )
endif()


set_target_properties(sendmsg  PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin" LINKER_LANGUAGE CXX)
set_target_properties(sigstart PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin" LINKER_LANGUAGE CXX)
set_target_properties(sigend   PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin" LINKER_LANGUAGE CXX)

