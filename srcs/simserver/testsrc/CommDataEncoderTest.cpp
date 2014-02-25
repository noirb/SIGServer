#include "WorldXMLReader.h"
#include "SimWorld.h"
#include "CommDataEncoder.h"

#include <stdio.h>

int main()
{
	const char *worldfile = "MyWorld.xml";
	WorldXMLReader read;
	if (!read(worldfile)) {
		fprintf(stderr,"error\n");
		return 1;
	}
	
	SimWorld * w = read.release();

	CommResultGetAllEntitiesEncoder enc(*w);
	for (int i=0; i<enc.packetNum(); i++) {
		int n;
		char *p = enc.encode(i, n);
		if (p) {
			fwrite(p, 1, n, stdout);
		}
	}
	delete w;

}
