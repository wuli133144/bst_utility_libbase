#include "jw_pthread.h"


void create_pthread_detach(pthread_t *pid,void *(*start_routine)(void *),void *arg)
{
      pthread_attr_t attr;

	  int nRet=pthread_attr_init(&attr);
	  if(nRet<0){
	  	 goto RESET_PTHREAD_ATTR;
	  }
	  
	  nRet=pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
	  if(nRet<0){
	  	 goto RESET_PTHREAD_ATTR;
	  }

	  pthread_create(pid,&attr,start_routine,arg);

	  nRet=pthread_detach(pid);
	  if(nRet < 0){
	  	 goto  RESET_PTHREAD_ATTR;
	  }
     
	  
	  RESET_PTHREAD_ATTR:
	       pthread_attr_destroy(&attr);
}



