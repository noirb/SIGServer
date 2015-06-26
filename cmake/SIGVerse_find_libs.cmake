#
#
find_path(SIGVERSE_ROOT_DIR docs/sig_server
 HINTS "${CMAKE_SOURCE_DIR}/*" "${CMAKE_SOURCE_DIR}/../*" 
 "${CMAKE_SOURCE_DIR}/../../*" 
 "/usr/local/*/*"
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

find_path(JDK_ROOT_DIR bin/javac.exe
 HINTS
   "C:/Program Files (x86)/Java/*"
   "C:/Program Files/Java/*"
	 "/usr/local/*/*"
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

find_path(JRE_ROOT_DIR bin/client/jvm.dll
 HINTS
   "C:/Program Files (x86)/Java/*"
   "C:/Program Files/Java/*"
	 "/usr/local/*/*"
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
