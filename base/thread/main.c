#include "jw_pthread.h"
#include "play_socket.h"
#include <time.h>

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

void *start(void *args){

  printf("hello world");
 
}


int main(){


  char listen_addr[100];
  int listen_port;
  memset(listen_addr,0,sizeof(listen_addr));
  
  lua_State * lstate=luaL_newstate();
  luaL_openlibs(lstate);
  if(luaL_loadfile(lstate, "config.lua"))
  {
    printf("load config files failed!");  
	exit(-1);
  }
  
  lua_pcall(lstate, 0,0,0,0);
  
  lua_getglobal(lstate,"ipaddr");
  lua_getglobal(lstate,"port");
  
  if(lua_isstring(lstate,-2) && lua_isnumber(lstate,-1)){
       strncpy(listen_addr,lua_tostring(lstate,-2),sizeof(listen_addr));
	   //strncpy(listen_port,lua_tonumber(lstate,-1),sizeof(listen_port));
	   listen_port=lua_tonumber(lstate,-1);
  }
  lua_close(lstate);


  //printf("%s %d",listen_addr,listen_port);
  //pthread_t pid;
  //create_pthread_detach(&pid,start,NULL);
  //usleep(1000*1000);
  
  return 0;
}
