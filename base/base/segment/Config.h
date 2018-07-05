/*
 * CPPJIEBA 
 * modified by qinjian@sogou-inc.com
 */
#ifndef BA_CONFIG_H
#define BA_CONFIG_H

#include <map>
#include <fstream>
#include <iostream>

#include "StringUtil.h"
#include "util.h"

namespace ba
{
    using namespace std;
    class Config
    {
        public:
            explicit Config(const string& filePath)
            {
                _loadFile(filePath);
            }
            bool get(const string& key, string& value) const;
            const char* operator [] (const char* key) const;
        private:
            void _loadFile(const string& filePath);

            map<string, string> _map;
            friend ostream& operator << (ostream& os, const Config& config);
    };
    
    inline ostream& operator << (ostream& os, const Config& config)
    {
        return os << config._map;
    }
}

#endif
