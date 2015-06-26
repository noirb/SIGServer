/*
 *
 *
 */

#ifndef _PTHREAD_DEP_H_
#define _PTHREAD_DEP_H_

#ifndef WIN32
#include <pthread.h>
#else
#include <Windows.h>
#include <process.h>

typedef HANDLE pthread_t;
typedef HANDLE pthread_mutex_t;

void pthread_mutex_init(pthread_mutex_t *hM, void *arg);
int pthread_create(pthread_t *hT, void *attr,void (*func)(void *), void *arg);
//void pthread_detach(pthread_t hT);
void pthread_mutex_lock(pthread_mutex_t *hM);
void pthread_mutex_unlock(pthread_mutex_t *hM);
void pthread_mutex_destroy(pthread_mutex_t *hM);
//void pthread_join(pthread_t hT, void *thread_return);
//void pthread_exit(pthread_t hT);
#endif

#endif