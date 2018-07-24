#include "jw_pthread.h"

#include <time.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "config.h"

void *start(void *args){

  printf("hello world");
  
}




int main(){


  char listen_addr[100];
  int listen_port;
  memset(listen_addr,0,sizeof(listen_addr));
  
  load_config("config.lua");
  get_Value_string("ipaddr",listen_addr);
  get_Value_int("port",&listen_port);
  char *p=filter_http(L,"https://www.baidu.com/get?key1=fsdfa&&key2=432qfds42q3");
  print(" main %s\n",p)
  free(p);
  
  close_config();

  
  //start_socket_server();


  printf("%s %d",listen_addr,listen_port);
  //pthread_t pid;
  //create_pthread_detach(&pid,start,NULL);
  //usleep(1000*1000);
  
  return 0;
}
