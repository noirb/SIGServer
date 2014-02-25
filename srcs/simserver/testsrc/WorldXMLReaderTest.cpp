/* $Id: WorldXMLReaderTest.cpp,v 1.1.1.1 2011-03-25 02:18:51 okamoto Exp $ */
//#ifdef IRWAS_SIMSERVER
#if 1

#include "WorldXMLReader.h"
#include "SimWorld.h"

#include "CommDataDecoder.h"

#include "CommDataEncoder.h"
int main()
{
	const char *fname = "MyWorld.xml";

	WorldXMLReader read("./conf");
	read(fname);
	SimWorld *w = read.release();
	if (w) {
		w->dump();
	}
#if 0
	CommResultGetAllEntitiesEncoder enc(*w);
	int n;
	char *data = enc.encode(0, n);
	if (data) {
		printf("datasize = %d\n", n);
		CommDataHeader h;
		char *p = data;
		int r = CommDataDecoder::getHeader(data, n, h);
		if (r > 0) {
			p += r;
			CommResultGetAllEntitiesDecoder decode;
			decode.set(h, p, n-r);
			printf("decoded\n");
			decode.dump();
		}
	}
#endif
	delete w;
	return 0;
}

#elif IRWAS_CLIENT

#include "CommDataDecoder.h"

int main()
{
	CommResultGetAllEntitiesEncoder enc(*w);
	int n;
	char *data = enc.encode(0, n);
	if (data) {
		printf("datasize = %d\n", n);
		CommDataHeader h;
		char *p = data;
		int r = CommDataDecoder::getHeader(data, n, h);
		if (r > 0) {
			p += r;
			CommResultGetAllEntitiesDecoder decode;
			decode.set(h, p, n-r);
			printf("decoded\n");
			decode.dump();
		}
	}
	return 0;

}
#endif

