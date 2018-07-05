/*
 * CPPJIEBA 
 * modified by qinjian@sogou-inc.com
 */
#ifndef BA_STOP_WORD_DICT_H
#define BA_STOP_WORD_DICT_H

#include <set>

#include "segment/StringUtil.h"
#include "segment/InitOnOff.h"
#include "util.h"

namespace ba
{
    using namespace ba;

    /*utf8*/
    class StopWordDict: public InitOnOff
    {
        public:
            StopWordDict(){_setInitFlag(false);}
            StopWordDict(const string& stopWordPath)
            {
                _setInitFlag(init(stopWordPath));
            }
            ~StopWordDict(){}

            bool init(const string& stopWordPath);

            bool removeStopWords(vector<string>& srcVec, vector<string>& dstVec, vector<string>& stopWordsVec) const;

        private:
            unordered_set<string> _stopWords;

            void _loadStopWordDict(const string& filePath);

    };
}

#endif


