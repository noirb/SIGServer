/*
 * Modified by kawamoto on 2011-04-05
 */

#ifndef SSimWorld_h
#define SSimWorld_h

#include <vector>
#include <map>

#include "SimWorld.h"
#include "ODEWorld.h"
#include "SSimObj.h"
#include "Vector3d.h"
#include "SSimEntity.h"


class SSimWorld : public SimWorld
{
public:
	typedef std::vector<SSimObj*> C;


private:
	typedef SimWorld Super;
private:
	ODEWorld  *m_odeWorld;
	bool	m_run;
	bool    m_autostep;
	bool    m_v21; 

	std::map<std::string, Vector3d*> m_posMap;
	std::map<std::string, SSimEntity*> m_ents;

public:
	SSimWorld(const char *name);
	~SSimWorld() { free_(); }

	SSimObj * getSObj(const char *name) {
		return (SSimObj*)Super::getObj(name);
	}

	C & 	copyObjs(C &c) {
		M::iterator i;
		for (i=m_objs.begin(); i!=m_objs.end(); i++) {
			SSimObj *obj = (SSimObj*)i->second;
			c.push_back(obj);
		}
		return c;
	}

	typedef std::vector<std::string> NameC;
	NameC &	copyNames(bool needEntities, NameC &c);

	double	time() const;
	void	time(double t);

	bool getV21(){ return m_v21;}

	void setV21(bool v){m_v21 = v;}

	void    setAutoStep(bool flag){m_autostep = flag;}

	void    setQuickStep(bool flag){m_odeWorld->setQuickStep(flag);}

	void    setStepSize(double stepsize){  m_odeWorld->setStepSize(stepsize);}

	void    setERP(double value){m_odeWorld->setERP(value);}

	void    setCFM(double value){m_odeWorld->setCFM(value);}

	void    setCollisionParam(std::string name, double value){m_odeWorld->setCollisionParam(name, value);}

	bool    getAutoStep(){return m_autostep;}

	unsigned stepCount() const { return m_odeWorld->stepCount(); }

	double	timeStep() const { return m_odeWorld->step(); }

	void addEntityPosition(std::string name, Vector3d *pos) { 
	  m_posMap.insert(std::map<std::string, Vector3d*>::value_type(name,pos));
	}

	bool addSSimEntity(std::string name, SSimEntity *ent){ 
	  std::map<std::string, SSimEntity*>::iterator it = m_ents.find(name);
	  
	  if(it != m_ents.end()){
	    LOG_ERR(("addSSimEntity: same name already exist"));
	    return false;
	  }
	  else{
	    m_ents.insert(std::map<std::string, SSimEntity*>::value_type(name, ent));
	  }
	  return true;
	}

	std::map<std::string, SSimEntity*>  getAllSSimEntities(){return m_ents;} 

	Vector3d* findEntityPosition(std::string name) { 
	  std::map<std::string, Vector3d*>::iterator i = m_posMap.find(name);
	  return i != m_posMap.end()? i->second: NULL;
	}


	void 	set(const ODEWorld::Gravity &g, double erp);
	void	setup();
	void	start();
	void	stop();
	bool	isRunning() { return m_run; }
	ODEWorld * odeWorld() { return m_odeWorld; }

	void	nextStep();

	void	nextStep(double stepsize, bool quick = false);

	void print();

private:
	void	preStep();
	void	postStep();
	void 	free_();
public:
#ifdef USE_OPENGL
	void	draw(DrawContext &c);
#endif
};


#endif // SSimWorld_h
 


