/*
 * Modified by MSI on 2011-05-19
 */

#ifndef ReadTask_h
#define ReadTask_h

class ReadTask
{
public:
	virtual ~ReadTask() {}
	virtual void execute() = 0;
	virtual const char *getKey() = 0;
};

class ReadTaskContainer
{
public:
	virtual ~ReadTaskContainer() {}
	virtual void pushTask(ReadTask *) = 0;
	virtual std::vector<ReadTask *>& getTaskCol(const char *key) = 0;
};

#endif // ReadTask_h


