/* $Id: ThreadWin.h,v 1.2 2011-03-31 08:15:57 okamoto Exp $ */
#ifndef ThreadWin_h
#define ThreadWin_h

#undef USE_AFX_THREAD	// do not use AFX (MFC) thread functions

#ifdef USE_AFX_THREAD
#include <afxwin.h>
#else
#include <windows.h>
#include <process.h>
#endif // USE_AFX_THREAD

////////////////////////////////////////////////////////////////////////////////

template<class ParamType>
class Thread
{
public:
	typedef UINT RetType;
	typedef RetType (*ThreadFuncType)(LPVOID);

protected:
#ifdef USE_AFX_THREAD
	CWinThread *	m_thread;
#else
	HANDLE	m_thread;
#endif // USE_AFX_THREAD

#if 1
	unsigned int m_threadId;
#else
	// added by sekikawa(2009/02/09)
	DWORD	m_threadId;
#endif

private:
	void	freeThread()
	{
#ifdef USE_AFX_THREAD
		if (m_thread) { delete m_thread; m_thread = 0; }
#else
		m_thread = 0;
#endif // USE_AFX_THREAD
	}
	void	free_() { freeThread(); }

public:
	Thread() : m_thread(0) {;}
	virtual ~Thread() { terminate(); free_(); }

	bool	run(ThreadFuncType proc, ParamType param);
	void	wait();
	bool	running();
	void	terminate();

#if 1
	unsigned int getThreadId() { return m_threadId; }
#else
	// added by sekikawa(2009/02/09)
	DWORD	getThreadId() { return m_threadId; }
#endif
};

////////////////////////////////////////////////////////////////////////////////

#ifdef USE_AFX_THREAD
template<class ParamType>
bool	Thread<ParamType>::run(ThreadFuncType proc, ParamType param)
{
	freeThread();
	LPVOID lpParam = reinterpret_cast<LPVOID>(param);
	//		m_thread = AfxBeginThread(proc, lpParam);

	m_thread = AfxBeginThread(proc, lpParam, 0, 0, CREATE_SUSPENDED, 0);
	if (!m_thread) { return false; }

	m_thread->m_bAutoDelete = FALSE;
	ResumeThread(m_thread->m_hThread); 

	return true;
}
#else
typedef unsigned int (_stdcall *BEGINTHREADEXPROC)(void *);
template<class ParamType>
bool	Thread<ParamType>::run(ThreadFuncType proc, ParamType param)
{
	freeThread();
	LPVOID lpParam = reinterpret_cast<LPVOID>(param);


#if 1
	m_thread = (HANDLE)_beginthreadex(NULL, 0, (BEGINTHREADEXPROC)proc, lpParam, CREATE_SUSPENDED, &m_threadId);
	if (!m_thread) { return false; }
#else
	m_thread = (HANDLE)_beginthreadex(NULL, 0, (BEGINTHREADEXPROC)proc, lpParam, CREATE_SUSPENDED, NULL);
	if (!m_thread) { return false; }

	// begin(added by sekikawa)(2009/02/09)
	m_threadId = GetThreadId(m_thread);
	// end(add)
#endif

	ResumeThread(m_thread); 

	return true;
}
#endif // USE_AFX_THREAD

template<class ParamType>
void	Thread<ParamType>::wait()
{
	if (!m_thread) { return; }
#ifdef USE_AFX_THREAD
	WaitForSingleObject(m_thread->m_hThread, INFINITE);
#else
	WaitForSingleObject(m_thread, INFINITE);
#endif // USE_AFX_THREAD

	freeThread();
}

template<class ParamType>
bool Thread<ParamType>::running()
{
	if (!m_thread) { return false; }
	DWORD status;

#ifdef USE_AFX_THREAD
	GetExitCodeThread(m_thread->m_hThread, &status);
#else
	GetExitCodeThread(m_thread, &status);
#endif // USE_AFX_THREAD

	if (status != STILL_ACTIVE) {
		freeThread();
		return false;
	}

	return true;
}

template<class ParamType>
void Thread<ParamType>::terminate()
{
	if (m_thread) {
#ifdef USE_AFX_THREAD
		if (running()) { TerminateThread(m_thread->m_hThread, 1); }
#else
		if (running()) { TerminateThread(m_thread, 1); }
#endif // USE_AFX_THREAD

		freeThread();
	}
}

#endif // ThreadWin_h

