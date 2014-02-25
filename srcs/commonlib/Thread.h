/* $Id: Thread.h,v 1.2 2011-03-31 08:15:57 okamoto Exp $ */ 
#ifndef Thread_h
#define Thread_h

#ifdef WIN32

#include "ThreadWin.h"

#else

#include "ThreadPthread.h"

#endif



#endif // Thread_h
 
