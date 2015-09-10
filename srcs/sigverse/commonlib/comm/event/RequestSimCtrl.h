/* $Id: RequestSimCtrl.h,v 1.1.1.1 2011-03-25 02:18:50 okamoto Exp $ */ 
#ifndef RequestSimCtrl_h
#define RequestSimCtrl_h

#include <sigverse/commonlib/CommDataType.h>

class RequestSimCtrlEvent
{
private:
	SimCtrlCmdType m_cmd;
public:
	bool set(int packetNum, int seq, char *data, int n);
public:
	SimCtrlCmdType getCtrlCmd() { return m_cmd; }
};

#endif // RequestSimCtrl_h
 

