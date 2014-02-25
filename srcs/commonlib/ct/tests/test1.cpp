// $Id: test1.cpp,v 1.1.1.1 2011-03-25 02:18:50 okamoto Exp $
#include "ct/CTSimObj.h"
#include "CommDataEncoder.h"
#include "CommUtil.h"
#include "Logger.h"
#include <stdio.h>

#define ERR(MSG) fprintf(stderr, "%s(%d) : %s\n", __FILE__, __LINE__, MSG)

int main()
{

	LOG_STDOUT();
	LOG_OUTPUT_LEVEL(LOG_ALL);

	
	CommDataEncoder::setSendProc(CommUtil::sendData);

	CTSimObj obj;
	if (!obj.connect("localhost", 6501, "Robot1")) {
		ERR("cannot connect to server\n");
		return 1;
	}

#if 0
	obj.sendText(0.0, NULL, "Hello");
#else
	ViewImage *img = obj.captureView(COLORBIT_ANY, IMAGE_320X240);
	if (img) {
		printf("%d byte received\n", img->getBufferLength());
		delete img;
	}
#endif

	return 0;
}
