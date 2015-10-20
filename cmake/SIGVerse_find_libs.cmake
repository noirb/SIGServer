#
# find library

if(WIN32)
    list(APPEND LIB_SEARCH_PATH_WIN32 
        "C:/SIGVerse/GitHub/*"
        "C:/SIGVerse/ext/*"
        "C:/SIGVerse/ext/Java/*"
        "C:/Program Files (x86)/Java/*"
    )
    find_path(SIGVERSE_ROOT_DIR srcs/sigverse       HINTS ${LIB_SEARCH_PATH_WIN32})
    find_path(ODE_ROOT_DIR      ode.pc.in           HINTS ${LIB_SEARCH_PATH_WIN32})
    find_path(XERCES_ROOT_DIR   xerces-c.pc.in      HINTS ${LIB_SEARCH_PATH_WIN32})
    find_path(JDK_ROOT_DIR      bin/javac.exe       HINTS ${LIB_SEARCH_PATH_WIN32})
    find_path(JRE_ROOT_DIR      bin/client/jvm.dll  HINTS ${LIB_SEARCH_PATH_WIN32})
else()
    # Find JNI library
    FIND_PACKAGE(JNI)
    
    get_filename_component(JVM_PATH ${JAVA_JVM_LIBRARY} DIRECTORY)
    
    message(STATUS "JVM_PATH:" ${JVM_PATH})
endif()

