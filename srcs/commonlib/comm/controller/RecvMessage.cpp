/* $Id: RecvMessage.cpp,v 1.1.1.1 2011-03-25 02:18:50 okamoto Exp $ */
#include "RecvMessage.h"
#include "binary.h"
#include <stdlib.h>
#include <string.h>

bool RecvMessageEvent::set(int packetNum, int seq, char *data, int len)
{
	if (seq != 0 || len < sizeof(unsigned int)) { return false; }
	char *p = data;
	char *from = BINARY_GET_STRING_INCR(p);
	if (from) {
		m_from = from;
	}
	BINARY_FREE_STRING(from);
	
	unsigned int n = BINARY_GET_DATA_L_INCR(p, unsigned int);
	for (unsigned i=0; i<n; i++) {
		char *str = BINARY_GET_STRING_INCR(p);
		if (!str) { return false; }
		m_strs.push_back(S(str));
		BINARY_FREE_STRING(str);
	}
	return true;
}


bool RecvMsgEvent::setData(std::string data, int size)
{
  int strPos1 = 0;
  int strPos2;
  std::string tmpss;

  strPos2 = data.find(",", strPos1);
  m_from.assign(data, strPos1, strPos2-strPos1);

  strPos1 = strPos2 + 1;
  
  strPos2 = data.find(",", strPos1);
  tmpss.assign(data, strPos1, strPos2-strPos1);
  int msgSize = atoi(tmpss.c_str());

  strPos1 = strPos2 + 1;
  m_msg.assign(data, strPos1, msgSize);
  
  /*
  m_from = strtok(data,",");
  m_msg = strtok(NULL,",");
  */
  return true;
}

