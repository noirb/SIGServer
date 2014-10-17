/*
 * Created by okamoto on 2011-03-25
 */

#include "ControllerLib.h"

ControllerLib::ControllerLib() : m_lib(NULL), m_model(NULL), m_create(NULL)
{

}

ControllerLib::~ControllerLib()
{
	delete m_model;
	m_model = 0;
	if (m_lib) {
#ifndef WIN32
		dlclose(m_lib);
#else
		FreeLibrary(m_lib);
#endif
		m_lib = NULL;
	}
}

bool ControllerLib::load(const char *libname)
{
#ifndef WIN32
	void *lib = dlopen(libname, RTLD_LAZY);
	if (!lib) {
		err(("Loading shared library : %s\n", dlerror()));
		return false;
	}

	m_create = (CreateFunc*)dlsym(lib, "createController");
	if (!m_create) {
		err(("cannot load create symbol\n"));
		dlclose(lib);
		return false;
	}
#else
	HMODULE lib = LoadLibrary(libname);
	if(lib == NULL){
		err(("Loading shared library : %s(%d)\n", libname, GetLastError()));
		return false;
	}
	m_create = (CreateFunc)GetProcAddress(lib, "createController");
	if (m_create == NULL) {
		err(("cannot load create symbol\n"));
		FreeLibrary(lib);
		return false;
	}

#endif
	m_lib = lib;

	return true;
}


Controller & ControllerLib::getController()
{
	assert(m_lib);
	if (!m_model) {
		m_model = (this->m_create)();
	}
	return *m_model;
}
