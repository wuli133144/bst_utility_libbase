#include "config.h"


lua_State *L=NULL;

int load_config(const char * filename)
{
   L=luaL_newstate();
   if(NULL==L)
   	  goto ERROR;
   
   luaL_openlibs(L);
   if(luaL_loadfile(L,filename) || lua_pcall(L,0,0,0)){
      goto ERROR;
   }
   return 0;
 ERROR:
   lua_close(L);
   return -1; 
   
}


void close_config(){
      lua_close(L);
}

void  get_Value_int(const char * keys,int *value)
{          
	int nRet=-1;

	lua_getglobal(L,keys);
	if(lua_isnumber(L,-1))
	{
	  nRet=lua_tointeger(L, -1);
	  lua_pop(L,-1);
	  
	}	 
	(*value)=nRet;	
	
	
}

void  get_Value_string(const char * keys,char *values)
{
     
    char *ptem=NULL;
	lua_getglobal(L,keys);
	if(lua_isstring(L,-1))
	{
	  ptem=lua_tostring(L, -1);
	  lua_pop(L,-1);
	  
	}	 
	strncpy(values,ptem,strlen(ptem));	
}