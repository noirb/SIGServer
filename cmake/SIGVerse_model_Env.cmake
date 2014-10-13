#
#
set(SIGV_MAJOR_VERSION 2) 
set(SIGV_MINOR_VERSION 2)
set(SIGV_PATCH_VERSION 0)
set(SIGV_VERSION ${SIGV_MAJOR_VERSION}.${SIGV_MINOR_VERSION})
set(SIGV_FULL_VERSION ${SIGV_MAJOR_VERSION}.${SIGV_MINOR_VERSION}.${SIGV_PATCH_VERSION})

if(MSVC10)
  set(VCVER "vs2010")
  set(VCVER_S "VC10")
elseif(MSVC11)
  set(VCVER "vs2012")
  set(VCVER_S "VC11")
elseif(MSVC12)
  set(VCVER "vs2013")
  set(VCVER_S "VC12")
else()
  set(VCVER "")
  set(VCVER "")
endif()


include("${CMAKE_SOURCE_DIR}/cmake/SIGVerse_find_libs.cmake")

include_directories("${CMAKE_SOURCE_DIR}" "${ODE_ROOT_DIR}/include")

if(WIN32)
add_definitions(-DWIN32 -DUSE_ODE -DdDOUBLE -D CONTROLLER -D_DEBUG )
else()
add_definitions(-DIRWAS_SIMSERVER -DdDOUBLE -DSTRICT_INTERFACE)
endif()


