#ifndef __SUNLANDS_TOOLS_CONFIF__H
#define __SUNLANDS_TOOLS_CONFIF__H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"



int     load_config(const char *filename);
void    get_Value_int(const char *keys,int *value);
void    get_Value_string(const char *keys,char *values);

void    close_config();



#endif 