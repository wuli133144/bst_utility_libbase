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


int add(lua_State *l,int x,int y){
       int num=lua_gettop(l);
	   printf("stack size=%d",num);
	   lua_getglobal(l,"add");
	   printf("stack size=%d",lua_gettop(l);//1
	   lua_pushnumber(l,x);
	   lua_pushnumber(l,y);
	   printf("stack size=%d",lua_gettop(l);//3
	   lua_pcall(l,2,1,0);
	   printf("stack size=%d",lua_gettop(l);//1
	   int sum=lua_tonumber(l,1);
	   //int a=lua_tonumber(l,-2);
	   //int b=lua_tonumber(l,-3);
	   
	   printf("stack size=%d sum=%d ",lua_gettop(l),sum);	
	   return sum;
}







int main(){


  char listen_addr[100];
  int listen_port;
  memset(listen_addr,0,sizeof(listen_addr));
  
  load_config("config.lua");
  get_Value_string("ipaddr",listen_addr);
  get_Value_int("port",&listen_port);
  close_config();

  start_socket_server();


  printf("%s %d",listen_addr,listen_port);
  //pthread_t pid;
  //create_pthread_detach(&pid,start,NULL);
  //usleep(1000*1000);
  
  return 0;
}
