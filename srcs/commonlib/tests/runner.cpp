/*
 * Written by okamoto on 2011-03-25
 */

#include <dlfcn.h>
#include <cppunit/TestRunner.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>

int main(int argc, char **argv)
{
	/*
	if (argc >= 2) {
		fprintf(stderr, "no test suite library\n");
		return 1;
	}
	const char *fname = argv[1];
	void *h = dlopen(fname, RTLD_NOW|RTLD_GLOBAL);
	if (!h) {
		fprintf(stderr, "%s : cannot load test unit library\n", fname);
		return 1;
	}
	*/
	
	CppUnit::TestResult result;
	CppUnit::TestResultCollector c;
	result.addListener(&c);

	CppUnit::BriefTestProgressListener progress;
	result.addListener(&progress);

	CppUnit::TestRunner r;
	
	//r.addTest(RecvEntitiesTest::suite());
	r.addTest(CppUnit::TestFactoryRegistry::getRegistry().makeTest());
	r.run(result);

	CppUnit::CompilerOutputter o(&c, CppUnit::stdCOut());
	o.write();

	return 0;
}
