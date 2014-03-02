/*
 * Created by okamoto on 2011-03-25
 */

#ifndef ControllerLib_h
#define ControllerLib_h

class Controller;

class ControllerLib
{
private:
	typedef Controller *CreateFunc();	
private:
	void       *m_lib;
	Controller *m_model;
	CreateFunc *m_create;
public:
	
	ControllerLib();
	~ControllerLib();
	bool	load(const char *libname);
	Controller & getController();
};

#endif // ControllerLib_h
 
