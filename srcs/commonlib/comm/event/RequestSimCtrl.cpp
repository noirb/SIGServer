/* $Id: RequestSimCtrl.cpp,v 1.1.1.1 2011-03-25 02:18:50 okamoto Exp $ */ 
#include "RequestSimCtrl.h"
#include "binary.h"
#include "CommDataDef.h"
#include "Header.h"

bool RequestSimCtrlEvent::set(int packetNum, int seq, char *data, int n)
{
	const int DATA_SIZE = sizeof(SimCtrlCmdType) + COMM_DATA_FOOTER_SIZE;
	if (n < DATA_SIZE) { return false; }
	char *p = data;
	m_cmd = BINARY_GET_DATA_S_INCR(p, SimCtrlCmdType);
	
	return CommData::isPacketEnd(p)? true: false;
}

