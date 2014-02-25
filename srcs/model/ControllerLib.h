/* $Id: ControllerLib.h,v 1.1.1.1 2011-03-25 02:18:46 okamoto Exp $ */ 
#ifndef ControllerLib_h
#define ControllerLib_h

class Controller;

class ControllerLib
{
private:
	typedef Controller *CreateFunc();	
private:
	void	*m_lib;
	Controller	*m_model;
	CreateFunc *m_create;
public:
	
	ControllerLib();
	~ControllerLib();
	bool	load(const char *libname);
	Controller & getController();
};

#endif // ControllerLib_h
 
