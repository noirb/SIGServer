/*
 * Created by okamoto on 2011-03-25
 */

#ifndef ControllerLib_h
#define ControllerLib_h

#include "Controller.h"
#include "modelerr.h"

#include <assert.h>

#ifndef WIN32
#include <dlfcn.h>
#else
#include <windows.h>
#include <stdio.h>
#endif

//class Controller;

class ControllerLib
{
private:

#ifndef WIN32
	typedef Controller *CreateFunc();
#else
	typedef Controller *(__stdcall *CreateFunc)();
#endif
private:
#ifndef WIN32
	void       *m_lib;
	CreateFunc *m_create;
#else
	HMODULE    m_lib;
	CreateFunc m_create;
#endif
	Controller *m_model;
public:
	
	ControllerLib();
	~ControllerLib();
	bool load(const char *libname);
	Controller & getController();
};

#endif // ControllerLib_h
 
