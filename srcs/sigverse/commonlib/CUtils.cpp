/*
POSIX getopt for Windows

AT&T Public License

Code given out at the 1985 UNIFORUM conference in Dallas.  
*/

#ifdef WIN32
#include <sigverse/simserver/fname.h>
#include <sigverse/simserver/jjudge.h>
#include <sigverse/commonlib/wingetopt.h>

#include <Windows.h>
#include <process.h>

typedef HANDLE pthread_t;
typedef HANDLE pthread_mutex_t;

void pthread_mutex_init(pthread_mutex_t *hM, void *arg)
{
	*hM = CreateMutex(NULL, FALSE, NULL);
}

int pthread_create(pthread_t *hT, void *attr,void (*func)(void *), void *arg)
{
	uintptr_t tp;
	tp = _beginthread(func, NULL, arg);
	if(tp == -1L){
		*hT = (HANDLE)NULL;
		return -1;
	}
	*hT = (HANDLE)tp;
	return 0;
}

void pthread_mutex_lock(pthread_mutex_t *hM)
{
	WaitForSingleObject(*hM, INFINITE);
}
void pthread_mutex_unlock(pthread_mutex_t *hM)
{
	ReleaseMutex(*hM);
}
void pthread_mutex_destroy(pthread_mutex_t *hM)
{
	CloseHandle(*hM);
}

#endif
