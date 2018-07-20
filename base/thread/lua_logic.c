#include "lua_logic.h"



extern lua_State *L;


char * filter_http(lua_State *l, const char * http){
	 int capacity=lua_gettop(l);
	 lua_getglobal(l,"http_filter");
	 if (lua_isfunction(l,-1))
	 {
	      lua_pushstring(l,http);
	 }
	 lua_pcall(l,1,1,0);
	 int length=strlen(lua_tostring(l,-1));
	 const char *ptemp=(char *)malloc(length*sizeof(char));
	 strncpy(ptemp,lua_tostring(l,-1),length);
	 lua_pop(l,-1);
	 return ptemp;
}



bool isvalid_request_param(lua_State *l, const char * httpreq)
{
	lua_getglobal(l,"http_param_request");
	if (lua_isfunction(l,-1))
	{
	  lua_pushstring(l,httpreq);
	}
	
	lua_pcall(l,1,1,0);
    bool bRet=lua_toboolean(L,-1);
	lua_pop(l,-1);
	return bRet;
	
}


