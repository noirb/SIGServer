#
# find library

if(WIN32)
    find_path(SIGVERSE_ROOT_DIR docs/sig_server
        HINTS 
        "C:/Program Files (x86)/*"
        "C:/Program Files/*"
        "C:/*" 
        "D:/*/*/*"
        "D:/*/*"
        "D:/*" 
        "E:/*/*/*"
        "E:/*/*"
        "E:/*"
    )
    
    find_path(ODE_ROOT_DIR ode.pc.in
        HINTS 
        "C:/Program Files (x86)/*"
        "C:/Program Files/*"
        "C:/*" 
        "D:/*/*/*"
        "D:/*/*"
        "D:/*" 
        "E:/*/*/*"
        "E:/*/*"
        "E:/*"
     )
    
    find_path(XERCES_ROOT_DIR doc/xerces-c_book.xml
        HINTS 
        "C:/Program Files (x86)/*"
        "C:/Program Files/*"
        "C:/*" 
        "D:/*/*/*"
        "D:/*/*"
        "D:/*" 
        "E:/*/*/*"
        "E:/*/*"
        "E:/*"
    )
    
    # Find javac
    find_path(JDK_ROOT_DIR bin/javac.exe
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
    
    # Find jvm library
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
else()
    # Find JNI library
    FIND_PACKAGE(JNI)
endif()

