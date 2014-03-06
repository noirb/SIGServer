/*
 * Modified by okamoto on 2011-03-31
 */

#ifndef ThreadPthread_h 
#define ThreadPthread_h

#include <pthread.h>
#include <assert.h>
#include <unistd.h>

template<class ParamType>
class Thread
{
public:
	typedef void * RetType;
	typedef RetType (*ThreadFuncType)(void *);
private:
	pthread_t *m_thread;
private:
	void	free_()
	{
		if (m_thread) { delete m_thread; m_thread = 0 ; }
	}
public:
	Thread() : m_thread(0) {;}
	virtual ~Thread() { terminate(); free_(); }

	bool	run(ThreadFuncType proc, ParamType param)
	{
		assert(!m_thread);
		m_thread = new pthread_t;
		if (pthread_create(m_thread, 0, proc, param) != 0) {
			free_();
			return false;
		}
		sleep(1); 
		return true;
	}

	void	terminate()
	{
		if (!m_thread) { return; }
		pthread_cancel(*m_thread);
		free_();
	}
	void	wait()
	{
		if (!m_thread) { return; }
		pthread_join(*m_thread, 0);
	}
};

#endif // ThreadPthread_h

