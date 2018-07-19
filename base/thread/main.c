#include "jw_pthread.h"
#include <time.h>


void *start(void *args){

  printf("hello world");
 
}


int main(){

  pthread_t pid;
  create_pthread_detach(&pid,start,NULL);
  usleep(1000*1000);
  
  return 0;
}
