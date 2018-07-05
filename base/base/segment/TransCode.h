/*
 * CPPJIEBA 
 * modified by qinjian@sogou-inc.com
 */
#ifndef BA_TRANSCODE_H
#define BA_TRANSCODE_H

#include "segment/StringUtil.h"
#include "segment/LocalVector.h"

namespace ba
{
    using namespace ba;

    typedef uint16_t UnicodeValueType;
    typedef ba::LocalVector<UnicodeValueType> Unicode;

    namespace TransCode
    {
        inline bool decode(const string& str, Unicode& res)
        {
            return utf8ToUnicode(str, res);
        }

        inline bool encode(Unicode::const_iterator begin, Unicode::const_iterator end, string& res)
        {
            return unicodeToUtf8(begin, end, res);
        }
        
        inline bool encode(const Unicode& uni, string& res)
        {
            return encode(uni.begin(), uni.end(), res);
        }
    }
}

#endif
