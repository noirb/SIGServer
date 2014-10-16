/*
 * Modified by Okamoto on 2011-05-19
 */

#include "systemdef.h"

#if (defined USE_XERCES && defined EXEC_SIMULATION)
#include "WorldXMLReader.h"

#include "SimWorld.h"
#include "SSimWorld.h"
#include "SSimObj.h"
#include "SimObjBase.h"
#include "Parts.h"
#include "EntityXMLReader.h"
#include "Logger.h"
#include "CJNIUtil.h"
#include "FilenameDB.h"
#include "SimpleShapeDB.h"
#include "XMLUtils.h"
#include "Attribute.h"
#include "Value.h"
#include "ShapeFileReader.h"
#include "CX3DParser.h"
#include "SSimObjBuilder.h"
#include "SSimEntity.h"
#include <iostream>

#include <xercesc/sax/AttributeList.hpp>
#include <xercesc/util/XMLString.hpp>

#include <xercesc/parsers/SAXParser.hpp>
#include <xercesc/util/PlatformUtils.hpp>

#include <map>
#include <string>

XERCES_CPP_NAMESPACE_USE;

#define NS WorldXMLReaderNS
#define START_NS(NAME) namespace NAME {
#define END_NS() }

#define GET_VALUE(ATTRS, STR) XMLString::transcode(ATTRS.getValue(STR))
#define RELEASE(STR) if (STR) { XMLString::release(&STR); }

static std::string filename(std::string dirname, std::string fname)
{
	std::string f = "";
	if (dirname.length() > 0) {
		f = dirname + "/";
	}
	f += fname;
	return f;
}

#define NOATTR_ERR(NODE,ATTR, ALIST) { m_failed++; LOG_ERR(("%s : <%s> : no \"%s\" attribute\n", this->m_currfname.c_str(), NODE, ATTR)); XMLUtils::dumpAttributeList(ALIST); }
#define NOFILE_ERR(FNAME) { m_failed++; LOG_ERR(("no such file : %s", FNAME)); }

void WorldXMLReader::startElement(const XMLCh * const tagName_,
								  xercesc::AttributeList &attrs)
{
	char *tagName = XMLString::transcode(tagName_);
	// std::cout << tagName << std::endl;

	if (strcmp(tagName, "world") == 0 || strcmp(tagName, "World") == 0) {
		if (!m_world) {
			if (char *n = GET_VALUE(attrs, "name")) {
				m_world = new SSimWorld(n);
				RELEASE(n);
			} 
			else {
				NOATTR_ERR("world", "name", attrs);
			}

		}

		if (char *fname = GET_VALUE(attrs, "inherit")) {
			SAXParser *parser = new SAXParser();
			parser->setDocumentHandler(this);

			char buf[4096];
			const char *fpath = m_fdb.getPath(fname, buf);
			if (fpath != NULL) {
				S last = setFilename(fpath);
				parser->parse(fpath);
				delete parser;
				setFilename(last);
			} else {
				NOFILE_ERR(fname);
			}

			RELEASE(fname);
		}
	} else if (strcmp(tagName, "gravity") == 0 || strcmp(tagName, "Gravity") == 0) {
		if (m_world) {
			dReal x=0.0, y=0.0, z=0.0;
			char *p;
			p = GET_VALUE(attrs, "x");
			if (p) {
				x = atof(p);
				RELEASE(p);
			} else {
				NOATTR_ERR("gravity", "x", attrs);
			}
			p = GET_VALUE(attrs, "y");
			if (p) {
				y = atof(p);
				RELEASE(p);
			} else {
				NOATTR_ERR("gravity", "y", attrs);
			}
      
			p = GET_VALUE(attrs, "z");
			if (p) {
				z = atof(p);
				RELEASE(p);
			} else {
				NOATTR_ERR("gravity", "z", attrs);
			}
#if 0
			std::cout << "x = "  << x << std::endl;
			std::cout << "y = "  << y << std::endl;
			std::cout << "z = "  << z << std::endl;
#endif
			m_world->set(ODEWorld::Gravity(x, y, z), 0.0);
		}
	}
	else if (strcmp(tagName, "worldparam") == 0 || strcmp(tagName, "worldParam") == 0) {
		char *erp = GET_VALUE(attrs, "erp");
		if (erp) {
			m_world->setERP(atof(erp));
			RELEASE(erp);
		} 
		char *cfm = GET_VALUE(attrs, "cfm");
		if (cfm) {
			m_world->setCFM(atof(cfm));
			RELEASE(cfm);
		} 
		if (char *autostep = GET_VALUE(attrs, "autostep")) {
			if (strcmp(autostep, "false") == 0) {
				m_world->setAutoStep(false);
				RELEASE(autostep);
			}
		}
		if (char *quickstep = GET_VALUE(attrs, "quickstep")) {
			if (strcmp(quickstep, "true") == 0) {
				m_world->setQuickStep(true);
				RELEASE(quickstep);
			}
		}
		if (char *stepsize = GET_VALUE(attrs, "stepsize")) {
			m_world->setStepSize(atof(stepsize));
			RELEASE(stepsize);
		}
	}
	else if (strcmp(tagName, "collisionparam") == 0 || strcmp(tagName, "collisionParam") == 0) {

		char *mu = GET_VALUE(attrs, "mu");
		if (mu) {
			m_world->setCollisionParam("mu",atof(mu));
			RELEASE(mu);
		} 
		char *mu2 = GET_VALUE(attrs, "mu2");
		if (mu2) {
			m_world->setCollisionParam("mu2",atof(mu2));
			RELEASE(mu2);
		} 
		char *slip1 = GET_VALUE(attrs, "slip1");
		if (slip1) {
			m_world->setCollisionParam("slip1", atof(slip1));
			RELEASE(slip1);
		} 
		char *slip2 = GET_VALUE(attrs, "slip2");
		if (slip2) {
			m_world->setCollisionParam("slip2", atof(slip2));
			RELEASE(slip2);
		} 
		char *soft_erp = GET_VALUE(attrs, "soft_erp");
		if (soft_erp) {
			m_world->setCollisionParam("soft_erp", atof(soft_erp));
			RELEASE(soft_erp);
		} 
		char *soft_cfm = GET_VALUE(attrs, "soft_cfm");
		if (soft_cfm) {
			m_world->setCollisionParam("soft_cfm", atof(soft_cfm));
			RELEASE(soft_cfm);
		} 
		char *bounce = GET_VALUE(attrs, "bounce");
		if (bounce) {
			m_world->setCollisionParam("bounce", atof(bounce));
			RELEASE(bounce);
		} 
		char *bounce_vel = GET_VALUE(attrs, "bounce_vel");
		if (bounce_vel) {
			m_world->setCollisionParam("bounce_vel", atof(bounce_vel));
			RELEASE(bounce);
		} 


	}
	else if (strcmp(tagName, "instanciate") == 0) {
		ODEWorld *w = m_world->odeWorld();
		SSimObj *obj;

		// To check whether the type is robot or not
		char *type = GET_VALUE(attrs, "type");

		if (!type)  obj = new SSimObj(w->space());

		else if (strcmp(type,"Robot") == 0) {
			SRobotObj *robj = new SRobotObj(w->space());
			obj = (SSimObj*)robj;
			RELEASE(type);
		}
		else{
			obj = new SSimObj(w->space());
			RELEASE(type);
		}

		if (char *fname = GET_VALUE(attrs, "class")) {
			assert(m_world);
			assert(w);

			// Read contents of the entity from XML file
			// EntityXMLReader read(m_fdb, *obj, *w, m_x3ddb);
			EntityXMLReader read(m_fdb, *obj, *w, m_x3ddb, m_ssdb);

			read.setReadTaskContainer(this);
			read(fname);

		} 
		
		else {
			NOATTR_ERR("instanciate", "class", attrs);
		}
		m_currobj = obj;
	}
	else if (strcmp(tagName, "set-attr-value") == 0) {
		if (m_currobj) {
			char *n = GET_VALUE(attrs, "name");
			char *v = GET_VALUE(attrs, "value");
			if (!n) {
				NOATTR_ERR("set-attr-value", "name", attrs);
			}
			if (!v) {
				NOATTR_ERR("set-attr-value", "value", attrs);
			}
			if (n && v) {
				m_currobj->setAttrValue(n, v);
			}
		}
	}
	// Creation of new entity
	else if (strcmp(tagName, "entity") == 0 || strcmp(tagName, "Entity") == 0) {
		ODEWorld *w = m_world->odeWorld();

		// Set a new version flag later than v2.1
		m_world->setV21(true);

		// Create ODE world and space
		dWorldID world = w->world();
		dSpaceID space = w->space();

		char *entityName = GET_VALUE(attrs, "name");
		if (!entityName) {
			LOG_ERR(("Entity has no name"));
			assert(entityName); 
		}
		SSimEntity *ent;

		char *robot = GET_VALUE(attrs, "robot");
		if (robot && strcmp(robot, "true") == 0) {
			ent = new SSimRobotEntity(world, space, entityName);
			ent->setIsRobot(true);
		}
		else{
			ent = new SSimEntity(world, space, entityName);
		}

		static int eid = 0;
		ent->setID(eid);
		eid++;

		// Check whether it is agent or normal entity
		char *ag = GET_VALUE(attrs, "agent");
		if (ag && strcmp(ag, "true") == 0) {
			ent->setIsAgent(true);
		}
		m_current = ent;
	}

	else if (strcmp(tagName, "x3d") == 0 || strcmp(tagName, "X3D") == 0) {
		if (m_current != NULL) {

			char *scale = GET_VALUE(attrs, "scale");
	    
			Vector3d sc(1.0, 1.0, 1.0);
			if (scale) {
				char *scalex = strtok(scale, " ");
				char *scaley = strtok(NULL, " ");
				char *scalez = strtok(NULL, "");

				if (scalex == NULL || scaley == NULL || scalez == NULL) {
					LOG_ERR(("scale setting failed (%s)",m_current->name().c_str()));
				}
				else{
					sc.set(atof(scalex), atof(scaley), atof(scalez));
				}
			}
	    
			m_current->setScale(sc);
		}

		// Find the target XML file from current directory or SIGVERSE_DATADIR
		std::string tmp_fname = GET_VALUE(attrs, "filename");
		std::string path = getenv("SIGVERSE_DATADIR");
		std::string fname = "./" + tmp_fname;
		FILE *fp;
		if ((fp = fopen(fname.c_str(), "r")) == NULL) {
			fname = path + "/shape/" + tmp_fname;
			if ((fp = fopen(fname.c_str(), "r")) == NULL) {
				LOG_ERR(("cannot find shape file. [%s]", fname.c_str()));
				assert(fp != NULL);
			}
		}

		m_current->setShapeFile(tmp_fname);

		bool b = false;

		// Preparation of JNI
		char *cfg = getenv("SIGVERSE_X3DPARSER_CONFIG");
		if (cfg == NULL || strlen(cfg) == 0) {
			b = CJNIUtil::init("X3DParser.cfg");
		}
		else{
			b = CJNIUtil::init(cfg);
		}
	
		if (!b) {
			fprintf(stderr, "cannot read x3d config file");
			exit(1);
		}

		CX3DParser parser;
		parser.parse((char*)fname.c_str());
		//sread.read(fname.c_str());
		ODEWorld *w = m_world->odeWorld();
		//SSimObjBuilder builder(*m_currobj, *w);
		//m_current = dynamic_cast<SSimRobotEntity*>(m_current);
		//SSimRobotEntity *tmp;
		//m_current = dynamic_cast<SSimRobotEntity*>(m_current);
		//m_current = tmp;
		ShapeFileReader sread(m_current);
		LOG_SYS(("Creating object \"%s\"",m_current->name().c_str()));
		LOG_SYS(("Reading shape file [%s]", fname.c_str())); 
    
		if (m_current->isRobot()) {
			if (!sread.createRobotObj(&parser)) {
				LOG_ERR(("Failed to read robot shape file [%s]", fname.c_str())); 
			}
		}
		else{
			if (!sread.createObj(&parser)) {
				LOG_ERR(("Failed to read shape file [%s]", fname.c_str())); 
			}
		}

	} //   else if (strcmp(tagName, "x3d") == 0 || strcmp(tagName, "X3D") == 0) {
	else if (strcmp(tagName, "attribute") == 0 || strcmp(tagName, "Attribute") == 0) {
		if (m_current) {
			char *position  = GET_VALUE(attrs, "position");	 // entity position
			char *direction = GET_VALUE(attrs, "direction");	 // entity direction
			char *mass      = GET_VALUE(attrs, "mass");	 // entity mass
			char *collision = GET_VALUE(attrs, "collision");	 // collision detection flag
			char *quaternion= GET_VALUE(attrs, "quaternion");	 // quaternion
      
			if (position) {
				char *x = strtok(position, " ");
				char *y = strtok(NULL, " ");
				char *z = strtok(NULL, "");
				Vector3d pos(atof(x), atof(y), atof(z));
				if (m_current->isRobot()) {
					SSimRobotEntity *tmp_ent = (SSimRobotEntity*)m_current;
					tmp_ent->setInitPosition(pos);
				}
				else{
					m_current->setInitPosition(pos);
				}
			}
			//[ToDo]
			if (direction) {
			}
			//[ToDo]
			if (mass) {
				m_current->setMass(atof(mass));
			}
			if (collision) {
				if (strcmp(collision, "true") == 0) {
					if (m_current->isRobot()) {
						SSimRobotEntity *tmp_ent = (SSimRobotEntity*)m_current;
						tmp_ent->setCollision(true);
					}
					else{
						m_current->setCollision(true);
					}
				}
			}
			//[Todo]
			if (quaternion) {
			}
		}
	}
  
	// Added by okamoto on 2012-08-11
	// Reading and setting of camera parameter
	else if (strcmp(tagName, "camera") == 0 || strcmp(tagName, "Camera") == 0) {
		if (m_currobj) {
			char *cid  = GET_VALUE(attrs, "id");          // id number 
			char *link = GET_VALUE(attrs, "link");        // link name 
			char *fov  = GET_VALUE(attrs, "fov");	      // field of view
			char *as   = GET_VALUE(attrs, "aspectRatio"); // aspect ratio
			int iid    = -1;

			std::string id = cid;
//			double dfov, das;
			// Whether value is specified by users
			bool isid   = false;
			bool islink = false;
			bool isfov  = false;
			bool isas   = false;

			if (link) islink = true;
			if (fov)  isfov  = true;
			if (as)   isas   = true;

			if (!cid) {
				LOG_ERR(("Cannot find camera ID."));
			}
			else {
				isid = true;
				iid = atoi(cid);
			}
			// Add camera ID
			m_currobj->addCameraID(iid);

			// Setting of camera parameters
			Value *vfov  = new DoubleValue();
			Value *vas   = new DoubleValue();
			Value *vlink = new StringValue();

			// Setting of each attributions
			std::string sfov  = "FOV"         + id;
			std::string sas   = "aspectRatio" + id;
			std::string slink = "elnk"        + id;
			vfov ->setString(sfov. c_str());
			vas  ->setString(sas.  c_str());	    
			vlink->setString(slink.c_str());	    

			// Add attribution info to entity
			m_currobj->push(new Attribute(sfov, vfov, "camera"));
			m_currobj->push(new Attribute(sas,  vas,  "camera"));
			if (iid > 2)
				m_currobj->push(new Attribute(slink, vlink, "camera"));

			if (isfov)
				m_currobj->setAttrValue(sfov.c_str(), fov);
			else 
				m_currobj->setAttrValue(sfov.c_str(), "45.0"); // default value

			if (isas)
				m_currobj->setAttrValue(sas.c_str(), as);
			else
				m_currobj->setAttrValue(sas.c_str(), "1.5");   // default value

			if (islink)
				m_currobj->setAttrValue(slink.c_str(), link);
			else
				m_currobj->setAttrValue(slink.c_str(), "body");// default value

			char *position = GET_VALUE(attrs, "position");
	    
			std::string epx = "epx" + id;
			std::string epy = "epy" + id;
			std::string epz = "epz" + id;
	    
			if (position) {
				std::string x = strtok(position, " ");
				std::string y = strtok(NULL, " ");
				std::string z = strtok(NULL, "");
				Vector3d pos(atof(x.c_str()), atof(y.c_str()), atof(z.c_str()));
	      
				if (iid > 2) {
					Value *v_x = new DoubleValue();
					Value *v_y = new DoubleValue();
					Value *v_z = new DoubleValue();

					v_x->setString(epx.c_str());
					v_y->setString(epy.c_str());
					v_z->setString(epz.c_str());

					// [Comment] Is it OK to execute new here?
					m_currobj->push(new Attribute(epx, v_x, "camera"));
					m_currobj->push(new Attribute(epy, v_y, "camera"));
					m_currobj->push(new Attribute(epz, v_z, "camera"));

				}
				m_currobj->setAttrValue(epx.c_str(), x.c_str());
				m_currobj->setAttrValue(epy.c_str(), y.c_str());
				m_currobj->setAttrValue(epz.c_str(), z.c_str());
				RELEASE(position);
			}

			else {
				// Default values
				m_currobj->setAttrValue(epx.c_str(), "0.0");
				m_currobj->setAttrValue(epy.c_str(), "0.0");
				m_currobj->setAttrValue(epz.c_str(), "0.0");
			}

			// camera direction
			char *direction  = GET_VALUE(attrs, "direction");
			char *quaternion = GET_VALUE(attrs, "quaternion");   // aspect ratio

			std::string evx = "evx" + id;
			std::string evy = "evy" + id;
			std::string evz = "evz" + id;

			std::string quw = "quw" + id;
			std::string qux = "qux" + id;
			std::string quy = "quy" + id;
			std::string quz = "quz" + id;

			if (direction && quaternion) {
				LOG_MSG(("cannot set camera quaternion and direction simultaneously")); 
			}

			if (direction) {
				std::string vx = strtok(direction, " ");
				std::string vy = strtok(NULL, " ");
				std::string vz = strtok(NULL, "");
				Vector3d dir(atof(vx.c_str()), atof(vy.c_str()), atof(vz.c_str()));
	
				if (iid > 2) {
					Value *v_x = new DoubleValue();
					Value *v_y = new DoubleValue();
					Value *v_z = new DoubleValue();

					v_x->setString(evx.c_str());
					v_y->setString(evy.c_str());
					v_z->setString(evz.c_str());
					// [Comment] Is it OK to execute new here?
					m_currobj->push(new Attribute(evx, v_x, "camera"));
					m_currobj->push(new Attribute(evy, v_y, "camera"));
					m_currobj->push(new Attribute(evz, v_z, "camera"));

				}
				m_currobj->setAttrValue(evx.c_str(), vx.c_str());
				m_currobj->setAttrValue(evy.c_str(), vy.c_str());
				m_currobj->setAttrValue(evz.c_str(), vz.c_str());	      
				RELEASE(direction);
			}
			else {
				Vector3d dir(0.0, 0.0, 1.0);
				m_currobj->setAttrValue(evx.c_str(), "0.0");
				m_currobj->setAttrValue(evy.c_str(), "0.0");
				m_currobj->setAttrValue(evz.c_str(), "1.0");	      
			}

			Value *q_w = new DoubleValue();
			Value *q_x = new DoubleValue();
			Value *q_y = new DoubleValue();
			Value *q_z = new DoubleValue();
      
			q_w->setString(quw.c_str());
			q_x->setString(qux.c_str());
			q_y->setString(quy.c_str());
			q_z->setString(quz.c_str());
      
			m_currobj->push(new Attribute(quw, q_w, "camera"));
			m_currobj->push(new Attribute(qux, q_x, "camera"));
			m_currobj->push(new Attribute(quy, q_y, "camera"));
			m_currobj->push(new Attribute(quz, q_z, "camera"));
      
			if (quaternion) {
				std::string qw = strtok(quaternion, " ");
				std::string qx = strtok(NULL, " ");
				std::string qy = strtok(NULL, " ");
				std::string qz = strtok(NULL, "");

				m_currobj->setAttrValue(quw.c_str(), qw.c_str());
				m_currobj->setAttrValue(qux.c_str(), qx.c_str());
				m_currobj->setAttrValue(quy.c_str(), qy.c_str());
				m_currobj->setAttrValue(quz.c_str(), qz.c_str());	      
				RELEASE(quaternion);
			}
			else {
				m_currobj->setAttrValue(quw.c_str(), "1.0");
				m_currobj->setAttrValue(qux.c_str(), "0.0");
				m_currobj->setAttrValue(quy.c_str(), "0.0");
				m_currobj->setAttrValue(quz.c_str(), "0.0");
			}
			//RELEASE(id.c_str());
			if (isid)   RELEASE(cid);
			if (islink) RELEASE(link);
			if (isfov)  RELEASE(fov);
			if (isas)   RELEASE(as);
		} // if (m_currobj)
	}
	RELEASE(tagName);
}


void WorldXMLReader::endElement(const XMLCh * const tagName_)
{
	char *tagName = XMLString::transcode(tagName_);
	if (strcmp(tagName, "instanciate") == 0) {
		if (m_currobj) {
			// There is no required attribution
			if (!m_currobj->checkAttrs()) {
				m_failed++;
			}	  
			else {
				m_currobj->addId();
				m_world->push(m_currobj);
				// Record the name of entity which is alread read
				m_objNames.push_back(m_currobj->name());	// FIX20110421(ExpSS)
			}
		}
		else{

		}
		m_currobj = 0;
	}
	else if (strcmp(tagName, "entity") == 0 ||
			 strcmp(tagName, "Entity") == 0) {

		m_world->addSSimEntity(m_current->name(),m_current);
		m_current = 0;
	}

	RELEASE(tagName);
}
#if 1
// FIX20110421(ExpSS)
WorldXMLReader::WorldXMLReader(FilenameDB &fdb, X3DDB &x3ddb, SimpleShapeDB &ssdb)
	: m_fdb(fdb), m_x3ddb(x3ddb), m_ssdb(ssdb), m_world(NULL), m_currobj(NULL), m_failed(0), m_current(NULL)
#else
	  WorldXMLReader::WorldXMLReader(FilenameDB &fdb, X3DDB &x3ddb)
	  : m_fdb(fdb), m_x3ddb(x3ddb), m_world(NULL), m_currobj(NULL), m_failed(0), m_current(NULL)
#endif
{
}

#define FREE(P) if (P) { delete P; P = NULL; }
 
WorldXMLReader::~WorldXMLReader()
{
	FREE(m_world);
}


bool WorldXMLReader::read(const char *fname)
{
	int c;
	// Initialization of counter
	Parts::initCounter();
	SSimObj::initCounter();

	// Initialization of XML library
	XMLPlatformUtils::Initialize();

	// Clear the entity names which are already read
	m_objNames.clear();

	SAXParser *parser = new SAXParser();
	parser->setDocumentHandler(this);

	bool ret = false;
	try {
		char buf[4096];
		const char *fpath = m_fdb.getPath(fname, buf);
		if (fpath != NULL) {
			setFilename(fpath);

			parser->parse(fpath);
		} else {
			NOFILE_ERR(fname);
		}

		if (m_failed > 0) {
			goto err;
		}
	} catch(const XMLException &e) {
		char *msg = XMLString::transcode(e.getMessage());
		LOG_ERR(("parse error :%s", fname));
		LOG_ERR(("%s", msg));
		XMLString::release(&msg);
		goto err;
	}

#if 1
	// Execute after the load of attribution of SimObj
	// Loading of X3D file is executed here
	c = 0;
	for (TaskC::iterator i=m_tasks.begin(); i!=m_tasks.end(); i++)
		{
			std::vector<ReadTask *> taskCol = i->second;
			std::vector<ReadTask *>::iterator j;
			for (j=taskCol.begin(); j!=taskCol.end(); j++)
				{
					ReadTask *t = *j;
					if (t)
						{
							//				printf("[%d] calling X3DReadTask ... \n", c++);
							t->execute();
							// Process of the execution
							// --> EntityXMLReader.cpp: X3DReadTask::execute()
							// ----> X3DFileReader::read()
							// ------> X3DSimObjCreator::createSSimObjFromX3D()
							// The last line actually loads X3D file
						}

					delete t;
				}
		}
	m_tasks.clear();	
#else
	// orig
	for (TaskC::iterator i=m_tasks.begin(); i!=m_tasks.end(); i++) {
		ReadTask *t = *i;
		t->execute();
		delete t;
	}
#endif
	m_tasks.clear();

	//	printf("\n***** world dump (2) [%s:%d] *****\n", __FILE__, __LINE__);
	//	m_world->dump();

	if (m_world) {
		m_world->setup();
	} else {
		LOG_ERR(("%s : no world", fname));
		goto err;
	}

	ret = true;
 err:
	delete parser;
	//CJNIUtil::destroy();

	return ret;
}

#endif // USE_XERCES && EXEC_SIMULATION

