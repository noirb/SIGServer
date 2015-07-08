/*
 * Created by okamoto on 2011-03-25
 */

#ifndef SimWorldProvider_h
#define SimWorldProvider_h

class SSimWorld;
class X3DDB;

class SimWorldProvider
{
public:
	virtual ~SimWorldProvider() {;}
	virtual SSimWorld *get() = 0;
	virtual SSimWorld *create() = 0;
	virtual X3DDB & getX3DDB() = 0;
	virtual double calcDistance(const char *name1, const char *name2) = 0;
};


#endif // SimWorldProvider_h
