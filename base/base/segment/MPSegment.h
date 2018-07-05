/*
 * CPPJIEBA 
 * modified by qinjian@sogou-inc.com
 */
#ifndef BA_MPSEGMENT_H
#define BA_MPSEGMENT_H

#include <algorithm>
#include <set>
#include <cassert>

#include "DictTrie.h"
#include "ISegment.h"
#include "SegmentBase.h"
#include "util.h"

namespace ba
{

    struct SegmentChar 
    {
        uint16_t uniCh;
        DagType dag;
        const DictUnit * pInfo;
        double weight;
        size_t nextPos;
        SegmentChar():uniCh(0), pInfo(NULL), weight(0.0), nextPos(0)
        {}
    };

    class MPSegment: public SegmentBase
    {
        protected:
            DictTrie _dictTrie;

        public:
            MPSegment(){_setInitFlag(false);};
            explicit MPSegment(const string& dictPath, const string& userDictPath = "")
            {
                _setInitFlag(init(dictPath, userDictPath));
            };
            virtual ~MPSegment(){};

            bool init(const string& dictPath, const string& userDictPath = "");
            bool isUserDictSingleChineseWord(const Unicode::value_type & value) const;
			using SegmentBase::cut;
            bool cut(Unicode::const_iterator begin, Unicode::const_iterator end, vector<string>& res);
            bool cut(Unicode::const_iterator begin , Unicode::const_iterator end, vector<Unicode>& res);

        private:
            void _calcDP(vector<SegmentChar>& SegmentChars) const;
            bool _cut(const vector<SegmentChar>& segmentChars, vector<Unicode>& res);

    };
}

#endif
