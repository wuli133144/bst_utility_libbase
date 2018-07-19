#ifndef  JW___TOOLS_PTHREAD_CREATE_DETACH_H
#define  JW___TOOLS_PTHREAD_CREATE_DETACH_H

#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>


void create_pthread_detach(pthread_t *pid,void *(*start_routine)(void *),void *arg);


#endif 


