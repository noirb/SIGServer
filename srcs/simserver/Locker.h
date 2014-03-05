/*
 * Created by okamoto on 2011-03-25
 */

#ifndef Locker_h
#define Locker_h

#include <pthread.h>


class Locker
{
private:
	pthread_mutex_t m_mutex;
public:
	Locker()      { pthread_mutex_init(&m_mutex, 0); }
	~Locker()     { pthread_mutex_destroy(&m_mutex); }
	void lock()   { pthread_mutex_lock(&m_mutex);    }
	void unlock() { pthread_mutex_unlock(&m_mutex);  }
			
};

#endif // Locker_h
 

