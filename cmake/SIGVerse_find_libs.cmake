#
#
find_path(SIGVERSE_ROOT_DIR docs/sig_server
 HINTS "${CMAKE_SOURCE_DIR}/*" 
    "${CMAKE_SOURCE_DIR}/../*" 
    "${CMAKE_SOURCE_DIR}/../../*" 
    "/usr/local/*/*"
    "/usr/lib/*/*"
    "/usr/share/*/*"
 "C:/Program Files (x86)/*"
 "C:/Program Files/*"
 "$ENV{HOME}/*"
 "$ENV{HOME}/*/*"
 "C:/*" 
 "D:/*/*/*"
 "D:/*/*"
 "D:/*" 
 "E:/*/*/*"
 "E:/*/*"
 "E:/*"

)

find_path(ODE_ROOT_DIR ode.pc.in
  HINTS "${CMAKE_SOURCE_DIR}/../../*"
	"${SIGVERSE_ROOT_DIR}/../*/*"
	"${SIGVERSE_ROOT_DIR}/../../*/*/*"
	 "/usr/local/*/*"
	 "/usr/lib/*/*"
	 "/usr/share/*/*"
 "$ENV{HOME}/*"
 "$ENV{HOME}/*/*"
 "C:/*" 
 "D:/*/*/*"
 "D:/*/*"
 "D:/*"
 "E:/*/*/*"
 "E:/*/*"
 "E:/*"
 )

find_path(XERCES_ROOT_DIR doc/xerces-c_book.xml
	 HINTS  "${CMAKE_SOURCE_DIR}/../../*"
	 "/usr/local/*/*"
	 "/usr/lib/*/*"
	 "/usr/share/*/*"
 "$ENV{HOME}/*"
 "$ENV{HOME}/*/*"
 "C:/*" 
 "D:/*/*/*"
 "D:/*/*"
 "D:/*"
 "E:/*/*/*"
 "E:/*/*"
 "E:/*"
)

# Find javac
if(WIN32)
find_path(JDK_ROOT_DIR javac
 HINTS
   "C:/Program Files (x86)/Java/*"
   "C:/Program Files/Java/*"
 "$ENV{HOME}/*"
 "$ENV{HOME}/*/*"
 "C:/*" 
 "D:/*/*/*"
 "D:/*/*"
 "D:/*"
 "E:/*/*/*"
 "E:/*/*"
 "E:/*"
 )
else()
#    find_path(JDK_ROOT_DIR bin/javac
#        HINTS
#        "/usr/local/*/*"
#        "/usr/lib/*/*"
#        "/usr/share/*/*"
#        "$ENV{HOME}/*"
#        "$ENV{HOME}/*/*"
#        )
    FIND_PACKAGE(JNI)
endif()


# Find jvm library
#unset(JRE_ROOT_DIR CACHE)
#
if(WIN32)
find_path(JRE_ROOT_DIR bin/client/jvm.dll
 HINTS
   "C:/Program Files (x86)/Java/*"
   "C:/Program Files/Java/*"
 "C:/*" 
 "D:/*/*/*"
 "D:/*/*"
 "D:/*"
 "E:/*/*/*"
 "E:/*/*"
 "E:/*"
 )
#else()
#    find_path(JRE_ROOT_DIR libjvm.so
#       　PATHS
##        "/usr/local/*/*"
#        "/usr/lib/jvm/java-7-openjdk-amd64/jre/*/*"
##        "/usr/lib/jvm/java-7-openjdk-amd64/jre/lib/*/*"
##        "/usr/share/*/*"
##        "$ENV{HOME}/*"
##        "$ENV{HOME}/*/*"
#        )
##    file(GLOB_RECURSE JRE_ROOT_DIR  "libjvm.so")
endif()

#unset(JRE_ROOT_DIR CACHE)
#set(JRE_ROOT_DIR ${JAVA_JVM_LIBRARY})
#file(GLOB JRE_ROOT_DIR "/usr/lib/jvm/*/jre/lib/amd64/server/libjvm.so" FOLLOW_SYMLINKS)
