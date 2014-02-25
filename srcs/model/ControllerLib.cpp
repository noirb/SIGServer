/* $Id: ControllerLib.cpp,v 1.1.1.1 2011-03-25 02:18:46 okamoto Exp $ */
#include "ControllerLib.h"
#include "Controller.h"
#include "modelerr.h"

#include <dlfcn.h>
#include <assert.h>

ControllerLib::ControllerLib() : m_lib(0), m_model(0), m_create(0)
{

}

ControllerLib::~ControllerLib()
{
	delete m_model;
	m_model = 0;
	if (m_lib) {
		dlclose(m_lib);
		m_lib = 0;
	}
}

bool ControllerLib::load(const char *libname)
{
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
