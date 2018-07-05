/*
 * CPPJIEBA 
 * modified by qinjian@sogou-inc.com
 */
#ifndef BA_ISEGMENT_H
#define BA_ISEGMENT_H

#include "TransCode.h"

namespace ba
{
    class ISegment
    {
        public:
            virtual ~ISegment(){};
        public:
            virtual bool cut(Unicode::const_iterator begin , Unicode::const_iterator end, vector<string>& res) = 0;
            virtual bool cut(const string& str, vector<string>& res) = 0;
    };
}

#endif
