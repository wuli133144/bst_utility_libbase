/*
 * CPPJIEBA 
 * modified by qinjian@sogou-inc.com
 */
#include <assert.h>
#include "Config.h"

using namespace ba;

bool Config::get(const string& key, string& value) const
{
	map<string, string>::const_iterator it = _map.find(key);
	if(_map.end() != it)
	{
		value = it->second;
		return true;
	}
	return false;
}

const char* Config::operator [] (const char* key) const
{
	if(NULL == key)
	{
		return NULL;
	}
	map<string, string>::const_iterator it = _map.find(key);
	if(_map.end() != it)
	{
		return it->second.c_str();
	}
	return NULL;
}

void Config::_loadFile(const string& filePath)
{
	ifstream ifs(filePath.c_str());
	if(!ifs){
		INFO("Config::_loadFile, cannot open file : ", filePath.c_str());
		assert(ifs);
	}
	string line;
	vector<string> vecBuf;
	size_t lineno = 0;
	while(getline(ifs, line))
	{
		lineno ++;
		trim(line);
		if(line.empty() || startsWith(line, "#"))
		{
			continue;
		}
		vecBuf.clear();
		if(!split(line, vecBuf, "=") || 2 != vecBuf.size())
		{
			INFO("Config::_loadFile, lineno: %d error, content: %s", lineno, line.c_str());
			continue;
		}
		string& key = vecBuf[0];
		string& value = vecBuf[1];
		trim(key);
		trim(value);
		if(!_map.insert(make_pair(key, value)).second)
		{
			INFO("Config::_loadFile, lineno: %d error, key already exist!", lineno);
			continue;
		}
	}
	ifs.close();
}
