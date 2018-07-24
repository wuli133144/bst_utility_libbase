
#ifndef  __TOOLS___LUA_ACTIVE_H
#define  __TOOLS___LUA_ACTIVE_H

#include <stdbool.h>
#include "config.h"



char * filter_http(lua_State *l, const char * http);
bool isvalid_request_param(lua_State *l,const char * httpreq);













#endif 




